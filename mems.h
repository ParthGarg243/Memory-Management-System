/*
All the main functions with respect to the MeMS are inplemented here
read the function discription for more details

NOTE: DO NOT CHANGE THE NAME OR SIGNATURE OF FUNCTIONS ALREADY PROVIDED
you are only allowed to implement the functions 
you can also make additional helper functions a you wish

REFER DOCUMENTATION FOR MORE DETAILS ON FUNSTIONS AND THEIR FUNCTIONALITY
*/
// add other headers as required
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>

struct SubChainNode {
    int* physicalAddress;
    int startAddress; 
    int endAddress;
    size_t size;
    char processOrHole;
    struct SubChainNode* nextNode;
    struct SubChainNode* prevNode;
};

struct MainChainNode { //you cannot go back to the mainchain from the subchain
    int* physicalAddress;
    int pages;
    int startAddress;
    size_t size;
    struct SubChainNode* subChainHead;
    struct MainChainNode* nextNode;
    struct MainChainNode* prevNode;
};

struct MainChainNode* mainChainHead;
int startingVirtualAddress;

/*
Use this macro where ever you need PAGE_SIZE.
As PAGESIZE can differ system to system we should have flexibility to modify this 
macro to make the output of all system same and conduct a fair evaluation. 
*/
#define PAGE_SIZE 4096


/*
Initializes all the required parameters for the MeMS system. The main parameters to be initialized are:
1. the head of the free list i.e. the pointer that points to the head of the free list
2. the starting MeMS virtual address from which the heap in our MeMS virtual address space will start.
3. any other global variable that you want for the MeMS implementation can be initialized here.
Input Parameter: Nothing
Returns: Nothing
*/
void mems_init() {
    mainChainHead = NULL;
    startingVirtualAddress = 1000;
}


/*
This function will be called at the end of the MeMS system and its main job is to unmap the 
allocated memory using the munmap system call.
Input Parameter: Nothing
Returns: Nothing
*/
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


