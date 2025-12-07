.intel_syntax noprefix
.set CAFEBABE, 0xCAFEBABE

.section .text
.global kernel_main_s
kernel_main_s:
    push 2
    push 2
    call test_stack
    mov ebx, eax
    jmp loop
loop:
    jmp loop


