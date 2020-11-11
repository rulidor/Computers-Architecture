all: runCounter

runCounter: digitCounter.o
	gcc -g -Wall -m32 -fno-pie -o runCounter digitCounter.o	

digitCounter.o: digitCounter.c
	gcc -g -Wall -m32 -fno-pie -c -o digitCounter.o digitCounter.c	
	
.PHONY: clean

clean:
	rm -f runCounter digitCounter.o
