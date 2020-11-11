all: exec

exec: asmAssignment0.o
	nasm -g -f elf -o exec asmAssignment0.o
	
asmAssignment0.o: asmAssignment0.s
	nasm -g -f elf -o asmAssignment0.o asmAssignment0.s
	
.PHONY: clean

clean:
	rm -f exec
	rm -f asmAssignment0.o