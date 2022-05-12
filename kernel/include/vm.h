#ifndef __VM_H__
#define __VM_H__

#include <common.h>

void inituvm(AddrSpace* as, char* init, int sz);
int allocuvm(AddrSpace* as, int newsz, int oldsz);
int deallocuvm(AddrSpace* as, int newsz, int oldsz);

#endif