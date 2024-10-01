#pragma once
#include "common.h"
#include "tokenTypes.h"

void init_trie();
void addKeyword(const char* keyword, tokenType_e associatedToken);
void build_trie();

tokenType_e trie_getToken(const char* lexemeStart, const int32_t length);