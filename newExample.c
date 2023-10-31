#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

#define PAGE_SIZE 4096

// Memory Segment structure
typedef struct SubNode {
    void* mem_ptr;  // MeMS virtual address
    size_t size;    // Size of the segment
    char is_allocated;  // 'P' for Process, 'H' for Hole
    struct SubNode* nextNode;
    struct SubNode* prevNode;
} SubNode;

// Main Chain Node structure
typedef struct MainNode {
    SubNode* sub_chain;
    size_t size;
    struct MainNode* nextNode;
    struct MainNode* prevNode;
} MainNode;

MainNode* head;  // Head of the free list
void* mem_start;     // Starting MeMS virtual address

// Function to initialize the MeMS system
void mems_init() {
    // You can perform any necessary initialization here
    // For example, initializing the head of the free list or other global variables
    head = NULL;  // Initialize the head of the free list
    mem_start = NULL; // Initialize the starting MeMS virtual address
}

// Function to allocate memory using MeMS
void* mems_malloc(size_t size) {
    int found = 0; 
    SubNode* finalSubNode = NULL;
    SubNode* cursorSubNode = NULL;
    MainNode* cursorMainNode = head;
    while (cursorMainNode != NULL) {
        cursorSubNode = cursorMainNode -> sub_chain;
        while (cursorSubNode != NULL) {
            if (cursorSubNode -> is_allocated == 'H') {
                if (cursorSubNode -> size >= size) {
                    finalSubNode = cursorSubNode;
                    found = 1;
                    break;
                }
            }
            cursorSubNode = cursorSubNode -> nextNode;
        }
        if (found == 1) {
            break;
        }
        cursorMainNode = cursorMainNode -> nextNode;
    }
    if (found == 1) {
        if (size == finalSubNode -> size) {
            finalSubNode -> is_allocated = 'P';
            return finalSubNode -> mem_ptr;
        }
        else {      // size <= finalSubNode -> size
            SubNode* newHole = (SubNode*) malloc(sizeof(SubNode));
            newHole -> prevNode = finalSubNode;
            newHole -> nextNode = finalSubNode -> nextNode;
            finalSubNode -> nextNode = newHole;
            newHole -> size = (finalSubNode -> size) - size;
            newHole -> mem_ptr = (finalSubNode -> mem_ptr) + size;
            newHole -> is_allocated = 'H';
            finalSubNode -> size = size;
        }
    }
    else {
        size_t newMainNodeSize = ((int) (size/PAGE_SIZE) + 1) * PAGE_SIZE;
        void* mem_ptr = mmap(NULL, newMainNodeSize, PROT_WRITE, MAP_PRIVATE, -1, 0);
        MainNode* newMainNode = (MainNode*) malloc(sizeof(MainNode));
        newMainNode -> size = newMainNodeSize;
        if (head == NULL) {
            newMainNode -> nextNode = NULL;
            newMainNode -> prevNode = NULL;
            head = newMainNode;
        }
        else {
            MainNode* anotherCursorMainNode = head;
            while (anotherCursorMainNode -> nextNode != NULL) {
                anotherCursorMainNode = anotherCursorMainNode -> nextNode;
            }
            anotherCursorMainNode -> nextNode = newMainNode;
            newMainNode -> nextNode = NULL;
            newMainNode -> prevNode = anotherCursorMainNode;
        }
        if (newMainNodeSize > size) {
            SubNode* newProcess = (SubNode*) malloc(sizeof(SubNode));
            SubNode* newHole = (SubNode*) malloc(sizeof(SubNode));
            newProcess -> mem_ptr = mem_ptr;
            newProcess -> nextNode = newHole;
            newProcess -> is_allocated = 'P';
            newProcess -> size = size;
            newProcess -> prevNode = NULL;
            newHole -> mem_ptr = mem_ptr + size;
            newHole -> nextNode = NULL;
            newHole -> is_allocated = 'H';
            newHole -> size = newMainNodeSize - size;
            newHole -> prevNode = newProcess;
            newMainNode -> sub_chain = newProcess;
        }
        else if (newMainNodeSize == size) {
            SubNode* newProcess = (SubNode*) malloc(sizeof(SubNode));
            newProcess -> mem_ptr = mem_ptr;
            newProcess -> nextNode = NULL;
            newProcess -> is_allocated = 'P';
            newProcess -> size = size;
            newProcess -> prevNode = NULL;
            newMainNode -> sub_chain = newProcess;
        }
    }
}

