#include "common.h"
#include "chunk.h"
#include "debug.h"

int main()
{
	chunk_t chunk;
	initChunk(&chunk);

	double val = 1.2;
	FOR (i,300)
	{
		writeConstant(&chunk, val, 123);
		val += 1.0;
	}
	
	writeChunk(&chunk, OP_RETURN, 123);


	disassembleChunk(&chunk, "test");
}