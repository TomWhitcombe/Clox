#include "scanner.h"
#include "trie.h"
#include <stdio.h>
#include <string.h>

typedef struct {
	const char* start;
	const char* current;
	int32_t line;
} scanner_t;

scanner_t g_scanner;

void initScanner(const char* source)
{
	g_scanner.start = source;
	g_scanner.current = source;
	g_scanner.line = 1;

	init_trie();
}

NO_LINK bool isAtEnd() {
	return *g_scanner.current == '\0';
}

NO_LINK token_t makeToken(tokenType_e type) {
	token_t token;
	token.type = type;
	token.start = g_scanner.start;
	token.length = (int32_t)(g_scanner.current - g_scanner.start);
	token.line = g_scanner.line;
	return token;
}

NO_LINK token_t errorToken(const char* message) {
	token_t token;
	token.type = TOKEN_ERROR;
	token.start = message;
	token.length = (int32_t)strlen(message);
	token.line = g_scanner.line;
	return token;
}

NO_LINK char advance() {
	g_scanner.current++;
	return g_scanner.current[-1];
}

NO_LINK char peek()
{
	return *g_scanner.current;
}

NO_LINK char peekNext()
{
	if (isAtEnd())
	{
		return '\0';
	}

	return *(g_scanner.current + 1);
}

NO_LINK bool match(char expected) {
	if (isAtEnd())
	{
		return false;
	}
	if (*g_scanner.current != expected)
	{
		return false;
	}
	g_scanner.current++;
	return true;
}

NO_LINK bool isAlpha(const char c)
{
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_';
}

NO_LINK bool isDigit(const char c)
{
	return (c >= '0' && c <= '9');
}

token_t scanToken() {
	g_scanner.start = g_scanner.current;

	if (isAtEnd()) return makeToken(TOKEN_EOF);

	char c = advance();
	
	// skip whitespace
	bool skipping = true;
	while (skipping)
	{
		char c = peek();
		switch (c)
		{
			case ' ':
			case '\t':
			case '\r':
				advance();
				break;
			case '\n':
				g_scanner.line++;
				advance();
				break;
			case '/':
				if (peekNext() == '/')
				{
					while (peek() != '\n' && !isAtEnd())
					{
						advance();
					}
				}
				else
				{
					// it's a divide :D
					skipping = false;
				}
				break;
			default:
				skipping = false;
				break;
		}
	}

	//Handle number literals differently; if we used the switch we'd need 10 cases; NO thx m8
	if (isDigit(c))
	{
		while (isDigit(peek()))
		{
			advance();
		}

		if (peek() == '.' && isDigit(peekNext()))
		{
			advance();//passed the .
			while (isDigit(peek()))
			{
				advance();
			}
		}

		return makeToken(TOKEN_NUMBER);
	}

	//symbols
	if (isAlpha(c))
	{
		while ( isAlpha(peek()) || isDigit(peek()) )
		{
			advance();
		}
		makeToken(TOKEN_IDENTIFIER);
	}

	// Main switch
	switch (c) 
	{
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);
		case '-': return makeToken(TOKEN_MINUS);
		case '+': return makeToken(TOKEN_PLUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);
		case '!':
			return makeToken(
				match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=':
			return makeToken(
				match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<':
			return makeToken(
				match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>':
			return makeToken(
				match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
		case '"':
		{
			// String literal
			while (peek() != '"' && !isAtEnd())
			{
				if (peek() == '\n')
				{
					g_scanner.line++;
				}
			 	advance();
			}

			if (isAtEnd()) 
			{ 
				return errorToken("Non terminated string"); 
			}

			//advance past the closing "
			advance();

			makeToken(TOKEN_STRING);
		}

	}

	return errorToken("Unexpected character.");
}