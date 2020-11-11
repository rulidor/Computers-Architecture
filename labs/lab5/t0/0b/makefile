all: looper

looper: looper.o
	gcc -g -Wall -m32 -o looper looper.o

looper.o: looper.c
	gcc -g -Wall -m32 -c -o looper.o looper.c	
	
.PHONY: clean

clean:
	rm -f looper looper.o