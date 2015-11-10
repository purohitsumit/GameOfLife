/*
 * main.c
 *
 *  Created on: Nov 9, 2015
 *      Author: Eric
 *      Autho: Sumit
 */
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<mpi.h>

// Include file from last project.
#include "random_number.c"


int main(int argc, char* argv[]){


	 MPI_Init(&argc, &argv);
	//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//    MPI_Comm_size(MPI_COMM_WORLD, &p);

	// Get a pointer to the random number array
	 // returned by method from previous PA.
	 int *randomNumberArray = randomNumber();

}

