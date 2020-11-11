all: exec

libs: c-lib

c-lib: t1d.c
	# flags meanings:
	# -g = debug symbols
	# -m32 = compile to x86_32 architecture (32bit)
	# -c = Don't link (i.e., create an object file, not an executable)
	# -o numbers.o = output compiled file to numbers.o
	gcc -g -m32 -c -o t1d.o t1d.c

exec: libs
	gcc -g -m32  t1d.o -o exec
	rm t1d.o

.PHONY: clean
clean:
	rm -rf ./*.o exec
