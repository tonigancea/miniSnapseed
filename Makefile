all: homework

homework: homework.c homework.h main.c
	gcc -o homework homework.c main.c -lpthread -Wall -lm -g

.PHONY: clean

clean:
	rm -f homework
