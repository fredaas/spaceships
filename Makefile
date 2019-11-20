CC=gcc
CFLAGS=-std=c99 -Wall -O3 -lm -lGL -lGLU -lglfw -fopenmp
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c,%.o,$(SRC))

all : clean main

main : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ) : %.o : %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean :
	rm -f main *.o