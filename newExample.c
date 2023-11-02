// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/mman.h>
// #include <errno.h>

// #define PAGE_SIZE 4096

// // Memory Segment structure
// typedef struct SubNode {
//     void* mem_ptr;  // MeMS virtual address
//     size_t size;    // Size of the segment
//     char is_allocated;  // 'P' for Process, 'H' for Hole
//     struct SubNode* nextNode;
//     struct SubNode* prevNode;
// } SubNode;

// // Main Chain Node structure
// typedef struct MainNode {
//     SubNode* sub_chain;
//     size_t size;
//     struct MainNode* nextNode;
//     struct MainNode* prevNode;
//     int pages;
// } MainNode;

// MainNode* head;  // Head of the free list
// void* mem_start;     // Starting MeMS virtual address

// // Function to initialize the MeMS system
// void mems_init() {
//     // You can perform any necessary initialization here
//     // For example, initializing the head of the free list or other global variables
//     head = NULL;  // Initialize the head of the free list
//     mem_start = (void*) 1000; // Initialize the starting MeMS virtual address
// }

// void* mems_malloc(size_t size) {
//     int found = 0;
//     int firstNode = 1;
//     SubNode* finalSubNode = NULL;
//     SubNode* cursorSubNode = NULL;
//     MainNode* cursorMainNode = head;
//     size_t lastAccessedAddress = (size_t) mem_start;
//     while (cursorMainNode != NULL) {
//         cursorSubNode = cursorMainNode -> sub_chain;
//         while (cursorSubNode != NULL) {
//             if (cursorSubNode -> is_allocated == 'H') {
//                 if (cursorSubNode -> size >= size) {
//                     finalSubNode == cursorSubNode;
//                     found = 1;
//                     break;
//                 }
//             }
//             cursorSubNode = cursorSubNode -> nextNode;
//         }
//         if (found == 1) {
//             break;
//         }
//         cursorMainNode = cursorMainNode -> nextNode;
//     }
//     if (found == 1) {
//         if (size == finalSubNode -> size) {
//             finalSubNode -> is_allocated = 'P';
//             return finalSubNode -> mem_ptr;
//         }
//         else { // size < finalSubNode -> size
//             newHole = 
//             finalSubNode -> is_allocated = 'P'
//         }
//     }
// }

// // void* mems_malloc(size_t size) {
// //     int found = 0;
// //     SubNode* finalSubNode = NULL;
// //     SubNode* cursorSubNode = NULL;
// //     MainNode* cursorMainNode = head;

// //     // Initialize the last virtual address to the starting address
// //     size_t lastVirtualAddress = (size_t)mem_start;

// //     while (cursorMainNode != NULL) {
// //         cursorSubNode = cursorMainNode->sub_chain;
// //         while (cursorSubNode != NULL) {
// //             if (cursorSubNode->is_allocated == 'H') {
// //                 if (cursorSubNode->size >= size) {
// //                     finalSubNode = cursorSubNode;
// //                     found = 1;
// //                     break;
// //                 }
// //             }
// //             cursorSubNode = cursorSubNode->nextNode;
// //         }
// //         if (found == 1) {
// //             break;
// //         }
// //         cursorMainNode = cursorMainNode->nextNode;
// //     }
// //     if (found == 1) {
// //         if (size == finalSubNode->size) {
// //             finalSubNode->is_allocated = 'P';
// //             return finalSubNode->mem_ptr;
// //         } 
// //         else {  // size < finalSubNode's size
// //             SubNode* newHole = (SubNode*)malloc(sizeof(SubNode));
// //             newHole->prevNode = finalSubNode;
// //             newHole->nextNode = finalSubNode->nextNode;
// //             finalSubNode->nextNode = newHole;
// //             newHole->size = (finalSubNode->size) - size;

// //             // Update the mem_ptr and lastVirtualAddress
// //             newHole->mem_ptr = (void*)((size_t)finalSubNode->mem_ptr + size);
// //             lastVirtualAddress += size;

