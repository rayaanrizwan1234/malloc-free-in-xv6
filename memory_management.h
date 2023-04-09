// making the node for the linked list
typedef struct memory_block{  
    struct memory_block *next; // the next node 
    int isfree;  //check if tis free 
    int size; // size requested by the user
    int memSize; // actual size of the memory block
    int *memoryAddress; // value that sbrk gives us
}memory_block; 
// defining null to 0 as xv6 doesn't have null
#define null 0
// block size of memory
#define BLOCK_SIZE sizeof(memory_block)
void* _malloc(int size);
void _free(void *ptr);
void* best_fit(int size);
void make_contiguous(void *ptr);
void* splitMemory(memory_block *bestFit, int size);
