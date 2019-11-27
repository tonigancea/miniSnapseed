#include "homework.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

int num_threads;
int resize_factor;

int computeKernel(int mat[3][3]) {
	int GaussianKernel[3][3] = {{1, 2, 1},
								{2, 4, 2},
								{1, 2, 1}};
	int multiplicationResult[3][3] = { 0 };
	int sum = 0;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
				multiplicationResult[i][j]
				+= mat[i][j]*GaussianKernel[i][j];
				sum += multiplicationResult[i][j];
		}
	}
	return sum / 16;

}

void* threadFunction(void *var) {
	blackbox info = *(blackbox*)var;

	int start = info.start;
	int end = info.end;
	image *in = info.in;
	image *out = info.out;
	int num_colors = info.num_colors;

	int sum;
	int value;

	for (int c = 0; c < num_colors; c++) { 	//each color at a time
		for (int i = start; i < end; i++) {
			for (int j = 0; j < out->width; j++) {
				
				if (resize_factor == 3) {
					int matrix[3][3];
					
					for (int k = 0; k < 3; k++) {
						for (int p = 0; p < 3; p++) {
						
							matrix[k][p] = (unsigned char)in->data[
								   ((( i*resize_factor + k )*in->width) + (j*resize_factor + p) ) * num_colors + c];
						}
					}
					value = computeKernel(matrix);
				} else {
					value = 0;
					sum = 0;
					for (int k = 0; k < resize_factor; k++) {
						for (int p = 0; p < resize_factor; p++) {
						
							int aux = in->data[
								   ((( i*resize_factor + k )*in->width) + (j*resize_factor + p) ) * num_colors + c];
							sum += (unsigned char)aux;
						}
					}
					value = sum / (resize_factor * resize_factor);
				}
				out->data[(i * out->width + j) * num_colors + c] = (unsigned char)value;
			}
		}
	}
	return NULL;
}

int find_num_colors(char type) {
	if (type == '5') {
		return 1;
	} else {
		return 3;
	}
}

void readInput(const char * fileName, image *img) {
	
	FILE* fp;
	int num_colors;
	int byte_length;

	fp = fopen(fileName, "rb");
	// should check for errors

	fscanf(fp,"%s",img->type);
	num_colors = find_num_colors(img->type[1]);

	fscanf(fp,"%d",&img->width);
	fscanf(fp,"%d",&img->height);
	fscanf(fp,"%d",&img->max_val);
	
	fscanf(fp, " "); // skip that last /n

	byte_length = img->width * img->height * num_colors;
	img->data = (char*) malloc(byte_length * sizeof(char));
	fread(img->data, sizeof(char), byte_length, fp);

	fclose(fp);
}

void writeData(const char * fileName, image *img) {

	FILE* fp;
	int num_colors, byte_length;

	fp = fopen(fileName, "wb");

	fprintf(fp,"%c%c\n", img->type[0], img->type[1]);
	num_colors = find_num_colors(img->type[1]);

	fprintf(fp, "%d %d\n", img->width, img->height);
	fprintf(fp, "%d\n", img->max_val);

	byte_length = img->width * img->height * num_colors;
	fwrite(img->data, byte_length, sizeof(char), fp);

	// FREE MEMORY FROM IMAGE OUT
	free(img->data);

	fclose(fp);

}

void resize(image *in, image * out) { 
	
	int num_colors, byte_length;

	out->type[0] = in->type[0];
	out->type[1] = in->type[1];
	num_colors = find_num_colors(in->type[1]);

	out->width = in->width / resize_factor;
	out->height = in->height / resize_factor;
	out->max_val = in->max_val;

	byte_length = out->width * out->height * num_colors;

	out->data = (char*) malloc(byte_length * sizeof(char));

	// shall the magic begin
	pthread_t tid[num_threads];
	blackbox info[num_threads];

	for (int i = 0; i < num_threads; i++) {
		info[i].start = i * out->height / num_threads;
		info[i].in = in;
		info[i].out = out;
		info[i].num_colors = num_colors;
		if (i == num_threads - 1) {
			info[i].end = out->height;
		} else {
			info[i].end = ((i + 1) * out->height / num_threads);
		}
		pthread_create(&(tid[i]), NULL, threadFunction, &(info[i]));
	}

	for (int i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
	// the magic ends here

	// FREE MEMORY FROM IMAGE IN
	free(in->data);
}