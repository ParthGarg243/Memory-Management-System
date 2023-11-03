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

#define PAGE_SIZE 4096

// Define structures for the main chain and sub-chain nodes
struct SubChainNode {
    size_t size;
    int is_process;
    struct SubChainNode* next;
    struct SubChainNode* prev;
};

struct MainChainNode {
    void* start_address;
    size_t num_pages;
    size_t size;
    struct SubChainNode* sub_chain_head;
    struct MainChainNode* next;
    struct MainChainNode* prev;
};

// Declare function prototypes
struct SubChainNode* createSubChainNode(size_t size, int is_process);
struct MainChainNode* createMainChainNode(size_t num_pages, size_t size);

// Define global variables to maintain MeMS state
struct MainChainNode* free_list_head = NULL;
void* mems_virtual_address = (void*)1000;  // Starting MeMS virtual address

void mems_init() {
    // Initialize any required parameters for the MeMS system
    free_list_head = (struct MainChainNode*)mmap(mems_virtual_address, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (free_list_head == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    free_list_head->start_address = mems_virtual_address;
    free_list_head->num_pages = 1;
    free_list_head->size = PAGE_SIZE;
    free_list_head->sub_chain_head = NULL;
    free_list_head->next = free_list_head->prev = NULL;
}

void* mems_get(void* v_ptr) {
    // Return the MeMS physical address mapped to the given MeMS virtual address
    struct MainChainNode* main_node = free_list_head;
    while (main_node) {
        struct SubChainNode* sub_node = main_node->sub_chain_head;
        while (sub_node) {
            if (sub_node->is_process && (v_ptr >= main_node->start_address + sub_node->size) &&
                (v_ptr < main_node->start_address + sub_node->size + sub_node->size)) {
                // MeMS virtual address is within this sub-chain node
                size_t offset = (size_t)(v_ptr - main_node->start_address - sub_node->size);
                return main_node->start_address + offset;
            }
            sub_node = sub_node->next;
        }
        main_node = main_node->next;
    }
    return NULL;  // MeMS virtual address not found
}

struct SubChainNode* createSubChainNode(size_t size, int is_process) {
    struct SubChainNode* node = (struct SubChainNode*)mmap(NULL, sizeof(struct SubChainNode), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    node->size = size;
    node->is_process = is_process;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

struct MainChainNode* createMainChainNode(size_t num_pages, size_t size) {
    struct MainChainNode* node = (struct MainChainNode*)mmap(mems_virtual_address, sizeof(struct MainChainNode), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (node == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    mems_virtual_address += sizeof(struct MainChainNode);
    node->start_address = mems_virtual_address;
    node->num_pages = num_pages;
    node->size = size;
    node->sub_chain_head = createSubChainNode(size, 1);  // Initial segment is PROCESS
    node->next = NULL;
    node->prev = NULL;
    if (free_list_head == NULL) {
        free_list_head = node;
    } else {
        struct MainChainNode* current = free_list_head;
        while (current->next) {
            current = current->next;
        }
        current->next = node;
        node->prev = current;
    }
    return node;
}

void* mems_malloc(size_t size) {
    struct MainChainNode* current = free_list_head;
    while (current) {
        struct SubChainNode* sub_chain = current->sub_chain_head;
        while (sub_chain) {
            if (!sub_chain->is_process && sub_chain->size >= size) {
                // Allocate from the hole
                if (sub_chain->size == size) {
                    sub_chain->is_process = 1; // Mark as PROCESS
                    return (void*)(current->start_address + current->size - sub_chain->size);
                } else {
                    // Create a new hole from the remaining space
                    struct SubChainNode* new_hole = createSubChainNode(sub_chain->size - size, 0); // Mark as HOLE
                    new_hole->next = sub_chain->next;
                    new_hole->prev = sub_chain;
                    sub_chain->next = new_hole;
                    sub_chain->size = size;
                    sub_chain->is_process = 1; // Mark as PROCESS

                    // Update mems_virtual_address
                    mems_virtual_address += size;

                    return (void*)(current->start_address + current->size - sub_chain->size);
                }
            }
            sub_chain = sub_chain->next;
        }
        current = current->next;
    }

    // If there are no holes to allocate from, create a new node
    current = createMainChainNode((size + PAGE_SIZE - 1) / PAGE_SIZE, size);

    // Update mems_virtual_address
    mems_virtual_address += size;

    return (void*)(current->start_address);
}

void mems_free(void* ptr) {
    // Memory deallocation function
    struct MainChainNode* current = free_list_head;
    while (current) {
        if (ptr >= current->start_address && ptr < current->start_address + current->size) {
            struct SubChainNode* sub_chain = current->sub_chain_head;
            while (sub_chain) {
                if (ptr >= current->start_address + current->size - sub_chain->size && ptr < current->start_address + current->size) {
                    sub_chain->is_process = 0; // Mark as HOLE
                    return;
                }
                sub_chain = sub_chain->next;
            }
        }
        current = current->next;
    }
}

void mems_print_stats() {
    // Memory statistics function
    struct MainChainNode* current = free_list_head;
    while (current) {
        printf("MAIN[%lu:%lu]-> ", (size_t)current->start_address, (size_t)(current->start_address + current->size - 1));

        struct SubChainNode* sub_chain = current->sub_chain_head;
        while (sub_chain) {
            printf("%s[%lu:%lu] <-> ", sub_chain->is_process ? "P" : "H",
                   (size_t)(current->start_address + current->size - sub_chain->size),
                   (size_t)(current->start_address + current->size - 1));
            sub_chain = sub_chain->next;
        }

        printf("\n");
        current = current->next;
    }
}

void mems_finish() {
    // Cleanup function
    struct MainChainNode* current = free_list_head;
    while (current) {
        struct SubChainNode* sub_chain = current->sub_chain_head;
        while (sub_chain) {
            struct SubChainNode* temp = sub_chain;
            sub_chain = sub_chain->next;
            munmap(temp, sizeof(struct SubChainNode));
        }

        free_list_head = current;
        current = current->next;
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
        printf("Virtual address: %p\n", ptr[i]);
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
    printf("Virtual address: %p\tPhysical Address: %p\n", ptr[0], phy_ptr2);
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