section .text                    
        global assFunc 
        str:    db "%d", 10, 0      
       	extern printf
        extern c_checkValidity


assFunc:                       
        push ebp
        mov ebp, esp
        pushad
        mov ebx, dword [ebp+8]		;   get function argument on stack 
        mov ecx, dword [ebp+12]		;    get function argument on stack
        
        push ecx
        push ebx
        call c_checkValidity
        add esp, 8
        
        sub eax , '0'
        cmp eax, 0
        je addLabel    
        sub ebx, ecx
        jmp end
        addLabel:add ebx,ecx
        end: 

        mov eax,ebx
        push eax
	push dword str		; address of ctrl string
        call printf
        add esp, 8

        popad                    	; restore all previously used registers
        mov esp,ebp
        pop ebp				; restore Base Pointer previous value (to returnt to the activation frame of main(...))
        ret				; returns from do_Str(...) function
