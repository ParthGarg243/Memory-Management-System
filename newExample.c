// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/mman.h>

// #define PAGE_SIZE 4096

// // Define structures for the main chain and sub-chain nodes
// struct SubChainNode {
//     size_t size;
//     int is_process;
//     struct SubChainNode* next;
//     struct SubChainNode* prev;
// };

// struct MainChainNode {
//     void* start_address;
//     size_t num_pages;
//     size_t size;
//     struct SubChainNode* sub_chain_head;
//     struct MainChainNode* next;
//     struct MainChainNode* prev;
// };

// // Define global variables to maintain MeMS state
// struct MainChainNode* free_list_head = NULL;
// void* mems_virtual_address = (void*)1000;  // Starting MeMS virtual address

// void mems_init() {
//     // Initialize any required parameters for the MeMS system
//     free_list_head = (struct MainChainNode*)mmap(mems_virtual_address, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//     if (free_list_head == MAP_FAILED) {
//         perror("mmap");
//         exit(1);
//     }
//     free_list_head->start_address = mems_virtual_address;
//     free_list_head->num_pages = 1;
//     free_list_head->size = PAGE_SIZE;
//     free_list_head->sub_chain_head = NULL;
//     free_list_head->next = free_list_head->prev = NULL;
// }

// void* mems_get(void* v_ptr) {
//     // Return the MeMS physical address mapped to the given MeMS virtual address
//     struct MainChainNode* main_node = free_list_head;
//     while (main_node) {
//         struct SubChainNode* sub_node = main_node->sub_chain_head;
//         while (sub_node) {
//             if (sub_node->is_process && (v_ptr >= main_node->start_address + sub_node->size) &&
//                 (v_ptr < main_node->start_address + sub_node->size + sub_node->size)) {
//                 // MeMS virtual address is within this sub-chain node
//                 size_t offset = (size_t)(v_ptr - main_node->start_address - sub_node->size);
//                 return main_node->start_address + offset;
//             }
//             sub_node = sub_node->next;
//         }
//         main_node = main_node->next;
//     }
//     return NULL;  // MeMS virtual address not found
// }

// struct SubChainNode* createSubChainNode(size_t size, int is_process) {
//     struct SubChainNode* node = (struct SubChainNode*)mmap(NULL, sizeof(struct SubChainNode), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//     node->size = size;
//     node->is_process = is_process;
//     node->prev = NULL;
//     node->next = NULL;
//     return node;
// }

// struct MainChainNode* createMainChainNode(size_t num_pages, size_t size) {
//     struct MainChainNode* node = (struct MainChainNode*)mmap(mems_virtual_address, sizeof(struct MainChainNode), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
//     if (node == MAP_FAILED) {
//         perror("mmap");
//         exit(1);
//     }
//     mems_virtual_address += sizeof(struct MainChainNode);
//     node->start_address = mems_virtual_address;
//     node->num_pages = num_pages;
//     node->size = size;
//     node->sub_chain_head = createSubChainNode(size, 1);  // Initial segment is PROCESS
//     node->next = NULL;
//     node->prev = NULL;
//     if (free_list_head == NULL) {
//         free_list_head = node;
//     } else {
//         struct MainChainNode* current = free_list_head;
//         while (current->next) {
//             current = current->next;
//         }
//         current->next = node;
//         node->prev = current;
//     }
//     return node;
// }

// void* mems_malloc(size_t size) {
//     struct MainChainNode* current = free_list_head;
//     while (current) {
//         struct SubChainNode* sub_chain = current->sub_chain_head;
//         while (sub_chain) {
//             if (!sub_chain->is_process && sub_chain->size >= size) {
//                 // Allocate from the hole
//                 if (sub_chain->size == size) {
//                     sub_chain->is_process = 1; // Mark as PROCESS
//                     return (void*)(current->start_address + current->size - sub_chain->size);
//                 } else {
//                     // Create a new hole from the remaining space
//                     struct SubChainNode* new_hole = createSubChainNode(sub_chain->size - size, 0); // Mark as HOLE
//                     new_hole->next = sub_chain->next;
//                     new_hole->prev = sub_chain;
//                     sub_chain->next = new_hole;
//                     sub_chain->size = size;
//                     sub_chain->is_process = 1; // Mark as PROCESS

//                     // Update mems_virtual_address
//                     mems_virtual_address += size;

//                     return (void*)(current->start_address + current->size - sub_chain->size);
//                 }
//             }
//             sub_chain = sub_chain->next;
//         }
//         current = current->next;
//     }

//     // If there are no holes to allocate from, create a new node
//     current = createMainChainNode((size + PAGE_SIZE - 1) / PAGE_SIZE, size);

//     // Update mems_virtual_address
//     mems_virtual_address += size;

//     return (void*)(current->start_address);
// }