// //             newHole->is_allocated = 'H';
// //             finalSubNode->size = size;
// //             return finalSubNode->mem_ptr;
// //         }
// //     } 
// //     else {
// //         size_t newMainNodeSize = ((size + sizeof(SubNode) + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
// //         void* mem_ptr = mmap(NULL, newMainNodeSize, PROT_WRITE, MAP_PRIVATE, -1, 0);
// //         MainNode* newMainNode = (MainNode*)malloc(sizeof(MainNode));
// //         newMainNode->size = newMainNodeSize;
// //         newMainNode->pages = (size + sizeof(SubNode) + PAGE_SIZE - 1) / PAGE_SIZE;

// //         if (head == NULL) {
// //             newMainNode->nextNode = NULL;
// //             newMainNode->prevNode = NULL;
// //             head = newMainNode;
// //         } 
// //         else {
// //             MainNode* anotherCursorMainNode = head;
// //             while (anotherCursorMainNode->nextNode != NULL) {
// //                 anotherCursorMainNode = anotherCursorMainNode->nextNode;
// //             }
// //             anotherCursorMainNode->nextNode = newMainNode;
// //             newMainNode->nextNode = NULL;
// //             newMainNode->prevNode = anotherCursorMainNode;
// //         }

// //         if (newMainNodeSize > size) {
// //             SubNode* newProcess = (SubNode*)malloc(sizeof(SubNode));
// //             SubNode* newHole = (SubNode*)malloc(sizeof(SubNode));
// //             newProcess->mem_ptr = (void*)((size_t)mem_ptr + sizeof(SubNode));
// //             newProcess->nextNode = newHole;
// //             newProcess->is_allocated = 'P';
// //             newProcess->size = size;
// //             newProcess->prevNode = NULL;

// //             // Update the mem_ptr and lastVirtualAddress
// //             newHole->mem_ptr = (void*)lastVirtualAddress;
// //             lastVirtualAddress += size;

// //             newHole->nextNode = NULL;
// //             newHole->is_allocated = 'H';
// //             newHole->size = newMainNodeSize - size - sizeof(SubNode);
// //             newHole->prevNode = newProcess;
// //             newMainNode->sub_chain = newProcess;
// //             return newProcess->mem_ptr;
// //         } 
// //         else if (newMainNodeSize == size) {
// //             SubNode* newProcess = (SubNode*)malloc(sizeof(SubNode));
// //             newProcess->mem_ptr = (void*)lastVirtualAddress;
// //             lastVirtualAddress += size;
// //             newProcess->nextNode = NULL;
// //             newProcess->is_allocated = 'P';
// //             newProcess->size = size;
// //             newProcess->prevNode = NULL;
// //             newMainNode->sub_chain = newProcess;
// //             return newProcess->mem_ptr;
// //         }
// //     }

// //     return NULL;  // Return NULL if allocation fails
// // }

// // Function to free memory using MeMS
// void mems_free(void* ptr) {
//     // Find the segment in the free list based on the MeMS virtual address (ptr)
//     MainNode* main_chain_node = head;
//     while (main_chain_node != NULL) {
//         SubNode* sub_chain = main_chain_node->sub_chain;
//         while (sub_chain != NULL) {
//             if (sub_chain->mem_ptr == ptr) {
//                 // Mark the segment as HOLE
//                 sub_chain->is_allocated = 'H';

//                 // If the next segment is also a HOLE, merge the two HOLEs
//                 if (sub_chain->nextNode != NULL && sub_chain->nextNode->is_allocated == 'H') {
//                     sub_chain->size += sub_chain->nextNode->size + sizeof(SubNode);
//                     sub_chain->nextNode = sub_chain->nextNode->nextNode;
//                 }

//                 // If the previous segment is also a HOLE, merge the two HOLEs
//                 if (sub_chain->prevNode != NULL && sub_chain->prevNode->is_allocated == 'H') {
//                     sub_chain->prevNode->size += sub_chain->size + sizeof(SubNode);
//                     sub_chain->prevNode->nextNode = sub_chain->nextNode;

//                     if (sub_chain->nextNode != NULL) {
//                         sub_chain->nextNode->prevNode = sub_chain->prevNode;
//                     }
//                 }
//                 return;
//             }
//             sub_chain = sub_chain->nextNode;
//         }
//         main_chain_node = main_chain_node->nextNode;
//     }