// // Function to allocate memory using MeMS
// void* mems_malloc(size_t size) {
//     SubNode* segment = NULL;
//     // Iterate through the free list to find a suitable segment
//     MainNode* main_chain_node = head;
//     while (main_chain_node != NULL) {
//         SubNode* sub_chain = main_chain_node -> sub_chain;
//         while (sub_chain != NULL) {
//             if (sub_chain -> is_allocated == 0 && sub_chain -> size >= size) {
//                 segment = sub_chain;
//                 break;
//             }
//             sub_chain = sub_chain -> nextNode;
//         }
//         if (segment != NULL) {
//             break;
//         }
//         main_chain_node = main_chain_node -> nextNode;
//     }

//     if (segment != NULL) {
//         // Allocate the requested memory from the segment
//         segment -> is_allocated = 'P';
//         return segment -> mem_ptr;
//     } 
//     else {
//         // Use mmap to allocate a new segment
//         size_t required_size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
//         void* new_mem = mmap(NULL, required_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
//         if (new_mem == MAP_FAILED) {
//             // Handle mmap error
//             return NULL;
//         }
        
//         SubNode* new_segment = (SubNode*)malloc(sizeof(SubNode));
//         new_segment -> mem_ptr = new_mem;
//         new_segment -> size = required_size;
//         new_segment -> is_allocated = 1;
//         // Add the new segment to the free list
//         // ...

//         return new_mem;
//     }
// }

void mems_free(void* ptr) {
     
}

// // Function to free memory using MeMS
// void mems_free(void* ptr) {
//     // Find the segment in the free list based on the MeMS virtual address (ptr)
//     MainNode* main_chain_node = head;
//     while (main_chain_node != NULL) {
//         SubNode* sub_chain = main_chain_node -> sub_chain;
//         while (sub_chain != NULL) {
//             if (sub_chain -> mem_ptr == ptr) {
//                 // Mark the segment as HOLE
//                 sub_chain -> is_allocated = 'H';
//                 return;
//             }
//             sub_chain = sub_chain -> nextNode;
//         }
//         main_chain_node = main_chain_node -> nextNode;
//     }

//     // If the segment with the provided MeMS virtual address is not found, you can handle this as an error.
//     // You may print an error message or take appropriate action as needed.
//     // For example:
//     fprintf(stderr, "Error: Segment with MeMS virtual address %p not found in the free list.\n");
//     // You might also consider returning an error code to indicate failure.
// }

// Function to print MeMS statistics
void mems_print_stats() {
    // Initialize variables to store statistics
    size_t total_mapped_pages = 0;
    size_t total_unused_memory = 0;

    // Iterate through the free list and print details
    MainNode* main_chain_node = head;
    while (main_chain_node != NULL) {
        SubNode* sub_chain = main_chain_node -> sub_chain;
        while (sub_chain != NULL) {
            // Print details about the segment
            printf("Segment at MeMS virtual address: %p\n", sub_chain -> mem_ptr);
            printf("Size: %zu bytes\n", sub_chain -> size);
            printf("Status: ");
            if (sub_chain -> is_allocated == 'H') {
                printf("Hole\n");
            }
            else {
                printf("Process\n");
            }

            // Update statistics
            total_mapped_pages += sub_chain -> size / PAGE_SIZE;
            if (sub_chain -> is_allocated == 'H') {
                total_unused_memory += sub_chain -> size;
            }

            sub_chain = sub_chain -> nextNode;
        }

        main_chain_node = main_chain_node -> nextNode;
    }

    // Print total statistics
    printf("Total Mapped Pages: %zu\n", total_mapped_pages);
    printf("Total Unused Memory: %zu bytes\n", total_unused_memory);
}


void* mems_get(void* v_ptr) {
    // Iterate through the free list to find the segment with the matching MeMS virtual address
    MainNode* main_chain_node = head;
    while (main_chain_node != NULL) {
        SubNode* sub_chain = main_chain_node -> sub_chain;
        while (sub_chain != NULL) {
            if (sub_chain -> mem_ptr == v_ptr) {
                // Found the segment with the matching virtual address
                return sub_chain -> mem_ptr;  // Return the MeMS physical address
            }
            sub_chain = sub_chain -> nextNode;
        }
        main_chain_node = main_chain_node -> nextNode;
    }

    // If the segment with the provided virtual address is not found, return NULL
    return NULL;
}


// Function to clean up the MeMS system
// Function to clean up the MeMS system
void mems_finish() {
    // Iterate through the free list and unmap all allocated memory segments
    MainNode* main_chain_node = head;
    while (main_chain_node != NULL) {
        SubNode* sub_chain = main_chain_node -> sub_chain;
        while (sub_chain != NULL) {
            if (sub_chain -> is_allocated == 'P') {
                // Unmap the allocated memory segment
                munmap(sub_chain -> mem_ptr, sub_chain -> size);
            }
            sub_chain = sub_chain -> nextNode;
        }
        main_chain_node = main_chain_node -> nextNode;
    }
}


// include other header files as needed
#include"mems.h"


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
    return 0;
}