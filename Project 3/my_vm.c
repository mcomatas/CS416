#include "my_vm.h"

/*
    To do list
    1. figure out data structures
    2. SetPhysicalMem(), Translate(), PageMap()
    3. myalloc(), myfree()
    4. matrix multiplication
    5. implemenet direct mapped TLB
*/

/*
Function responsible for allocating and setting your physical memory 
*/
void SetPhysicalMem() {

    //Allocate physical memory using mmap or malloc; this is the total size of
    //your memory you are simulating
    physicalMem = malloc(MEMSIZE * sizeof(char));  //had it as MEM_SIZE, I think it is just MEMSIZE though
    virtualMem = malloc(MAX_MEMSIZE * sizeof(char));

    memset(physicalMem, '0', MEMSIZE * sizeof(char));
    memset(virtualMem, '0', MAX_MEMSIZE * sizeof(char));
    //HINT: Also calculate the number of physical and virtual pages and allocate
    //virtual and physical bitmaps and initialize them
    physBitMap = malloc( NUM_PHYS_PGS * sizeof(char));
    virtBitMap = malloc( NUM_VIRT_PGS * sizeof(char));

    memset(physBitMap, '0', NUM_PHYS_PGS * sizeof(char));
    memset(virtBitMap, '0', NUM_VIRT_PGS * sizeof(char));

    //Initialize Page Directory here

    pageDirectory = malloc(1024 * 1024 * sizeof(pde_t));

    memset(pageDirectory, 0, 1024 * 1024 * sizeof(pde_t));

    oneBit( 0 );//allocate outer page table on the first page in mem

    //referring to the offset, outer, and inner bits
    offset = 12;
    outer = 12;
    inner = 10;

}



/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
pte_t * Translate(pde_t *pgdir, void *va) {
    //HINT: Get the Page directory index (1st level) Then get the
    //2nd-level-page table index using the virtual address.  Using the page
    //directory index and page table index get the physical address
    
    //uintptr_t virtualAddress = (uintptr_t)va;
    pte_t virtualAddress = (pte_t)va;

    //consider the possibility of va being NULL


    //so we have a virtual addess [ 20 bit VPN | 12 bit offset ]
    // [ 10 bit outer index | 10 bit inner index | 12 bit offset ] this is for 4KB pages
    //outer bit index is page directory, inner index is page table

    //get the outer index from VA
    //int outerIndex = ( virtualAddress >> 22 ) | 0; //shfit 22 right for outer 10 bits then bitwise OR it with 0 (Might not need the bitwise OR actually)
    //int innerIndex = (( virtualAddress << 10 ) >> 22 ) | 0; //I am not sure if this works (shifting 10 left then 22 right), but esentially we get the inner index (the 10 bits that rep the inner index)
    //int offset; //need to get offset here which is the last twelve bits

    //((( 1 << k ) - 1) & (num >> ( p - 1 )))
    
    int outerIndex = virtualAddress >> 22; //bit shift 22 times to get outer
    int innerIndex = ((( 1 << 10 ) - 1) & ( virtualAddress >> ( 13 - 1 )));

    pte_t* innerPage = (pte_t*)pgdir[outerIndex];
    //if the inner page table is not allocated
    if( !innerPage )
    {
        return NULL;
    }

    pte_t* page = (pte_t*)innerPage[innerIndex];
    //if the page is not allocated
    if( !page )
    {
        return NULL;
    }

    return page;
}


/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
int
PageMap(pde_t *pgdir, void *va, void *pa)
{

    /*HINT: Similar to Translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */

    //uintptr_t virtualAddress = (uintptr_t)va;
    pte_t virtualAddress = (pte_t)va;

    int outerIndex = virtualAddress >> 22; //bit shift 22 times to get outer
    int innerIndex = ((( 1 << 10 ) - 1) & ( virtualAddress >> ( 13 - 1 )));

    pte_t* innerPage = (pte_t*)pgdir[outerIndex];
    //if inner page table is not allocated, allocate it
    if( !innerPage )
    {
        pgdir[outerIndex] = (pte_t)get_next_avail(NUM_PHYS_PGS);
    }

    pte_t* page = (pte_t*)innerPage[innerIndex];
    if( page )
    {
        innerPage[innerIndex] = (pte_t)pa;
        return 0;//on success
    }

    return -1;//on failure
}


/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
 
    //Use virtual address bitmap to find the next free page
    
    //searching for contiguous pages
    int i, j;
    int success = 0, jCount = 0;
    uintptr_t foundAddr = 0;
    for(i = 1; i < NUM_VIRT_PGS; i++){
        jCount = 0;
        //check if the pages starting from the 0 is contig
        if(virtBitMap[i] == '0'){
            foundAddr = (uintptr_t)virtualMem + (i * PGSIZE);
            for(j = i; j < num_pages, j < NUM_VIRT_PGS; j++){
                if(virtBitMap[j] == '0'){
                    jCount++;
                }
            }
            if(jCount == num_pages) success = 1;
        }
        //if we found the contiguous pages, return the address
        if(success == 1){
            return (void*)foundAddr;
        }
        //else skip the i variable forward to where j left off
        else{
            i = j;
        }
    }

    //couldn't find anything, return null
    return NULL;
}