//     // If the segment with the provided MeMS virtual address is not found, you can handle this as an error.
//     // You may print an error message or take appropriate action as needed.
//     // For example:
//     fprintf(stderr, "Error: Segment with MeMS virtual address %p not found in the free list.\n");
//     // You might also consider returning an error code to indicate failure.
// }


// // // Function to free memory using MeMS
// // void mems_free(void* ptr) {
// //     // Find the segment in the free list based on the MeMS virtual address (ptr)
// //     MainNode* main_chain_node = head;
// //     while (main_chain_node != NULL) {
// //         SubNode* sub_chain = main_chain_node -> sub_chain;
// //         while (sub_chain != NULL) {
// //             if (sub_chain -> mem_ptr == ptr) {
// //                 // Mark the segment as HOLE
// //                 sub_chain -> is_allocated = 'H';
// //                 return;
// //             }
// //             sub_chain = sub_chain -> nextNode;
// //         }
// //         main_chain_node = main_chain_node -> nextNode;
// //     }

// //     // If the segment with the provided MeMS virtual address is not found, you can handle this as an error.
// //     // You may print an error message or take appropriate action as needed.
// //     // For example:
// //     fprintf(stderr, "Error: Segment with MeMS virtual address %p not found in the free list.\n");
// //     // You might also consider returning an error code to indicate failure.
// // }

// // Function to print MeMS statistics
// void mems_print_stats() {
//     // Initialize variables to store statistics
//     size_t total_mapped_pages = 0;
//     size_t total_unused_memory = 0;

//     // Iterate through the free list and print details
//     MainNode* main_chain_node = head;
//     while (main_chain_node != NULL) {
//         SubNode* sub_chain = main_chain_node->sub_chain;
//         while (sub_chain != NULL) {
//             // Print details about the segment
//             printf("Segment at MeMS virtual address: %p\n", sub_chain->mem_ptr);
//             printf("Size: %zu bytes\n", sub_chain->size);
//             printf("Status: ");
//             if (sub_chain->is_allocated == 'H') {
//                 printf("Hole\n");
//             } else {
//                 printf("Process\n");
//             }

//             // Update statistics
//             total_mapped_pages += sub_chain->size / PAGE_SIZE;
//             if (sub_chain->is_allocated == 'H') {
//                 total_unused_memory += sub_chain->size;
//             }

//             sub_chain = sub_chain->nextNode;
//         }

//         main_chain_node = main_chain_node->nextNode;
//     }

//     // Print total statistics
//     printf("Total Mapped Pages: %zu\n", total_mapped_pages);
//     printf("Total Unused Memory: %zu bytes\n", total_unused_memory);
// }

// // // Function to print MeMS statistics
// // void mems_print_stats() {
// //     // Initialize variables to store statistics
// //     size_t total_mapped_pages = 0;
// //     size_t total_unused_memory = 0;

// //     // Iterate through the free list and print details
// //     MainNode* main_chain_node = head;
// //     while (main_chain_node != NULL) {
// //         SubNode* sub_chain = main_chain_node -> sub_chain;
// //         while (sub_chain != NULL) {
// //             // Print details about the segment
// //             printf("Segment at MeMS virtual address: %p\n", sub_chain -> mem_ptr);
// //             printf("Size: %zu bytes\n", sub_chain -> size);
// //             printf("Status: ");
// //             if (sub_chain -> is_allocated == 'H') {
// //                 printf("Hole\n");
// //             }
// //             else {
// //                 printf("Process\n");
// //             }

// //             // Update statistics
// //             total_mapped_pages += sub_chain -> size / PAGE_SIZE;
// //             if (sub_chain -> is_allocated == 'H') {
// //                 total_unused_memory += sub_chain -> size;
// //             }

// //             sub_chain = sub_chain -> nextNode;
// //         }

// //         main_chain_node = main_chain_node -> nextNode;
// //     }

// //     // Print total statistics
// //     printf("Total Mapped Pages: %zu\n", total_mapped_pages);
// //     printf("Total Unused Memory: %zu bytes\n", total_unused_memory);
// // }