/*
Allocates memory of the specified size by reusing a segment from the free list if 
a sufficiently large segment is available. 

Else, uses the mmap system call to allocate more memory on the heap and updates 
the free list accordingly.

Note that while mapping using mmap do not forget to reuse the unused space from mapping
by adding it to the free list.
Parameter: The size of the memory the user program wants
Returns: MeMS Virtual address (that is created by MeMS)
*/ 

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

    if (found == 1) {// what does intptr_t do?
        if (size == finalSubNode -> size) {
            finalSubNode->processOrHole = 'P';
            return (void*)(long)finalSubNode->startAddress;
        } 
        else if (size < finalSubNode->size){
            struct SubChainNode* newHole = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            newHole->processOrHole = 'H';
            newHole -> nextNode = finalSubNode -> nextNode;
            newHole -> prevNode = finalSubNode;
            finalSubNode -> nextNode = newHole;
            if (newHole -> nextNode != NULL) {
                newHole -> nextNode -> prevNode = newHole;
            }          
            newHole->size = (finalSubNode->size) - size;
            newHole->endAddress = finalSubNode->endAddress;
            finalSubNode->size = size;
            finalSubNode->endAddress = (finalSubNode->startAddress) + size - 1;
            newHole->startAddress = (finalSubNode->endAddress) + 1;
            newHole->physicalAddress = (int*)((long) cursorMainNode -> physicalAddress + newHole -> startAddress - cursorMainNode -> startAddress);
            finalSubNode->processOrHole = 'P';
            return (void*)(long)finalSubNode->startAddress;
        }
    } 
    else if (found == 0) {
        size_t pages = (size / PAGE_SIZE) + 1;
        size_t newMainNodeSize = pages * PAGE_SIZE;
        struct MainChainNode* newMainNode = (struct MainChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        int* address = (int *) mmap(NULL, newMainNodeSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        newMainNode->size = newMainNodeSize;
        newMainNode->physicalAddress = (int*)address;
        newMainNode->pages = pages;

        if (mainChainHead == NULL) {
            mainChainHead = newMainNode;
            newMainNode->startAddress = startingVirtualAddress;
            newMainNode->nextNode = NULL;
            newMainNode->prevNode = NULL;
            newMainNode->subChainHead = NULL;
        } 
        else if (mainChainHead) {
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
            struct SubChainNode* newProcess = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            struct SubChainNode* newHole = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
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
            newProcess->physicalAddress = (int *)((long) newMainNode -> physicalAddress + newProcess -> startAddress - newMainNode -> startAddress);
            newHole->physicalAddress = (int *)((long) newMainNode -> physicalAddress + newHole -> startAddress - newMainNode -> startAddress);
            newMainNode->subChainHead = newProcess;

            // Update the startingVirtualAddress
            startingVirtualAddress += newMainNodeSize;
            return (void*)(long)newProcess->startAddress;
        } 
        else if (newMainNodeSize == size) {
            struct SubChainNode* newProcess = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            newProcess->startAddress = newMainNode->startAddress;
            newProcess->endAddress = (newProcess->startAddress) + size - 1;
            newProcess->processOrHole = 'P';
            newProcess->size = size;
            newProcess->prevNode = NULL;
            newProcess->nextNode = NULL;
            newProcess->physicalAddress = (int*)((long) newMainNode -> physicalAddress + newProcess -> startAddress - newMainNode -> startAddress);
            newMainNode->subChainHead = newProcess;

            // Update the startingVirtualAddress
            startingVirtualAddress += newMainNodeSize;
            return (void*)(long)newProcess->startAddress;
        }
    }
    return NULL; // Return NULL at the end if allocation fails
}

/*
this function print the stats of the MeMS system like
1. How many pages are utilised by using the mems_malloc
2. how much memory is unused i.e. the memory that is in freelist and is not used.
3. It also prints details about each node in the main chain and each segment (PROCESS or HOLE) in the sub-chain.
Parameter: Nothing
Returns: Nothing but should print the necessary information on STDOUT
*/
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
            printf("[%d:%d] <-> ", (int)(cursorSubNode->startAddress), (int)(cursorSubNode->endAddress));
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


/*
Returns the MeMS physical address mapped to ptr ( ptr is MeMS virtual address).
Parameter: MeMS Virtual address (that is created by MeMS)
Returns: MeMS physical address mapped to the passed ptr (MeMS virtual address).
*/

void* mems_get(void* v_ptr) {
    long v_ptr_int=(long) v_ptr;
    // Return the MeMS physical address mapped to the given MeMS virtual address
    struct MainChainNode* mainNode = mainChainHead;
    while (mainNode!=NULL) {
        if (mainNode -> startAddress <= v_ptr_int && mainNode -> startAddress + mainNode -> size >= v_ptr_int) {
            return (void*)(long) mainNode -> physicalAddress + v_ptr_int - mainNode -> startAddress;
        }
        mainNode = mainNode -> nextNode;
    }
    // return NULL;  // MeMS virtual address not found
}


/*
this function free up the memory pointed by our virtual_address and add it to the free list
Parameter: MeMS Virtual address (that is created by MeMS) 
Returns: nothing
*/
void mems_free(void* ptr) {
    struct MainChainNode* currentMain = mainChainHead;

    while (currentMain != NULL) {
        struct SubChainNode* currentSub = currentMain->subChainHead;

        while (currentSub != NULL) {
            // Check if the provided pointer is within the bounds of the current sub-chain
            if (ptr >= (void*)(intptr_t)(currentSub->startAddress) && 
                ptr <= (void*)(intptr_t)(currentSub->endAddress)) {
                currentSub->processOrHole = 'H'; // Mark as HOLE

                if (currentSub->nextNode != NULL && currentSub->prevNode != NULL) {
                    if (currentSub->nextNode->processOrHole == 'H' && currentSub->prevNode->processOrHole == 'H') {
                        // Merge with both the previous and next holes
                        currentSub->prevNode->nextNode = currentSub->nextNode;
                        if(currentSub->nextNode) currentSub->nextNode->prevNode = currentSub->prevNode;
                        currentSub->prevNode->size += currentSub->size + currentSub->nextNode->size;

                        // Update start and end addresses
                        currentSub->prevNode->startAddress = currentSub->prevNode->startAddress;
                        currentSub->prevNode->endAddress = currentSub->nextNode->endAddress;

                        // Deallocate the currentSub node
                        struct SubChainNode* nextPNode = currentSub->nextNode->nextNode;
                        
                        currentSub->prevNode->nextNode=nextPNode;
                        if(nextPNode) nextPNode->prevNode=currentSub->prevNode;
                    

                        munmap(currentSub, PAGE_SIZE);
                        // munmap(currentSub->nextNode, PAGE_SIZE);

                    
                        
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
                else if(currentSub->nextNode==NULL){
                    if(currentSub->prevNode->processOrHole=='P'){
                        currentSub->processOrHole='H';
                    }
                    else{
                        currentSub->prevNode->nextNode=NULL;
                        currentSub->prevNode->endAddress = currentSub->endAddress;
                    }
                }
                
                return; 
            }

            currentSub = currentSub->nextNode;
        }
        currentMain = currentMain->nextNode;
    }
}