all: myELF

myELF: task3.o
	gcc -g -Wall -m32 -o myELF task3.o	

task3.o: task3.c
	gcc -g -Wall -m32 -c -o task3.o task3.c	
	
.PHONY: clean

clean:
	rm -f myELF task3.o
