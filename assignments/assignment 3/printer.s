section .rodata

;drone struct offset
    coor_x_drone: equ 0
    coord_y_drone: equ 4
    speeds_drone: equ 8
    headings_drone: equ 12
    scores_drone: equ 16
    active_drone: equ 20
    size_drone_struct: equ 24



    new_line: db 10, 0
    comma: db "," , 0

    ;not needed:
    str_drone_index: db "drone index: ", 0
    str_coord_x: db "x coord: ", 0
    str_coord_y: db "y coord: ", 0
    str_angle: db "angle: ", 0
    str_speed: db "speed: ", 0
    str_score: db "score: ", 0
    str_target: db "target: ", 0



section .text
global PRINTER_ROUTINE_CODE
extern SCHEDULER_CO_ROUTINE
extern drones
extern croarry
extern coord_x_target
extern coord_y_target
extern formatInt
extern formatFloat
extern resume
extern printf



%macro printw 1
    pushad
    push %1
    call printf
    add esp, 4
    popad
%endmacro

%macro printFloat 1
    pushad
    finit
    fld dword %1
    sub esp, 8
    fstp qword [esp]
    ffree
    push dword formatFloat
    call printf
    add esp, 12
    popad
%endmacro

%macro printInt 1
    pushad
    push dword %1
    push dword formatInt
    call printf
    add esp, 8
    popad
%endmacro

PRINTER_ROUTINE_CODE:
    ;printw str_target
    ;printw str_coord_x
    printFloat [coord_x_target]
    printw comma
    ;printw str_coord_y
    printFloat [coord_y_target]
    printw new_line
    
    mov ebx, [croarry]
    mov ecx, 0

    loop:
        mov edx, dword [ebx + active_drone]
        cmp edx, 0
        je stepToNextDrone
        inc ecx
        ;printw str_drone_index
        printInt ecx
        dec ecx
        printw comma
        ;printw str_coord_x
        printFloat [ebx + coor_x_drone]
        printw comma
        ;printw str_coord_y
        printFloat [ebx + coord_y_drone]
        printw comma
        ;printw str_angle
        printFloat [ebx + headings_drone]
        printw comma
        ;printw str_speed
        printFloat [ebx + speeds_drone]
        printw comma
        ;printw str_score
        printInt [ebx + scores_drone]
        printw new_line
    stepToNextDrone:
    add ebx, size_drone_struct
    inc ecx
    cmp ecx, [drones]
    jne loop
    mov ebx, SCHEDULER_CO_ROUTINE
    call resume
    jmp PRINTER_ROUTINE_CODE

