#pragma once
#include "common.h"

FORWARD_DEC(chunk_t);

#define STACK_MAX 256
typedef struct vm_t
{
	const chunk_t* chunk;
	uint8_t* instruction_ptr;
	value_t stack[STACK_MAX];
	value_t* stackTop;
} vm_t;

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} interpretResult_e;

void initVM();
interpretResult_e vm_interpret(const char* source);
void freeVM();