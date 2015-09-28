/*
 * GameOfLife.c
 *
 *  Created on: Sep 27, 2015
 *      Author: sumit.purohit
 *      Author: Eric Thomas
 */


#include <stdio.h>
#include <mpi.h>
#include <assert.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>

void GenerateInitialGoL (int rank,int p, int start,int n, int m,int matrix[n][m],int seed)
{
int i,j;
srand(seed);
printf("\ncall from %d rank start row  is %d and size is %d  and number of cols\n",rank, start,n,m);
	for (i = 0; i < n; i++)
	     for (j = 0; j < m; j++)
	     {
	    	  int num;
	    	  num = rand();
	    	  if(num%2 == 0)
	    		  matrix[i][j] = 1;
	    	  else
	    		  matrix[i][j] = 0;
	     }


	for (i = 0; i < n; i++)
		 {
		 for (j = 0; j < m; j++)
			   printf("v = %d  ",matrix[i][j]);
				 printf("\n");
		 }


}



int main(int argc, char *argv[]) {

	int rank, p, N, G;
	struct timeval t1, t2;
	N = 16;
	p = 2;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int MAX_SIZE = 30;
	int MAX_TRIAL = 100;

	int randomseed[p];
	srand(time(NULL));

	if (rank == 0) {
		int i;

		for (i = 0; i < p; i++) {
			randomseed[i] = rand();
			//(int)(1+(int)(1000*drand48()));
		}

		for (i = 1; i < p; i++) {
			MPI_Send(&randomseed[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		int start, end;
			start = rank * (N / p);
			end = (rank + 1) * (N / p) - 1;
			int rows = (end - start);
			int matrix[rows][N];

			//initialize matrix part
			GenerateInitialGoL(rank, p, start, rows, N, matrix,randomseed[0]);
	} else {
		int a = 0;
		MPI_Status status;
		MPI_Recv(&a, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		printf("\n ranke = %d and seend = %d", rank, a);
		int start, end;
			start = rank * (N / p);
			end = (rank + 1) * (N / p) - 1;
			int rows = (end - start);
			int matrix[rows][N];

			//initialize matrix part
			GenerateInitialGoL(rank, p, start, rows, N, matrix,a);
	}


	MPI_Finalize();
	//This will output the array.

}
