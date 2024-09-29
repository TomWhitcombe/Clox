#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "VM.h"

int main()
{
	chunk_t chunk;
	initChunk(&chunk);

	double val = 1.2;

	writeConstant(&chunk, val, 123);
	
	writeChunk(&chunk, OP_RETURN, 123);
	initVM();
	vm_interpret(&chunk);

	//disassembleChunk(&chunk, "test");
}