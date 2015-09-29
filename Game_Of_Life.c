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

void GenerateInitialGoL (int rank,int p, int start,int N, int effective_cols_size,int matrix[N][effective_cols_size],int seed)
{
	int i, j;
	srand(seed);
	printf("\ncall from %d rank start row  is %d and size is %d  and number of cols %d \n",
			rank, start, N, effective_cols_size);
	for (i = 0; i < N; i++)
		for (j = 1; j < effective_cols_size-1; j++) {
			int num;
			num = rand();
			if (num % 2 == 0)
				matrix[i][j] = 1;
			else
				matrix[i][j] = 0;
		}

	int next_rank = (rank + 1) % p;
	int previous_rank = (rank + p - 1) % p;

	//send ACTUAL last col to next_rank
	for(i=0;i<N;i++)
	{
		MPI_Send(&matrix[i][effective_cols_size-2], 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
	}
	//send ACTUAL first col to previous_rank
	for(i=0;i<N;i++)
	{
		MPI_Send(&matrix[i][1], 1, MPI_INT, previous_rank, 0, MPI_COMM_WORLD);
	}
	//update previous rank ghost column
	for(i = 0 ; i < N; i++)
	{
		int a = 0;
		MPI_Status status;
		MPI_Recv(&a, 1, MPI_INT, previous_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		matrix[i][0] = a;
	}

	//update next rank ghost column
	for(i = 0 ; i < N; i++)
	{
		int a = 0;
		MPI_Status status;
		MPI_Recv(&a, 1, MPI_INT, next_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		matrix[i][effective_cols_size-1] = a;
	}

//	printf("\n");
//	for (i = 0; i < N; i++) {
//		for (j = 0; j < effective_cols_size; j++)
//			printf("v%d = %d  ",rank, matrix[i][j]);
//		printf("\n");
//	}

}
int getNorthState(int i,int j,int rank,int N, int effective_cols_size,int matrix[N][effective_cols_size])
{
	if(i==0)
		return matrix[N][j];
	return matrix[i-1][j];

}

int getSouthState(int i,int j,int rank,int N, int effective_cols_size,int matrix[N][effective_cols_size])
{
	if(i==N)
		return matrix[0][j];
	return matrix[i+1][j];

}

int getEastState(int i,int j,int rank,int N, int effective_cols_size,int matrix[N][effective_cols_size])
{
	return matrix[i][j+1];
}
int getWestState(int i,int j,int rank,int N, int effective_cols_size,int matrix[N][effective_cols_size])
{
	return matrix[i][j-1];
}
int DetermineState(int i,int j,int rank,int N, int effective_cols_size,int matrix[N][effective_cols_size])
{
	int No = getNorthState(i,j,rank,N,effective_cols_size,matrix);
	int S = getSouthState(i,j,rank,N,effective_cols_size,matrix);
	int E = getEastState(i,j,rank,N,effective_cols_size,matrix);
	int W = getWestState(i,j,rank,N,effective_cols_size,matrix);
	//int NE = getNortEastState();


	return (No + S + E + W);
}
void Simulate(int g,int rank,int N, int effective_cols_size,int matrix[N][effective_cols_size])
{
	int tmpdata[N][effective_cols_size];
	int i,j;
	for (i = 0; i < N; i++)
		for (j = 0; j < effective_cols_size; j++)
			tmpdata[i][j] = matrix[i][j];

		for (i = 0; i < N; i++) {
			for (j = 1; j < effective_cols_size - 1; j++) {
				int state;
				state = DetermineState(i, j, rank, N, effective_cols_size,
						tmpdata);
				if ((state < 2) || (state > 3))
					matrix[i][j] = 0;
				else
					matrix[i][j] = 1;

			}
		}


	printf("\n Last State : of rank %d\n",rank);
		for (i = 0; i < N; i++) {
			for (j = 0; j < effective_cols_size; j++)
				printf("v%d-%d-%d = %d  ",rank,i,j, tmpdata[i][j]);
			printf("\n");
		}

	printf("\n \n New State : of rank %d\n",rank);
				for (i = 0; i < N; i++) {
					for (j = 0; j < effective_cols_size; j++)
						printf("v%d-%d-%d = %d  ",rank,i,j, matrix[i][j]);
					printf("\n");
				}
}


int main(int argc, char *argv[]) {

	int rank, p, N, G;
	struct timeval t1, t2;
	N = 16;
	p = 2;
	G = 1;

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
		int start_index, end_index;
		start_index = rank * (N / p);
		end_index = (rank + 1) * (N / p) - 1;
		int cols_size = (end_index - start_index) +1;
		int effective_cols_size = cols_size + 2;
		int matrix[N][effective_cols_size];

		//initialize matrix part
		GenerateInitialGoL(rank, p, start_index, N, effective_cols_size,matrix,randomseed[0]);
		for(i=0;i<G;i++)
		{
			MPI_Barrier(MPI_COMM_WORLD);
			Simulate(i,rank,N,effective_cols_size,matrix);
		}
	} else {
		int a = 0;
		MPI_Status status;
		MPI_Recv(&a, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		printf("\n ranke = %d and seend = %d", rank, a);
		int start_index, end_index;
		start_index = rank * (N / p);
		end_index = (rank + 1) * (N / p) - 1;
		int cols_size = (end_index - start_index) +1;
		int effective_cols_size = cols_size + 2;
		int matrix[N][effective_cols_size];

		//initialize matrix part
		GenerateInitialGoL(rank, p, start_index, N, effective_cols_size,matrix,a);
		int i;
		for(i=0;i<G;i++)
		{
			MPI_Barrier(MPI_COMM_WORLD);
			Simulate(i,rank,N,effective_cols_size,matrix);
		}
	}


	MPI_Finalize();
	//This will output the array.

}
