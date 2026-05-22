#include "pmsis.h"
#include "perf.h"
#include "math.h"


void compute_functions(float x, float *res1, float *res2, float *res3, float *res4) {
  int i;
  float r1 = 0.0f, r2 = 0.0f, r3 = 0.0f, r4 = 0.0f;;
  
  for(i=1; i<60; i++)
    r1 += i*sinf(((float)i/2)*x);

  for(i=1; i<60; i++)
    r2 += i*cosf(((float)i/2)*x);		    

  for(i=1; i<60; i++)
    r3 += (float)(i/2)*sinf(((float)i/4)*x);

  for(i=1; i<60; i++)
    r4 += (float)(i/2)*cosf(((float)i/4)*x);	    

  *res1 = r1;
  *res2 = r2;
  *res3 = r3;
  *res4 = r4;
}


void cluster_fn() {

  float f1, f2, f3, f4;

  // init performance counters
  INIT_STATS();

  // executing the code multiple times to perform average statistics
  ENTER_STATS_LOOP();
  
  // start measuring
  START_STATS();

  compute_functions(1.28f, &f1, &f2, &f3, &f4);

  // stop measuring
  STOP_STATS();

  // end of the performance statistics loop
  EXIT_STATS_LOOP();

  printf("Output = %f, %f, %f, %f\n", f1, f2, f3, f4);
}
