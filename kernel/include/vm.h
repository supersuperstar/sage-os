#ifndef __VM_H__
#define __VM_H__

#ifndef FRAME_NUM
#define FRAME_NUM 40
#endif

#include <common.h>

void inituvm(task_t* proc, unsigned char* init, int sz);
int allocuvm(task_t* proc, int newsz, int oldsz);
int deallocuvm(task_t* proc, int newsz, int oldsz);
void copyuvm(task_t* proc, task_t* src, int sz);
void uproc_pgmap(task_t* proc, void* vaddr, void* paddr, int prot);

#endif