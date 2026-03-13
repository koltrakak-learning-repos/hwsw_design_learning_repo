/*
 * Copyright (C) 2020 University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Manuele Rusci, UniBO (manuele.rusci@unibo.it)
 */

#include "pmsis.h"

#define N 50 // matrix size
int array[N];

//intialize an array A_ar with size number of elements
void init_array(int * A_ar, int size)
{
	for(int i=0;i<size;i++)
		A_ar[i] = i;	// this can be changed to whatever pattern
}

// return the index of the item==element of the array A_ar with size number of elements
int find_element(int * A_ar, int size, int element){
	int find = -1;	// return the index of the found value else -1 if the element is not found

    for(int i=0; i<size; i++) {
        if(A_ar[i] == element) {
            find = i;
            break;
        }
    }

	return find;
}

int main()
{
	int element_to_find = 10;	// you can try to change this value to find within the array!
    init_array(array, N);

    // call the find function
    int found = find_element(array, N, element_to_find);
    if (found == -1)
    	printf("Element not found\n");
    else
    	printf("Element %d found at position %d\n", element_to_find, found);
}
