/*
 * random_number_serial.c
 *
 *  Created on: Oct 25, 2015
 *      Author: puro755
 */


int main(int argc,char *argv[])
{
	int rank, p, N, a,b,Prime,seed;

    N = 16;

    a = 3;
    b = 5;
    seed = 10;
    Prime = 93563;

    int* random_numbers = malloc(N * sizeof(int));
    random_numbers[0] = (seed * a +b) % Prime;
    int i;
    for(i=1;i<N;i++)
    {
    	random_numbers[i] = (random_numbers[i-1] * a +b) % Prime;
    }

    printf("random numbers are");
    for(i=0;i<N;i++)
        {
        	printf("\n %d",random_numbers[i] );
        }
}
