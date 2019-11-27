#include "homework.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int num_threads;

int main(int argc, char * argv[]) {

	if(argc <= 4) {
		printf("Arguments should look like:\n");
		printf("./homework input_file output_file num_threads_used command1 ...\n");
		printf("The available commands are: resize x, rotate_left, rotate_right, flip_vert, flip_hor.\n");
		exit(-1);
	}

	num_threads = atoi(argv[3]);

	image input;
	image tmp;

	readInput(argv[1], &input);

	for (int i = 4; i < argc; i++) {
		if (strcmp(argv[i],"resize") == 0) {
			int resize_fact = atoi(argv[i+1]);
			resize(&input, &tmp, resize_fact);
			i += 1;
		} else if (strcmp(argv[i],"rotate_left") == 0) {
			// rotate_left()
		}
		copyImage(&tmp, &input);
	}

	writeData(argv[2], &input);
	return 0;
}
