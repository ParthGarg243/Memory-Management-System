#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>

#define PAGE_SIZE 4096

struct SubChainNode {
    void* physicalAddress;
    int startAddress; 
    int endAddress;
    size_t size;
    char processOrHole;
    struct SubChainNode* nextNode;
    struct SubChainNode* prevNode;
};

struct MainChainNode { //you cannot go back to the mainchain from the subchain
    void* physicalAddress;
    int pages;
    int startAddress;
    size_t size;
    struct SubChainNode* subChainHead;
    struct MainChainNode* nextNode;
    struct MainChainNode* prevNode;
};

struct MainChainNode* mainChainHead;
int startingVirtualAddress;

void mems_init() {
    mainChainHead = NULL;
    startingVirtualAddress = 1000;
}

// void* mems_malloc(size_t size) {
//     int found = 0;
//     struct SubChainNode* finalSubNode = NULL;
//     struct SubChainNode* cursorSubNode = NULL;
//     struct MainChainNode* cursorMainNode = mainChainHead;

//     // This loop searches for a suitable hole in existing chains
//     while (cursorMainNode != NULL) {
//         cursorSubNode = cursorMainNode->subChainHead;
//         while (cursorSubNode != NULL) {
//             if (cursorSubNode->processOrHole == 'H') {
//                 if (cursorSubNode->size >= size) {
//                     finalSubNode = cursorSubNode;
//                     found = 1;
//                     break;
//                 }
//             }
//             cursorSubNode = cursorSubNode->nextNode;
//         }
//         if (found == 1) {
//             break;
//         }
//         cursorMainNode = cursorMainNode->nextNode;
//     }

//     if (found == 1) {// what does intptr_t do?
//         if (size == finalSubNode -> size) {
//             finalSubNode->processOrHole = 'P';
//             return (void*)(intptr_t)finalSubNode->startAddress;
//         } 
//         else if (size < finalSubNode->size){
//             struct SubChainNode* newHole = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//             newHole->physicalAddress = (void*) newHole;
//             newHole->processOrHole = 'H';
//             newHole -> nextNode = finalSubNode -> nextNode;
//             newHole -> prevNode = finalSubNode;
//             finalSubNode -> nextNode = newHole;
//             if (newHole -> nextNode != NULL) {
//                 newHole -> nextNode -> prevNode = newHole;
//             }          
//             newHole->size = (finalSubNode->size) - size;
//             newHole->endAddress = finalSubNode->endAddress;
//             finalSubNode->size = size;
//             finalSubNode->endAddress = (finalSubNode->startAddress) + size - 1;
//             newHole->startAddress = (finalSubNode->endAddress) + 1;
//             finalSubNode->processOrHole = 'P';
//             return (void*)(intptr_t)finalSubNode->startAddress;
//         }
//     } 
//     else if (found == 0) {
//         size_t pages = (size / PAGE_SIZE) + 1;
//         size_t newMainNodeSize = pages * PAGE_SIZE;
//         struct MainChainNode* newMainNode = (struct MainChainNode*) mmap(NULL, newMainNodeSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//         newMainNode->size = newMainNodeSize;
//         newMainNode->physicalAddress = (void*)newMainNode;
//         newMainNode->pages = pages;

//         if (mainChainHead == NULL) {
//             mainChainHead = newMainNode;
//             newMainNode->startAddress = startingVirtualAddress;
//             newMainNode->nextNode = NULL;
//             newMainNode->prevNode = NULL;
//             newMainNode->subChainHead = NULL;
//         } 
//         else if (mainChainHead) {
//             struct MainChainNode* anotherCursorMainNode = mainChainHead;
//             while (anotherCursorMainNode->nextNode != NULL) {
//                 anotherCursorMainNode = anotherCursorMainNode->nextNode;
//             }
//             anotherCursorMainNode->nextNode = newMainNode;
//             newMainNode->startAddress = (anotherCursorMainNode->startAddress) + (anotherCursorMainNode->size);
//             newMainNode->subChainHead = NULL;
//             newMainNode->nextNode = NULL;
//             newMainNode->prevNode = anotherCursorMainNode;
//         }

