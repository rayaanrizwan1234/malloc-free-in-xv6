#include "memory_management.h"
#include "kernel/types.h"
#include "user/user.h"



// intializes the head of the linked list
memory_block *head = null;

// malloc function
void* _malloc(int size){
    // if requested size for is zero then program returns null
    if (size == 0){
        return null;
    }
    // check if any previously allocated memory block is free and check if that is big enough for our size
    if (head != null){
       void *bestFit = best_fit(size);
       // if no memory block is found then it will return null and continue with the program
       if (bestFit != null){
        return bestFit;
       }   
    } 

    // gets the end of he initialized data segment
    void *point = sbrk(0);
    // intialises a node with the memory adress of the start of the uninitialized memory 
    memory_block *block = (memory_block*)point;   
    // allocate memory by calling sbrk with the size of how much we want to allocate
    void *allocate = (void*)sbrk((size+BLOCK_SIZE));
    // if it fails to allocate then it will return (void*) - 1
    if (allocate == (void*)-1){
        return null;
    } else {
        // initializes memory address to the end of the block
        // block->memoryAddress = point + BLOCK_SIZE + size;
        block->memoryAddress = point;
        // initializes the next node to null
        block->next = null;
        // makes it not free 
        block->isfree = 0;
        // blocks size is the size user passes in 
        block->size = size;
        // blcoks memory size
        block->memSize = size;
    }
    // inserts the newly created node to the end of the linekd list
    if (head == null){
        head = block;
    } else {
        memory_block * temp = head; 
        while (temp != null)
        {
            if (temp->next == null){
                temp->next = block;
                break;
            }
            temp = temp->next;
        }
    }
    // returns the bliock
    return block;
}

// method to find best fit node
void* best_fit(int size){
// intialize variables
        memory_block *bestFit = null;
        memory_block *temp = head;
        /* baseline for allocation into existing memory block as if it is 
        * too much bigger then we will just create new memory block
        */
        int fit = 10000; 
        // iterates over the linked list 
        while (temp != null){
            // checks if the memeory block is free
            if (temp->isfree == 1){
                // checks if it has enough space for the asked size
                if (temp->memSize >= size){
                    // it checks how good its fit is
                    if ((temp->memSize - size) < fit){
                        fit = (temp->memSize - size);
                        bestFit = temp;
                    }
                }   
            }
            temp = temp->next;
        }
        /* the memory block that has the lowest fit value is set to the best fit block. if there is nothing in 
        * our linked list that satisfies the request it will return null
        * if the best fit block has 33 or more extra bytes we will split the block
        */
        if (bestFit != null){
            if (fit >= 33){
                memory_block *newBlock = splitMemory(bestFit, size);
                bestFit->next = newBlock;
                bestFit->memSize = size;
            } 
            bestFit->size = size;
            bestFit->isfree = 0;
            return bestFit;
        }
        return null;
}
// splits the memory block if mroe than 33 bytes aren't used in it
void* splitMemory(memory_block *bestFit, int size){
    // the memory address for the new memory block is at the end of the best fit block
    int *newMemAddy = bestFit->memoryAddress + BLOCK_SIZE + (bestFit->memSize - size);
    // create memory block
    memory_block *newBlock = (memory_block*)newMemAddy;
    // sets it to free 
    newBlock->isfree = 1;
    newBlock->memoryAddress = newMemAddy;
    // initializes the next node to null
    newBlock->next = bestFit->next;
    // blocks size is the size user passes in 
    newBlock->size = 0;
    // the memory size is the extra space minus the block size as thats the only memory that can be used for allocation
    newBlock->memSize = bestFit->memSize - size - BLOCK_SIZE;
    return newBlock;
}


// method for free 
void _free(void *ptr){
    // loops over the whole linked list and frees the pointer passed in to the function
    // doesn't delete it from memory
    memory_block *temp = head;
    while (temp != null){
        if (temp == ptr){
            temp->isfree = 1;
            temp->size = 0;
            break;
        }
        temp = temp->next;
    }
    make_contiguous(head);
}

// makes free blocks of memory contiguous 
void make_contiguous(void *ptr){
    // loops over linked list 
    memory_block *current = head;
    while (current != null){
        // if current is ptr then checks it
        if (current == ptr){
            // if its free then it checks its next node
            if (current->isfree == 1){
                memory_block *nextNode = current->next;
                // if its next node is null then we just break because then it wont be contiguous
                if (nextNode == null){
                    break;
                }
                // if the next node is free then we call the same function with the next node as the parameter
                if (nextNode->isfree == 1){
                    make_contiguous(nextNode);
                    // it makes the current nodes next the next nodes next
                    current->next = nextNode->next;
                    // adds their memory sizes plus the BLOCK_SIZE
                    current->memSize += nextNode->memSize + BLOCK_SIZE;
                    // size is zero because it is free
                    current->size = 0;
                } 
            }
        }
        current = current->next;
    }
}


