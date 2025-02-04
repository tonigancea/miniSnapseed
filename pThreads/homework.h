#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
	char type[2];
	int width;
	int height;
	int max_val;
	char *data;

}image;

typedef struct {
	image *in;
	image *out;
	int num_colors;
	int start;
	int end;
}blackbox;

void readInput(const char * fileName, image *img);
void writeData(const char * fileName, image *img);
void resize(image *in, image * out, int resize_fac);
void rotate(image *in, image * out, int type);
void copyImage(image *source, image *destination);

#endif /* HOMEWORK_H */