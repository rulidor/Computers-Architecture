;****preliminary***
;The overall idea is:
;printer, target and scheduler co routines: will be held in 3 co routines: PRINTER_CO_ROUTINE, TARGET_CO_ROUTINE and SCHEDULER_CO_ROUTINE, accordingly.
;                                           each of them will have its code and its stack.
;drone co routines will be held in the array dronesCOArray. ech of these routines will have its code and its stack.
;drone structs: will be held in the array croarry, with these offsets: coord_x_drone, coord_y_drone, speeds_drone, headings_drone, scores_drone, active_drone (used as: isActive)
;               each drone struct size is 24 bytes.
;
;
;
;
 
    

    
    
    global coord_x_drone
    global coord_y_drone
    global speeds_drone
    global headings_drone
    global scores_drone
    global active_drone
    global size_drone_struct ;used as: isActive
    global DRONE_ROUTINE_STRUCT_SIZE
    global formatFloat
    global formatInt
    global max_value
    global randomize


section .rodata

    extern PRINTER_ROUTINE_CODE
    extern TARGET_ROUTINE_CODE
    extern SCHEDULER_ROUTINE_CODE
    
    extern DRONE_ROUTINE_CODE

    ;drone struct offset
    coord_x_drone: equ 0
    coord_y_drone: equ 4
    speeds_drone: equ 8
    headings_drone: equ 12
    scores_drone: equ 16
    active_drone: equ 20 ;used as: isActive
    size_drone_struct: equ 24

    ;drone co routine struct offset
    DRONE_ROUTINE_STACK: equ 4
    DRONE_ROUTINE_STRUCT_SIZE: equ 8


    ;co routine struct offset
    ROUTINE_CODE: equ 0
    ROUTINE_STACK: equ 4
    SIZE_ROUTINE_STRUCT: equ 8

    formatInt: db "%d", 0, 0
    formatFloat: db "%.2f", 0, 0
    formatString: db "%s", 10, 0

    max_value: dd 65535

    STACK_SIZE_CO_ROUTINE: equ 16*1024



section .bss
    global drones
    global cycles
    global steps
    global distance
    global seed
    global seedc
    global seedn
    extern coord_x_target
    extern coord_y_target
    global croarry
    global dronesCOArray



    drones: resd 1 ;int
    cycles: resd 1 ;int
    steps: resd 1 ;int
    distance: resd 1 ;float
    seed: resd 1 ;int

    croarry: resd 1 ;coord_x, coord_y, speed, angle, score


    dronesCOArray: resd 1 ; Pointer to array of the drones' co-routine structs



    
    ;stack pointers
    curr_co_p: resd 1 ;Points to current co routine
    stack_p_temp: resd 1
    stack_p_ass3: resd 1

    ;stacks for co routines
    PRINTER_STACK: resb STACK_SIZE_CO_ROUTINE
    TARGET_STACK: resb STACK_SIZE_CO_ROUTINE
    SCHEDULER_STACK: resb STACK_SIZE_CO_ROUTINE

section .data
    global PRINTER_CO_ROUTINE
    global SCHEDULER_CO_ROUTINE
    global TARGET_CO_ROUTINE
    global currDrone
    global rightLimit
    global distanceToLeftLimit
    global random_num

	seedc: db 10, 'seed Changes:', 0
	seedn: db 10, 'seed Now is:', 0
    badArgs: db 'Error: Not enough args', 10, 0


    
    SCHEDULER_CO_ROUTINE: dd SCHEDULER_ROUTINE_CODE
                              dd SCHEDULER_STACK + STACK_SIZE_CO_ROUTINE
    TARGET_CO_ROUTINE: dd TARGET_ROUTINE_CODE
                              dd TARGET_STACK + STACK_SIZE_CO_ROUTINE
    PRINTER_CO_ROUTINE: dd PRINTER_ROUTINE_CODE
                              dd PRINTER_STACK + STACK_SIZE_CO_ROUTINE
    
    rightLimit: dd 100
    distanceToLeftLimit: dd 0
    random_num: dd 0
    currDrone: dd 0


