#include "pmsis.h"
#include "perf.h"

#define M 32
#define N 32
#define O 32


// APPLICATION DATA
PI_L1 int matA[M*N];
PI_L1 int matB[N*O];
PI_L1 int matC[M*O];


void matMul(int * pSrcA, int  * pSrcB, int * pDstC, int m, int n, int o);


// Helper functions

void matrix_init(int * A, int * B, int * C) {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      A[i*N+j] = (int)(j+1);
    }
  }

  for (int i = 0; i <N; i++) {
    for (int j = 0; j < O; j++) {
      B[i*O+j] = i+1;
    }
  }

  for (int i = 0; i < M; i++) {
    for (int j = 0; j < O; j++) {
      C[i*O+j] = 0;
    }
  }

}

unsigned int matrix_check(int * C) {
  unsigned int errors = 0;
  // check
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < O; j++) {
      int ref_val = ((N)*(N+1)*(2*(N)+1))/6;
      if (C[i*O+j] != ref_val) {
        printf("Error at index (%d,%d): %d instead of %d\n", i, j, C[i*N+j], ref_val);
        errors++;
      }
    }
  }
  return errors;
}


void cluster_fn() {
  volatile int m = M;
  volatile int n = N;
  volatile int o = O;

  // init performance counters
  INIT_STATS();

  // executing the code multiple times to perform average statistics
  ENTER_STATS_LOOP();
  
  // set initial values (not considered by performance counters)
  matrix_init(matA, matB, matC);

  // start measuring
  START_STATS();

  // workload
  matMul(matA, matB, matC, m, n, o);

  // stop measuring
  STOP_STATS();

  // end of the performance statistics loop
  EXIT_STATS_LOOP();

#ifdef DEBUG  
  // check the result (optional)
  matrix_check(matC);
#endif  
}
