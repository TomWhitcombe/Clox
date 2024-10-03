#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "VM.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

NO_LINK void repl() {
    char line[1024];
    while(true) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        vm_interpret(line);
    }
}

NO_LINK char* readFile(const char* path) {
    FILE* file;
    fopen_s(&file, path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1); //TODO: Use an arena
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

NO_LINK void runFile(const char* path) {
    char* source = readFile(path);
    interpretResult_e result = vm_interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(const char** argv, int argc)
{

    init_trie();
    addKeyword("const", TOKEN_AND);
    addKeyword("comma", TOKEN_BANG);//BUG - const can't be found if comma exists
    build_trie();

    const char* string = "a const thing";
    tokenType_e token = getTokenType(&string[2], 5);

	initVM();
    if (argc == 1) {
        repl();
    }
    else if (argc == 2) {
       runFile(argv[1]);
    }
    else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }
}