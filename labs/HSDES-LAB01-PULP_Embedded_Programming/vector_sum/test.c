/*
 * Copyright (C) 2021 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Manuele Rusci, UniBO (manuele.rusci@unibo.it)
 */

#include "pmsis.h"

// #define N 350 // array size
unsigned int array_1[N]; 	// note: 8 bit array from 0 to 255

//intialize an array A_ar with _size_ number of elements
//and precompute the result
int init_array(unsigned int* a_ar, int size)
{
	for(int i=0;i<size;i++)
		a_ar[i] = i+1;	// this can be changed to whatever pattern

	return (size * (size + 1)) >> 1;
}

// return the index of the item==element of the array a_ar with size number of elements
int vector_sum(unsigned int* a_ar, int size){
	int sum = 0;	// initialize accumulator to 0

	// perform the sum
	for(int i=0; i<size; i++){
		sum += a_ar[i];
	}
	return sum;
}

int main()
{
	printf("This example program computes the sum of the first %d integer numbers!\n", N);

	// initialize the array and get the expected result
	int expected_result = init_array(array_1, N);

    // call the vector_sum function
    int computed_result = vector_sum(array_1, N);

	// check if the results is correct
    if (computed_result == expected_result)
    	printf("Result is correct\n");
    else
    	printf("Result is not correct. Got %d instead of %d\n", computed_result, expected_result);
}
