

.global load_gdt

load_gdt:
    movl 4(%esp), %eax   # address of the gdt table
    lgdt (%eax)
    ljmp $0x08, $reload_registers

reload_registers:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    ret
