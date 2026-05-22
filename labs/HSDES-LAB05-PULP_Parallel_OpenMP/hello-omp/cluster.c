#include "omp.h"
#include "math.h"

float a = 0.5f;
int b = 1, c = 2;

void cluster_fn() {

  // Put a pragma here!!!
  #pragma omp parallel num_threads(NUM_CORES) firstprivate(a)
  {
    a = cosf(omp_get_thread_num()*a);
    printf("Core %d, var a (before) =  %f\n", omp_get_thread_num(), a);
    printf("Core %d, var b =  %d\n", omp_get_thread_num(), b);
    printf("Core %d, var c =  %d\n", omp_get_thread_num(), c);
    printf("Core %d, var a (after) =  %f\n", omp_get_thread_num(), a);
  }
}
