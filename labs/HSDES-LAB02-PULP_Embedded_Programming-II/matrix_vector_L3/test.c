/*
 * Copyright (C) 2022 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Manuele Rusci, UniBO (manuele.rusci@unibo.it)
 */
#include "pmsis.h"
#include <bsp/bsp.h>    // including bps for interfacing with the RAM external devices

/*  defines */
#define N 500
#define M 500
#define MAT_EL (2)  // matrix constant values
#define VEC_EL (4)  // vector constant values

// global struct define
static struct pi_device ram;
static struct pi_hyperram_conf conf;

// matrix operand on external memory
static uint32_t matrix_ptr_L3;  // pointer to the L3 location
// PI_L2 int matrix_row[M]; //temporary location
// usiamo double buffering con due buffer
PI_L2 int matrix_row[2][M];

// input variables in L2
PI_L2 int vector[M];
// output variable in L2
PI_L2 int output_vec[N];  // N*M x M*1 -> N*1

/*
    generic matrix-vector multiplication
*/
int __attribute__((noinline)) gemv( int size_N, int size_M, int* mat_i, int*vec_i, int* vec_o ){
    for (int i=0; i<size_N; i++){
      int temp = 0;
      for (int j=0; j<size_M; j++){
          // multiply accumulate operation (MAC)
          temp += mat_i[i*size_M+j] * vec_i[j];
      }
      vec_o[i] = temp;
    }
}


int main()
{
  // Open and configure the external memory peripheral
  pi_hyperram_conf_init(&conf);
  pi_open_from_conf(&ram, &conf);
  if (pi_ram_open(&ram))
  {
      printf("Error ram open !\n");
      pmsis_exit(-3);
  }

  // Allocate output buffer
  if (pi_ram_alloc(&ram, &matrix_ptr_L3, M*N*sizeof(int)))
  {
      printf("Ram malloc failed !\n");
      pmsis_exit(-4);
  }
  // initialize the matrix using vector as temporary buffer
  for (int i=0; i<M; i++) {
    matrix_row[0][i] = MAT_EL;
    matrix_row[1][i] = MAT_EL;
  }
  for (int i=0; i<N; i++) { // copying n matrix rows from L2 to L3
    pi_ram_write(&ram,
        matrix_ptr_L3+(i*M*sizeof(int)), // dst pointer
        matrix_row,                          // L2 address
        (uint32_t) M*sizeof(int) );      // data size in bytes
  }

  // Initialization of operands and reset the ouput
  for (int i=0; i<M; i++) {
    vector[i] = VEC_EL;
  }
  for (int i=0; i<N; i++) {
    output_vec[i] = 0;
  }

  // call the matrix-vector fucntion

  // enable the perf counters of interest
  pi_perf_conf(  	1 << PI_PERF_CYCLES |
        1 << PI_PERF_INSTR );

  // reset the performance counters
  pi_perf_reset();
  // start the performance counters
  pi_perf_start();

  // task to profile
  // versione sincrona
//   for(int i=0; i<N;i++){
//     pi_ram_read(
//         &ram,
//         matrix_ptr_L3+(i*M*sizeof(int)),
//         matrix_row,
//         (uint32_t) M*sizeof(int)
//     );
//     // the core waits for the data from the dma!
//     gemv(1, M, matrix_row, vector, &output_vec[i]);
//   }

  // versione asincrona double-buffered
  pi_task_t task;
  // la prima riga la dobbiamo aspettare
  pi_ram_read(
      &ram,
      matrix_ptr_L3+(M*sizeof(int)),
      matrix_row[0],
      (uint32_t) M*sizeof(int)
  );
  int i;
  for(i=0; i<N-1;i++){
    // avvio immediatamente il trasferimento della prossima riga
    pi_ram_read_async(
        &ram,
        matrix_ptr_L3+((i+1)*M*sizeof(int)),
        matrix_row[(i+1) % 2], // copiamo nell'altro buffer
        (uint32_t) M*sizeof(int),
        pi_task_block(&task)
    );
    // calcolo il risultato della riga corrente
    gemv(1, M, matrix_row[i%2], vector, &output_vec[i]);
    // aspetto il trasferimento della prossima riga altrimenti non posso
    // altrimenti non ho i dati con cui fare i calcoli per la prossima
    // iterazione
    pi_task_wait_on(&task);
  }
  // ultima gemv
  gemv(1, M, matrix_row[i%2], vector, &output_vec[i]);

  // stop the performance counters
  pi_perf_stop();

  // collect and print statistics
  int mac = N*M;
  uint32_t instr_cnt = pi_perf_read(PI_PERF_INSTR);
  uint32_t cycles_cnt = pi_perf_read(PI_PERF_CYCLES);
  float CPI = (float) cycles_cnt / (float) instr_cnt;
  float cyc_mac = (float) cycles_cnt / (float) mac;
  float inst_mac = (float) instr_cnt / (float) mac;

  printf("---------------------------------------------\n");
  printf("---------------- statistics -----------------\n");
  printf("---------------------------------------------\n");
  printf("Number of instr = %d\n", instr_cnt);
  printf("Num of clk cyc = %d\n",cycles_cnt);
  printf("CPI = %f\n", CPI);
  printf("Number of instr/MAC = %f \n", inst_mac );
  printf("Num of clk cyc / MAC = %f\n", cyc_mac);
  printf("---------------------------------------------\n");
  printf("---------------------------------------------\n");
  printf("---------------------------------------------\n");

  // print and check the results
  printf("The %d output elements are: ", N);
  for (int i=0; i<N; i++) {
    printf("%d, ", output_vec[i]);
  }
  printf("\n");

  // check here the results
  int correctness = 1;
  for (int i=0; i<N; i++) {
    if ( output_vec[i] != (M*MAT_EL*VEC_EL))
    {
      correctness = 0;
      break;
    }
  }
  printf( correctness ?
          "Results OK\n" :
          "Results NOT OK\n"
      );
}
