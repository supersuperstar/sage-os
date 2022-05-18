#ifndef __VM_H__
#define __VM_H__

#include <common.h>

void inituvm(AddrSpace* as, unsigned char* init, int sz);
int allocuvm(AddrSpace* as, int newsz, int oldsz);
int deallocuvm(AddrSpace* as, int newsz, int oldsz);
void copyuvm(AddrSpace* dst, AddrSpace* src, int sz);
void uproc_pgmap(AddrSpace* as, void* vaddr, void* paddr, int prot);

#endif