// void* mems_get(void* v_ptr) {
//     // Iterate through the free list to find the segment with the matching MeMS virtual address
//     MainNode* main_chain_node = head;
//     while (main_chain_node != NULL) {
//         SubNode* sub_chain = main_chain_node -> sub_chain;
//         while (sub_chain != NULL) {
//             if (sub_chain -> mem_ptr == v_ptr) {
//                 // Found the segment with the matching virtual address
//                 return sub_chain -> mem_ptr;  // Return the MeMS physical address
//             }
//             sub_chain = sub_chain -> nextNode;
//         }
//         main_chain_node = main_chain_node -> nextNode;
//     }

//     // If the segment with the provided virtual address is not found, return NULL
//     return NULL;
// }


// // Function to clean up the MeMS system
// // Function to clean up the MeMS system
// void mems_finish() {
//     // Iterate through the free list and unmap all allocated memory segments
//     MainNode* main_chain_node = head;
//     while (main_chain_node != NULL) {
//         SubNode* sub_chain = main_chain_node -> sub_chain;
//         while (sub_chain != NULL) {
//             if (sub_chain -> is_allocated == 'P') {
//                 // Unmap the allocated memory segment
//                 munmap(sub_chain -> mem_ptr, sub_chain -> size);
//             }
//             sub_chain = sub_chain -> nextNode;
//         }
//         main_chain_node = main_chain_node -> nextNode;
//     }
// }


// // include other header files as needed
// #include"mems.h"


// int main(int argc, char const *argv[])
// {
//     // initialise the MeMS system 
//     mems_init();
//     int* ptr[10];

//     /*
//     This allocates 10 arrays of 250 integers each
//     */
//     printf("\n------- Allocated virtual addresses [mems_malloc] -------\n");
//     for(int i=0;i<10;i++){
//         ptr[i] = (int*)mems_malloc(sizeof(int)*250);
//         printf("Virtual address: %lu\n", (unsigned long)ptr[i]);
//     }

//     /*
//     In this section we are tring to write value to 1st index of array[0] (here it is 0 based indexing).
//     We get get value of both the 0th index and 1st index of array[0] by using function mems_get.
//     Then we write value to 1st index using 1st index pointer and try to access it via 0th index pointer.

//     This section is show that even if we have allocated an array using mems_malloc but we can 
//     retrive MeMS physical address of any of the element from that array using mems_get. 
//     */
//     printf("\n------ Assigning value to Virtual address [mems_get] -----\n");
//     // how to write to the virtual address of the MeMS (this is given to show that the system works on arrays as well)
//     int* phy_ptr= (int*) mems_get(&ptr[0][1]); // get the address of index 1
//     phy_ptr[0]=200; // put value at index 1
//     int* phy_ptr2= (int*) mems_get(&ptr[0][0]); // get the address of index 0
//     printf("Virtual address: %lu\tPhysical Address: %lu\n",(unsigned long)ptr[0],(unsigned long)phy_ptr2);
//     printf("Value written: %d\n", phy_ptr2[1]); // print the address of index 1 

//     /*
//     This shows the stats of the MeMS system.  
//     */
//     printf("\n--------- Printing Stats [mems_print_stats] --------\n");
//     mems_print_stats();

//     /*
//     This section shows the effect of freeing up space on free list and also the effect of 
//     reallocating the space that will be fullfilled by the free list.
//     */
//     printf("\n--------- Freeing up the memory [mems_free] --------\n");
//     mems_free(ptr[3]);
//     mems_print_stats();
//     ptr[3] = (int*)mems_malloc(sizeof(int)*250);
//     mems_print_stats();
//     return 0;
// }

// #include <stdio.h>
// #include <stdlib.h>

// #define MAX_PAGES 100   // Define the maximum number of pages (adjust as needed)
// #define PAGE_SIZE 4096  // Define the page size (4 KB as an example)

// size_t page_sizes[MAX_PAGES];  // Array to store the remaining memory in each page

// void* page_start;  // Define the starting address of the MeMS virtual address space

// struct NodeInfo {
//     void* mem_ptr;  // MeMS virtual address
//     size_t size;    // Size of the segment
//     int page_no;    // Page number where memory is allocated
// };

