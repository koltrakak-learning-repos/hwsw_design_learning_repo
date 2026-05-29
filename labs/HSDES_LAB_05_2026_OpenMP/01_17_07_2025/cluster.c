#include "pmsis.h"
#include "omp.h"
#include "math.h"
#include "perf.h"

#define N 1000

/*
 * [2.2] Parallelize the for loop below using a reduction pattern expressed
 *       by means of OpenMP directives.
 *
 *       Overheads to use for the speedup computation:
 *         - Fork:                              112 cycles
 *         - Join:                               16 cycles
 *         - Static for loop with reduction:  6 cycles (per chunk) + 32 cycles (reduction)
 *
 * TODO: Add the appropriate #pragma omp directive before the for loop.
 */
float analyze_data(float *data, unsigned int n, float alpha, float beta) {
    float sum = 0.0f;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++)
        sum += alpha * data[i] + beta;

    return sum;
}

void cluster_fn() {
    static float sensor_data[N];

    /* Data initialization (core 0 only) */
    if (pi_core_id() == 0) {
        for (int i = 0; i < N; i++)
            sensor_data[i] = (float)i * 0.001f;
    }

    float result = 0.0f;

    INIT_STATS();
    ENTER_STATS_LOOP()
        START_STATS()

        result = analyze_data(sensor_data, N, 2.0f, 1.5f);

        STOP_STATS()
    EXIT_STATS_LOOP()

    if (pi_core_id() == 0)
        printf("analyze_data result: %f\n", result);
}
