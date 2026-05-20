#include "pmsis.h"

// Sequential version
void vectAdd(int * pSrcA, int  * pSrcB, int * pDstC, int n) {
    int i;
    for (i = 0; i < n; i++) {
        pDstC[i] = pSrcA[i] + pSrcB[i];

    }
}

// Parallel version

// versione in cui la distribuzione del lavoro non è coalescente
// void vectAddPar(int * pSrcA, int  * pSrcB, int * pDstC, int n) {
//     int core_id = pi_core_id();
//     for (int i = core_id; i < n; i+=NUM_CORES) {
//         pDstC[i] = pSrcA[i] + pSrcB[i];
//     }

//     pi_cl_team_barrier();
// }

void vectAddPar(int * pSrcA, int  * pSrcB, int * pDstC, int n) {
    int core_id = pi_core_id();
    int elements_per_core = (n + NUM_CORES-1) / NUM_CORES; // == ceil(n/NUM_CORES)
    int lb = core_id * elements_per_core;
    int ub = lb + elements_per_core > n ? n : lb + elements_per_core;

    for (int i = lb; i < ub; i++) {
        pDstC[i] = pSrcA[i] + pSrcB[i];
    }

    pi_cl_team_barrier();
}
