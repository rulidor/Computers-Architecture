all: exec

exec: task4.o LineParser.o
	gcc -g -Wall -m32 -o exec task4.o LineParser.o	

task4.o: task4.c
	gcc -g -Wall -m32 -c -o task4.o task4.c	

LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c	
	
.PHONY: clean

clean:
	rm -f exec task4.o LineParser.o
