all: random
		
random:
		mpicc -g -o exe main.c -lm
run:
		mpiexec -machinefile ../machinefile.txt -n 16 ./exe
