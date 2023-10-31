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
    char is_allocated;  // P for Process, H for Hole
    struct SubNode* nextNode;
    struct SubNode* prevNode;
} SubNode;

// Main Chain Node structure
typedef struct MainNode {
    SubNode* sub_chain;
    struct MainNode* nextNode;
    struct MainNode* prevNode;
} MainNode;

MainNode* head = NULL;  // Head of the free list
void* mem_start = NULL;     // Starting MeMS virtual address

// Function to allocate memory using MeMS
void* mems_malloc(size_t size) {
    SubNode* segment = NULL;
    // Iterate through the free list to find a suitable segment
    MainNode* main_chain_node = head;
    while (main_chain_node != NULL) {
        SubNode* sub_chain = main_chain_node->sub_chain;
        while (sub_chain != NULL) {
            if (sub_chain->is_allocated == 0 && sub_chain->size >= size) {
                segment = sub_chain;
                break;
            }
            sub_chain = sub_chain->nextNode;
        }
        if (segment != NULL) {
            break;
        }
        main_chain_node = main_chain_node->nextNode;
    }

    if (segment != NULL) {
        // Allocate the requested memory from the segment
        segment->is_allocated = 1;
        return segment->mem_ptr;
    } 
    else {
        // Use mmap to allocate a new segment
        size_t required_size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
        void* new_mem = mmap(NULL, required_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, -1, 0);
        if (new_mem == MAP_FAILED) {
            // Handle mmap error
            return NULL;
        }
        
        SubNode* new_segment = (SubNode*)malloc(sizeof(SubNode));
        new_segment->mem_ptr = new_mem;
        new_segment->size = required_size;
        new_segment->is_allocated = 1;
        // Add the new segment to the free list
        // ...

        return new_mem;
    }
}

// Function to free memory using MeMS
void mems_free(void* ptr) {
    // Find the segment in the free list based on the MeMS virtual address (ptr)
    MainNode* main_chain_node = head;
    while (main_chain_node != NULL) {
        SubNode* sub_chain = main_chain_node->sub_chain;
        while (sub_chain != NULL) {
            if (sub_chain->mem_ptr == ptr) {
                // Mark the segment as HOLE
                sub_chain->is_allocated = 0;
                return;
            }
            sub_chain = sub_chain->nextNode;
        }
        main_chain_node = main_chain_node->nextNode;
    }

    // If the segment with the provided MeMS virtual address is not found, you can handle this as an error.
    // You may print an error message or take appropriate action as needed.
    // For example:
    fprintf(stderr, "Error: Segment with MeMS virtual address %p not found in the free list.\n", ptr);
    // You might also consider returning an error code to indicate failure.
}

// Function to print MeMS statistics
void mems_print_stats() {
    // Initialize variables to store statistics
    size_t total_mapped_pages = 0;
    size_t total_unused_memory = 0;

    // Iterate through the free list and print details
    MainNode* main_chain_node = head;
    while (main_chain_node != NULL) {
        SubNode* sub_chain = main_chain_node->sub_chain;
        while (sub_chain != NULL) {
            // Print details about the segment
            printf("Segment at MeMS virtual address: %p\n", sub_chain->mem_ptr);
            printf("Size: %zu bytes\n", sub_chain->size);
            printf("Status: %s\n", sub_chain->is_allocated ? "PROCESS" : "HOLE");

            // Update statistics
            total_mapped_pages += sub_chain->size / PAGE_SIZE;
            if (sub_chain->is_allocated == 0) {
                total_unused_memory += sub_chain->size;
            }

            sub_chain = sub_chain->nextNode;
        }

        main_chain_node = main_chain_node->nextNode;
    }

    // Print total statistics
    printf("Total Mapped Pages: %zu\n", total_mapped_pages);
    printf("Total Unused Memory: %zu bytes\n", total_unused_memory);
}


void* mems_get(void* v_ptr) {
    // Iterate through the free list to find the segment with the matching MeMS virtual address
    MainNode* main_chain_node = head;
    while (main_chain_node != NULL) {
        SubNode* sub_chain = main_chain_node->sub_chain;
        while (sub_chain != NULL) {
            if (sub_chain->mem_ptr == v_ptr) {
                // Found the segment with the matching virtual address
                return sub_chain->mem_ptr;  // Return the MeMS physical address
            }
            sub_chain = sub_chain->nextNode;
        }
        main_chain_node = main_chain_node->nextNode;
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
        SubNode* sub_chain = main_chain_node->sub_chain;
        while (sub_chain != NULL) {
            if (sub_chain->is_allocated) {
                // Unmap the allocated memory segment
                munmap(sub_chain->mem_ptr, sub_chain->size);
            }
            sub_chain = sub_chain->nextNode;
        }
        main_chain_node = main_chain_node->nextNode;
    }
}


int main(int argc, char const* argv[]) {
    // Initialize the MeMS system (you should implement this part based on your requirements)
    // ...

    // Example usage of MeMS functions

    // Allocate memory using MeMS
    void* mem_ptr1 = mems_malloc(1024);
    void* mem_ptr2 = mems_malloc(2048);

    // Print MeMS statistics
    mems_print_stats();

    // Free memory using MeMS
    mems_free(mem_ptr1);

    // Retrieve the MeMS physical address
    void* physical_ptr = mems_get(mem_ptr2);
    if (physical_ptr != NULL) {
        printf("MeMS Physical Address: %p\n", physical_ptr);
    } else {
        printf("Physical Address not found for the given MeMS virtual address.\n");
    }

    // Clean up the MeMS system
    mems_finish();

    return 0;
}