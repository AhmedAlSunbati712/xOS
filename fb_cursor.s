.intel_syntax noprefix

.global outb

outb:
    # C Call: outb(port, data)
    # Stack: [esp+4] = port, [esp+8] = data

    mov dx, [esp + 4]     # LOAD PORT INTO DX (Was [esp+8])
    mov al, [esp + 8]     # LOAD DATA INTO AL (Was [esp+4])
    
    out dx, al            # Output byte AL to port DX
    ret