// struct NodeInfo nodes[MAX_PAGES];  // Array to store information about nodes
// int node_count = 0;

// void initialize_page_sizes() {
//     for (int i = 0; i < MAX_PAGES; i++) {
//         page_sizes[i] = PAGE_SIZE;
//     }
// }

// void* mems_malloc(size_t size) {
//     // Iterate through the page_sizes array to find a suitable page
//     int page_no = -1;
//     for (int i = 0; i < MAX_PAGES; i++) {
//         if (page_sizes[i] >= size) {
//             page_no = i;
//             break;
//         }
//     }

//     if (page_no != -1) {
//         // Allocate the requested memory from the selected page
//         void* mem_ptr = (void*)((char*)page_start + (PAGE_SIZE - page_sizes[page_no]));
//         page_sizes[page_no] -= size;

//         // Update the node information in the nodes array
//         nodes[node_count].mem_ptr = mem_ptr;
//         nodes[node_count].size = size;
//         nodes[node_count].page_no = page_no;
//         node_count++;

//         return mem_ptr;
//     } else {
//         // Handle the case where there is no suitable page available
//         return NULL;
//     }
// }

// void mems_free(void* ptr) {
//     int node_index = -1;
//     // Find the node index based on the provided MeMS virtual address
//     for (int i = 0; i < node_count; i++) {
//         if (nodes[i].mem_ptr == ptr) {
//             node_index = i;
//             break;
//         }
//     }

//     if (node_index != -1) {
//         // Free the memory, update page_sizes
//         int page_no = nodes[node_index].page_no;
//         page_sizes[page_no] += nodes[node_index].size;
//     }
// }

// void* mems_get(void* v_ptr) {
//     // Iterate through the nodes array to find the node with the matching MeMS virtual address
//     for (int i = 0; i < node_count; i++) {
//         if (nodes[i].mem_ptr == v_ptr) {
//             // Found the segment with the matching virtual address
//             // Return the corresponding MeMS physical address
//             int page_no = nodes[i].page_no;
//             void* physical_ptr = (void*)((char*)page_start + (PAGE_SIZE - page_sizes[page_no]));
//             return physical_ptr;
//         }
//     }

//     // If the segment with the provided virtual address is not found, return NULL
//     return NULL;
// }

// void mems_print_stats() {
//     // Initialize variables to store statistics
//     size_t total_mapped_pages = 0;
//     size_t total_unused_memory = 0;

//     // Iterate through the nodes array and print details
//     for (int i = 0; i < node_count; i++) {
//         printf("MeMS SYSTEM STATS\n");
//         printf("MAIN[%p:%p]-> ", page_start, (char*)page_start + PAGE_SIZE - 1);
//         printf("P[%p:%p] ", nodes[i].mem_ptr, (char*)nodes[i].mem_ptr + nodes[i].size - 1);
//         if (i < node_count - 1) {
//             printf("<-> ");
//         }
//         printf("\n");

//         // Update statistics
//         total_mapped_pages += nodes[i].size / PAGE_SIZE;
//     }

//     for (int i = 0; i < MAX_PAGES; i++) {
//         if (page_sizes[i] < PAGE_SIZE) {
//             printf("MAIN[%p:%p]-> ", (char*)page_start + (i * PAGE_SIZE),
//                    (char*)page_start + ((i + 1) * PAGE_SIZE) - 1);
//             printf("H[%p:%p] ", (char*)page_start + ((i + 1) * PAGE_SIZE) - page_sizes[i],
//                    (char*)page_start + ((i + 1) * PAGE_SIZE) - 1);
//             if (i < MAX_PAGES - 1) {
//                 printf("<-> ");
//             }
//             printf("\n");
//             total_unused_memory += page_sizes[i];
//         }
//     }

//     // Print total statistics
//     printf("Pages used: %zu\n", total_mapped_pages);
//     printf("Space unsued: %zu\n", total_unused_memory);
//     printf("Main Chain Length: %d\n", node_count);
// }

// int main() {
//     // Initialize the MeMS system
//     initialize_page_sizes();
//     page_start = malloc(MAX_PAGES * PAGE_SIZE);  // Allocate MeMS virtual address space

