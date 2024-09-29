#pragma once
#include "common.h"
#include "dArray.h"

typedef enum {
	OP_CONSTANT,
	OP_CONSTANT_LONG,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_RETURN,
} opcode_e;

typedef struct lineInfo_t
{
	int32_t line;
	int32_t count;
} lineInfo_t;

DARRAY(uint8_t, code_t);
DARRAY(value_t, valuesArray_t);
DARRAY(lineInfo_t, lines_t);

typedef struct chunk_t {
	code_t code;
	lines_t lines;//compressed
	valuesArray_t constants;
} chunk_t;

void initChunk(chunk_t* chunk);

void writeChunk(chunk_t* chunk, const uint8_t byte, const int32_t lineNumber);
void writeConstant(chunk_t* chunk, const value_t constant, const int32_t lineNumber);

uint32_t getLongConstantIndex(const chunk_t* chunk, const int32_t offset);

int32_t getLineNumberAtOffset(const chunk_t* chunk, const int32_t offset);