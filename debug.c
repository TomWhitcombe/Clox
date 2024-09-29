#include "debug.h"
#include "chunk.h"
#include <stdio.h>

NO_LINK int simpleInstruction(const char* name, int offset) 
{
    printf("%s\n", name);
    return offset + 1;
}

NO_LINK int constantInstruction(const char* name,const chunk_t* chunk,
    int offset, bool isLong) {
    uint32_t constantIndex = 0;
    uint32_t sizeOffset = 0;
    if (isLong)
    {
        constantIndex   = getLongConstantIndex(chunk, offset + 1);
        sizeOffset      = 4;
    }
    else
    {
        constantIndex   = chunk->code.data[offset + 1];
        sizeOffset      = 2;
    }
     
    printf("%-16s %4d '", name, constantIndex);
    printf("%g", chunk->constants.data[constantIndex]); //skipped values impl from book
    printf("'\n");

    return offset + sizeOffset;
}

static int32_t s_lastLine = 0;
void disassembleChunk(const chunk_t* chunk, const char* name) {
    printf("== %s ==\n", name);
    s_lastLine = 0;
    for (int offset = 0; offset < chunk->code.count;) 
    {
        // This for loop is weird and I'm not sure I like it (book)
        offset = disassembleInstruction(chunk, offset);
    }
}

int32_t disassembleInstruction(const chunk_t* chunk, const int32_t offset) {
    printf("%04d ", offset);
    assert(offset < chunk->code.count);

    int32_t line = getLineNumberAtOffset(chunk, offset);
    if (line != s_lastLine)
    {
        s_lastLine = line;
        printf("%4d ", line);
    }
    else
    {
        printf("   | ");
    }

    uint8_t instruction = chunk->code.data[offset];
    switch (instruction) {
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset, /*islong*/false);
    case OP_CONSTANT_LONG:
        return constantInstruction("OP_CONSTANT_LONG", chunk, offset, /*islong*/true);
    case OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);
    case OP_ADD:
        return simpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}