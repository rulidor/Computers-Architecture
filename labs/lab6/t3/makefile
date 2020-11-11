all: exec


exec: task3.o
	gcc -g -Wall -m32 -o exec task3.o
	
task3.o: task3.c
	gcc -g -Wall -m32 -c -o task3.o task3.c	

.PHONY: clean

clean:
	rm -f exec task3.o
