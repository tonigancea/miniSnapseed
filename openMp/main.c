#include "homework.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]) {

	if(argc <= 3) {
		printf("Arguments should look like:\n");
		printf("./homework input_file output_file_used command1 ...\n");
		printf("The available commands are: resize x, rotate_left, rotate_right, flip_vert, flip_hor.\n");
		exit(-1);
	}

	image input;
	image tmp;

	readInput(argv[1], &input);

	for (int i = 3; i < argc; i++) {
		if (strcmp(argv[i],"resize") == 0) {
			int resize_fact = atoi(argv[i+1]);
			resize(&input, &tmp, resize_fact);
			i += 1;
		} else if (strcmp(argv[i],"rotate_left") == 0) {
			// 0 means rotate left
			rotate(&input, &tmp, 0);
		} else if (strcmp(argv[i],"rotate_right") == 0) {
			// 1 means rotate right
			rotate(&input, &tmp, 1);
		} else if (strcmp(argv[i],"flip_vert") == 0) {
			// 2 means flip vertical
			rotate(&input, &tmp, 2);
		} else if (strcmp(argv[i],"flip_hor") == 0) {
			// 3 means flip horizontal
			rotate(&input, &tmp, 3);
		}
		copyImage(&tmp, &input);
	}

	writeData(argv[2], &input);
	return 0;
}
