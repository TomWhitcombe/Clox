#include "Arena.h"
#include <Windows.h>

// Windows page size
#define PAGE_SIZE 4096

typedef struct linearAllocator_t {
    void* base_address;
    size_t reserved_size;
    size_t committed_size;
    size_t used;
} linearAllocator_t;

// Create the allocator, reserve the address space and commit the first page
linearAllocator_t* linear_allocator_create(size_t reserve_size) 
{
    linearAllocator_t* allocator = (linearAllocator_t*)malloc(sizeof(linearAllocator_t));
    if (!allocator)
    {
        printf("Failed to allocate allocator book keeping struct\n");
        return NULL;
    }

    // Reserve a large block of virtual address space
    allocator->base_address = VirtualAlloc(NULL, reserve_size, MEM_RESERVE, PAGE_READWRITE);
    if (!allocator->base_address) {
        printf("Failed to reserve address space\n");
        free(allocator);
        return NULL;
    }

    // Initially commit the first page
    allocator->committed_size = PAGE_SIZE;
    void* first_page = VirtualAlloc(allocator->base_address, PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);
    if (!first_page) {
        printf("Failed to commit initial page\n");
        VirtualFree(allocator->base_address, 0, MEM_RELEASE);
        free(allocator);
        return NULL;
    }

    allocator->reserved_size = reserve_size;
    allocator->used = 0;

    return allocator;
}

// Allocates memory from the linear allocator, committing more pages as necessary
void* linear_allocator_push(linearAllocator_t* allocator, size_t size) 
{
    size_t new_used = allocator->used + size;

    // If we exceed the currently committed size, we need to commit more pages
    if (new_used > allocator->committed_size) {
        size_t commit_size = ((new_used + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
        size_t size_to_commit = commit_size - allocator->committed_size;

        // Commit additional pages
        void* result = VirtualAlloc((char*)allocator->base_address + allocator->committed_size,
            size_to_commit, MEM_COMMIT, PAGE_READWRITE);
        if (!result) {
            printf("Failed to commit more memory\n");
            return NULL;
        }

        allocator->committed_size += size_to_commit;
    }

    // Allocate memory by returning the current used pointer and incrementing it
    void* result = (char*)allocator->base_address + allocator->used;
    allocator->used = new_used;

    return result;
}

// Returns the current cursor position (used memory size)
size_t linear_allocator_tell(linearAllocator_t* allocator) 
{
    return allocator->used;
}

// Resets the allocator to a specific cursor position, optionally decommitting memory
void linear_allocator_popTo(linearAllocator_t* allocator, size_t cursor_position, bool shrink) 
{
    if (cursor_position > allocator->used) {
        printf("Invalid cursor position: exceeds current used memory\n");
        return;
    }

    // If shrink is enabled, decommit memory beyond the cursor position
    if (shrink && cursor_position < allocator->committed_size) {
        size_t aligned_cursor = (cursor_position + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;
        // Never decommit the first page
        aligned_cursor = aligned_cursor == 0 ? PAGE_SIZE : aligned_cursor;
        if (aligned_cursor < allocator->committed_size) {
            VirtualFree((char*)allocator->base_address + aligned_cursor,
                allocator->committed_size - aligned_cursor, MEM_DECOMMIT);
            allocator->committed_size = aligned_cursor;
        }
    }

    // Reset the used memory pointer to the cursor position
    allocator->used = cursor_position;
}

// Resets the allocator, optionally decommitting back to a single page
void linear_allocator_reset(linearAllocator_t* allocator, bool shrink) {
    linear_allocator_popTo(allocator, 0, shrink);
}

// Frees the entire allocator (releases both reserved and committed memory)
void linear_allocator_destroy(linearAllocator_t* allocator) {
    // Release the entire reserved address space, including decommitted pages
    if (allocator->base_address) {
        VirtualFree(allocator->base_address, 0, MEM_RELEASE);  // Release both reserved and committed memory
    }

    // Free the allocator structure itself
    free(allocator);
}