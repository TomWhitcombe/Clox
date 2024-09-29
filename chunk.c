#include "chunk.h"

DARRAY_IMPL(uint8_t, code_t);
DARRAY_IMPL(value_t, valuesArray_t);
DARRAY_IMPL(lineInfo_t, lines_t);

void initChunk(chunk_t* chunk) {
    code_t_init(&chunk->code);
    valuesArray_t_init(&chunk->constants);
    lines_t_init(&chunk->lines);
}

void writeChunk(chunk_t* chunk, const uint8_t byte, const int32_t lineNumber) {
    code_t_push(&chunk->code, byte);
    if (chunk->lines.count)
    {
        lineInfo_t* currentLine = &chunk->lines.data[chunk->lines.count - 1];
        if (currentLine->line == lineNumber)
        {
            currentLine->count++;
            return;
        }
    }
    lines_t_push(&chunk->lines, (lineInfo_t) { lineNumber, 1 });
}

NO_LINK int32_t addConstant(valuesArray_t* constantArray, value_t value)
{
    assert(valuesArray_t_push(constantArray, value));
    return constantArray->count - 1;
}

void writeConstant(chunk_t* chunk, const value_t constant, const int32_t lineNumber)
{
    const uint32_t SHORT_CONSTANT_LIMIT = 255u;
    uint32_t constantIndex = addConstant(&chunk->constants, constant);
    // TODO: If > 24bit range then yell; we can't store that
    if (constantIndex <= SHORT_CONSTANT_LIMIT)
    {
        writeChunk(chunk, OP_CONSTANT, lineNumber);
        writeChunk(chunk, (uint8_t)constantIndex, lineNumber);
    }
    else
    {
        writeChunk(chunk, OP_CONSTANT_LONG, lineNumber);
        uint8_t little  = (uint8_t)constantIndex;
        uint8_t mid     = (uint8_t)(constantIndex >> 8);
        uint8_t top     = (uint8_t)(constantIndex >> 16);
        writeChunk(chunk, little, lineNumber);
        writeChunk(chunk, mid, lineNumber);
        writeChunk(chunk, top, lineNumber);
    }
}

uint32_t getLongConstantIndex(const chunk_t* chunk, const int32_t offset)
{
    uint8_t little  = chunk->code.data[offset];
    uint8_t mid     = chunk->code.data[offset + 1];
    uint8_t top     = chunk->code.data[offset + 2];

    return little | (mid << 8) | (top << 16);
}

int32_t getLineNumberAtOffset(const chunk_t* chunk, const int32_t offset)
{
    assert(offset < chunk->code.count);

    int32_t total = 0;
    for (int32_t i = 0; i < chunk->lines.count; i++)
    {
        lineInfo_t currentLine = chunk->lines.data[i];
        total += currentLine.count;
        if (offset < total)
        {
            return currentLine.line;
        }
    }

    return -1;
}