//     // Example usage of MeMS functions

//     // Allocate memory using MeMS
//     void* mem_ptr1 = mems_malloc(1024);
//     void* mem_ptr2 = mems_malloc(2048);

//     // Print MeMS statistics
//     mems_print_stats();

//     // Free memory using MeMS
//     mems_free(mem_ptr1);

//     // Retrieve the MeMS physical address
//     void* physical_ptr = mems_get(mem_ptr2);
//     if (physical_ptr != NULL) {
//         printf("MeMS SYSTEM STATS\n");
//         printf("MAIN[%p:%p]-> ", page_start, (char*)page_start + PAGE_SIZE - 1);
//         printf("P[%p:%p] ", mem_ptr2, (char*)mem_ptr2 + 2048 - 1);
//         printf("H[%p:%p] ", (char*)mem_ptr2 + 2048, (char*)mem_ptr2 + 4095);
//         printf("<-> ");
//         printf("MAIN[%p:%p]-> ", (char*)page_start + 2048, (char*)page_start + 2 * PAGE_SIZE - 1);
//         printf("H[%p:%p] ", (char*)page_start + 2048, (char*)page_start + 2 * PAGE_SIZE - 2048 - 1);
//         printf("\n");
//     } else {
//         printf("Physical Address not found for the given MeMS virtual address.\n");
//     }

//     free(page_start);  // Deallocate MeMS virtual address space

//     return 0;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/mman.h>

// #define MAX_PAGES 100   // Define the maximum number of pages (adjust as needed)
// #define PAGE_SIZE 4096  // Define the page size (4 KB as an example)

// size_t page_sizes[MAX_PAGES];  // Array to store the remaining memory in each page

// void* page_start;  // Define the starting address of the MeMS virtual address space

// struct NodeInfo {
//     void* mem_ptr;  // MeMS virtual address
//     size_t size;    // Size of the segment
//     int page_no;    // Page number where memory is allocated
// };

// struct NodeInfo nodes[MAX_PAGES];  // Array to store information about nodes
// int node_count = 0;

// void initialize_page_sizes() {
//     for (int i = 0; i < MAX_PAGES; i++) {
//         page_sizes[i] = PAGE_SIZE;
//     }
// }

// void* mems_malloc(size_t size) {
//     // Iterate through the page_sizes array to find a suitable page
//     int page_no = -1;
//     for (int i = 0; i < MAX_PAGES; i++) {
//         if (page_sizes[i] >= size) {
//             page_no = i;
//             break;
//         }
//     }

//     if (page_no != -1) {
//         // Allocate the requested memory from the selected page using mmap
//         void* mem_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//         if (mem_ptr == MAP_FAILED) {
//             perror("mmap");
//             return NULL;
//         }

//         // Update the node information in the nodes array
//         nodes[node_count].mem_ptr = mem_ptr;
//         nodes[node_count].size = size;
//         nodes[node_count].page_no = page_no;
//         node_count++;

//         // Update page_sizes
//         page_sizes[page_no] -= size;

//         return mem_ptr;
//     } else {
//         // Handle the case where there is no suitable page available
//         return NULL;
//     }
// }

// void mems_free(void* ptr) {
//     int node_index = -1;
//     // Find the node index based on the provided MeMS virtual address
//     for (int i = 0; i < node_count; i++) {
//         if (nodes[i].mem_ptr == ptr) {
//             node_index = i;
//             break;
//         }
//     }

//     if (node_index != -1) {
//         // Free the memory using munmap
//         size_t size = nodes[node_index].size;
//         void* mem_ptr = nodes[node_index].mem_ptr;
//         if (munmap(mem_ptr, size) == -1) {
//             perror("munmap");
//         }

//         // Update page_sizes
//         int page_no = nodes[node_index].page_no;
//         page_sizes[page_no] += size;
//     }
// }

// void* mems_get(void* v_ptr) {
//     // Iterate through the nodes array to find the node with the matching MeMS virtual address
//     for (int i = 0; i < node_count; i++) {
//         if (nodes[i].mem_ptr == v_ptr) {
//             // Found the segment with the matching virtual address
//             // Return the corresponding MeMS physical address
//             int page_no = nodes[i].page_no;
//             void* physical_ptr = (void*)((char*)page_start + (PAGE_SIZE - page_sizes[page_no]));
//             return physical_ptr;
//         }
//     }

