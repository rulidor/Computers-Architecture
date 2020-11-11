section .rodata
    
    coor_x_drone: equ 0
    coord_y_drone: equ 4
    speeds_drone: equ 8
    headings_drone: equ 12
    scores_drone: equ 16
    active_drone: equ 20
    size_drone_struct: equ 24

    DRONE_ROUTINE_STRUCT_SIZE: equ 8

section .text
    extern drones
    extern steps
    extern cycles
    extern dronesCOArray
    extern scheduler_finish

    extern croarry
    extern currDrone
    extern resume
    global SCHEDULER_ROUTINE_CODE
    extern PRINTER_CO_ROUTINE

section .data
    makeNotActive: dd 0 ;will point to the drone to set as not active
    minScore: dd 0 ;will hold the minScore. score.
    drones_count: dd 0 ;will get the value of drones (num. of drones) and will be decremented in every iteration

;Divides 1st param in the 2nd. Puts division in eax and remainder in edx
%macro make_division 2
    push ecx
    mov edx, 0
    mov dword eax, %1
    mov dword ecx, %2
    div ecx
    pop ecx
%endmacro



SCHEDULER_ROUTINE_CODE:
    mov ecx, 0
    mov eax, [drones]
    mov dword [drones_count], eax
    loop_scheduler:
        make_division ecx, [drones] ;put in edx the remainder of (current index in the iretation) divided by (number of drones)
        mov dword [currDrone], edx   
        push ecx
        mov edx, 0
        mov dword eax, edx
        mov dword ecx, size_drone_struct
        mul ecx
        pop ecx        
        mov ebx, [croarry]
        mov edx, [ebx + eax + active_drone]
        cmp edx, 0
        je callPrinter
        push ecx
        mov edx, 0
        mov dword eax, [currDrone]
        mov dword ecx, DRONE_ROUTINE_STRUCT_SIZE
        mul ecx
        pop ecx

        mov ebx, [dronesCOArray]
        add ebx, eax
        call resume

        callPrinter:
        make_division ecx, [steps] ;put in edx the remainder of (current index in the iretation) divided by (steps between game board printings)
        cmp edx, 0
        jne finish_printer
        mov ebx, PRINTER_CO_ROUTINE
        call resume
        ;now we will check if:
        ;1. it is time to make one of the drone not active, by checking (num of cycles)/(num of drones) % (defined num of cycles between each elimination)
        ;2. it is a full scheduler cycle (iterate through all of the drones)
        finish_printer:
        make_division ecx, [drones]
        mov ebx, edx
        make_division eax, [cycles]
        cmp edx, 0 ;checks if it is time to make one of the drones not active
        jne cont_loop_scheduler ;case not - continue loop
        cmp ebx, 0 ;checks if it is a full scheduler cycle (if we have iterated through all of the drones)
        jne cont_loop_scheduler ; case not - continue loop

        ;case it is time to make one of the drones not active - now we will check which of the drones has the min score and will set it as not active
        pushad
        mov dword ecx, 0
        mov dword [minScore], 999999999 ;as a max_value
        mov dword [makeNotActive], 0 ;will point to the drone to set as not active
        mov ebx, [croarry]
        loop_find_min_score:
            cmp ecx, [drones] ;counter
            je end_find_loop ;needs to stop when -1 bc 0 its still valid in the array 
            cmp dword [ebx + active_drone], 0
            je next_iteration
            mov eax, [minScore]
            cmp dword [ebx + scores_drone], eax
            jbe set_as_min
            jmp next_iteration
            set_as_min:
                mov eax, [ebx + scores_drone]
                mov dword [minScore], eax
                mov dword [makeNotActive], ecx 
            next_iteration:
                add ebx, size_drone_struct
                inc ecx
                jmp loop_find_min_score
        end_find_loop:
            dec dword [drones_count] 
            mov eax, [makeNotActive] 
            mov ebx, size_drone_struct
            mul ebx
            mov ebx, [croarry]
            mov dword [ebx + eax + active_drone], 0
            popad

        
    cont_loop_scheduler:
        inc ecx
        cmp dword [drones_count], 1 ;checking if there is only one drone left ;will get the value of drones (num. of drones) and will be decremented every iteration
        je terminate_scheduler
        jmp loop_scheduler

terminate_scheduler:
    mov ebx, PRINTER_CO_ROUTINE
    call resume
    jmp scheduler_finish