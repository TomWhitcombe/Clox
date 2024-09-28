#pragma once
#include "common.h"
#include "Arena.h"

#define DARRAY(type, arrayName) \
typedef struct arrayName{ \
	type* data;\
	linearAllocator_t* alloc;\
	int32_t count;\
}arrayName;\
\
void arrayName##_init(arrayName* array);\
type* arrayName##_push(arrayName* array, type data);

#define DARRAY_IMPL(type, arrayName) \
void arrayName##_init(arrayName* array){ array->count = 0; array->data = NULL; array->alloc = linear_allocator_create(GB);}\
\
type* arrayName##_push(arrayName* array, type data) {\
	type* t = (type*)linear_allocator_push(array->alloc, sizeof(type));\
	if(!t) {return NULL;}\
	*t = data;\
	array->count++;\
	if(array->data == NULL){array->data = t;}\
	return t;\
}