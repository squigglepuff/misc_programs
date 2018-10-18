; CPUID Assembly Program
; This program simply discovers information about the processor and dumps it.
; (c) 2016 Travis M Ervin
; Built using NASM on Ubuntu Linux
; Debug assemble: nasm -f elf64 -F dwarf -g cpuid.asm && ld -m elf_x86_64 -o cpuid cpuid.o
; Release assemble: nasm -f elf64 cpuid.asm && ld s -o cpuid cpuid.o

; Set 64-bit!
BITS 64

; Some constants.
SYS_STDIN  equ 0
SYS_STDOUT equ 1
SYS_EXIT   equ 1
SYS_READ   equ 3
SYS_WRITE  equ 4

; data section.
section .data
    lfChar db 0xA ;
    vendStr db 'Vendor ID: ' ;
    vendLen equ $-vendStr    ;
    
    ; CPU attributes.
    stepStr db 'Stepping: 0x' ;
    stepLen equ $-stepStr     ;
    
    modelStr db 'Model: 0x' ;
    modelLen equ $-modelStr  ;
    
    famidStr db 'Family ID: 0x' ;
    famidLen equ $-famidStr  ;
    
    typeStr db 'Type: 0x'  ;
    typeLen equ $-typeStr  ;
    
    ; Quit.
    quitStr db 'Quiting...', 0xA ;
    quitLen equ $-quitStr   ;

; Variable section.
section .bss
    strPrt1 resb 4 ;
    strPrt2 resb 4 ;
    strPrt3 resb 4 ;
    tmpBuff resb 8 ;
    tmpChar resw 1 ;
    tmpHI   resb 1 ;
    tmpLO   resb 1 ;

; Text section.
section .text
    global _start

_start:
    call vendor_id ;
    call cpu_info  ;
    call _exit     ;

; This method prints the vendor ID.
vendor_id:
    ; Query the CPU for it's details using the CPUID OpCode.
    mov rax, 0h ; Set the calling code.
    CPUID ; Call CPUID.

    mov [strPrt1], rbx ;
    mov [strPrt2], rdx ;
    mov [strPrt3], rcx ;
    
    mov rcx, vendStr ;
    mov rdx, vendLen ;
    call print       ;
    
    mov rcx, strPrt1 ;
    mov rdx, 4       ;
    call print       ;
    
    mov rcx, strPrt2 ;
    mov rdx, 4       ;
    call print       ;
    
    mov rcx, strPrt3 ;
    mov rdx, 4       ;
    call print       ;
    
    mov rcx, lfChar  ;
    mov rdx, 1       ;
    call print       ;
    
    ret ;

cpu_info:
    mov rax, 1h ; Set the calling code.
    CPUID       ; Query the CPU for it's details using the CPUID OpCode.
    
    ; Stepping number.
    and rax, 0x000F ;
    cmp rax, 0xA    ;
    jl ci1          ;
    jge dth1        ;
dth1:
    add rax, 0x7 ;
ci1:
    add rax, '0'       ;
    mov [tmpChar], rax ;
    
    mov rcx, stepStr   ;
    mov rdx, stepLen   ;
    call print         ;
    
    mov rcx, tmpChar   ;
    mov rdx, 2         ;
    call print         ;
    call printlf       ;
    
    ; Model number.
    mov rax, 1h ; Set the calling code.
    CPUID       ; Query the CPU for it's details using the CPUID OpCode.
    
    and rax, 0x00F0     ; 0011 1010 == 0x3A
    shr rax, 4          ;
    add rax, '0'        ;
    mov [tmpBuff], rax  ;
    
    mov rcx, modelStr   ;
    mov rdx, modelLen   ;
    call print          ;
    
    mov rax, [tmpBuff] ;
    call printhex      ;
    call printlf       ;
    
    ; Family number.
    mov rax, 1h ; Set the calling code.
    CPUID       ; Query the CPU for it's details using the CPUID OpCode.
    
    and rax, 0x0F00     ;
    shr rax, 8 ; Shift by 1 byte so we're getting an accurate number.
    mov [tmpChar], rax  ;
    
    mov rcx, famidStr   ;
    mov rdx, famidLen   ;
    call print          ;
    
    mov rax, [tmpChar] ;
    cmp rax, 0xA       ;
    jl ci2             ;
    jge dth2           ;
dth2:
    add rax, 0x7  ;
ci2:
    add rax, '0' ;
    mov [tmpChar], rax ;
    
    mov rcx, tmpChar ;
    mov rdx, 1       ;
    call print       ;
    call printlf     ;
    
    ; Type ID.
    mov rax, 1h ; Set the calling code.
    CPUID       ; Query the CPU for it's details using the CPUID OpCode.
    
    and rax, 0x0300     ;
    shr rax, 12         ; Chomp 3 bytes off.
    add rax, '0'        ;
    mov [tmpChar], rax  ;
    
    mov rcx, typeStr    ;
    mov rdx, typeLen    ;
    call print          ;
    
    mov rcx, tmpChar   ;
    mov rdx, 2         ;
    call print         ;
    call printlf       ;    
    
    ret ;

; This procedure assumes rax is filled with the data to be printed.
printhex:
    mov rcx, rax ;
    mov rdx, rax ;
    
    and rcx, 0xF0 ;
    and rdx, 0x0F ;
    
    shr rcx, 4 ;
    
    cmp rcx, 0xA ;
    jl  ph1  ;
    jge dth3 ;    
dth3:
    add rcx, 0x7 ;
ph1:    
    cmp rdx, 0xA ;
    jl ph2   ;
    jge dth4 ;
dth4:
    add rdx, 0x7 ;
ph2:    
    add rcx, 0x30 ;
    add rdx, 0x30 ;
    
    mov [tmpHI], rcx ;
    mov [tmpLO], rdx ;
    
    mov rcx, tmpHI ;
    mov rdx, 1     ;
    call print     ;
    
    mov rcx, tmpLO ;
    mov rdx, 1     ;
    call print     ;
    
    ret ;

; This method assumes you've filled rdx and rcx.
print:
    mov rax, SYS_WRITE  ;
    mov rbx, SYS_STDOUT ;
    int 0x80            ; Call the kernel.
    ret                 ;

printlf:
    mov rax, SYS_WRITE  ;
    mov rbx, SYS_STDOUT ;
    mov rcx, lfChar     ;
    mov rdx, 1          ;
    int 0x80            ; Call the kernel.
    ret                 ;

_exit:
    mov rcx, quitStr ;
    mov rdx, quitLen ;
    call print       ;
    
    mov rax, SYS_EXIT
    mov rbx, SYS_STDIN
    int 0x80 ; Call the kernel.
