all: exec

exec: hexeditplus.o
	gcc -g -Wall -m32 -o exec hexeditplus.o	

hexeditplus.o: hexeditplus.c
	gcc -g -Wall -m32 -c -o hexeditplus.o hexeditplus.c	
	
.PHONY: clean

clean:
	rm -f exec hexeditplus.o
