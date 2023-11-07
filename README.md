# MeMS: Memory Management System [CSE231 OS Assignment 3]
[Documentation](https://docs.google.com/document/d/1Gs9kC3187lLrinvK1SueTc8dHCJ0QP43eRlrCRlXiCY/edit?usp=sharing)
---

# MeMS (Memory Management System)

MeMS is a simple memory management system implemented in C. It allows you to allocate and manage memory efficiently by keeping track of available memory spaces and optimizing memory usage.

## 1. Introduction
The MeMS code provides a basic memory allocation and management system. It allows you to allocate memory, retrieve physical addresses for virtual addresses, free memory, and obtain statistics about memory usage.

## 2. Code Structure
**Data Structures:**
- `struct SubChainNode`: Represents a node in a subchain.
- `struct MainChainNode`: Represents a node in the mainchain.

**Global Variables:**
- `mainChainHead`: Points to the head of the mainchain.
- `startingVirtualAddress`: Represents the starting virtual address for memory allocation.

**Functions:**
- `mems_init`: Initializes the MeMS system.
- `mems_malloc`: Allocates memory efficiently.
- `mems_get`: Retrieves physical addresses for virtual addresses.
- `mems_free`: Frees memory and optimizes memory usage.
- `mems_print_stats`: Provides statistics about memory usage.
- `mems_finish`: Cleans up the MeMS system.

## 3. Data Structures
### `struct SubChainNode`
- `physicalAddress`: Physical address of the memory block.
- `startAddress`: Start address of the memory block.
- `endAddress`: End address of the memory block.
- `size`: Size of the memory block.
- `processOrHole`: Indicates whether the block is used by a process ('P') or is a hole ('H').
- `nextNode`: Pointer to the next subchain node.
- `prevNode`: Pointer to the previous subchain node.

### `struct MainChainNode`
- `physicalAddress`: Physical address of the main chain node.
- `pages`: Number of pages allocated for the main chain.
- `startAddress`: Start virtual address of the main chain.
- `size`: Size of the main chain.
- `subChainHead`: Pointer to the first subchain node in this main chain.
- `nextNode`: Pointer to the next main chain node.
- `prevNode`: Pointer to the previous main chain node.

## 4. Functions
### `mems_init`
- Initializes the MeMS system.
- Sets `mainChainHead` to `NULL` and initializes the starting virtual address.

### `mems_malloc(size_t size)`
- Allocates memory efficiently.
- Searches for available holes in existing chains or creates new chains if needed.
- Merges adjacent holes to optimize memory usage.
- Returns the virtual address of the allocated memory.

### `mems_get(void* v_ptr)`
- Retrieves the physical address corresponding to a virtual address.
- Searches the main and subchains for the corresponding physical address.

### `mems_free(void* ptr)`
- Frees memory by marking the specified memory block as a hole ('H').
- Merges adjacent holes in the subchain to optimize memory usage.

### `mems_print_stats`
- Provides statistics about the MeMS system.
- Displays the number of pages used, unused space, main chain length, and sub-chain lengths for each main chain.

### `mems_finish`
- Cleans up the MeMS system by deallocating all allocated memory.
- Resets `mainChainHead` to `NULL.

## 6. Compilation and Execution
To compile and execute the MeMS code, follow these steps:

```shell
rm -rf newExample
gcc -o newExample newExample.c
./newExample

