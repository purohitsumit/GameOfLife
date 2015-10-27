/*
 * random_number_serial.c
 *
 *  Created on: Oct 25, 2015
 *      Author: puro755
 */
#include <sys/time.h>
#include<stdlib.h>
#include<stdio.h>
int main(int argc,char *argv[])
{
    int rank, p, N, a,b,Prime,seed;

//    N = 16;
    N = 1000000000;
    a = 3;
    b = 5;
    seed = 10;
    Prime = 93563;
    
    struct timeval timeStart, timeEnd;

    gettimeofday(&timeStart, NULL);

    int* random_numbers = malloc(N * sizeof(int));
    random_numbers[0] = (seed * a +b) % Prime;
    int i;
    for(i=1;i<N;i++)
    {
        random_numbers[i] = (random_numbers[i-1] * a +b) % Prime;
    }

/*    printf("random numbers are");
    for(i=0;i<N;i++)
        {
            printf("\n %d",random_numbers[i] );
        }
*/
    gettimeofday(&timeEnd, NULL);

    int tSend;
    tSend = (timeEnd.tv_sec-timeStart.tv_sec)*1000 + (timeEnd.tv_usec-timeStart.tv_usec)/1000;
    printf("TOTAL RUNTIME=%d/n", tSend);

}
