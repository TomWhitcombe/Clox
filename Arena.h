#pragma once
#include "common.h"

FORWARD_DEC(linearAllocator_t);

linearAllocator_t* linear_allocator_create(size_t reserve_size);
void* linear_allocator_push(linearAllocator_t* allocator, size_t size);
size_t linear_allocator_tell(linearAllocator_t* allocator);
void linear_allocator_reset(linearAllocator_t* allocator, bool shrink);
void linear_allocator_popTo(linearAllocator_t* allocator, size_t cursor_position, bool shrink);
void linear_allocator_destroy(linearAllocator_t* allocator);