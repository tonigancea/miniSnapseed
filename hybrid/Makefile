build: homework
homework: homework.c
	mpicc -o homework homework.c -lm -Wall -g -fopenmp
serial: homework
	mpirun -np 1 homework test.pnm export.pnm resize 3 rotate_right rotate_left flip_vert flip_vert
distrib: homework
	mpirun -np 10 homework test.pnm export.pnm resize 3 rotate_right rotate_left flip_vert flip_vert
clean:
	rm -f homework
