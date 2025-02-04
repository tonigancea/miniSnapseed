#include "homework.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>

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

void* RotateLeft(image *in, image *out, int num_colors) {

	#pragma omp parallel
	for (int c = 0; c < num_colors; c++) { 	//each color at a time
		for (int i = 0; i < out->height; i++) {
			for (int j = 0; j < out->width; j++) {
				out->data[(i * out->width + j) * num_colors + c] \
				= in->data[((j * in->width) + (in->width - i - 1)) * num_colors + c];
			}
		}     
	}
	return NULL;
}

void* RotateRight(image *in, image *out, int num_colors) {

	#pragma omp parallel
	for (int c = 0; c < num_colors; c++) { 	//each color at a time
		for (int i = 0; i < out->height; i++) {
			for (int j = 0; j < out->width; j++) {
				out->data[(i * out->width + j) * num_colors + c] \
				= in->data[((in->height - j - 1) * in->width + i) * num_colors + c];
			}
		}
	}
	return NULL;
}

void* FlipHor(image *in, image *out, int num_colors) {

	#pragma omp parallel
	for (int c = 0; c < num_colors; c++) { 	//each color at a time
		for (int i = 0; i < out->height; i++) {
			for (int j = 0; j < out->width; j++) {
				out->data[(i * out->width + j) * num_colors + c] \
				= in->data[((in->height - i - 1) * in->width + j) * num_colors + c];
			}
		}
	}
	return NULL;
}

void* FlipVer(image *in, image *out, int num_colors) {

	#pragma omp parallel
	for (int c = 0; c < num_colors; c++) { 	//each color at a time
		for (int i = 0; i < out->height; i++) {
			for (int j = 0; j < out->width; j++) {
				out->data[(i * out->width + j) * num_colors + c] \
				= in->data[(i * in->width + (in->width - j - 1)) * num_colors + c];
			}
		}
	}
	return NULL;
}

void* ResizeWork(image *in, image *out, int num_colors) {
	
	int sum;
	int value;

	for (int c = 0; c < num_colors; c++) { 	//each color at a time
		
		#pragma omp parallel private(value, sum)
		for (int i = 0; i < out->height; i++) {
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

void copyImage(image *source, image *destination) {
	destination->type[0] = source->type[0];
	destination->type[1] = source->type[1];
	destination->width = source->width;
	destination->height = source->height;
	destination->max_val = source->max_val;

	free(destination->data);
	int byte_len = find_num_colors(source->type[1]) * source->width * source->height;
	destination->data = (char*) malloc(byte_len * sizeof(char));
	memcpy(destination->data, source->data, byte_len);
	
	free(source->data);
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

	fclose(fp);
	free(img->data);
}

void rotate(image *in, image * out, int type) {
	int num_colors, byte_length;

	out->type[0] = in->type[0];
	out->type[1] = in->type[1];
	num_colors = find_num_colors(in->type[1]);

	if (type == 0 || type == 1) {
		out->width = in->height;
		out->height = in->width;
	} else {
		out->width = in->width;
		out->height = in->height;
	}
	
	out->max_val = in->max_val;

	byte_length = out->width * out->height * num_colors;

	out->data = (char*) malloc(byte_length * sizeof(char));

	/* 0 means rotate left, 1 means rotate right */
	/* 2 means flip vertical, 3 means flip horizontal */
	if (type == 0) {
		RotateLeft(in, out, num_colors);
	} else if (type == 1){
		RotateRight(in, out, num_colors);
	} else if (type == 2) {
		FlipVer(in, out, num_colors);
	} else {
		FlipHor(in, out, num_colors);
	}
}

void resize(image *in, image * out, int resize_fac) { 
	
	int num_colors, byte_length;

	resize_factor = resize_fac;

	out->type[0] = in->type[0];
	out->type[1] = in->type[1];
	num_colors = find_num_colors(in->type[1]);

	out->width = in->width / resize_factor;
	out->height = in->height / resize_factor;
	out->max_val = in->max_val;

	byte_length = out->width * out->height * num_colors;

	out->data = (char*) malloc(byte_length * sizeof(char));

	ResizeWork(in, out, num_colors);
}