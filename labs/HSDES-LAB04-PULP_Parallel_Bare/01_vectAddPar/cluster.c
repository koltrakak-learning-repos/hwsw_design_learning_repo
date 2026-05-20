#include "pmsis.h"
#include "perf.h"

#define N 512


// APPLICATION DATA
PI_L2 int vectA[N];
PI_L2 int vectB[N];
PI_L2 int vectC[N];

// Non ho ancora capito perchè marcare esplicitamente come L2 se è il default
// int vectA[N];
// int vectB[N];
// int vectC[N];

#if NUM_CORES == 1
  void vectAdd(int * pSrcA, int  * pSrcB, int * pDstC, int n);
#else
  // this must be designed!
  void vectAddPar(int * pSrcA, int  * pSrcB, int * pDstC, int n);
#endif


// Helper functions

void vect_init(int * A, int * B, int * C, int size) {
  for (int i = 0; i < size; i++) {
    A[i] = i;
    B[i] = i+2;
    C[i] = 0;
  }
}

unsigned int vect_check(int * C, int size) {
  unsigned int errors = 0;
  // check
  for (int i = 0; i < size; i++) {
    int ref_val = 2*i+2;
    if (C[i] != ref_val) {
      printf("Error at index (%d): %d instead of %d\n", i, C[i], ref_val);
      errors++;
    }
  }
  if (errors==0)
    printf("Checksum is OK!\n");
  else
    printf("Total Number of errrors is %d\n", errors);
  return errors;
}


void cluster_fn() {
  volatile int n = N;

  // init performance counters
  INIT_STATS();

  // executing the code multiple times to perform average statistics
  ENTER_STATS_LOOP();

  // set initial values (not considered by performance counters)
  vect_init(vectA, vectB, vectC, n);

  // start measuring
  START_STATS();

  // workload
#if NUM_CORES == 1
  vectAdd(vectA, vectB, vectC, n);
#else
  vectAddPar(vectA, vectB, vectC, n);
#endif
  // stop measuring
  STOP_STATS();

  // end of the performance statistics loop
  EXIT_STATS_LOOP();

#ifdef DEBUG
  // check the result (optional)
  vect_check(vectC, n);
#endif
}
