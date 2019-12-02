#include<mpi.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct {
	char type[2];
	int width;
	int height;
	int max_val;
	char *data;

}image;

int find_num_colors(char type) {
	if (type == '5') {
		return 1;
	} else {
		return 3;
	}
}

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

void initOuput(image *in, image *out) {
	int num_colors, byte_length;

	out->type[0] = in->type[0];
	out->type[1] = in->type[1];
	num_colors = find_num_colors(in->type[1]);

	out->width = in->width;
	out->height = in->height;
	out->max_val = in->max_val;

	byte_length = out->width * out->height * num_colors;

	out->data = (char*) malloc(byte_length * sizeof(char));
	memcpy(out->data, in->data, byte_length);
}

void resize(image *in, image *out, int resize_fact, int start, int end, int newW, int newH) {
	int num_colors, byte_length;

	num_colors = find_num_colors(in->type[1]);
	out->type[0] = in->type[0];
	out->type[1] = in->type[1];

	//old output is the new input
	byte_length = out->width * out->height * num_colors;
	free(in->data);
	in->data = (char*) malloc (byte_length * sizeof(char));
	memcpy(in->data, out->data, byte_length);

	//compute the new output
	out->width = newW;
	out->height = newH;
	out->max_val = in->max_val;

	byte_length = out->width * out->height * num_colors;
	
	free(out->data);
	out->data = (char*) malloc(byte_length * sizeof(char));

	int sum;
	int value;

	for (int c = 0; c < num_colors; c++) { 	//each color at a time
		for (int i = start; i < end; i++) {
			for (int j = 0; j < out->width; j++) {
				
				if (resize_fact == 3) {
					int matrix[3][3];
					
					for (int k = 0; k < 3; k++) {
						for (int p = 0; p < 3; p++) {
						
							matrix[k][p] = (unsigned char)in->data[
								   ((( i*resize_fact + k )*in->width) + (j*resize_fact + p) ) * num_colors + c];
						}
					}
					value = computeKernel(matrix);
				} else {
					value = 0;
					sum = 0;
					for (int k = 0; k < resize_fact; k++) {
						for (int p = 0; p < resize_fact; p++) {
						
							int aux = in->data[
								   ((( i*resize_fact + k )*in->width) + (j*resize_fact + p) ) * num_colors + c];
							sum += (unsigned char)aux;
						}
					}
					value = sum / (resize_fact * resize_fact);
				}
				out->data[(i * out->width + j) * num_colors + c] = (unsigned char)value;
			}
		}
	}
	in->width = out->width;
	in->height = out->height;
}

void rotate(image *in, image *out, int type, int start, int end) {
	int num_colors, byte_length;
	num_colors = find_num_colors(in->type[1]);
	byte_length = out->width * out->height * num_colors;
	
	//old output is the new input
	memcpy(in->data, out->data, byte_length);

	if (type == 0) {
		// rotate left
		out->width = in->height;
		out->height = in->width;

		for (int c = 0; c < num_colors; c++) { // for each color
			for (int i = start; i < end; i++) { 
				for (int j = 0; j < out->width; j++) {
						// printf("\n\n\nHeey! %d %d %d\n\n\n",c,i,j);
						out->data[(i * out->width + j) * num_colors + c] \
							= in->data[((j * in->width) + (in->width - i - 1)) * num_colors + c];
				}
			}
		}
	} else if (type == 1) {
		// rotate right
		out->width = in->height;
		out->height = in->width;

		for (int c = 0; c < num_colors; c++) { // for each color
			for (int i = start; i < end; i++) { 
				for (int j = 0; j < out->width; j++) {
						out->data[(i * out->width + j) * num_colors + c] \
							= in->data[((in->height - j - 1) * in->width + i) * num_colors + c];
				}
			}
		}
	} else if (type == 2) {
		// flip vertical
		for (int c = 0; c < num_colors; c++) { // for each color
			for (int i = start; i < end; i++) { 
				for (int j = 0; j < in->width; j++) {
						out->data[(i * out->width + j) * num_colors + c] \
							= in->data[(i * in->width + (in->width - j - 1)) * num_colors + c];
				}
			}
		}
 	} else if (type == 3) {
		 // flip horizontal
		for (int c = 0; c < num_colors; c++) { // for each color
			for (int i = start; i < end; i++) { 
				for (int j = 0; j < in->width; j++) {
						out->data[(i * out->width + j) * num_colors + c] \
							= in->data[((in->height - i - 1) * in->width + j) * num_colors + c];
				}
			}
		}	 
	}
	in->width = out->width;
	in->height = out->height;
	
}

