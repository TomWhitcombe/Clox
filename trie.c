#pragma once
#include "trie.h"
#include "arena.h"
#include <string.h>

typedef struct trie_child
{
	char value;
	uint8_t nodeOffset; // uint16 offset from child position
}trie_child_t;

typedef struct trie_node
{
	uint8_t tokenValue; // TOKEN_indentifier == none end
	uint8_t numChildren;
}trie_node_t;

typedef struct trie
{
	trie_node_t* head;
	linearAllocator_t* arena;

	int32_t numKeywords;
	const char* keywords[64];
	tokenType_e tokens[64];
}keywordTrie_t;

keywordTrie_t g_keywords;

void init_trie() 
{
	g_keywords.arena = linear_allocator_create(MB);
	g_keywords.numKeywords = 0;
	g_keywords.head = linear_allocator_push(g_keywords.arena, sizeof(trie_node_t));
}

void addKeyword(const char* keyword, tokenType_e associatedToken)
{
	g_keywords.keywords[g_keywords.numKeywords] = keyword;
	g_keywords.tokens[g_keywords.numKeywords] = associatedToken;
	
	g_keywords.numKeywords++;
}

NO_LINK trie_node_t* findNode(char* word, int32_t charIndex)
{
	trie_node_t* node = g_keywords.head;
	int32_t searchIndex = 0;
	while (searchIndex < charIndex)
	{
		FOR(childIndex, node->numChildren)
		{
			trie_child_t* child = node + childIndex + 1; //unify
			if (child->value == word[searchIndex])
			{
				node = child + child->nodeOffset;
			}
		}
		searchIndex++;
	}

	return node;
}

void build_trie()
{
	// prepass to find longest keyword
	size_t longestKeyword = 0;
	FOR(kwIdx, g_keywords.numKeywords)
	{
		const char* keyword = g_keywords.keywords[kwIdx];
		size_t l = strlen(keyword);
		if (l > longestKeyword)
		{
			longestKeyword = l;
		}
	}

	FOR(charIndex, longestKeyword)
	{
		trie_child_t* addedChildren[32];
		tokenType_e tokenType[32];
		uint32_t numAddedChildren = 0;

		FOR(kwIdx, g_keywords.numKeywords)
		{
			const char* keyword = g_keywords.keywords[kwIdx];
			size_t length = strlen(keyword);

			//TODO: If at length should be add a token
			if (length <= charIndex)
			{
				continue;
			}

			trie_node_t* node = charIndex == 0 ? g_keywords.head : findNode(keyword, charIndex);
			bool shouldAdd = true;
			FOR(childIndex, node->numChildren)
			{
				static_assert(sizeof(trie_child_t) == sizeof(trie_node_t), "Bugger");
				trie_child_t* child = node + childIndex + 1;
				if (child->value == keyword[charIndex])
				{
					shouldAdd = false;
					break;
				}
			}

			if (shouldAdd)
			{
				trie_child_t* child = linear_allocator_push(g_keywords.arena, sizeof(trie_child_t));
				child->value = keyword[charIndex];
				child->nodeOffset = 0;//TODO: Figure when to allocate node
				node->numChildren++;
				tokenType[numAddedChildren] = (charIndex == length - 1) ? g_keywords.tokens[kwIdx] : TOKEN_IDENTIFIER;
				addedChildren[numAddedChildren++] = child;
			}
		}

		FOR(addedIdx, numAddedChildren)
		{
			trie_child_t* child = addedChildren[addedIdx];
			trie_node_t* newNode = linear_allocator_push(g_keywords.arena, sizeof(trie_node_t));
			newNode->tokenValue = tokenType[addedIdx];
			child->nodeOffset = newNode - child;
		}
	}
}


/*
[HEAD] 0
[c1][c2][c3]
[CH1]

*/