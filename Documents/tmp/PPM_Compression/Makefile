.phonY: build run clear

all : build run clear

build: main.o tree.o
	gcc -Wall -Werror -Wextra -o main main.o tree.o -lm
	
main.o: main.c tree.h
	gcc -Wall -Werror -Wextra -c main.c -o main.o -lm

tree.o: tree.c tree.h
	gcc -Wall -Werror -Wextra -c tree.c -o tree.o -lm

run:
	./main

clear: 
	rm main main.o tree.o