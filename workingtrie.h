#include <stdio.h>

#include "Arena.h"

#define MAX_KEYWORDS_PER_NODE 59

enum nodeType
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
}nodeBig;

typedef struct childReferenceBig
{	
	uint8_t nodeType;
	nodeBig* parent;
	uint8_t offsetToNode;
	char value;
	uint8_t padding[sizeof(nodeBig) - 19];
}childReferenceBig;

typedef struct node
{
	uint8_t tokenValue;
	uint8_t numChildren;
} node_t;

typedef struct childReference
{
	uint8_t offsetToNode;
	char value;
} childReference;

typedef enum tokenType
{
	NADA,
	CNST,
	CNT,
	FOR
}tokenType;

static_assert(sizeof(nodeBig) == sizeof(childReferenceBig), "Must match size for ezier ptr math");
static_assert(sizeof(node_t) == sizeof(childReference), "Must match size for ezier ptr math");

tokenType getTokenType(node_t* header, const char* str)
{
	node_t* next = header;
	uint8_t depth = 0;
	FOR(depth, strlen(str))
	{
		bool match = false;
		FOR(c, next->numChildren)
		{
			childReference* child = next + c + 1;
			if (child->value == str[depth])
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

int main()
{
	linearAllocator_t* alloc = linear_allocator_create(MB);
	const char* keywords [] = {"cnst", "cnt", "for"};
	tokenType tokens [] = {CNST, CNT, FOR};
	uint8_t numKeywords = 3;//Just for testing
	
	nodeBig* header = linear_allocator_push(alloc, sizeof(nodeBig));
	header->nodeType = NODE_MAIN;

	FOR(k, numKeywords)
	{
		header->keywordIdxs[header->numKeywords++] = k;
	}

	nodeBig* n = header;
	childReferenceBig* pendingChildrenStack[64];
	uint8_t childrenStackCount = 0;

	while(n)
	{
		//find unique character offspring
		char uniqueOffspring[MAX_KEYWORDS_PER_NODE];
		uint8_t numUniqueOffspring = 0;
		FOR(k, n->numKeywords)
		{
			const char* keyword = keywords[n->keywordIdxs[k]];
			n->tokenValue = NADA;
			if(strlen(keyword) > n->depth)
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

		childReferenceBig* nextChild = NULL;
		FOR(uc, numUniqueOffspring)
		{
			childReferenceBig* child = linear_allocator_push(alloc, sizeof(childReferenceBig));
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
			n = linear_allocator_push(alloc, sizeof(nodeBig));
			n->nodeType = NODE_MAIN;

			nodeBig* parent = nextChild->parent;
			
			FOR(pk, parent->numKeywords)
			{
				const char* keyword = keywords[parent->keywordIdxs[pk]];
				if (keyword[parent->depth] == nextChild->value)
				{
					n->keywordIdxs[n->numKeywords++] = parent->keywordIdxs[pk];//TODO; assert don't have more than max per node
				}
			}

			n->depth = parent->depth + 1;

			size_t offset =  n - (nodeBig*)nextChild; // same size- so works;
			//todo assert offset < 255
			nextChild->offsetToNode = offset;
		}
		else
		{
			n = NULL;
		}
	}

	uint32_t tell = linear_allocator_tell(alloc);
	uint32_t numNodesAndChildren = tell / sizeof(nodeBig);

	node_t* searchHeader = NULL;

	FOR(n, numNodesAndChildren)
	{
		node_t* node = NULL;
		
		nodeBig* nextBigNode = header + n;
		if (nextBigNode->nodeType == NODE_MAIN)
		{
			node = linear_allocator_push(alloc, sizeof(node_t));
			node->numChildren = nextBigNode->numChildren;
			node->tokenValue = nextBigNode->tokenValue;
		}
		else
		{
			childReferenceBig* bigKid = (childReferenceBig*)nextBigNode;
			childReference* child = linear_allocator_push(alloc, sizeof(childReference));
			child->value = bigKid->value;
			child->offsetToNode = bigKid->offsetToNode;
		}

		if (searchHeader == NULL && node)
		{
			searchHeader = node;
		}
	}

	tokenType f = getTokenType(searchHeader, "for");
	tokenType c = getTokenType(searchHeader, "cnst");
	tokenType c2 = getTokenType(searchHeader, "cnt");
	tokenType nope = getTokenType(searchHeader, "cant");


	return 0;
}