//         if (newMainNodeSize > size) {
//             struct SubChainNode* newProcess = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//             struct SubChainNode* newHole = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//             newProcess->physicalAddress = (void*)newProcess;
//             newHole->physicalAddress = (void*)newHole;
//             newProcess->processOrHole = 'P';
//             newHole->processOrHole = 'H';
//             newProcess->prevNode = NULL;
//             newProcess->nextNode = newHole;
//             newHole->prevNode = newProcess;
//             newHole->nextNode = NULL;
//             newProcess->size = size;
//             newHole->size = newMainNodeSize - size;
//             newProcess->startAddress = newMainNode->startAddress;
//             newProcess->endAddress = (newProcess->startAddress) + size - 1;
//             newHole->startAddress = newProcess->endAddress + 1;
//             newHole->endAddress = (newHole->startAddress) + (newHole->size) - 1;
//             newMainNode->subChainHead = newProcess;

//             // Update the startingVirtualAddress
//             startingVirtualAddress += newMainNodeSize;
//             return (void*)(intptr_t)newProcess->startAddress;
//         } 
//         else if (newMainNodeSize == size) {
//             struct SubChainNode* newProcess = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//             newProcess->physicalAddress = (void*)newProcess;
//             newProcess->startAddress = newMainNode->startAddress;
//             newProcess->endAddress = (newProcess->startAddress) + size - 1;
//             newProcess->processOrHole = 'P';
//             newProcess->size = size;
//             newProcess->prevNode = NULL;
//             newProcess->nextNode = NULL;
//             newMainNode->subChainHead = newProcess;

//             // Update the startingVirtualAddress
//             startingVirtualAddress += newMainNodeSize;
//             return (void*)(intptr_t)newProcess->startAddress;
//         }
//     }
//     return NULL; // Return NULL at the end if allocation fails
// }

void* mems_malloc(size_t size) {
    int found = 0;
    struct SubChainNode* finalSubNode = NULL;
    struct SubChainNode* cursorSubNode = NULL;
    struct MainChainNode* cursorMainNode = mainChainHead;

    // This loop searches for a suitable hole in existing chains
    while (cursorMainNode != NULL) {
        cursorSubNode = cursorMainNode->subChainHead;
        while (cursorSubNode != NULL) {
            if (cursorSubNode->processOrHole == 'H') {
                if (cursorSubNode->size >= size) {
                    finalSubNode = cursorSubNode;
                    found = 1;
                    break;
                }
            }
            cursorSubNode = cursorSubNode->nextNode;
        }
        if (found == 1) {
            break;
        }
        cursorMainNode = cursorMainNode->nextNode;
    }

    if (found == 1) {
        if (size == finalSubNode->size) {
            finalSubNode->processOrHole = 'P';
            return (void*)(uintptr_t)finalSubNode->startAddress;
        } else if (size < finalSubNode->size) {
            // Allocate a new hole at the appropriate position within the SubChain
            struct SubChainNode* newHole = mmap(
                (void*)(uintptr_t)((uintptr_t)finalSubNode->physicalAddress + size),
                finalSubNode->size - size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0
            );
            newHole->physicalAddress = (void*)newHole;// this was changed
            newHole->processOrHole = 'H';
            newHole->nextNode = finalSubNode->nextNode;
            newHole->prevNode = finalSubNode;
            finalSubNode->nextNode = newHole;
            if (newHole->nextNode != NULL) {
                newHole->nextNode->prevNode = newHole;
            }
            newHole->size = finalSubNode->size - size;
            newHole->endAddress = finalSubNode->endAddress;
            finalSubNode->size = size;
            finalSubNode->endAddress = (finalSubNode->startAddress) + size - 1;
            newHole->startAddress = (finalSubNode->endAddress) + 1;
            finalSubNode->processOrHole = 'P';
            return (void*)(uintptr_t)finalSubNode->startAddress;
        }
    } else if (found == 0) {
        size_t pages = (size / PAGE_SIZE) + 1;
        size_t newMainNodeSize = pages * PAGE_SIZE;

        // Allocate a new MainChainNode and initialize its physical address
        struct MainChainNode* newMainNode = mmap(
            (void*)(uintptr_t)(0), // Specify address as 0 to let the system choose
            newMainNodeSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0
        );
        newMainNode->size = newMainNodeSize;
        newMainNode->physicalAddress = (void*)(uintptr_t)newMainNode;
        newMainNode->pages = pages;

        if (mainChainHead == NULL) {
            mainChainHead = newMainNode;
            newMainNode->startAddress = startingVirtualAddress;
            newMainNode->nextNode = NULL;
            newMainNode->prevNode = NULL;
            newMainNode->subChainHead = NULL;
        } else {
            struct MainChainNode* anotherCursorMainNode = mainChainHead;
            while (anotherCursorMainNode->nextNode != NULL) {
                anotherCursorMainNode = anotherCursorMainNode->nextNode;
            }
            anotherCursorMainNode->nextNode = newMainNode;
            newMainNode->startAddress = (anotherCursorMainNode->startAddress) + (anotherCursorMainNode->size);
            newMainNode->subChainHead = NULL;
            newMainNode->nextNode = NULL;
            newMainNode->prevNode = anotherCursorMainNode;
        }

        if (newMainNodeSize > size) {
            // Allocate a new process block within the new MainChainNode
            struct SubChainNode* newProcess = mmap(
                (void*)(uintptr_t)((uintptr_t)newMainNode->physicalAddress + (newMainNode->startAddress - newMainNode->startAddress)),
                size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0
            );

            // Allocate a new hole within the new MainChainNode
            struct SubChainNode* newHole = mmap(
                (void*)(uintptr_t)((uintptr_t)newMainNode->physicalAddress + (newProcess->endAddress - newMainNode->startAddress + 1)),
                newMainNodeSize - size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0
            );
            newProcess->physicalAddress = (void*)(uintptr_t)((uintptr_t)newMainNode->physicalAddress + (newMainNode->startAddress - newMainNode->startAddress));
            newHole->physicalAddress = (void*)(uintptr_t)((uintptr_t)newMainNode->physicalAddress + (newProcess->endAddress - newMainNode->startAddress + 1));
            newProcess->processOrHole = 'P';
            newHole->processOrHole = 'H';
            newProcess->prevNode = NULL;
            newProcess->nextNode = newHole;
            newHole->prevNode = newProcess;
            newHole->nextNode = NULL;
            newProcess->size = size;
            newHole->size = newMainNodeSize - size;
            newProcess->startAddress = newMainNode->startAddress;
            newProcess->endAddress = (newProcess->startAddress) + size - 1;
            newHole->startAddress = newProcess->endAddress + 1;
            newHole->endAddress = (newHole->startAddress) + (newHole->size) - 1;
            newMainNode->subChainHead = newProcess;

            // Update the startingVirtualAddress
            startingVirtualAddress += newMainNodeSize;
            return (void*)(uintptr_t)newProcess->startAddress;
        } else if (newMainNodeSize == size) {
            // Allocate a new process block for the entire MainChainNode
            struct SubChainNode* newProcess = mmap(
                (void*)(uintptr_t)((uintptr_t)newMainNode->physicalAddress + (newMainNode->startAddress - newMainNode->startAddress)),
                size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0
            );
            newProcess->physicalAddress = (void*)(uintptr_t)((uintptr_t)newMainNode->physicalAddress + (newMainNode->startAddress - newMainNode->startAddress));
            newProcess->startAddress = newMainNode->startAddress;
            newProcess->endAddress = (newProcess->startAddress) + size - 1;
            newProcess->processOrHole = 'P';
            newProcess->size = size;
            newProcess->prevNode = NULL;
            newProcess->nextNode = NULL;
            newMainNode->subChainHead = newProcess;

            // Update the startingVirtualAddress
            startingVirtualAddress += newMainNodeSize;
            return (void*)(uintptr_t)newProcess->startAddress;
        }
    }
    return NULL; // Return NULL at the end if allocation fails
}


