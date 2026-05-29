#include "pmsis.h"
#include "omp.h"
#include "math.h"
#include "string.h"
#include "perf.h"

#define DATA_LEN    1020
#define COEFFS_LEN    96

static float input[DATA_LEN + COEFFS_LEN];
static float output[DATA_LEN + 2 * COEFFS_LEN];
static float W[COEFFS_LEN];

/* Stub — implementation not required for this exercise */
static void apply_padding(float *y, int data_len, int coeffs_len) {
    memset(y, 0, (data_len + 2 * coeffs_len) * sizeof(float));
}

/*
 * [2.2] Parallelize the outer for loop using a single OpenMP directive.
 *
 * The pragma must be placed AFTER apply_padding (which is sequential) and
 * BEFORE the outer for loop. All data clauses must be explicitly stated.
 *
 * Directive to use:
 *   #pragma omp parallel for
 *
 * Clause guide:
 *   schedule(static)         — divides the 'data_len' iterations into equal
 *                              contiguous chunks assigned to each core at
 *                              compile time; no runtime overhead, correct here
 *                              because all iterations have identical cost.
 *   shared(x, y, coeffs,     — these are pointers / scalars already visible to
 *          data_len,            all threads; a single shared copy is correct
 *          coeffs_len)          since each iteration i writes only y[i].
 *   private(j) or             — NOT needed: j is declared inside the inner for
 *   (none needed for j)         loop (C99 scope), so it is implicitly private.
 *                              i is the parallel-for loop variable — also
 *                              implicitly private.
 *
 * Iteration distribution with data_len=1020, 8 cores, schedule(static):
 *   base = 1020/8 = 127,  rem = 1020%8 = 4
 *   cores 0–3 : 128 iterations    core 0: i ∈ [0,   127]
 *                                 core 1: i ∈ [128, 255]
 *                                 core 2: i ∈ [256, 383]
 *                                 core 3: i ∈ [384, 511]
 *   cores 4–7 : 127 iterations    core 4: i ∈ [512, 638]
 *                                 core 5: i ∈ [639, 765]
 *                                 core 6: i ∈ [766, 892]
 *                                 core 7: i ∈ [893, 1019]
 *
 * [2.3] Parallel speedup (calculation only — no additional code required):
 *   Sequential work  = apply_padding (10 + 2*96 = 202 ops)
 *                    + 1020 * (3 + 48*10) = 1020 * 483 = 492 660 ops
 *                    = 492 862 ops total
 *   Parallel work    = 492 660 / 8 = 61 582.5 ops  (per core, static schedule)
 *   Overheads        = 100 (open parallel) + 6*128 (static for, worst-case core)
 *                    + 10 (implicit barrier at end of for)
 *   Speedup = T_seq / T_par  (use the instruction counts above with the given CPI)
 *
 * TODO [2.2]: Add the #pragma omp directive below.
 */
int filter(float x[], float y[], int data_len, float coeffs[], int coeffs_len) {
    // Padding output data (10 + 2 * coeffs_len operations, sequential)
    apply_padding(y, data_len, coeffs_len);

    for (int i = 0; i < data_len; i++) {
        // Init y[i] with zero (3 cycles)
        y[i] = 0;
        for (int j = 0; j < coeffs_len; j += 2) {
            // Update y (10 operations per iteration)
            y[i] += x[i + j] * coeffs[j] + x[i + j + 1] * coeffs[j + 1];
        }
    }
    return 0;
}

void cluster_fn() {
    for (int i = 0; i < DATA_LEN + COEFFS_LEN; i++)
        input[i] = (float)i * 0.001f;
    for (int i = 0; i < COEFFS_LEN; i++)
        W[i] = (float)(i + 1) * 0.01f;

    INIT_STATS();
    ENTER_STATS_LOOP()
        START_STATS()

        filter(input, output, DATA_LEN, W, COEFFS_LEN);

        STOP_STATS()
    EXIT_STATS_LOOP()

    printf("filter output[0]: %f\n", output[0]);
}
