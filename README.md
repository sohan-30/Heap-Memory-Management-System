# Heap Allocator in C

## Overview
A simple implementation of a custom heap allocator that manages memory allocation and deallocation using a linked list of metadata blocks.
This project implements a basic heap allocator that provides `malloc()` and `free()` functionality using a fixed-size heap. The allocator uses a first-fit allocation strategy and includes automatic coalescing of adjacent free blocks to minimize fragmentation.

## Features

- **Custom Memory Management**: Implements allocation and deallocation without using system `malloc()`/`free()`
- **First-Fit Allocation**: Finds the first available free block that can satisfy the allocation request
- **Block Splitting**: Splits larger blocks when allocated size is smaller than available space
- **Automatic Coalescing**: Merges adjacent free blocks to reduce fragmentation
- **Memory Validation**: Validates pointers before freeing to prevent double-free errors
- **Interactive Interface**: Menu-driven program for testing allocation/deallocation
- **Heap Visualization**: Displays current heap state with block information

## How It Works

### Memory Management Strategy

This heap allocator implements a **linked list-based approach** where each allocated or free block contains metadata that points to the next block. The entire heap is treated as a contiguous array of bytes with embedded metadata structures.

### Initialization Process

1. **Heap Setup**: A static array `Heap[HeapSize]` serves as the memory pool
2. **Initial Block**: The entire heap starts as one large free block
3. **Metadata Placement**: The first `MetaBlock` structure is placed at the beginning of the heap
4. **Available Space**: Remaining space after metadata becomes allocable memory

### Allocation Algorithm (First-Fit)

When `Allocate(size)` is called:

1. **Search Phase**: Traverse the linked list of blocks to find first free block ≥ requested size
2. **Block Selection**: If found, check if block should be split
3. **Splitting Decision**: 
   - If `block_size > requested_size + sizeof(MetaBlock)`: Split the block
   - Otherwise: Use entire block (avoid tiny fragments)
4. **Block Splitting Process**:
   ```
   Before: [MetaBlock: 1000 bytes, free]
   After:  [MetaBlock: 100 bytes, allocated][MetaBlock: 876 bytes, free]
   ```
5. **Status Update**: Mark allocated block with status 'a'
6. **Return Pointer**: Return address of usable memory (after metadata)

### Deallocation Process

When `Free(ptr)` is called:

1. **Pointer Validation**: 
   - Check if pointer is within heap bounds
   - Verify pointer points to start of an allocated block
   - Prevent double-free by checking block status
2. **Status Change**: Mark block as free (status = 'f')
3. **Automatic Coalescing**: Call `Merge()` to combine adjacent free blocks

### Merging Algorithm

The `Merge()` function eliminates fragmentation:

1. **Forward Traversal**: Walk through linked list from beginning
2. **Adjacent Check**: For each free block, check if next block is also free
3. **Merge Operation**: 
   ```
   Before: [Free: 100][Free: 200][Allocated: 150]
   After:  [Free: 324][Allocated: 150]
           (100 + 200 + 24 bytes of eliminated metadata)
   ```
4. **Link Update**: Adjust next pointers to skip merged blocks
5. **Size Calculation**: Add sizes plus eliminated metadata overhead

### Error Prevention Mechanisms

1. **Boundary Checking**: All pointers validated against heap boundaries
2. **Double-Free Protection**: Status checking prevents freeing already-free blocks
3. **Invalid Pointer Detection**: Ensures pointers correspond to actual block starts
4. **Null Pointer Handling**: Graceful handling of NULL input parameters

### Performance Characteristics

- **Allocation Time**: O(n) where n = number of blocks (first-fit search)
- **Deallocation Time**: O(1) for freeing + O(n) for coalescing
- **Space Overhead**: 24 bytes per block + alignment padding
- **Fragmentation**: Minimized through immediate coalescing

## Core Functions

### `Initialize()`
- Sets up the initial heap with a single large free block
- Displays heap configuration information

### `Allocate(size_t size)`
- Finds first free block that can accommodate the requested size
- Splits blocks when necessary to minimize waste
- Returns pointer to usable memory (not metadata)

### `Free(void *ptr)`
- Validates the pointer before freeing
- Marks block as free and prevents double-free errors
- Returns success/failure status

### `Merge()`
- Coalesces adjacent free blocks to reduce fragmentation
- Called automatically after each deallocation

### `DisplayHeap()`
- Shows current heap state with all blocks
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

// The heap will automatically merge adjacent free blocks
Merge();

// Display current heap state
DisplayHeap();
```

## Sample Output

```
===== Heap Allocator Initialized =====
  - Total heap size: 10000 bytes
  - Metadata size: 24 bytes per block
  - Available memory: 9976 bytes
  - Heap start address: 0x404040
======================================

Successfully allocated 100 bytes at address 0x404058
Split block: allocated 100 bytes, created new free block of 9852 bytes

===== HEAP MEMORY MAP =====
Block Address              Status     Size       Usable Memory  
----------------------------------------------------------------
0     0x404040             a          100        0x404058       
1     0x4040c0             f          9852       0x4040d8       
----------------------------------------------------------------
Total blocks: 2
Total allocated: 100 bytes
Total free: 9852 bytes
Metadata overhead: 48 bytes
```

## Key Features

### Block Splitting
When allocating memory smaller than an available free block, the allocator splits the block:
- The requested portion becomes allocated
- The remainder becomes a new free block
- This minimizes internal fragmentation

### Merging
After freeing memory, adjacent free blocks are automatically merged:
- Reduces external fragmentation
- Creates larger contiguous free spaces
- Improves allocation success rate

### Error Handling
- Validates all pointer operations
- Prevents double-free errors
- Handles boundary cases (NULL pointers, invalid sizes)
- Provides informative error messages
