#pragma once
#include "common.h"
#include "tokenTypes.h"

void init_trie();
void addKeyword(const char* keyword, tokenType_e associatedToken);
void build_trie();

bool isKeyword(const char* lexemeStart, const int32_t length, tokenType_e* out_tokenType);