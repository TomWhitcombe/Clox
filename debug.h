#pragma once
#include "common.h"

FORWARD_DEC(chunk_t);

void disassembleChunk(chunk_t* chunk, const char* name);
int32_t disassembleInstruction(chunk_t* chunk, int32_t offset);