void* mems_get(void* v_ptr) {
    // Return the MeMS physical address mapped to the given MeMS virtual address
    if (v_ptr == NULL) {
        return NULL;
    }

    uintptr_t virtual_addr = (uintptr_t)v_ptr;
    struct MainChainNode* mainNode = mainChainHead;

    while (mainNode != NULL) {
        struct SubChainNode* subNode = mainNode->subChainHead;

        while (subNode != NULL) {
            uintptr_t start_addr = (uintptr_t)subNode->startAddress;
            uintptr_t end_addr = (uintptr_t)subNode->endAddress;

            if (virtual_addr >= start_addr && virtual_addr <= end_addr) {
                return (void*)(uintptr_t)subNode->physicalAddress;
            }

            subNode = subNode->nextNode;
        }

        mainNode = mainNode->nextNode;
    }

    return NULL;  // MeMS virtual address not found
}

void mems_free(void* ptr) {
    struct MainChainNode* currentMain = mainChainHead;

    while (currentMain != NULL) {
        struct SubChainNode* currentSub = currentMain->subChainHead;

        while (currentSub != NULL) {
            // Check if the provided pointer is within the bounds of the current sub-chain
            if (ptr >= (void*)(uintptr_t)(currentSub->startAddress) && 
                ptr <= (void*)(uintptr_t)(currentSub->endAddress)) {
                currentSub->processOrHole = 'H'; // Mark as HOLE

                if (currentSub->nextNode != NULL && currentSub->prevNode != NULL) {
                    if (currentSub->nextNode->processOrHole == 'H' && currentSub->prevNode->processOrHole == 'H') {
                        // Merge with both the previous and next holes
                        currentSub->prevNode->nextNode = currentSub->nextNode;
                        currentSub->nextNode->prevNode = currentSub->prevNode;
                        currentSub->prevNode->size += currentSub->size + currentSub->nextNode->size;

                        // Update start and end addresses
                        currentSub->prevNode->startAddress = currentSub->prevNode->startAddress;
                        currentSub->prevNode->endAddress = currentSub->nextNode->endAddress;

                        // Deallocate the currentSub node
                        munmap(currentSub, PAGE_SIZE);
                    }
                    else if (currentSub->nextNode->processOrHole == 'H' && currentSub->prevNode->processOrHole == 'P') {
                        // Merge with the next hole
                        currentSub->prevNode->nextNode = currentSub->nextNode;
                        currentSub->nextNode->prevNode = currentSub->prevNode;
                        currentSub->nextNode->size += currentSub->size;

                        // Update start and end addresses
                        currentSub->nextNode->startAddress = currentSub->startAddress;
                        currentSub->nextNode->endAddress = currentSub->nextNode->endAddress;

                        // Deallocate the currentSub node
                        munmap(currentSub, PAGE_SIZE);
                    }
                    else if (currentSub->nextNode->processOrHole == 'P' && currentSub->prevNode->processOrHole == 'H') {
                        // Merge with the previous hole
                        currentSub->prevNode->nextNode = currentSub->nextNode;
                        currentSub->nextNode->prevNode = currentSub->prevNode;
                        currentSub->prevNode->size += currentSub->size;

                        // Update start and end addresses
                        currentSub->prevNode->startAddress = currentSub->prevNode->startAddress;
                        currentSub->prevNode->endAddress = currentSub->endAddress;

                        // Deallocate the currentSub node
                        munmap(currentSub, PAGE_SIZE);
                    }
                }
                return; 
            }

            currentSub = currentSub->nextNode;
        }
        currentMain = currentMain->nextNode;
    }
}


