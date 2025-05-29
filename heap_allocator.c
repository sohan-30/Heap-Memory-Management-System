#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define HeapSize 10000
char Heap[HeapSize];

typedef struct MetaData
{
    size_t size;
    char status; 
    struct MetaData *next; 
} MetaBlock;

MetaBlock *heap_block_ptr = (void *)Heap;

void Initialize()
{
    heap_block_ptr->size = HeapSize - sizeof(MetaBlock);
    heap_block_ptr->status = 'f';
    heap_block_ptr->next = NULL;
    
    printf("\n===== Heap Allocator Initialized =====\n");
    printf("  - Total heap size: %d bytes\n", HeapSize);
    printf("  - Metadata size: %zu bytes per block\n", sizeof(MetaBlock));
    printf("  - Available memory: %zu bytes\n", heap_block_ptr->size);
    printf("  - Heap start address: %p\n", heap_block_ptr);
    printf("  - First usable memory: %p\n", (MetaBlock *)heap_block_ptr + sizeof(MetaBlock));
    printf("======================================\n\n");
}

void *Allocate(size_t size_to_be_allocated)
{
    if (size_to_be_allocated == 0) 
    {
        printf("Error: Cannot allocate 0 bytes\n");
        return NULL;
    }
    
    MetaBlock *current = heap_block_ptr;
    void *ret_ptr = NULL;
    int flag = 0;
    while (current != NULL && flag!=1) 
    {
        if (current->status == 'f' && current->size >= size_to_be_allocated) 
        {
            flag = 1;
        } 
        else{current = current->next;}
    }
    
    if (current == NULL) 
    {
        printf("Error: No free space left to allocate %zu bytes\n", size_to_be_allocated);
        return NULL;
    }

    if (current->size > size_to_be_allocated + sizeof(MetaBlock)) 
    {
        MetaBlock *new_block = (void *)((MetaBlock *)current + sizeof(MetaBlock) + size_to_be_allocated);
        
        new_block->size = current->size - size_to_be_allocated - sizeof(MetaBlock);
        new_block->next = current->next;
        new_block->status = 'f';
        
        current->size = size_to_be_allocated;
        current->next = new_block;
        
        printf("Split block: allocated %zu bytes, created new free block of %zu bytes\n", 
               size_to_be_allocated, new_block->size);
    }
    
    current->status = 'a';
    ret_ptr = (MetaBlock*)current + sizeof(MetaBlock);
    
    printf("Successfully allocated %zu bytes at address %p\n", size_to_be_allocated, ret_ptr);
    
    return ret_ptr;
}

MetaBlock *GetBlockMetadata(void *ptr)
{
    if (ptr == NULL) 
    {
        return NULL;
    }
    
    if (ptr < (void *)Heap || ptr >= (void *)(Heap + HeapSize)) 
    {
        return NULL;
    }
    
    MetaBlock *current = heap_block_ptr;
    
    while (current != NULL) 
    {
        void *block_data = (MetaBlock*)current + sizeof(MetaBlock);
        
        if (block_data == ptr) 
        {
            return current;
        }
        
        current = current->next;
    }
    
    return NULL;  
}

bool Free(void *ptr_to_free)
{
    if (ptr_to_free == NULL) 
    {
        printf("Error: Cannot free NULL pointer\n");
        return false;
    }
    
    MetaBlock *block_to_free = GetBlockMetadata(ptr_to_free);
    
    if (block_to_free == NULL) 
    {
        printf("Error: Invalid pointer - not the start of an allocated block\n");
        return false;
    }
    
    if (block_to_free->status != 'a') 
    {
        printf("Error: Double free detected or block already free\n");
        return false;
    }
    
    block_to_free->status = 'f';
    printf("Successfully freed block of size %zu bytes at address %p\n", block_to_free->size, ptr_to_free);
    
    return true;
}

