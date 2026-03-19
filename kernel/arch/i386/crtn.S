/* So from what i understand, this is for any constructor functions or destructory
 functions that have to run before or after the kernel main.
 this file sets up the stack frame, gcc will nicely place crtend.o's nicely here before
 popping the value of the frame pointer into the break pointer register
 crtbegin.o -> kernel.o -> crtend.o
 */
.section .init
    popl %ebp
    ret

.section .fini
    popl %ebp
    ret
    