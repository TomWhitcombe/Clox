#pragma once
#include "trie.h"
#include "arena.h"
#include <string.h>

#define MAX_KEYWORDS_PER_NODE 59

enum nodeType_e
{
	NODE_MAIN,
	NODE_CHILD
};

typedef struct nodeBig
{
	uint8_t nodeType;
	uint8_t keywordIdxs[MAX_KEYWORDS_PER_NODE];
	uint8_t numKeywords;
	uint8_t depth;

	uint8_t tokenValue;
	uint8_t numChildren;
}nodeBig_t;

typedef struct childReferenceBig
{
	uint8_t nodeType;
	nodeBig_t* parent;
	uint8_t offsetToNode;
	char value;
	uint8_t padding[45];
}childReferenceBig_t;

typedef struct node
{
	uint8_t tokenValue;
	uint8_t numChildren;
} node_t;

typedef struct childReference
{
	uint8_t offsetToNode;
	char value;
} childReference_t;

static_assert(sizeof(nodeBig_t) == sizeof(childReferenceBig_t), "Must match size for ezier ptr math");
static_assert(sizeof(node_t) == sizeof(childReference_t), "Must match size for ezier ptr math");

typedef struct trie
{
	node_t* searchHead;
	nodeBig_t* bigHead;
	linearAllocator_t* arena;

	int32_t numKeywords;
	const char* keywords[64];
	tokenType_e tokens[64];
}keywordTrie_t;

keywordTrie_t g_keywords;

void init_trie() 
{
	g_keywords.numKeywords = 0;

	g_keywords.arena = linear_allocator_create(MB);
	g_keywords.bigHead = linear_allocator_push(g_keywords.arena, sizeof(nodeBig_t));
}

void addKeyword(const char* keyword, tokenType_e associatedToken)
{
	g_keywords.keywords[g_keywords.numKeywords] = keyword;
	g_keywords.tokens[g_keywords.numKeywords] = associatedToken;
	
	g_keywords.numKeywords++;
}

tokenType_e getTokenType(const char* lexemeStart, const int32_t length)
{
	node_t* next = g_keywords.searchHead;
	uint8_t depth = 0;
	FOR(depth, length)
	{
		bool match = false;
		FOR(c, next->numChildren)
		{
			childReference_t* child = next + c + 1;

			if (child->value == lexemeStart[depth])
			{
				next = child + child->offsetToNode;
				match = true;
				break;
			}
		}

		if (!match)
		{
			break;
		}
	}

	return next->tokenValue;
}

void build_trie()
{
	linearAllocator_t* alloc = g_keywords.arena;
	const char** keywords = g_keywords.keywords;
	const tokenType_e* tokens = g_keywords.tokens;
	const int32_t numKeywords = g_keywords.numKeywords;

	nodeBig_t* bigHead = g_keywords.bigHead;

	bigHead->nodeType = NODE_MAIN;

	FOR(k, numKeywords)
	{
		bigHead->keywordIdxs[bigHead->numKeywords++] = k;
	}

	nodeBig_t* n = bigHead;
	childReferenceBig_t* pendingChildrenStack[64];
	uint8_t childrenStackCount = 0;

	while (n)
	{
		//find unique character offspring
		char uniqueOffspring[MAX_KEYWORDS_PER_NODE];
		uint8_t numUniqueOffspring = 0;
		FOR(k, n->numKeywords)
		{
			const char* keyword = keywords[n->keywordIdxs[k]];
			n->tokenValue = TOKEN_IDENTIFIER;
			if (strlen(keyword) > n->depth)
			{
				char nextChar = keyword[n->depth];
				bool shouldAddNewUnique = true;
				FOR(u, numUniqueOffspring)
				{
					char uniqueChar = uniqueOffspring[u];
					if (uniqueChar == nextChar)
					{
						shouldAddNewUnique = false;
						break;
					}
				}

				if (shouldAddNewUnique)
				{
					uniqueOffspring[numUniqueOffspring++] = nextChar;
				}
			}
			else
			{
				printf("End of node chain for %s", keyword);
				//TODO: Really should assert only one token
				n->tokenValue = tokens[n->keywordIdxs[k]];
			}
		}

		childReferenceBig_t* nextChild = NULL;
		FOR(uc, numUniqueOffspring)
		{
			childReferenceBig_t* child = linear_allocator_push(alloc, sizeof(childReferenceBig_t));
			child->parent = n;
			child->value = uniqueOffspring[uc];
			child->nodeType = NODE_CHILD;
			if (nextChild == NULL)
			{
				nextChild = child;
			}
			else
			{
				pendingChildrenStack[childrenStackCount++] = child;
			}
			n->numChildren++;
		}

		if (nextChild == NULL && childrenStackCount > 0)
		{
			childrenStackCount--;
			nextChild = pendingChildrenStack[childrenStackCount];
		}

		if (nextChild)
		{
			n = linear_allocator_push(alloc, sizeof(nodeBig_t));
			n->nodeType = NODE_MAIN;

			nodeBig_t* parent = nextChild->parent;

			FOR(pk, parent->numKeywords)
			{
				const char* keyword = keywords[parent->keywordIdxs[pk]];
				if (keyword[parent->depth] == nextChild->value)
				{
					n->keywordIdxs[n->numKeywords++] = parent->keywordIdxs[pk];//TODO; assert don't have more than max per node
				}
			}

			n->depth = parent->depth + 1;

			size_t offset = n - (nodeBig_t*)nextChild; // same size- so works;
			//todo assert offset < 255
			nextChild->offsetToNode = offset;
		}
		else
		{
			n = NULL;
		}
	}

	uint32_t tell = linear_allocator_tell(alloc);
	uint32_t numNodesAndChildren = tell / sizeof(nodeBig_t);

	FOR(n, numNodesAndChildren)
	{
		node_t* node = NULL;

		nodeBig_t* nextBigNode = bigHead + n;
		if (nextBigNode->nodeType == NODE_MAIN)
		{
			node = linear_allocator_push(alloc, sizeof(node_t));
			node->numChildren = nextBigNode->numChildren;
			node->tokenValue = nextBigNode->tokenValue;
		}
		else
		{
			childReferenceBig_t* bigKid = (childReferenceBig_t*)nextBigNode;
			childReference_t* child = linear_allocator_push(alloc, sizeof(childReference_t));
			child->value = bigKid->value;
			child->offsetToNode = bigKid->offsetToNode;
		}

		if (g_keywords.searchHead == NULL && node)
		{
			g_keywords.searchHead = node;
		}
	}

	assert(g_keywords.searchHead);
}