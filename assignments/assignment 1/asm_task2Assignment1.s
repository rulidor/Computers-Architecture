section	.rodata			; we define (global) read-only variables in .rodata section
	format_string: db "%s", 10, 0	; format string

section .bss			; we define (global) uninitialized variables in .bss section
	an: resb 12		; enough to store integer in [-2,147,483,648 (-2^31) : 2,147,483,647 (2^31-1)]

section .text
	global convertor
	extern printf

convertor:
	push ebp
	mov ebp, esp	
	pushad			

	mov ecx, dword [ebp+8]	; get function argument (pointer to string)
	; your code comes here...
	;convert from ascii to decimal

	mov dword [an],0

	mov eax, 0
	start:
		movzx edx, byte [ecx] 
		inc ecx 
		cmp edx, '0'
		jb done
		cmp edx, '9'
		ja start
		sub edx, '0'
		imul eax, 10 
		add eax, edx
		jmp start 
	done:
	
	
	mov ecx, dword an
	Loop:
		mov edx,0
		cmp eax, 0
		je End

		mov ebx, 0x10
		div ebx

		cmp dl , 9
		jle ifNumber

		add dl, 55
		jmp Next

	ifNumber: 
		add dl, '0'

	Next:
		
		mov byte[ecx], dl
		inc ecx
		jmp Loop

	End:

		mov al, 0	
		dec ecx
		mov edx, dword an
	swap:
	    cmp ecx , edx
		je Result
		mov al , [ecx]
		mov bl , [edx]

		mov byte[edx], al
		mov byte[ecx], bl

		mov al,0
		mov bl,0
		dec ecx
	    cmp ecx , edx
		je Result
		inc edx
		cmp ecx , edx
		je Result
		jmp swap

	Result:


	push an		; call printf with 2 arguments -  
	push format_string	; pointer to str and pointer to format string
	call printf
	add esp, 8		; clean up stack after call
	popad			
	mov esp, ebp	
	pop ebp
	ret