void mems_print_stats() {
    printf("\n----- MeMS SYSTEM STATS -----\n");
    int i = 0;
    int pages = 0;
    int mainNodes = 0;
    size_t unusedSize = 0;

    struct MainChainNode* mainNode = mainChainHead;
    while (mainNode != NULL) {
        mainNodes++;
        mainNode = mainNode->nextNode;
    }

    int subNodesArray[mainNodes];
    struct MainChainNode* cursorMainNode = mainChainHead;
    while (cursorMainNode != NULL) {
        int subNodes = 0;
        pages += cursorMainNode->pages;
        printf("MAIN[%d:%d] -> ", (int)(cursorMainNode->startAddress), (int)((cursorMainNode->startAddress) + (cursorMainNode->size) - 1));
        struct SubChainNode* cursorSubNode = cursorMainNode->subChainHead;
        while (cursorSubNode != NULL) {
            subNodes++;
            if (cursorSubNode->processOrHole == 'P') {
                printf("P");
            } else if (cursorSubNode->processOrHole == 'H') {
                printf("H");
                unusedSize += cursorSubNode->size;
            }
            printf("[%d:%d] <-> ", (int)cursorSubNode->startAddress, (int)cursorSubNode->endAddress);
            cursorSubNode = cursorSubNode->nextNode;
        }
        subNodesArray[i] = subNodes;
        i++;
        printf("NULL\n");
        cursorMainNode = cursorMainNode->nextNode;
    }
    printf("Pages Used: %d\n", pages);
    printf("Space Unused: %ld\n", unusedSize);
    printf("Main Chain Length: %d\n", mainNodes);
    printf("Sub-Chain Length Array: [");
    for (int j = 0; j < mainNodes; j++) {
        printf("%d,", subNodesArray[j]);
    }
    printf("]\n");
    printf("\n-----------------------------\n");
}

void mems_finish() {
    // Cleanup function
    struct MainChainNode* current = mainChainHead;
    while (current) {
        struct SubChainNode* subChain = current->subChainHead;
        while (subChain) {
            struct SubChainNode* temp = subChain;
            subChain = subChain->nextNode;
            munmap(temp, sizeof(struct SubChainNode));
        }

        struct MainChainNode* tempMain = current;
        current = current->nextNode;
        munmap(tempMain, sizeof(struct MainChainNode));
    }

    // After deallocating all memory, set mainChainHead to NULL
    mainChainHead = NULL;
}


