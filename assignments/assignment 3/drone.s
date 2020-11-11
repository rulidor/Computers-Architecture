section .rodata

    formatInt: db "%d", 0, 0
    num_360: dd 360
    num_180: dd 180
    num_100: dd 100
    num_0: dd 0

    ;drone struct offset
    coor_x_drone: equ 0
    coord_y_drone: equ 4
    speeds_drone: equ 8
    headings_drone: equ 12
    scores_drone: equ 16
    active_drone: equ 20
    size_drone_struct: equ 24

section .text
    extern coord_x_target
    extern coord_y_target
    
    extern SCHEDULER_CO_ROUTINE
    extern TARGET_CO_ROUTINE
    extern distance
    extern croarry
    extern currDrone


    extern distanceToLeftLimit
    extern printf
    extern random_num
    extern rightLimit
    global generateRandomAndScale
    global DRONE_ROUTINE_CODE
    extern resume
    extern max_value
    extern seed
    extern seedc
    extern seedn


%macro seedChange 0
	printw seedc
	printInt dword[seed]
%endmacro

%macro seedNow 0
	printw seedn
	printInt dword[seed]
%endmacro

%macro printw 1
	pusha
	push %1
	call printf
	add esp, 4
	popa
%endmacro

%macro printInt 1
	pusha
	push %1
	push formatInt
	call printf
	add esp, 8
	popa
%endmacro


%macro random 0
    ;seedNow
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
    ;seedChange
%endmacro


%macro changeDroneCoordinate 1
    fld dword [edx + speeds_drone]
    fmulp
    fld dword [edx + %1]
    faddp
    fild dword [num_100]
    ;check if number satisfies: number>100
    fcomi
    ja %%isLessThan0  ; jump if number<100
    fsubp st1, st0 ; number-=100
    jmp %%finish
    %%isLessThan0:
    fisub dword [num_100] ;change sign
    fcomip
    jb %%finish ;jump if number>0
    fiadd dword [num_100] ;number+=100
    %%finish:
    fstp dword [edx + %1]
%endmacro


DRONE_ROUTINE_CODE:
    call getActiveDroneAdress ; put the adrs of active drone struct in eax
    mov edx, eax ;put the adrs in edx
    
    call changePosition
    call changeDroneHeading
    call changeDroneSpeed

    loop:
    call mayDestroy
    cmp eax, 0
    je make_step
    cmp dword [edx + active_drone], 0
    je make_step
    ;case drone can destroy target - increment its score + call target routine and resume
    inc dword [edx + scores_drone]
    mov ebx, TARGET_CO_ROUTINE
    call resume
    make_step:
    call changePosition
    call changeDroneHeading
    call changeDroneSpeed
    mov ebx, SCHEDULER_CO_ROUTINE
    call resume
    jmp loop


;**functions:**

changePosition:
    finit
    fld dword [edx + headings_drone]
    ;converting to radian: according to the formula: x[degree] × π/180 = y[rad]
    fldpi
    fmulp
    fild dword [num_180]
    fdivp
    fsincos
    changeDroneCoordinate coor_x_drone
    changeDroneCoordinate coord_y_drone
    ffree
    ret

changeDroneHeading:
    mov dword [rightLimit], 120
    mov dword [distanceToLeftLimit], 60
    call generateRandomAndScale ;will set a number in range [-60, 60], into the variable: random_number
    finit
    fld dword [random_num]
    fadd dword [edx + headings_drone]
    fild dword [num_360]
    fcomip
    ja isHeadgindLessThan0 ;jump if updated heading<360
    fisub dword [num_360]
    jmp goodHeading
    
    isHeadgindLessThan0:
    fild dword [num_0]
    fcomip
    jb goodHeading ;jump if updated heading>0
    fiadd dword [num_360]
    
    goodHeading:
    fstp dword [edx + headings_drone]
    ffree
    ret


;updates eax to point the currect active drone struct
getActiveDroneAdress:
    push ebx
    mov eax, [currDrone]
    mov ebx, size_drone_struct
    mul ebx
    add eax, [croarry] 
    pop ebx
    ret



; Returns 1 in eax if may destroy, else 0
mayDestroy:
    mov eax, 0
    finit
    fld dword [edx + coor_x_drone]
    fsub dword [coord_x_target] ;put in st0: (coor_x_drone - target_coord_X)
    fmul st0, st0

    fld dword [edx + coord_y_drone]
    fsub dword [coord_y_target] ;put in st0: (coor_y_drone - target_coord_y)
    fmul st0, st0

    faddp
    fsqrt
    fld dword [distance] ;ditance is the max distance to destroy target
    fcomi
    jb finishDestroyCheck ; jump if max distance to destroy > current drone's distance to target
    mov eax, 1

    finishDestroyCheck:
    ffree
ret


changeDroneSpeed:
    mov dword [rightLimit], 20
    mov dword [distanceToLeftLimit], 10
    call generateRandomAndScale ;will set a number in range [-10, 10], in the variable: random_number
    finit
    fld dword [random_num]
    fadd dword [edx + speeds_drone]
    fild dword [num_100]
    fcomip
    ja isSpeedLessThan0 ; jump if updated speed<100
    fild dword [num_100] ;set speed to 100
    jmp finishSpeedChanging
    
    isSpeedLessThan0:
    fild dword [num_0]
    fcomip
    jb finishSpeedChanging ; jump if updated speed>0
    fild dword [num_0] ;set speed to 0

    finishSpeedChanging:
    fstp dword [edx + speeds_drone]
    ffree
    ret


;sets random_num to a random number in the range: [rightLimit-distanceToLeftLimit, rightLimit]
;calculation is made as follows: (seed/max_value) * rightLimit - distanceToLeftLimit
generateRandomAndScale:
    random ;changes seed
    finit
    fild dword [seed]
    fild dword [max_value]
    fdivp
    fild dword [rightLimit]
    fmulp
    fild dword [distanceToLeftLimit]
    fsubp
    fstp dword [random_num]
    ffree
    ret