//     // If the segment with the provided virtual address is not found, return NULL
//     return NULL;
// }

// void mems_print_stats() {
//     // Initialize variables to store statistics
//     size_t total_mapped_pages = 0;
//     size_t total_unused_memory = 0;

//     // Iterate through the nodes array and print details
//     for (int i = 0; i < node_count; i++) {
//         printf("MeMS SYSTEM STATS\n");
//         printf("MAIN[%p:%p]-> ", page_start, (char*)page_start + PAGE_SIZE - 1);
//         printf("P[%p:%p] ", nodes[i].mem_ptr, (char*)nodes[i].mem_ptr + nodes[i].size - 1);
//         if (i < node_count - 1) {
//             printf("<-> ");
//         }
//         printf("\n");

//         // Update statistics
//         total_mapped_pages += nodes[i].size / PAGE_SIZE;
//     }

//     for (int i = 0; i < MAX_PAGES; i++) {
//         if (page_sizes[i] < PAGE_SIZE) {
//             printf("MAIN[%p:%p]-> ", (char*)page_start + (i * PAGE_SIZE),
//                    (char*)page_start + ((i + 1) * PAGE_SIZE) - 1);
//             printf("H[%p:%p] ", (char*)page_start + ((i + 1) * PAGE_SIZE) - page_sizes[i],
//                    (char*)page_start + ((i + 1) * PAGE_SIZE) - 1);
//             if (i < MAX_PAGES - 1) {
//                 printf("<-> ");
//             }
//             printf("\n");
//             total_unused_memory += page_sizes[i];
//         }
//     }

//     // Print total statistics
//     printf("Pages used: %zu\n", total_mapped_pages);
//     printf("Space unused: %zu\n", total_unused_memory);
//     printf("Main Chain Length: %d\n", node_count);
// }

// int main() {
//     // Initialize the MeMS system
//     initialize_page_sizes();
//     page_start = mmap(NULL, MAX_PAGES * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//     if (page_start == MAP_FAILED) {
//         perror("mmap");
//         return 1;
//     }

//     // Example usage of MeMS functions

//     // Allocate memory using MeMS
//     void* mem_ptr1 = mems_malloc(1024);
//     void* mem_ptr2 = mems_malloc(2048);

//     // Print MeMS statistics
//     mems_print_stats();

//     // Free memory using MeMS
//     mems_free(mem_ptr1);

//     // Retrieve the MeMS physical address
//     void* physical_ptr = mems_get(mem_ptr2);
//     if (physical_ptr != NULL) {
//         printf("MeMS SYSTEM STATS\n");
//         printf("MAIN[%p:%p]-> ", page_start, (char*)page_start + PAGE_SIZE - 1);
//         printf("P[%p:%p] ", mem_ptr2, (char*)mem_ptr2 + 2048 - 1);
//         printf("H[%p:%p] ", (char*)mem_ptr2 + 2048, (char*)mem_ptr2 + 4095);
//         printf("<-> ");
//         printf("MAIN[%p:%p]-> ", (char*)page_start + 2048, (char*)page_start + 2 * PAGE_SIZE - 1);
//         printf("H[%p:%p] ", (char*)page_start + 2048, (char*)page_start + 2 * PAGE_SIZE - 2048 - 1);
//         printf("\n");
//     } else {
//         printf("Physical Address not found for the given MeMS virtual address.\n");
//     }

//     // Unmap the MeMS virtual address space
//     if (munmap(page_start, MAX_PAGES * PAGE_SIZE) == -1) {
//         perror("munmap");
//         return 1;
//     }

//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define MAX_PAGES 100   // Define the maximum number of pages (adjust as needed)
#define PAGE_SIZE 4096  // Define the page size (4 KB as an example)

size_t page_sizes[MAX_PAGES];  // Array to store the remaining memory in each page

void* page_start;  // Define the starting address of the MeMS virtual address space

