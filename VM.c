#include "VM.h"
#include "chunk.h"
#include "debug.h"
#include <stdio.h>

#define DEBUG_TRACE_EXECUTION

//Global vm from the book.
vm_t g_vm;

NO_LINK void resetStack()
{
	g_vm.stackTop = g_vm.stack;
}

NO_LINK void pushStack(value_t value)
{
	*g_vm.stackTop = value;
	g_vm.stackTop++;
}

NO_LINK value_t popStack()
{
	g_vm.stackTop--;
	return *g_vm.stackTop;
}

NO_LINK interpretResult_e run()
{
#define POP_BYTE() (*g_vm.instruction_ptr++)
#define POP_CONSTANT() (g_vm.chunk->constants.data[POP_BYTE()])
#define POP_LONG_CONSTANT() (g_vm.chunk->constants.data[POP_BYTE() | POP_BYTE() << 8 | POP_BYTE() << 16])
#define BINARY_OP(op) \
	do { \
      double b = popStack(); \
      double a = popStack(); \
      pushStack(a op b); \
    } while (false)

	while (true)
	{
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		if (g_vm.stack == g_vm.stackTop)
		{
			printf("[ EMPTY STACK ]");
		}
		for (value_t* slot = g_vm.stack; slot < g_vm.stackTop; slot++) {
			printf("[ ");
			printf("%g", *slot);
			printf(" ]");
		}
		printf("\n");
		disassembleInstruction(g_vm.chunk,
		/*offset*/(int32_t)(g_vm.instruction_ptr - g_vm.chunk->code.data));
#endif

		uint8_t instruction;
		switch (instruction = POP_BYTE())
		{
			case OP_RETURN:
			{
				printf("%g", popStack());
				printf("\n");
				return INTERPRET_OK;
			}
			case OP_CONSTANT: 
			{
				value_t constant = POP_CONSTANT();
				pushStack(constant);
				break;
			}
			case OP_CONSTANT_LONG: 
			{
				value_t constant = POP_LONG_CONSTANT();
				pushStack(constant);
				break;
			}
			case OP_ADD:      BINARY_OP(+); break;
			case OP_SUBTRACT: BINARY_OP(-); break;
			case OP_MULTIPLY: BINARY_OP(*); break;
			case OP_DIVIDE:   BINARY_OP(/); break;

			case OP_NEGATE:
			{
				// Not sure which I prefer. Is 1 ptr op worth the muddy code?
				value_t* topVal = (g_vm.stackTop - 1);
				*topVal = -(*topVal);
				//pushStack(-popStack())
				break;
			}
		}
	}
#undef BINARY_OP
#undef POP_BYTE
#undef POP_CONSTANT
#undef POP_LONG_CONSTANT
}

void initVM()
{
	resetStack();
}

interpretResult_e vm_interpret(const char* source)
{
	//g_vm.chunk = chunk;
	//g_vm.instruction_ptr = chunk->code.data;
	compile(source);
	return INTERPRET_OK;
}

void freeVM() 
{
}