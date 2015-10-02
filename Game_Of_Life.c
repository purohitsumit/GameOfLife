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
    //printf("\ncall from %d rank start row  is %d and size is %d  and number of cols %d \n",
//            rank, start, N, effective_cols_size);
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

//  printf("\n");
//  for (i = 0; i < N; i++) {
//      for (j = 0; j < effective_cols_size; j++)
//          printf("v%d = %d  ",rank, matrix[i][j]);
//      printf("\n");
//  }

}
void UpdateNeighbors(int rank,int p,int N, int effective_cols_size,int matrix[N][effective_cols_size])
{
    int i;
    int next_rank = (rank + 1) % p;
    int previous_rank = (rank + p - 1) % p;

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

}
void DisplayGol(int N, int effective_cols_size, int matrix[N][effective_cols_size], int rank)
{

    int realColumnSize = effective_cols_size-2;
    int arraySize = N * realColumnSize;
    int tempArray[arraySize];
    int count = 0;
    int r, c;
    int displaymatrix[N][N];
    int tempTempArray[N*N];
    //printf("\nEFFECTIVE COL SIXE :%d",effective_cols_size);
	for(c=1;c<effective_cols_size-1;c++){
		for(r=0;r<N;r++){
				tempArray[count] = matrix[r][c];
				count++;
				//printf("SETTING RANK:%d, INDEX: %d and %d, VALUE: %d\n", rank, r,c, tempArray[count-1]);
		}
	}
if(rank==0)
	{
	MPI_Gather(tempArray, N * (realColumnSize), MPI_INT,  tempTempArray,N * (realColumnSize), MPI_INT, 0, MPI_COMM_WORLD);
	}
else
	{
	MPI_Gather(tempArray, N * (realColumnSize), MPI_INT,  NULL,0, MPI_INT, 0, MPI_COMM_WORLD);
	}

            int q = 0;

           // for(q=0; q< N*realColumnSize; q++){
           //     printf("RANK: %d, INDEX: %d, VALUE: %d\n", rank, q, tempArray[q]);
           // }

            if(rank==0){
                // If the rank is 0 we will need to gather from the array
                // put it into a matrix and
                for(c=0;c<N*N;c++){

                    displaymatrix[c%N][c/N] = tempTempArray[c];
                    //printf("INDEX 22:  %d, VALUE:  %d\n", c, tempTempArray[c]);

                }
//                printf("\n \n GATHER AT RANK %d\n",rank);
//			  for (r = 0; r < N; r++) {
//				  for (c = 0; c < N; c++)
//					  printf("V_G-%d-%d = %d  ",r,c, displaymatrix[r][c]);
//				  printf("\n");
//			  }

            }





   //return;
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
int getNWState(int i, int j, int rank, int N, int effective_cols_size, int  matrix[N][effective_cols_size])
{
    if(i==0)
        return matrix[N][j-1];
    return matrix[i-1][j-1];

}
int getNEState(int i, int j, int rank, int N, int effective_cols_size, int  matrix[N][effective_cols_size])
{
    if(i==0)
        return matrix[N][j+1];
    return matrix[i-1][j+1];
}
int getSWState(int i, int j, int rank, int N, int effective_cols_size, int  matrix[N][effective_cols_size])
{
    if(i==N)
        return matrix[0][j-1];
    return matrix[i+1][j-1];
}
int getSEState(int i, int j, int rank, int N, int effective_cols_size, int  matrix[N][effective_cols_size])
{
    if(i==N)
        return matrix[0][j+1];
    return matrix[i+1][j+1];

}

int DetermineState(int i,int j,int rank,int N, int effective_cols_size,int matrix[N][effective_cols_size])
{
    int No = getNorthState(i,j,rank,N,effective_cols_size,matrix);
    int S = getSouthState(i,j,rank,N,effective_cols_size,matrix);
    int E = getEastState(i,j,rank,N,effective_cols_size,matrix);
    int W = getWestState(i,j,rank,N,effective_cols_size,matrix);
    //int NE = getNortEastState();
    int Nw = getNWState(i,j,rank,N,effective_cols_size,matrix);
    int Ne = getNEState(i,j,rank,N,effective_cols_size,matrix);
    int Sw = getSWState(i,j,rank,N,effective_cols_size,matrix);
    int Se = getSEState(i,j,rank,N,effective_cols_size,matrix);

    return (No + S + E + W + Nw + Ne + Sw + Se);
}

void Simulate(int g,int rank,int N, int effective_cols_size,int matrix[N][effective_cols_size],int p)
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

    UpdateNeighbors(rank,p,N,effective_cols_size,matrix);

//    printf("\n Last State : of rank %d\n",rank);
//        for (i = 0; i < N; i++) {
//            for (j = 0; j < effective_cols_size; j++)
//                printf("v%d-%d-%d = %d  ",rank,i,j, tmpdata[i][j]);
//            printf("\n");
//        }

//    //printf("\n \n New State : of rank %d\n",rank);
//                for (i = 0; i < N; i++) {
//                    for (j = 0; j < effective_cols_size; j++)
//                        //printf("v%d-%d-%d = %d  ",rank,i,j, matrix[i][j]);
//                    //printf("\n");
//                }

//    UpdateNeighbors(rank,p,N,effective_cols_size,matrix);

}
int main(int argc,char *argv[])
{
	int rank, p, N, G,X;

    struct timeval t1, t2;

    // Change this "N" from 2 up to 16

    N = 16;

    p = 2;

    G = atoi(argv[1]);

//    X = 3;

    X = 1;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &p);



    //Timing for total runtime

    struct timeval totalRuntimeStart, totalRuntimeEnd;

    int totalRuntime;



    // Timing for average time per generation(excluding display)

    struct timeval avgPerGenerationStart, avgPerGenerationEnd;

    int averageTimePerGeneration;

    int tempTime;



    // Average time per the display function

    struct timeval displayRuntimeStart, displayRuntimeEnd;

    int averageDisplayTimePerGeneration;

    int displayTempTime;

    int displayMethodCounter = 0;





    // Time for different communication steps.

    struct timeval mpiCallRuntimeStart, mpiCallRuntimeEnd;





    int MAX_SIZE = 30;

    int MAX_TRIAL = 100;

    int userInput;


    int randomseed[p];

    srand(time(NULL));

    gettimeofday(&totalRuntimeStart, NULL);
	int i;
    if(rank == 0)
    {
    	for (i = 0; i < p; i++)
    	            randomseed[i] = rand();
    }
    int received_random;
    MPI_Scatter(randomseed, 1, MPI_INT, &received_random, 1,MPI_INT,0, MPI_COMM_WORLD);

    int start_index, end_index;

	start_index = rank * (N / p);

	end_index = (rank + 1) * (N / p) - 1;

	int cols_size = (end_index - start_index) +1;

	int effective_cols_size = cols_size + 2;

	int matrix[N][effective_cols_size];

     //initialize matrix part

     GenerateInitialGoL(rank, p, start_index, N, effective_cols_size,matrix,received_random);

     for(i=0;i<G;i++)

            {
             	gettimeofday(&avgPerGenerationStart, NULL);

                MPI_Barrier(MPI_COMM_WORLD);

                Simulate(i,rank,N,effective_cols_size,matrix,p);

                if(i%X==0)

                {
                    gettimeofday(&displayRuntimeStart, NULL);

                    DisplayGol(N, effective_cols_size, matrix,rank);

                    gettimeofday(&displayRuntimeEnd, NULL);

                    displayTempTime += ((displayRuntimeEnd.tv_sec-displayRuntimeStart.tv_sec)*1000 + ((displayRuntimeEnd.tv_usec - displayRuntimeStart.tv_usec)/1000));

                    displayMethodCounter++;
                }

                gettimeofday(&avgPerGenerationEnd, NULL);

                tempTime+= ((avgPerGenerationEnd.tv_sec-avgPerGenerationStart.tv_sec)*1000 + ((avgPerGenerationEnd.tv_usec - avgPerGenerationStart.tv_usec)/1000));

                averageTimePerGeneration = tempTime/G;





            }



            // Average the number of times the display method was called.

            averageDisplayTimePerGeneration = displayTempTime / displayMethodCounter;





            // Get the total runtime

            gettimeofday(&totalRuntimeEnd, NULL);

            totalRuntime = (totalRuntimeEnd.tv_sec - totalRuntimeStart.tv_sec)*1000 + (totalRuntimeEnd.tv_usec - totalRuntimeStart.tv_usec)/1000;


    printf("RANK: %d     TOTAL RUNTIME: %d\n",rank, totalRuntime);

    printf("RANK: %d     AVERAGE RUNTIME PER GENERATION: %d\n", rank, averageTimePerGeneration);

    //printf("RANK: %d     AVERAGE TIME PER DISPLAY METHOD: %d\n", rank, averageDisplayTimePerGeneration);



    MPI_Finalize();

    //This will output the array.



}
