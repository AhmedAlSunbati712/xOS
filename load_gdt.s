.intel_syntax noprefix


.global load_gdt

load_gdt:
    mov eax, [esp + 4] # address of the gdt table
    lgdt [eax]
    jmp 0x08:reload_registers

reload_registers:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret