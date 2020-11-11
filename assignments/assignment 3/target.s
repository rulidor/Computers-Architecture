section .rodata
    DRONE_ROUTINE_STRUCT_SIZE: equ 8

section .bss
    global coord_x_target
    global coord_y_target
    coord_x_target: resd 1 ; float
    coord_y_target: resd 1 ; float

section .text
    extern rightLimit
    extern distanceToLeftLimit
    extern generateRandomAndScale
    extern random_num
    global TARGET_ROUTINE_CODE
    
    global createTarget
    extern dronesCOArray
    extern currDrone
    
    extern resume

; Puts a random number in range (0 to 1st param) in 2nd paramater, which cannot be eax.
%macro putRandomInRange 2
    push eax
    mov dword [rightLimit], %1
    mov dword [distanceToLeftLimit], 0
    call generateRandomAndScale
    mov eax, [random_num]
    mov %2, eax
    pop eax
%endmacro



TARGET_ROUTINE_CODE:
    call createTarget
    call setNextDroneRoutine
    call resume
    jmp TARGET_ROUTINE_CODE


setNextDroneRoutine:
    mov eax, [currDrone]
    mov ebx, DRONE_ROUTINE_STRUCT_SIZE
    mul ebx  ;now we have (currDroneIndex * Size of struct drone) in eax
    add eax, [dronesCOArray]
    mov ebx, eax
    ret


createTarget:
    putRandomInRange 100, [coord_x_target]
    putRandomInRange 100, [coord_y_target]
    ret