// void mems_free(void* ptr) {
//     // Memory deallocation function
//     struct MainChainNode* current = free_list_head;
//     while (current) {
//         if (ptr >= current->start_address && ptr < current->start_address + current->size) {
//             struct SubChainNode* sub_chain = current->sub_chain_head;
//             while (sub_chain) {
//                 if (ptr >= current->start_address + current->size - sub_chain->size && ptr < current->start_address + current->size) {
//                     sub_chain->is_process = 0; // Mark as HOLE
//                     return;
//                 }
//                 sub_chain = sub_chain->next;
//             }
//         }
//         current = current->next;
//     }
// }

// void mems_print_stats() {
//     // Memory statistics function
//     struct MainChainNode* current = free_list_head;
//     while (current) {
//         printf("MAIN[%lu:%lu]-> ", (size_t)current->start_address, (size_t)(current->start_address + current->size - 1));

//         struct SubChainNode* sub_chain = current->sub_chain_head;
//         while (sub_chain) {
//             printf("%s[%lu:%lu] <-> ", sub_chain->is_process ? "P" : "H",
//                    (size_t)(current->start_address + current->size - sub_chain->size),
//                    (size_t)(current->start_address + current->size - 1));
//             sub_chain = sub_chain->next;
//         }

//         printf("\n");
//         current = current->next;
//     }
// }

// void mems_finish() {
//     // Cleanup function
//     struct MainChainNode* current = free_list_head;
//     while (current) {
//         struct SubChainNode* sub_chain = current->sub_chain_head;
//         while (sub_chain) {
//             struct SubChainNode* temp = sub_chain;
//             sub_chain = sub_chain->next;
//             munmap(temp, sizeof(struct SubChainNode));
//         }

//         free_list_head = current;
//         current = current->next;
//         munmap(current, sizeof(struct MainChainNode));
//     }
// }

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
//         printf("Virtual address: %p\n", ptr[i]);
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
//     printf("Virtual address: %p\tPhysical Address: %p\n", ptr[0], phy_ptr2);
//     printf("Value written: %d\n", phy_ptr2[1]); // Print the value at index 1

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

struct MainChainNode {
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
            printf("1) bleh\n");
            return (void*)(intptr_t)finalSubNode->startAddress;
        } 
        else if (size < finalSubNode->size){
            struct SubChainNode* newHole = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            newHole->physicalAddress = (void*) newHole;
            newHole->processOrHole = 'H';
            newHole -> nextNode = finalSubNode -> nextNode;
            newHole -> prevNode = finalSubNode;
            finalSubNode -> nextNode = newHole;
            if (newHole -> nextNode != NULL) {
                newHole -> nextNode -> prevNode = newHole;
            }          

            // newHole->prevNode = finalSubNode;
            // newHole->nextNode = finalSubNode->nextNode;
            // if (finalSubNode->nextNode) {
            //     finalSubNode->nextNode->prevNode = newHole;
            // }
            // finalSubNode->nextNode = newHole;
            newHole->size = (finalSubNode->size) - size;
            newHole->endAddress = finalSubNode->endAddress;
            finalSubNode->size = size;
            finalSubNode->endAddress = (finalSubNode->startAddress) + size - 1;
            newHole->startAddress = (finalSubNode->endAddress) + 1;
            printf("%ld\n", newHole -> size);
            printf("2) %ld\n", newHole -> size);
            return (void*)(intptr_t)finalSubNode->startAddress;
        }
    } 
    else {
        size_t pages = (size / PAGE_SIZE) + 1;
        size_t newMainNodeSize = pages * PAGE_SIZE;
        struct MainChainNode* newMainNode = (struct MainChainNode*) mmap(NULL, newMainNodeSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        newMainNode->size = newMainNodeSize;
        newMainNode->physicalAddress = (void*)newMainNode;
        newMainNode->pages = pages;

        if (mainChainHead == NULL) {
            mainChainHead = newMainNode;
            newMainNode->startAddress = startingVirtualAddress;
            newMainNode->nextNode = NULL;
            newMainNode->prevNode = NULL;
            newMainNode->subChainHead = NULL;
        } 
        else {
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
            newProcess->physicalAddress = (void*)newProcess;
            newHole->physicalAddress = (void*)newHole;
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
            printf("3) %ld\n", newHole -> size);
            return (void*)(intptr_t)newProcess->startAddress;
        } 
        else if (newMainNodeSize == size) {
            struct SubChainNode* newProcess = (struct SubChainNode*) mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            newProcess->physicalAddress = (void*)newProcess;
            newProcess->startAddress = newMainNode->startAddress;
            newProcess->endAddress = (newProcess->startAddress) + size - 1;
            newProcess->processOrHole = 'P';
            newProcess->size = size;
            newProcess->prevNode = NULL;
            newProcess->nextNode = NULL;
            newMainNode->subChainHead = newProcess;

            // Update the startingVirtualAddress
            startingVirtualAddress += newMainNodeSize;
            printf("4) bloop\n");
            return (void*)(intptr_t)newProcess->startAddress;
        }
    }
    return NULL; // Return NULL at the end if allocation fails
}


