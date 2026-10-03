# Heap Allocator in C

## Overview
A simple implementation of a custom heap allocator that manages memory allocation and deallocation using a linked list of metadata blocks.
This project implements a basic heap allocator that provides `malloc()` and `free()` functionality using a fixed-size heap. The allocator uses a first-fit allocation strategy and includes automatic coalescing of adjacent free blocks to minimize fragmentation.

## Features

- **Custom Memory Management**: Implements allocation and deallocation without using system `malloc()`/`free()`
- **First-Fit Allocation**: Finds the first available free block that can satisfy the allocation request
- **Block Splitting**: Splits larger blocks when allocation leaves enough space for another metadata block and free payload
- **Automatic Coalescing**: Merges adjacent free blocks to reduce fragmentation
- **Memory Validation**: Validates pointers before freeing to prevent double-free errors
- **Interactive Interface**: Menu-driven program for testing allocation/deallocation
- **Heap Visualization**: Displays current heap state with block information

## How It Works

### Memory Management Strategy

This heap allocator implements a **linked list-based approach** where each allocated or free block contains metadata that points to the next block. The entire heap is treated as a contiguous array of bytes with embedded metadata structures.

The heap is aligned so that metadata and payload addresses remain suitably aligned. The usable payload address is calculated immediately after a block's `MetaBlock` header using byte-wise pointer arithmetic.

### Initialization Process

1. **Heap Setup**: A static array `Heap[HeapSize]` serves as the memory pool
2. **Initial Block**: The entire heap starts as one large free block
3. **Metadata Placement**: The first `MetaBlock` structure is placed at the beginning of the heap
4. **Available Space**: Remaining space after metadata becomes allocable memory

### Allocation Algorithm (First-Fit)

When `Allocate(size)` is called:

1. **Size Alignment**: The requested size is rounded up to the nearest multiple of 8 bytes
2. **Search Phase**: Traverse the linked list of blocks to find the first free block with enough usable space
3. **Block Selection**: If found, check if the block should be split
4. **Splitting Decision**:
   - If `block_size > requested_size + sizeof(MetaBlock)`: Split the block
   - Otherwise: Use the entire block and avoid creating a fragment too small for another metadata block
5. **Status Update**: Mark the allocated block with status `'a'`
6. **Return Pointer**: Return the address of usable memory after the metadata header

An exact-sized free block is valid for allocation because its existing metadata header is reused. A new metadata block is needed only when the remaining space is split into another block.

### Deallocation Process

When `Free(ptr)` is called:

1. **Pointer Validation**:
   - Check if the pointer is within heap bounds
   - Verify the pointer points to the start of an allocated block
   - Prevent double-free by checking block status
2. **Status Change**: Mark the block as free
3. **Coalescing**: After a successful deallocation, the program calls `Merge()` to combine adjacent free blocks

### Merging Algorithm

The `Merge()` function eliminates fragmentation:

1. **Forward Traversal**: Walk through the linked list from the beginning
2. **Adjacent Check**: For each free block, check if the next block is also free
3. **Merge Operation**: Add the next block's payload size and metadata size to the current block
4. **Link Update**: Adjust next pointers to skip merged blocks
5. **Size Calculation**: Include the eliminated metadata overhead in the merged block size

The current linked-list implementation performs coalescing in `O(n)` time, where `n` is the number of blocks. This is suitable for the educational implementation. Constant-time neighbor coalescing would require additional metadata such as boundary tags or a doubly linked list.

### Error Prevention Mechanisms

1. **Boundary Checking**: All pointers are validated against heap boundaries
2. **Double-Free Protection**: Status checking prevents freeing already-free blocks
3. **Invalid Pointer Detection**: Ensures pointers correspond to actual block starts
4. **Null Pointer Handling**: Graceful handling of `NULL` input parameters

### Performance Characteristics

- **Allocation Time**: O(n) where n = number of blocks (first-fit search)
- **Deallocation Time**: O(1) for marking a block free, followed by O(n) for coalescing
- **Space Overhead**: `sizeof(MetaBlock)` bytes per block; the actual size depends on the platform and compiler
- **Alignment**: Requested sizes are rounded up to an 8-byte boundary
- **Fragmentation**: Minimized through immediate coalescing

## Core Functions

### `Initialize()`
- Sets up the initial heap with a single large free block
- Displays heap configuration information

### `Allocate(size_t size)`
- Finds the first free block that can accommodate the requested size
- Rounds the requested size to an 8-byte boundary
- Splits blocks when necessary to minimize waste
- Returns a pointer to usable memory, not metadata

### `Free(void *ptr)`
- Validates the pointer before freeing
- Marks the block as free and prevents double-free errors
- Returns a success/failure status

### `Merge()`
- Coalesces adjacent free blocks to reduce fragmentation
- Called by the interactive program after each successful deallocation

### `DisplayHeap()`
- Shows the current heap state with all blocks
- Displays memory usage statistics

## Compilation and Usage

### Compilation
```bash
gcc -o heap_allocator heap_allocator.c
```

### Running the Program
```bash
./heap_allocator
```

### Menu Options
1. **Allocate memory** - Request memory allocation of specified size
2. **Free memory** - Deallocate previously allocated memory
3. **Display heap status** - View current heap state and statistics
4. **Exit** - Terminate the program

## Example Usage

```c
// Initialize the heap
Initialize();

// Allocate 100 bytes
void *ptr1 = Allocate(100);

// Allocate 200 bytes
void *ptr2 = Allocate(200);

// Free the first allocation
Free(ptr1);

// Coalesce adjacent free blocks
Merge();

// Display the current heap state
DisplayHeap();
```

## Sample Output

The addresses shown by the program depend on the compiler, operating system, and run. The following addresses are illustrative:

```
===== Heap Allocator Initialized =====
  - Total heap size: 10000 bytes
  - Metadata size: 24 bytes per block
  - Available memory: 9976 bytes
  - Heap start address: 0000000000408970
  - First usable memory: 0000000000408988
======================================

Split block: allocated 104 bytes, created new free block of 9848 bytes
Successfully allocated 104 bytes at address 0000000000408988
Allocated pointer #0

===== HEAP MEMORY MAP =====
Block Address              Status     Size       Usable Memory  
----------------------------------------------------------------
0     0000000000408970     a          104        0000000000408988
1     00000000004089F0     f          9848       0000000000408A08
----------------------------------------------------------------
Total blocks: 2
Total allocated: 104 bytes
Total free: 9848 bytes
Metadata overhead: 48 bytes
============================
```

## Key Features

### Block Splitting
When allocating memory smaller than an available free block, the allocator splits the block:
- The requested portion becomes allocated
- The remainder becomes a new free block with its own metadata
- This minimizes internal fragmentation

If the remaining space is not large enough for another metadata block and usable memory, the allocator uses the whole free block instead of creating a tiny fragment.

### Merging
After freeing memory, adjacent free blocks are merged:
- Reduces external fragmentation
- Creates larger contiguous free spaces
- Improves allocation success rate

### Error Handling
- Validates all pointer operations
- Prevents double-free errors
- Handles boundary cases such as `NULL` pointers and zero-size requests
- Provides informative error messages
