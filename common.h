#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define FORWARD_DEC(x) typedef struct x x;
#define OPAQUE_PTR(x) FORWARD_DEC(x)

#define KB 1024
#define MB KB * 1024
#define GB MB * 1024

#define NO_LINK static

#define FOR(i, max) for(int i = 0; i < max; i++)

typedef double value_t;