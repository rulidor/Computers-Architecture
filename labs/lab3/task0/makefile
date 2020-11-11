all: exec

libs: c-lib

c-lib: lab3_bubblesort.c
	# flags meanings:
	# -g = debug symbols
	# -m32 = compile to x86_32 architecture (32bit)
	# -c = Don't link (i.e., create an object file, not an executable)
	# -o numbers.o = output compiled file to numbers.o
	gcc -g -m32 -c -o lab3_bubblesort.o lab3_bubblesort.c

exec: libs
	gcc -g -m32  lab3_bubblesort.o -o exec
	rm lab3_bubblesort.o

.PHONY: clean
clean:
	rm -rf ./*.o exec
