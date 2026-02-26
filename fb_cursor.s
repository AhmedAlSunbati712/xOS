.global outb

outb:
    # C Call: outb(port, data)
    # Stack: [esp+4] = port, [esp+8] = data

    movw 4(%esp), %dx    # LOAD PORT INTO DX (Was [esp+8])
    movb 8(%esp), %al    # LOAD DATA INTO AL (Was [esp+4])
    
    outb %al, %dx        # Output byte AL to port DX
    ret