section .text
    align 16
    extern free
    extern printf
    extern sscanf
    extern malloc  
    extern createTarget

    global main
    extern generateRandomAndScale
    global scheduler_finish
    global resume


%macro boardinitMea 0
	cmp dword[seed],100
	jb %%end2
	pusha
    	fld dword[seed]
    	fld dword[max_value] 
    	fdivp                  
    	fstp dword[ans]
	popa
	pusha
	fld dword[ans]
    	fld dword[mea]   
	fmulp
	fstp dword[ans2]
	popa
	jmp %%end22
	%%end2:
	fild dword[seed]
	fstp dword[ans2]
	%%end22:
	mov edi,edi
%endmacro

%macro makeitFix2 0
	mov ebx, dword[currDrone]
	mov eax,[4*ebx + croarry]
%endmacro

%macro makeitFix 0
	mov ecx, dword[ans]
	mov ebx, dword[currDrone]
	mov eax,[4*ebx + croarry]
%endmacro

%macro printInt 1
	pusha
	push %1
	push formatInt
	call printf
	add esp, 8
	popa
%endmacro


%macro seedChange 0
	printw seedc
	printInt dword[seed]
%endmacro

%macro seedNow 0
	printw seedn
	printInt dword[seed]
%endmacro


;1st param: count of elements to be allocated, 2nd param: size for every element. 3rd param: variable to point to the allocation
%macro allocate 3
    pushad
    push dword %3
    push dword %2
    push dword %1
    pop eax
    pop ebx
    mul ebx
    push eax
    call malloc
    add esp, 4
    pop ebx
    mov [ebx], eax
    popad
%endmacro


;1st param: format. 2nd param: variable to put in
%macro sscanfMacro 2
    add ebx, 4
    mov edx, [ebx]
    pushad
    push %2
    push %1
    push edx
    call sscanf
    add esp, 12
    popad
%endmacro
    

%macro start 0
	push ebp
	mov ebp, esp
	pushad
%endmacro

%macro printw 1
	pusha
	push %1
	call printf
	add esp, 4
	popa
%endmacro



;1st param: variable which points to the memory to be freed
%macro call_free 1
    pushad
    push %1
    call free
    add esp, 4
    popad
%endmacro



%macro random 0
	pusha
	mov eax, dword[seed]
	mov edx, 0
	%%loopRandom:
	mov dword[seed],eax 
	cmp edx, 15
	je %%end
	mov ebx, 1
	and ebx, eax
	mov ecx, 4
	and ecx, eax
	xor ebx,ecx
	mov ecx,8
	and ecx,eax
	xor ebx,ecx
	mov ecx,16
	and ecx,eax
	xor ebx,ecx
	shr eax,1
	inc edx 
	cmp ebx,1
	jne %%loopRandom
	or eax, 0x8000
	jmp %%loopRandom
	%%end:
	mov dword[seed], eax
	mov edi,0
	popa
%endmacro


;generates a random number in the range from 0 to the 1st param. 2nd param: the var. to set the new random number to
%macro randomToLimit 2
    push eax
    mov dword [rightLimit], %1
    mov dword [distanceToLeftLimit], 0
    call generateRandomAndScale
    mov eax, [random_num]
    mov %2, eax
    pop eax
%endmacro