void* mems_get(void* v_ptr) {
    // Return the MeMS physical address mapped to the given MeMS virtual address
    struct MainChainNode* mainNode = mainChainHead;
    while (mainNode) {
        struct SubChainNode* subNode = mainNode->subChainHead;
        while (subNode) {
            if (subNode->processOrHole == 'H') {
                if (v_ptr >= (void*)(intptr_t)(mainNode->startAddress + subNode->size) &&
                    v_ptr < (void*)(intptr_t)(mainNode->startAddress + subNode->size + subNode->size)) {
                    // MeMS virtual address is within this sub-chain node
                    size_t offset = (size_t)(v_ptr - (void*)(intptr_t)(mainNode->startAddress + subNode->size));
                    return (void*)(intptr_t)(mainNode->startAddress + offset);
                }
            }
            subNode = subNode->nextNode;
        }
        mainNode = mainNode->nextNode;
    }
    return NULL;  // MeMS virtual address not found
}

void mems_free(void* ptr) {
    // Memory deallocation function
    struct MainChainNode* current = mainChainHead;
    while (current) {
        if (ptr < (void*)(intptr_t)(current->startAddress + current->size) &&
            ptr < (void*)(intptr_t)(current->startAddress + current->size)) {
            struct SubChainNode* subChain = current->subChainHead;
            while (subChain) {
                if (ptr >= (void*)(intptr_t)(current->startAddress + current->size - subChain->size) &&
                    ptr < (void*)(intptr_t)(current->startAddress + current->size)) {
                    subChain->processOrHole = 'H'; // Mark as HOLE
                    return;
                }
                subChain = subChain->nextNode;
            }
        }
        current = current->nextNode;
    }
}

void mems_print_stats() {
    // Memory statistics function
    struct MainChainNode* current = mainChainHead;
    while (current) {
        printf("MAIN[%p:%p]-> ", (void*)(intptr_t)current->startAddress, (void*)(intptr_t)(current->startAddress + current->size - 1));

        struct SubChainNode* subChain = current->subChainHead;
        while (subChain) {
            printf("%s[%p:%p] <-> ", subChain->processOrHole == 'P' ? "P" : "H",
                   (void*)(intptr_t)(current->startAddress + current->size - subChain->size),
                   (void*)(intptr_t)(current->startAddress + current->size - 1));
            subChain = subChain->nextNode;
        }

        printf("\n");
        current = current->nextNode;
    }
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

        mainChainHead = current;
        current = current->nextNode;
        munmap(current, sizeof(struct MainChainNode));
    }
}

int main(int argc, char const *argv[]) {
    // Initialize the MeMS system
    mems_init();
    int* ptr[10];

    /*
    This allocates 10 arrays of 250 integers each
    */
    printf("\n------- Allocated virtual addresses [mems_malloc] -------\n");
    for(int i = 0; i < 10; i++) {
        ptr[i] = (int*)mems_malloc(sizeof(int) * 250);
        printf("Virtual address: %d\n", (int)(intptr_t)ptr[i]);
    }

    /*
    In this section, we are trying to write a value to the 1st index of array[0] (here it is 0-based indexing).
    We get the value of both the 0th index and 1st index of array[0] using the function mems_get.
    Then we write a value to the 1st index using the 1st index pointer and try to access it via the 0th index pointer.

    This section is to show that even if we have allocated an array using mems_malloc, we can retrieve MeMS physical addresses for any element from that array using mems_get.
    */
    printf("\n------ Assigning value to Virtual address [mems_get] -----\n");
    // How to write to the virtual address of the MeMS (this is given to show that the system works on arrays as well)
    int* phy_ptr = (int*)mems_get(&ptr[0][1]); // Get the address of index 1
    phy_ptr[0] = 200; // Put value at index 1
    int* phy_ptr2 = (int*)mems_get(&ptr[0][0]); // Get the address of index 0
    printf("Virtual address: %p\tPhysical Address: %p\n", (void*)(intptr_t)ptr[0], (void*)(intptr_t)phy_ptr2);
    printf("Value written: %d\n", phy_ptr2[1]); // Print the value at index 1

    /*
    This shows the stats of the MeMS system.
    */
    printf("\n--------- Printing Stats [mems_print_stats] --------\n");
    mems_print_stats();

    /*
    This section shows the effect of freeing up space on the free list and also the effect of reallocating the space that will be fulfilled by the free list.
    */
    printf("\n--------- Freeing up the memory [mems_free] --------\n");
    mems_free(ptr[3]);
    mems_print_stats();
    ptr[3] = (int*)mems_malloc(sizeof(int) * 250);
    mems_print_stats();

    // Clean up the MeMS system
    mems_finish();

    return 0;
}