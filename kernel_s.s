.set CAFEBABE, 0xCAFEBABE

.section .text
.global kernel_main_s
kernel_main_s:
    pushl $2
    pushl $2
    call test_stack
    movl %eax, %ebx
    jmp loop
loop:
    jmp loop