int main(int argc, char * argv[]) {
	
	//agrv[1] input
	//argv[2] output
	//argv[3] command1
	//argv[4] command2
	//...

	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

	image input;
	image output;
	
	for (int i = 3; i < argc; i++) {

		int start, end;
		int byte_length;

		if (i != 3 && rank != nProcesses - 1) {
			MPI_Recv(&byte_length, 1, MPI_INT, nProcesses - 1, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(output.data, byte_length, MPI_CHAR, nProcesses - 1, 0, MPI_COMM_WORLD, &status);
		}

		// if this is the first filter
		// and if this is rank 0
		// then we read from file
		if (i ==  3 && rank == 0) {
			readInput(argv[1], &input);
			initOuput(&input, &output);

			// send the input to all other proceses
			for (int r = 1; r < nProcesses; r++) {
				MPI_Send(&(input.type), 2, MPI_CHAR, r, 0, MPI_COMM_WORLD);
				MPI_Send(&(input.width), 1, MPI_INT, r, 0, MPI_COMM_WORLD);
				MPI_Send(&(input.height), 1, MPI_INT, r, 0, MPI_COMM_WORLD);
				MPI_Send(&(input.max_val), 1, MPI_INT, r, 0, MPI_COMM_WORLD);
				
				int num_colors = find_num_colors(output.type[1]);
				byte_length = num_colors * output.width * output.height;
				
				MPI_Send(&byte_length, 1, MPI_INT, r, 0, MPI_COMM_WORLD);
				MPI_Send(input.data, byte_length, MPI_CHAR, r, 0, MPI_COMM_WORLD);
			}
		}

		if (rank != 0) {
			if (i == 3) {
				MPI_Recv(&(input.type), 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(&(input.width), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(&(input.height), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(&(input.max_val), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(&byte_length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

				//alloc the input and output only at the first filter
				input.data = (char*) malloc(byte_length * sizeof(char));
				
				output.data = (char*) malloc(byte_length * sizeof(char));
				output.type[0] = input.type[0];
				output.type[1] = input.type[1];
				output.width = input.width;
				output.height = input.height;
				output.max_val = input.max_val;

				MPI_Recv(input.data, byte_length, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

				// output needs to have the margins of the orginal input
				// at the first filter
				memcpy(output.data, input.data, byte_length);
			}
		}

		// compute your slice
		start = rank * input.height / nProcesses;
		if (rank == nProcesses - 1) {
			end = input.height;
		} else {
			end = (rank + 1) * input.height / nProcesses;
		}

		// actual COMPUTATION
		if (strcmp(argv[i],"resize") == 0) {
			int resize_fact = atoi(argv[i+1]);
			
			// compute newW and newH
			int newW, newH;
			newW = input.width / resize_fact;
			newH = input.height / resize_fact;
			
			// recalibrare start & end
			start = rank * newH / nProcesses;
			if (rank == nProcesses - 1) {
				end = newH;
			} else {
				end = (rank + 1) * newH / nProcesses; 
			}

			resize(&input, &output, resize_fact, start, end, newW, newH);
			i += 1;

		} else if (strcmp(argv[i],"rotate_left") == 0) {
			
			// recalibrate start & end
			start = rank * input.width / nProcesses;
			if (rank == nProcesses - 1) {
				end = input.width;
			} else {
				end = (rank + 1) * input.width / nProcesses;
			}

			// 0 means rotate left
			rotate(&input, &output, 0, start, end);
			
		} else if (strcmp(argv[i],"rotate_right") == 0) {

			// recalibrate start & end
			start = rank * input.width / nProcesses;
			if (rank == nProcesses - 1) {
				end = input.width;
			} else {
				end = (rank + 1) * input.width / nProcesses;
			}

			// 1 means rotate right
			rotate(&input, &output, 1, start, end);

		} else if (strcmp(argv[i],"flip_vert") == 0) {
			// 2 means flip vertical
			rotate(&input, &output, 2, start, end);
		} else if (strcmp(argv[i],"flip_hor") == 0) {
			// 3 means flip horizontal
			rotate(&input, &output, 3, start, end);
		}
		// end of actual COMPUTATION

		if (rank != nProcesses - 1) {
			// send your slice to Prince Charming (np - 1)
			int num_colors = find_num_colors(output.type[1]);
			int quantity = (end - start) * output.width * num_colors;
			int offset = start * output.width * num_colors;
			
			MPI_Send(&quantity, 1, MPI_INT, nProcesses - 1, 0, MPI_COMM_WORLD);
			MPI_Send(&offset, 1, MPI_INT, nProcesses - 1, 0, MPI_COMM_WORLD);
			MPI_Send(output.data + offset, quantity, MPI_CHAR, nProcesses - 1, 0, MPI_COMM_WORLD);
		}
		
		if (rank == nProcesses - 1) {

			for (int r = 0; r < nProcesses - 1; r++) { //recv from others without nP-1
				//recieve each slice
				int quantity, offset;
				MPI_Recv(&quantity, 1, MPI_INT, r, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(&offset, 1, MPI_INT, r, 0, MPI_COMM_WORLD, &status);
				MPI_Recv(output.data + offset, quantity, MPI_CHAR, r, 0, MPI_COMM_WORLD, &status);
			}

			if (i == argc - 1) {
				free(input.data);
				//writeData
				writeData(argv[2], &output);
				
			} else {
				//send the output to all other processes
				for (int r = 0; r < nProcesses - 1; r++) {
					int num_colors = find_num_colors(output.type[1]);
					byte_length = num_colors * output.width * output.height;
					
					MPI_Send(&byte_length, 1, MPI_INT, r, 0, MPI_COMM_WORLD);
					MPI_Send(output.data, byte_length, MPI_CHAR, r, 0, MPI_COMM_WORLD);
				}		
			}	
		}
	}

	MPI_Finalize();
	return 0;
}