#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

//Assume the address space is 32 bits, so the max memory size is 4GB
//Page size is 4KB

//Add any important includes here which you may need

#define PGSIZE 4096

// Maximum size of your memory
#define MAX_MEMSIZE 4ULL*1024*1024*1024 //4GB

#define MEMSIZE 1024*1024*1024

// NEW: Size of physical/virtual pages
#define NUM_PHYS_PGS MEMSIZE / PGSIZE
#define NUM_VIRT_PGS MAX_MEMSIZE / PGSIZE

//#define NUM_PHYS_PGS 1024*1024*1024/4096
//#define NUM_VIRT_PGS 4ULL*1024*1024*1024/4096

// Represents a page table entry
//typedef void* pte_t; //used to be unsigned long, maybe we make this char* instead of void* though 
typedef unsigned long pte_t;

// Represents a page directory entry
//typedef pte_t* pde_t; //used to be unsigned long, TA said it was ok to change these and it might be easier to work with in this way. Can think of it as 
typedef unsigned long pde_t;

// NEW: physical memory, page states
char* physicalMem;
char* physPageMap;
char* virtPageMap;
pde_t** pageDirectory;
// pde_t pageDirectory;

//virtual bit map - 0-8191 for a total of 262,144 physical pages
//page 0 represented by 0-31, page 1 32-63, etc.
unsigned long virtBitMap[NUM_PHYS_PGS/32];

int outer;
int inner;
int offset;


#define TLB_SIZE 120

//Structure to represents TLB
struct tlb {

    //Assume your TLB is a direct mapped TLB of TBL_SIZE (entries)
    // You must also define wth TBL_SIZE in this file.
    //Assume each bucket to be 4 bytes
};
struct tlb tlb_store;


void SetPhysicalMem();
pte_t* Translate(pde_t *pgdir, void *va);
int PageMap(pde_t *pgdir, void *va, void* pa);
void *get_next_avail( int num_pages );
bool check_in_tlb(void *va);
void put_in_tlb(void *va, void *pa);
void *myalloc(unsigned int num_bytes);
void myfree(void *va, int size);
void PutVal(void *va, void *val, int size);
void GetVal(void *va, void *val, int size);
void MatMult(void *mat1, void *mat2, int size, void *answer);
void print_TLB_missrate();

void oneBit( int page );
void zeroBit( int page );
int isBit( int page );

#endif
