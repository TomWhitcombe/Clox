#pragma once
#include "common.h"
#include "tokenTypes.h" //allowed cos it's just an enum i think :D


typedef struct {
	tokenType_e type;
	const char* start;
	int32_t length;
	int32_t line;
} token_t;

void initScanner(const char* source);

token_t scanToken();