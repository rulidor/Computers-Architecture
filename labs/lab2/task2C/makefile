all: exec

libs: c-lib

c-lib: menu_map.c
	# flags meanings:
	# -g = debug symbols
	# -m32 = compile to x86_32 architecture (32bit)
	# -c = Don't link (i.e., create an object file, not an executable)
	# -o numbers.o = output compiled file to numbers.o
	gcc -g -m32 -c -o menu_map.o menu_map.c

exec: libs
	gcc -g -m32  menu_map.o -o exec
	rm t2c.o

.PHONY: clean
clean:
	rm -rf ./*.o exec