void Merge()
{
    MetaBlock *current = heap_block_ptr;
    int merges_done = 0;
    
    while (current != NULL && current->next != NULL) 
    {
        if (current->status == 'f' && current->next->status == 'f') 
        {
            current->size += current->next->size + sizeof(MetaBlock);
            current->next = current->next->next;
            merges_done++;
        } 
        else 
        {
            current = current->next;
        }
    }
    
    if (merges_done > 0) 
    {
        printf("Merged %d adjacent free blocks\n", merges_done);
    }
}

void DisplayHeap()
{
    MetaBlock *current = heap_block_ptr;
    int block_count = 0;
    size_t total_free = 0;
    size_t total_allocated = 0;
    
    printf("\n===== HEAP MEMORY MAP =====\n");
    printf("%-5s %-20s %-10s %-10s %-15s\n", "Block", "Address", "Status", "Size", "Usable Memory");
    printf("----------------------------------------------------------------\n");
    
    while (current != NULL) 
    {
        printf("%-5d %-20p %-10c %-10zu %-15p\n", block_count, (void*)current, current->status, 
               current->size, (MetaBlock*)current + sizeof(MetaBlock));
        
        if (current->status == 'f') 
        {
            total_free += current->size;
        } 
        else 
        {
            total_allocated += current->size;
        }
        
        block_count++;
        current = current->next;
    }
    
    printf("----------------------------------------------------------------\n");
    printf("Total blocks: %d\n", block_count);
    printf("Total allocated: %zu bytes\n", total_allocated);
    printf("Total free: %zu bytes\n", total_free);
    printf("Metadata overhead: %zu bytes\n", block_count * sizeof(MetaBlock));
    printf("============================\n\n");
}

void DisplayMenu()
{
    printf("\n===== HEAP ALLOCATOR MENU =====\n");
    printf("1. Allocate memory\n");
    printf("2. Free memory\n");
    printf("3. Display heap status\n");
    printf("4. Exit\n");
    printf("Enter your choice (1-4): ");
}

int main()
{
    Initialize();
    
    void *allocated_ptrs[100];
    size_t allocated_sizes[100];
    int ptr_count = 0;
    
    int choice;
    size_t size;
    int index;
    
    do 
    {
        DisplayMenu();
        scanf("%d", &choice);
        
        switch (choice) 
        {
            case 1:
                printf("Enter size to allocate (in bytes): ");
                scanf("%zu", &size);
                
                void *ptr = Allocate(size);
                if (ptr != NULL && ptr_count < 100) 
                {
                    allocated_ptrs[ptr_count] = ptr;
                    allocated_sizes[ptr_count] = size;
                    printf("Allocated pointer #%d\n", ptr_count);
                    ptr_count++;
                }
                break;
                
            case 2:
                if (ptr_count == 0) 
                {
                    printf("No allocated pointers to free\n");
                    break;
                }
                
                printf("Enter index of pointer to free (0-%d): ", ptr_count - 1);
                scanf("%d", &index);
                for (int i = 0; i < ptr_count; i++) 
                {
                    printf("%d: Address %p, Size %zu bytes\n", 
                           i, allocated_ptrs[i], allocated_sizes[i]);
                }
                if (index >= 0 && index < ptr_count) 
                {
                    if (Free(allocated_ptrs[index])) 
                    {
                        for (int i = index; i < ptr_count - 1; i++) 
                        {
                            allocated_ptrs[i] = allocated_ptrs[i + 1];
                            allocated_sizes[i] = allocated_sizes[i + 1];
                        }
                        ptr_count--;
                        Merge();
                    }
                } 
                else 
                {
                    printf("Invalid index\n");
                }
                break;
                
            case 3:
                DisplayHeap();
                break;
                
            case 4:
                printf("Exiting...\n");
                break;
                
            default:
                printf("Invalid choice. Please enter 1-4.\n");
        }
        
    } while (choice != 4);
    
    return 0;
}
