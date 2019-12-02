build: homework
homework: homework.c
	mpicc -o homework homework.c -lm -Wall
serial: homework
	mpirun -np 1 homework test.pnm export.pnm
distrib: homework
	mpirun -np 10 homework test.pnm export.pnm
clean:
	rm -f homework
