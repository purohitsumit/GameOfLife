/*
 * Random Number
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


int main(int argc,char *argv[])
{
	int rank, p, N, a,b,Prime,seed;

    struct timeval t1, t2, t3, t4;

    N = 16;

    a = 3;
    b = 5;
    seed = 10;
    Prime = 93563;
//    X = 3;

    // Get a,b,P and random seed from the command line.
    // Overwriting the hardcoded values above.
    if(argc > 5){
    	a = atoi(argv[1]);
    	b = atoi(argv[2]);
    	Prime = atoi(argv[3]);
    	seed = atoi(argv[4]);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    printf("Hello from rank %d",rank);


    //Step 1; Every proc has N/p entries of 2d matrix
    // create 2d matrix
    int **m = malloc(2 * (sizeof(int*)));
    m[0] = malloc(2 * (sizeof(int)));
    m[1] = malloc(2 * (sizeof(int)));
    m[0][0] = a;
    m[0][1] = 0;
    m[1][0] = b;
    m[1][1] = 1;

    //create array of 2d matrixes {0...to (N/p-1)}
    int ***myx;
    myx = malloc((N/p) * sizeof(int **));
    int i=0;
    for(i=0;i<N/p;i++)
    	myx[i] = m;




    int d3 = N/p -1;
    int u,v,w;

    //Step 2: Parallel Prefix with matrix multiplication
    // Step 2.1 : get local Identity matrix
    int **local = malloc(2 * (sizeof(int*)));
    local[0] = malloc(2 * (sizeof(int)));
    local[1] = malloc(2 * (sizeof(int)));
    local[0][0] = 1;
    local[0][1] = 0;
    local[1][0] = 0;
    local[1][1] = 1;
    //Step 2.2 : get global matrix
    int **global = malloc(2 * (sizeof(int*)));
    global[0] = malloc(2 * (sizeof(int)));
    global[1] = malloc(2 * (sizeof(int)));
    global[0][0] = a;
    global[0][1] = 0;
    global[1][0] = b;
    global[1][1] = 1;
    for(i=0;i<(N/p)-1;i++)
    	matrix_multiplication(m,global,global);
    //Step 2.3  parallel prefix for loop
    int number_of_steps = log10 (p) / log10 (2);
    int x=1;
    for(i=0;i<number_of_steps;i++)
    {
    	//send data
    	int* send_data = malloc(4*(sizeof(int)));
    	send_data[0] = global[0][0];
    	send_data[1] = global[0][1];
    	send_data[2] = global[1][0];
    	send_data[3] = global[1][1];

    	//find mate;update x
    	int mate = rank ^ x;
    	x=x<<1;

    	MPI_Send(send_data, 4, MPI_INT, mate, 0, MPI_COMM_WORLD);

    	//receive data
    	int* receive_data = malloc(4 * sizeof(int));
        MPI_Status status;
    	MPI_Recv(receive_data, 4, MPI_INT, mate, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    	//update your local Only if mate rank < my rank
    	int** received_global = malloc(2 * sizeof(int *));
    	received_global[0] = malloc(2 * sizeof(int));
    	received_global[1] = malloc(2 * sizeof(int));
    	received_global[0][0] = receive_data[0];
    	received_global[0][1] = receive_data[1];
    	received_global[1][0] = receive_data[2];
    	received_global[1][1] = receive_data[3];



    	if(rank > mate)
    	{
    		matrix_multiplication(local,received_global,local);
    	}

    	//always update global
    	matrix_multiplication(global,received_global,global);
    }


    int* resulting_random_numbers = malloc((N/p) * sizeof(int));
    //get first local matrix
    //matrix_multiplication(m,local,local);

    //get first random number for this rank
    resulting_random_numbers[0] = ( (seed * local[0][0]) + local[1][0]) % Prime;

    //get rest of the numbers
    for(i=1;i<(N/p);i++)
    {
    	resulting_random_numbers[i] = ((resulting_random_numbers[i-1] * a) + b) % Prime;
    }
	printf("\nfrom rank %d , anser  is ",rank);
	for(i=0;i<(N/p);i++)
		{
			printf("\nnumber= %d",resulting_random_numbers[i]);
		}
	printf("\n\n\n");
    struct timeval totalRuntimeStart, totalRuntimeEnd;

    int totalRuntime;


    MPI_Finalize();
}

void matrix_multiplication(int** a , int** b,int** mul)
{
	int i,j,k;
	int** tmp = malloc(2 * (sizeof(int*)));
	tmp[0] = malloc(2 * (sizeof(int)));
	tmp[1] = malloc(2 * (sizeof(int)));
	for(i=0;i<2;i++)
	{
		for(j=0;j<2;j++)
		{
			tmp[i][j] = 0;
			for(k=0;k<2;k++)
				{
				tmp[i][j] = tmp[i][j] + a[i][k] * b[k][j];
				}
		}
	}
	int v,w;

	 mul[0] = malloc(2 * (sizeof(int)));
	 mul[1] = malloc(2 * (sizeof(int)));
	for (v = 0; v < 2; v++) {
		for (w = 0; w < 2; w++) {
			mul[v][w]=tmp[v][w];
		}
	}


	//return tmp;
}
void print_matrix(int** matrix)
{
	int u, v, w;
	//dim = (N/p);
	for (v = 0; v < 2; v++) {
		printf("\n");
		for (w = 0; w < 2; w++) {
			printf(": %d", matrix[v][w]);

		}
		printf("\n");

	}
	printf("\n\n");

}