// int main(int argc, char const *argv[]) {
//     // Initialize the MeMS system
//     mems_init();
//     int* ptr[10];

//     /*
//     This allocates 10 arrays of 250 integers each
//     */
//     printf("\n------- Allocated virtual addresses [mems_malloc] -------\n");
//     for(int i = 0; i < 10; i++) {
//         ptr[i] = (int*)mems_malloc(sizeof(int) * 250);
//         printf("Virtual address: %lu\n", (unsigned long)ptr[i]);
//     }

//     /*
//     In this section, we are trying to write a value to the 1st index of array[0] (here it is 0-based indexing).
//     We get the value of both the 0th index and 1st index of array[0] using the function mems_get.
//     Then we write a value to the 1st index using the 1st index pointer and try to access it via the 0th index pointer.

//     This section is to show that even if we have allocated an array using mems_malloc, we can retrieve MeMS physical addresses for any element from that array using mems_get.
//     */
//     printf("\n------ Assigning value to Virtual address [mems_get] -----\n");
//     // How to write to the virtual address of the MeMS (this is given to show that the system works on arrays as well)
//     int* phy_ptr = (int*)mems_get(&ptr[0][1]); // Get the address of index 1
//     phy_ptr[0] = 200; // Put value at index 1
//     int* phy_ptr2 = (int*)mems_get(&ptr[0][0]); // Get the address of index 0
//     // printf("Virtual address: %p\tPhysical Address: %p\n", (void*)(intptr_t)ptr[0], (void*)(intptr_t)phy_ptr2);
//     printf("Virtual address: %lu\tPhysical Address: %lu\n",(unsigned long)ptr[0],(unsigned long)phy_ptr2);
//     printf("Value written: %d\n", phy_ptr[0]); // Print the value at index 1


//     /*
//     This shows the stats of the MeMS system.
//     */
//     printf("\n--------- Printing Stats [mems_print_stats] --------\n");
//     mems_print_stats();

//     /*
//     This section shows the effect of freeing up space on the free list and also the effect of reallocating the space that will be fulfilled by the free list.
//     */
//     printf("\n--------- Freeing up the memory [mems_free] --------\n");
//     mems_free(ptr[3]);
//     mems_print_stats();
//     ptr[3] = (int*)mems_malloc(sizeof(int) * 250);
//     mems_print_stats();

//     // Clean up the MeMS system
//     mems_finish();

//     return 0;
// }

int main(int argc, char const *argv[])
{
    // initialise the MeMS system 
    mems_init();
    int* ptr[10];

    /*
    This allocates 10 arrays of 250 integers each
    */
    printf("\n------- Allocated virtual addresses [mems_malloc] -------\n");
    for(int i=0;i<10;i++){
        ptr[i] = (int*)mems_malloc(sizeof(int)*250);
        printf("Virtual address: %lu\n", (unsigned long)ptr[i]);
    }

    /*
    In this section we are tring to write value to 1st index of array[0] (here it is 0 based indexing).
    We get get value of both the 0th index and 1st index of array[0] by using function mems_get.
    Then we write value to 1st index using 1st index pointer and try to access it via 0th index pointer.

    This section is show that even if we have allocated an array using mems_malloc but we can 
    retrive MeMS physical address of any of the element from that array using mems_get. 
    */
    printf("\n------ Assigning value to Virtual address [mems_get] -----\n");
    // how to write to the virtual address of the MeMS (this is given to show that the system works on arrays as well)
    int* phy_ptr= (int*) mems_get(&ptr[0][1]); // get the address of index 1
    phy_ptr[0]=200; // put value at index 1
    int* phy_ptr2= (int*) mems_get(&ptr[0][0]); // get the address of index 0
    printf("Virtual address: %lu\tPhysical Address: %lu\n",(unsigned long)ptr[0],(unsigned long)phy_ptr2);
    printf("Value written: %d\n", phy_ptr2[1]); // print the address of index 1 

    /*
    This shows the stats of the MeMS system.  
    */
    printf("\n--------- Printing Stats [mems_print_stats] --------\n");
    mems_print_stats();

    /*
    This section shows the effect of freeing up space on free list and also the effect of 
    reallocating the space that will be fullfilled by the free list.
    */
    printf("\n--------- Freeing up the memory [mems_free] --------\n");
    mems_free(ptr[3]);
    mems_print_stats();
    ptr[3] = (int*)mems_malloc(sizeof(int)*250);
    mems_print_stats();

    printf("\n--------- Unmapping all memory [mems_finish] --------\n\n");
    mems_finish();
    return 0;
}