all: exec

exec: task1.o LineParser.o
	gcc -g -Wall -m32 -o exec task1.o LineParser.o	

task1.o: task1.c
	gcc -g -Wall -m32 -c -o task1.o task1.c	

LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c	
	
.PHONY: clean

clean:
	rm -f exec task1.o LineParser.o