struct NodeInfo {
    void* mem_ptr;  // MeMS virtual address
    size_t size;    // Size of the segment
    int page_no;    // Page number where memory is allocated
};

struct NodeInfo nodes[MAX_PAGES];  // Array to store information about nodes
int node_count = 0;

void* custom_malloc(size_t size) {
    int *plen;
    int len = size + sizeof(int);  // Add sizeof(int) for holding length.

    plen = mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (plen == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    *plen = len;  // First 4 bytes contain length.
    return (void*)(&plen[1]);  // Memory that is after the length variable.
}

void custom_free(void* ptr) {
    int* plen = (int*)ptr;
    int len;

    plen--;          // Reach the top of memory
    len = *plen;     // Read length

    munmap((void*)plen, len);
}

void initialize_page_sizes() {
    for (int i = 0; i < MAX_PAGES; i++) {
        page_sizes[i] = PAGE_SIZE;
    }
}

void* mems_malloc(size_t size) {
    // Use custom_malloc instead of mmap
    void* mem_ptr = custom_malloc(size);

    if (mem_ptr == NULL) {
        return NULL;
    }

    // Update the node information in the nodes array
    nodes[node_count].mem_ptr = mem_ptr;
    nodes[node_count].size = size;
    nodes[node_count].page_no = -1;  // Page number not applicable in this context
    node_count++;

    return mem_ptr;
}

void mems_free(void* ptr) {
    // Use custom_free instead of munmap
    custom_free(ptr);

    // Update page_sizes not needed in this context
}

void* mems_get(void* v_ptr) {
    // Iterate through the nodes array to find the node with the matching MeMS virtual address
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].mem_ptr == v_ptr) {
            // Found the segment with the matching virtual address
            // Return the corresponding MeMS physical address
            return v_ptr;
        }
    }

    // If the segment with the provided virtual address is not found, return NULL
    return NULL;
}

void mems_print_stats() {
    // Initialize variables to store statistics
    size_t total_mapped_pages = 0;
    size_t total_unused_memory = 0;

    // Iterate through the nodes array and print details
    for (int i = 0; i < node_count; i++) {
        printf("MeMS SYSTEM STATS\n");
        printf("MAIN[%p:%p]-> ", page_start, (char*)page_start + PAGE_SIZE - 1);
        printf("P[%p:%p] ", nodes[i].mem_ptr, (char*)nodes[i].mem_ptr + nodes[i].size - 1);
        if (i < node_count - 1) {
            printf("<-> ");
        }
        printf("\n");

        // Update statistics
        total_mapped_pages += nodes[i].size / PAGE_SIZE;
    }

    for (int i = 0; i < MAX_PAGES; i++) {
        if (page_sizes[i] < PAGE_SIZE) {
            printf("MAIN[%p:%p]-> ", (char*)page_start + (i * PAGE_SIZE),
                   (char*)page_start + ((i + 1) * PAGE_SIZE) - 1);
            printf("H[%p:%p] ", (char*)page_start + ((i + 1) * PAGE_SIZE) - page_sizes[i],
                   (char*)page_start + ((i + 1) * PAGE_SIZE) - 1);
            if (i < MAX_PAGES - 1) {
                printf("<-> ");
            }
            printf("\n");
            total_unused_memory += page_sizes[i];
        }
    }

    // Print total statistics
    printf("Pages used: %zu\n", total_mapped_pages);
    printf("Space unused: %zu\n", total_unused_memory);
    printf("Main Chain Length: %d\n", node_count);
}

int main() {
    // Initialize the MeMS system
    initialize_page_sizes();
    page_start = mmap(NULL, MAX_PAGES * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page_start == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Example usage of MeMS functions

    // Allocate memory using MeMS
    void* mem_ptr1 = mems_malloc(1024);
    void* mem_ptr2 = mems_malloc(2048);

    // Print MeMS statistics
    mems_print_stats();

    // Free memory using MeMS
    mems_free(mem_ptr1);

    // Unmap the MeMS virtual address space
    if (munmap(page_start, MAX_PAGES * PAGE_SIZE) == -1) {
        perror("munmap");
        return 1;
    }

    return 0;
}
