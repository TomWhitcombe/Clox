#pragma once
#include "common.h"

FORWARD_DEC(chunk_t);

void disassembleChunk(const chunk_t* chunk, const char* name);
int32_t disassembleInstruction(const chunk_t* chunk, const int32_t offset);