#include "pmsis.h"
#include "math.h"
#include "string.h"
#include "perf.h"

#define DATA_LEN    970
#define COEFFS_LEN   96

static float input[DATA_LEN + COEFFS_LEN];
static float output[DATA_LEN + 2 * COEFFS_LEN];
static float W[COEFFS_LEN];

typedef struct {
    float *x;
    float *y;
    int    data_len;
    float *coeffs;
    int    coeffs_len;
} filter_args_t;

/* Sequential helper — implementation not required for this exercise */
static void apply_padding(float *y, int data_len, int coeffs_len) {
    memset(y, 0, (data_len + 2 * coeffs_len) * sizeof(float));
}

/*
 * [2.3] Bare metal parallelization of the filter loop.
 *
 * This function is executed by ALL NUM_CORES cores simultaneously,
 * launched via pi_cl_team_fork(). It must reproduce the behaviour of
 * the original OpenMP version which used schedule(static) on the
 * outer loop.
 *
 * Three steps are required:
 *   (a) Synchronize all cores after core 0 finishes apply_padding.
 *   (b) Compute each core's [start, end) range using the same partition
 *       rule as OpenMP schedule(static) without an explicit chunk size:
 *         base  = data_len / num_cores
 *         rem   = data_len % num_cores
 *         The first 'rem' cores receive one extra iteration.
 *   (c) Execute the outer loop over [start, end).
 */
static void filter_parallel(void *arg)
{
    filter_args_t *a = (filter_args_t *)arg;

    int core_id   = pi_core_id();
    int num_cores = pi_cl_team_nb_cores();

    /* Sequential padding — only core 0 */
    if (core_id == 0)
        apply_padding(a->y, a->data_len, a->coeffs_len);

    /* TODO [2.3a]: Synchronize all cores before entering the parallel loop.
     *              Use the appropriate PULP bare metal barrier primitive.    */
    pi_cl_team_barrier();

    /* TODO [2.3b]: Compute this core's iteration range [start, end) so that
     *              the work distribution matches OpenMP schedule(static).
     *
     *              Hint:
     *                int base  = a->data_len / num_cores;
     *                int rem   = a->data_len % num_cores;
     *              Use base and rem to derive start and end for this core_id. */

    int chunk_size = a->data_len / num_cores;
    int rem   = a->data_len % num_cores;
    int start = core_id*chunk_size;
    int end   = start + chunk_size;

    /* TODO [2.3c]: Implement the outer loop over [start, end).
     *              The loop body is given — only the bounds need to be filled
     *              in once (b) is complete.                                   */
    for (int i = start; i < end; i++) {
        a->y[i] = 0.0f;                                      /* 3 instructions */
        for (int j = 0; j < a->coeffs_len; j += 2)          /* 10 instructions per iteration */
            a->y[i] += a->x[i + j]     * a->coeffs[j] +
                       a->x[i + j + 1] * a->coeffs[j + 1];
    }

    /* Wait for all cores to finish before returning to cluster_fn */
    pi_cl_team_barrier();
}

void cluster_fn()
{
    /* Data initialization (runs on core 0 before the fork) */
    for (int i = 0; i < DATA_LEN + COEFFS_LEN; i++)
        input[i] = (float)i * 0.001f;
    for (int i = 0; i < COEFFS_LEN; i++)
        W[i] = (float)(i + 1) * 0.01f;

    filter_args_t args = {
        .x          = input,
        .y          = output,
        .data_len   = DATA_LEN,
        .coeffs     = W,
        .coeffs_len = COEFFS_LEN,
    };

    INIT_STATS();
    ENTER_STATS_LOOP()
        START_STATS()

        pi_cl_team_fork(NUM_CORES, filter_parallel, &args);

        STOP_STATS()
    EXIT_STATS_LOOP()

    printf("filter output[0]: %f\n", output[0]);
}