main:

    ;reading from agrv sscanf 
	;================================================================
    mov ebp, esp
        
    mov ecx, [esp+4] ;ecx holds argc
    cmp ecx, 6 ;check if not enough parameters entered
	jb error
    mov ebx, [esp+8] ;ebx holds argv

    ;reading args
    sscanfMacro formatInt, drones
    sscanfMacro formatInt, cycles
    sscanfMacro formatInt, steps
    sscanfMacro formatFloat, distance
    sscanfMacro formatInt, seed

    ;allocating memory:

    ;dronesCOArray mem. allocation
    allocateDronesCoRoutines:
        allocate [drones], DRONE_ROUTINE_STRUCT_SIZE, dronesCOArray
        mov ecx, 0
        mov ebx, [dronesCOArray]
        

        loop_allocate_CO:
            mov dword [ebx], DRONE_ROUTINE_CODE
            lea edx, [ebx + DRONE_ROUTINE_STACK]
            ;memory allocation for stack:
            allocate 1, STACK_SIZE_CO_ROUTINE, edx
            mov edx, [ebx + DRONE_ROUTINE_STACK]
            add edx, STACK_SIZE_CO_ROUTINE
            mov [ebx + ROUTINE_STACK], edx
            call co_init
            
            add ebx, DRONE_ROUTINE_STRUCT_SIZE
            inc ecx
            cmp ecx, [drones]
            jne loop_allocate_CO


    ;croarry mem. allocation
    allocate [drones], size_drone_struct, croarry
        

    ;initializing croarry
    mov ecx, 0
    mov ebx, [croarry]

    loop_croarry_init:
    mov dword [ebx + active_drone], 1 ;used as: isActive
    randomToLimit 100, [ebx + coord_x_drone]
    randomToLimit 100, [ebx + coord_y_drone]
    randomToLimit 360, [ebx + headings_drone]
    randomToLimit 100, [ebx + speeds_drone]
    mov dword [ebx + scores_drone], 0
    add ebx, size_drone_struct
    inc ecx
    cmp ecx, [drones]
    jne loop_croarry_init


    ;initializing printer, target and scheduler
    mov ebx, PRINTER_CO_ROUTINE
    call co_init

    call createTarget
    mov ebx, TARGET_CO_ROUTINE
    call co_init

    mov ebx, SCHEDULER_CO_ROUTINE
    call co_init

    ;starting scheduler
        mov dword [currDrone], 0
        pushad
        mov [stack_p_ass3], esp
        mov ebx, SCHEDULER_CO_ROUTINE
        jmp do_resume
        
    scheduler_finish: ;includes ESP + REGS restoration
        mov esp, [stack_p_ass3]
        popad
    

    call_free dword [croarry]
    call_free dword [dronesCOArray]

    ;program ending
    mov esp, ebp
    ret



;***functions:***

; Assuming ebx is pointer to the co-routine struct
; we will initialize every co routine stack to keep: return address, flags and regs.
co_init:
    pushad
    mov eax, [ebx + ROUTINE_CODE] ;get initial IP
    mov [stack_p_temp], esp ;saving stack pointer (ESP) so that it'll be possible to restore it later
    mov esp, [ebx + ROUTINE_STACK] ;get initial SP (move into the routine stack)
    mov ebp, esp
    push eax ; Push the return address into routine stack
    pushfd ;push flags into routine stack
    pushad ;push regs into routine stack
    mov [ebx + ROUTINE_STACK], esp ;save new SP
    mov esp, [stack_p_temp] ;restore old SP
    popad
    ret

;used when we want to move from one co routine to another.
;assuming ebx points to the co routine which we want to do resume for
resume:
    pushfd ;save state of caller
    pushad
    mov edx, [curr_co_p]
    mov [edx + ROUTINE_STACK], esp ;save current SP


;uses ebx which points to co routine needed to be resumed, changing the resumed co routine to its last state and returns to resumed co routine
do_resume: ;push ret address of 1st routine into stack of 2nd routine
    mov esp, [ebx + ROUTINE_STACK] ;load SP (resumed co) - meaning: ESP now points to head of stack of the co routine which we need to return to.
    mov [curr_co_p], ebx ;updating current co routine
    popad ;restore resumed co routine state
    popfd
    ret ;"return" to resumed co routine


error:
	printw badArgs
	jmp quit

quit:
	;q0:popad
	;q1:mov esp, ebp
	;q2:pop ebp
	q3:ret