/* Function responsible for allocating pages
and used by the benchmark
*/
void *myalloc(unsigned int num_bytes) {

    //HINT: If the physical memory is not yet initialized, then allocate and initialize.
    if(physicalMem == NULL){
        SetPhysicalMem();
    }

    int numPages = (int)ceil(num_bytes / PGSIZE);

   /* HINT: If the page directory is not initialized, then initialize the
   page directory. Next, using get_next_avail(), check if there are free pages. If
   free pages are available, set the bitmaps and map a new page. Note, you will 
   have to mark which physical pages are used. */
    char* nextStart = get_next_avail(numPages);
    if(nextStart == NULL){ //can't find any
        printf("malloc error: no space\n");
        return NULL;
    }

    int i;
    uintptr_t virtAdd = (uintptr_t)nextStart;
    int index = (int)ceil(virtAdd/4096);
    for(i = 0; i < numPages; i++){
        virtBitMap[index + i] = '1';
    }

    return NULL;
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void myfree(void *va, int size) {

    //Free the page table entries starting from this virtual address (va)
    // Also mark the pages free in the bitmap
    //Only free if the memory from "va" to va+size is valid
    int i = 0, success = 1;
    int numPages = (int)ceil(size/PGSIZE);

    uintptr_t virtAdd = (uintptr_t)va;
    int index = (int)ceil(virtAdd/4096);

    //check if all the memory is valid
    for(i = 0; i < numPages; i++){
        //check via going page by page and seeing if they're all '1' bits
        if(virtAdd + i == '0') success = 0;
    }
    //if all memory is not valid, return
    if(success == 0){
        printf("free error: not all valid space\n");
        return;
    }
    
    for(i = 0; i < numPages; i++){
        //free all the memory by setting the page map bits to '0'
        virtBitMap[index + i] = '0';
    }
}


/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
*/
void PutVal(void *va, void *val, int size) {

    /* HINT: Using the virtual address and Translate(), find the physical page. Copy
       the contents of "val" to a physical page. NOTE: The "size" value can be larger
       than one page. Therefore, you may have to find multiple pages using Translate()
       function.*/
    pte_t* physAddr = Translate( pageDirectory, va );
    memcpy( val, physAddr, size );
}


/*Given a virtual address, this function copies the contents of the page to val*/
void GetVal(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
    "val" address. Assume you can access "val" directly by derefencing them.
    If you are implementing TLB,  always check first the presence of translation
    in TLB before proceeding forward */
    pte_t* physAddr = Translate( pageDirectory, va ); 
    memcpy( physAddr, val, size );
}



/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void MatMult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
    matrix accessed. Similar to the code in test.c, you will use GetVal() to
    load each element and perform multiplication. Take a look at test.c! In addition to 
    getting the values from two matrices, you will perform multiplication and 
    store the result to the "answer array"*/

    //allocate storage of each matrices' numbers and matrix for final answer
    int** resultVector = malloc(size * size * sizeof(int));
    int* mat1Vector = malloc(size * size * sizeof(int));
    int* mat2Vector = malloc(size * size * sizeof(int));

    int i, j, r = 0;
    int temp1, temp2;

    //copy values into malloc'd vectors
    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            uintptr_t mat1addr = (uintptr_t)mat1 + (((i * size) * sizeof(int)) + (j * sizeof(int)));
            uintptr_t mat2addr = (uintptr_t)mat2 + (((i * size) * sizeof(int)) + (j * sizeof(int)));;
            GetVal((void*)mat1addr, &temp1, sizeof(int));
            GetVal((void*)mat2addr, &temp2, sizeof(int));
            mat1Vector[(i * size) + j] = temp1;
            mat2Vector[(i * size) + j] = temp2;
        }
    }

    //do the multiplication using mat1/mat2 vectors
    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            resultVector[(i * size) + j] = 0;
            for(r = 0; r < size; r++){
                resultVector[(i * size) + j] += (mat1Vector[(i * size) + r] * mat2Vector[(r * size) + j]);
            }
        }
    }

    //put back into answer from arguments
    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            uintptr_t resultAddr = (uintptr_t)answer + (((i * size) * sizeof(int)) + (j * sizeof(int)));
            PutVal((void*)resultAddr, (void*)&resultVector[(i*size) + j], sizeof(int)); 
        }
    }
}
/*
 * Part 2: Add a virtual to physical page translation to the TLB.
 * Feel free to extend the function arguments or return type.
 */


int
add_TLB(void *va, void *pa)
{

    /*Part 2 HINT: Add a virtual to physical page translation to the TLB */

    return -1;
}


/*
 * Part 2: Check TLB for a valid translation.
 * Returns the physical page address.
 * Feel free to extend this function and change the return type.
 */
pte_t *
check_TLB(void *va) {

    /* Part 2: TLB lookup code here */

}


/*
 * Part 2: Print TLB miss rate.
 * Feel free to extend the function arguments or return type.
 */
void
print_TLB_missrate()
{
    double miss_rate = 0;

    /*Part 2 Code here to calculate and print the TLB miss rate*/




    fprintf(stderr, "TLB miss rate %lf \n", miss_rate);
}

//sets the bit to 1 in virtual bit map, indicating it is in use
void oneBit( int page )
{
    int index = page / 32;
    int pos = page % 32;

    virtBitMap[index] = virtBitMap[index] | ( 1 << pos );
}

//sets the bit to 0 in virtual bit map, indicating not in use
void zeroBit( int page )
{
    int index = page / 32;
    int pos = page % 32;

    virtBitMap[index] = virtBitMap[index] & ~( 1 << pos );
}

//tests whether a certain bit is 0 or 1, to see if the bit is taken or not
int isBit( int page )
{
    int index = page / 32;
    int pos = page % 32;

    if( virtBitMap[index] & ( 1 << pos ) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}