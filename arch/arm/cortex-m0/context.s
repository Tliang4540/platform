.cpu    cortex-m0
.fpu    softvfp
.syntax unified
.thumb
.text

.extern _os_cur_sp
.extern os_update_sp

.global _os_start
.global _os_switch

_os_switch:
    MOV                 R3,R11
    MOV                 R2,R10
    MOV                 R1,R9
    MOV                 R0,R8
    PUSH                {R0-R7, LR}

    MOV R0, SP
    LDR R1, =_os_cur_sp
    STR R0, [R1]

    BL os_update_sp
_os_pop_stack:
    LDR R0, =_os_cur_sp
    LDR R0, [R0]
    MOV SP, R0

    POP                 {R0-R7}
    MOV                 R8,R0
    MOV                 R9,R1
    MOV                 R10,R2
    MOV                 R11,R3
    POP                 {PC}
    
_os_start:
    MOVS        R0, #0x02
    MSR         CONTROL, R0
    B           _os_pop_stack 
