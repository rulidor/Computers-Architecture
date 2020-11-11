all: myELF

myELF: task1.o
	gcc -g -Wall -m32 -o myELF task1.o	

task1.o: task1.c
	gcc -g -Wall -m32 -c -o task1.o task1.c	
	
.PHONY: clean

clean:
	rm -f myELF task1.o
