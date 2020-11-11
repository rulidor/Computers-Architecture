all: exec

exec: myshell.o LineParser.o
	gcc -g -Wall -m32 -o exec myshell.o LineParser.o	

myshell.o: myshell.c
	gcc -g -Wall -m32 -c -o myshell.o myshell.c	

LineParser.o: LineParser.c
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c	
	
.PHONY: clean

clean:
	rm -f exec myshell.o LineParser.o