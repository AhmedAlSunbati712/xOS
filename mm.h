#ifndef MM_H
#define MM_H

#define PAGESHIFT 12
#define PAGESIZE (1 << PAGESHIFT)
#define PAGEMASK (PAGESIZE - 1)
#define DOWN_TO_PAGE(ADDR) ((ADDR) & ~PAGEMASK)
#define UP_TO_PAGE(ADDR) ((ADDR + PAGEMASK) & ~PAGEMASK)

#endif