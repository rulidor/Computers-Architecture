all: encoder

libs: c-lib

c-lib: getString.c
	# flags meanings:
	# -g = debug symbols
	# -m32 = compile to x86_32 architecture (32bit)
	# -c = Don't link (i.e., create an object file, not an executable)
	# -o numbers.o = output compiled file to numbers.o
	gcc -g -m32 -c -o getString.o getString.c

encoder: encoder.c libs
	gcc -g -m32 -c -o encoder.o encoder.c
	gcc -g -m32  encoder.o getString.o -o encoder
	rm encoder.o getString.o

.PHONY: clean
clean:
	rm -rf ./*.o encoder
