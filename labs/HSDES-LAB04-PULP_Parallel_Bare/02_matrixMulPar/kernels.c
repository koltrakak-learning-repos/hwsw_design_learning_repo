#include "pmsis.h"

void matMulSequential(int * pSrcA, int  * pSrcB, int * pDstC, int m, int n, int o) {
    uint32_t i, j, k;

    for (k = 0; k < o; k++) {
        for (i = 0; i < m; i++) {
            int32_t sum = 0;

            for (j = 0; j < n; j++) {
                int32_t a = pSrcA[i * n + j];
                int32_t b = pSrcB[j * o + k];

                sum += a * b;
            }

            pDstC[i * o + k] = sum;
        }
    }
}

// parallel version
void matMul(int * pSrcA, int  * pSrcB, int * pDstC, int m, int n, int o) {
    uint32_t i, j, k;

    int core_id = pi_core_id();
    int outer_iterations_per_core = (o + NUM_CORES-1) / NUM_CORES; // == ceil(o/NUM_CORES)
    int o_lb = core_id * outer_iterations_per_core;
    int o_ub = o_lb + outer_iterations_per_core > n ? n : o_lb + outer_iterations_per_core;

    for (k = o_lb; k < o_ub; k++) {
        for (i = 0; i < m; i++) {
            int32_t sum = 0;

            for (j = 0; j < n; j++) {
                int32_t a = pSrcA[i * n + j];
                int32_t b = pSrcB[j * o + k];

                sum += a * b;
            }

            pDstC[i * o + k] = sum;
        }
    }

    pi_cl_team_barrier();
}
