
all:mytbf

mytbf:main.o	mytbf.o
	gcc main.o mytbf.o -o mytbf

main.o:main.c
	gcc -c main.c -o main.o

mytbf.o:mytbf.c	mytbf.h
	gcc -c mytbf.c -o mytbf.o

clean:
	rm -rf *.o mytbf
