#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "VM.h"

int main()
{
	initVM();
	{
		chunk_t chunk;
		initChunk(&chunk);

		writeConstant(&chunk, 1.2, 123);
		writeConstant(&chunk, 3.4, 123);
		writeChunk(&chunk, OP_ADD, 123);
		writeConstant(&chunk, 5.6, 123);
		writeChunk(&chunk, OP_DIVIDE, 123);
		writeChunk(&chunk, OP_NEGATE, 123);
		writeChunk(&chunk, OP_RETURN, 123);

		vm_interpret(&chunk);
	}
	{
		chunk_t chunk;
		initChunk(&chunk);
		//1 * 2 + 3
		writeConstant(&chunk, 1.0, 123);
		writeConstant(&chunk, 2.0, 123);
		writeChunk(&chunk, OP_MULTIPLY, 123);
		writeConstant(&chunk, 3.0, 123);
		writeChunk(&chunk, OP_ADD, 124);
		writeChunk(&chunk, OP_RETURN, 123);

		vm_interpret(&chunk);
	}
	{
		chunk_t chunk;
		initChunk(&chunk);
		//1 + 2 * 3
		writeConstant(&chunk, 1.0, 123);
		writeConstant(&chunk, 2.0, 123);
		writeConstant(&chunk, 3.0, 123); // This order is literally only difference from above 
		writeChunk(&chunk, OP_MULTIPLY, 123);
		writeChunk(&chunk, OP_ADD, 124);
		writeChunk(&chunk, OP_RETURN, 123);

		vm_interpret(&chunk);
	}
	{
		chunk_t chunk;
		initChunk(&chunk);
		//3 - 2 - 1
		writeConstant(&chunk, 3.0, 123);
		writeConstant(&chunk, 2.0, 123);
		writeChunk(&chunk, OP_SUBTRACT, 123);
		writeConstant(&chunk, 1.0, 123);
		writeChunk(&chunk, OP_SUBTRACT, 124);
		writeChunk(&chunk, OP_RETURN, 123);

		vm_interpret(&chunk);
	}
	{
		chunk_t chunk;
		initChunk(&chunk);
		//1 + 2 * 3 - 4 / -5
		writeConstant(&chunk, 1.0, 123);
		writeConstant(&chunk, 2.0, 123);
		writeConstant(&chunk, 3.0, 123);
		writeChunk(&chunk, OP_MULTIPLY, 124);
		writeConstant(&chunk, 4.0, 123);
		writeConstant(&chunk, -5.0, 123);
		writeChunk(&chunk, OP_DIVIDE, 123);
		writeChunk(&chunk, OP_SUBTRACT, 124);
		writeChunk(&chunk, OP_ADD, 124);
		writeChunk(&chunk, OP_RETURN, 123);

		vm_interpret(&chunk);
	}


	//disassembleChunk(&chunk, "test");
}