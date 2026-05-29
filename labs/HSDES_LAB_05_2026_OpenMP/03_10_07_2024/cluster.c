#include "pmsis.h"
#include "math.h"
#include "perf.h"

#define DATA_LEN    10000
#define CHUNK_SIZE    100

static float sensor_data[DATA_LEN];

typedef struct {
    float *data;
    int    data_len;
} filter_args_t;

static void apply_transform(float *data, int len) {
    for (int i = 0; i < len; i++)
        data[i] = data[i] * data[i] + 1.0f;
}

/* Stub — simulates sensor acquisition */
static void extract_sensor_data(int n) {
    (void)n;
}

/*
 * [2.2] Bare metal translation of the original OpenMP program.
 *
 * The table below maps each OpenMP construct to its PULP bare metal equivalent:
 *
 *   OpenMP construct                    Bare metal equivalent
 *   ─────────────────────────────────── ─────────────────────────────────────────
 *   #pragma omp parallel num_threads(8) pi_cl_team_fork(NUM_CORES, fn, arg)
 *   #pragma omp single                  if (pi_core_id() == 0) { ... }
 *   implicit barrier after single       pi_cl_team_barrier()
 *   #pragma omp for schedule(static)    manual [start, end) computation (see below)
 *   implicit barrier after for          pi_cl_team_barrier()
 *
 * The original loop has a non-unit step (i += CHUNK_SIZE), so the number of
 * logical iterations is:
 *     total_iter = data_len / CHUNK_SIZE          (= 100 for DATA_LEN = 10000)
 *
 * OpenMP schedule(static) without an explicit chunk size distributes them as:
 *     base       = total_iter / num_cores
 *     rem        = total_iter % num_cores
 *     → first 'rem' cores get (base + 1) iterations, the rest get base.
 *
 * Each logical iteration k maps to loop variable i = k * CHUNK_SIZE.
 */
static void parallel_body(void *arg)
{
    filter_args_t *a      = (filter_args_t *)arg;
    int core_id           = pi_core_id();
    int num_cores         = pi_cl_team_nb_cores();

    /* ── #pragma omp single ─────────────────────────────────────────────── */
    if (core_id == 0) {
        /* while(count_sensor_data() < 10000); // hardware-dependent, omitted */
        extract_sensor_data(1000);             /* 400 instructions, core 0 only */
    }
    /* implicit barrier at the end of omp single */
    pi_cl_team_barrier();

    /* ── #pragma omp for schedule(static) ──────────────────────────────── */

    int total_iter = a->data_len / CHUNK_SIZE;

    /* TODO [2.2a]: Compute this core's iteration range [iter_start, iter_end)
     *              using the OpenMP static schedule formula.
     *
     *              Hint:
     *                int base = total_iter / num_cores;
     *                int rem  = total_iter % num_cores;
     *              Derive iter_start and iter_end from base, rem, and core_id.
     *              (First 'rem' cores receive one extra iteration.)           */
    int iter_start = /* TODO */ 0;
    int iter_end   = /* TODO */ 0;

    /* TODO [2.2b]: Implement the loop over this core's chunk.
     *              Map each logical iteration k back to the original loop
     *              variable:  i = k * CHUNK_SIZE
     *              then call apply_transform exactly as in the original code. */
    for (int k = iter_start; k < iter_end; k++) {
        int i = /* TODO */ 0;
        (void)i;  /* remove once i is computed correctly */
        /* TODO: apply_transform(..., CHUNK_SIZE); */
    }

    /* implicit barrier at the end of omp for */
    pi_cl_team_barrier();
}

void cluster_fn()
{
    /* Initialize sensor data (runs on core 0 before the fork) */
    for (int i = 0; i < DATA_LEN; i++)
        sensor_data[i] = (float)i * 0.001f;

    filter_args_t args = {
        .data     = sensor_data,
        .data_len = DATA_LEN,
    };

    INIT_STATS();
    ENTER_STATS_LOOP()
        START_STATS()

        /* #pragma omp parallel shared(sensor_data) num_threads(8) { ... } */
        pi_cl_team_fork(NUM_CORES, parallel_body, &args);

        STOP_STATS()
    EXIT_STATS_LOOP()
}
