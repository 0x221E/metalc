#include "lex.h"

#include <assert.h>
#include <string.h>

#include <platform/osal.h>

#include <stdio.h>

static struct token keywords[] = {
	TOKEN(TOKEN_FUNCTION, "fn"),
	TOKEN(TOKEN_G8, "g8"),
	TOKEN(TOKEN_G16, "g16"),
	TOKEN(TOKEN_G32, "g32"),
	TOKEN(TOKEN_G64, "g64"),
	TOKEN(TOKEN_PACK, "pack"),
	TOKEN(TOKEN_RETURN, "return")
};

inline int lex_is_digit(char c)
{
	return c >= '0' && c <= '9';
}

inline int lex_is_char(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline int lex_is_whitespace(char c)
{
	return (c == ' ' || c == '\r' || c == '\t'); 
}

void lex_skipwhitespace(struct lex *l)
{
	int len = 0;
	while (lex_seek(l, len) && 
	       lex_is_whitespace(lex_seek(l, len))) {
		len++;
	}

	l->pos += len; // maybe add safety? (edit lex_advance to add offset)
}

char lex_advance(struct lex *l)
{
	assert(l != NULL);
	if (!l->buffer[l->pos+1])
		return l->buffer[l->pos];
	l->pos++;
	return l->buffer[l->pos];
}

char lex_seek(struct lex *l, size_t o)
{
	assert(l != NULL);	
	if (l->buffer[l->pos + o])
		return l->buffer[l->pos + o];
	return '\0';
}

struct token lex_token_create(int tt, char* lexeme, size_t s)
{
	return (struct token) {.type = tt, .length = s, .lexeme = lexeme};
}

struct token lex_number(struct lex *l)
{
	assert(l != NULL);

	int state = 0;

	if (lex_seek(l, 0) == '0') {
		
		if (lex_seek(l, 1) && lex_seek(l, 1) == 'x') {
			state = 1;
			l->pos += 2;	
		}

		if (lex_seek(l, 1) && lex_seek(l, 1) == 'b') {
			state = 2;
			l->pos += 2;
		}
	}

	int start = l->pos;
	int len = 0;

	int tt;

	switch (state) {
	case 0:
		tt = TOKEN_NUMBER;
		while (lex_seek(l, len) && lex_is_digit(lex_seek(l, len)))
			len++;
		break;
	case 1:
		tt = TOKEN_HEX;
		while (lex_seek(l, len) && (lex_is_digit(lex_seek(l, len)) || lex_is_char(lex_seek(l, len))))
			len++;
		break;
	case 2:
		tt = TOKEN_BINARY;
		while (lex_seek(l, len) && (lex_seek(l, len) == '0' || lex_seek(l, len) == '1')) {
			len++;
		}
		break;
	default:
		return (struct token) {.type = TOKEN_INVALID, .lexeme = "number"};
	}	

	l->pos += len;

	return lex_token_create(tt, &l->buffer[start], len); 
}

struct token lex_directive(struct lex *l)
{
//	return TOKEN(TOKEN_TODO, "directive");
}

struct token lex_link_unit(struct lex *l)
{
//	return TOKEN(TOKEN_TODO, "link_unit");
}

struct token lex_keyword(struct lex *l) //APPROPRIATE INC
{
	size_t len = 0;
	size_t start = l->pos;

	while (lex_seek(l, len)) {
		if (lex_is_char(lex_seek(l, len)) ||
		    lex_is_digit(lex_seek(l, len)) ||
		    lex_seek(l, len) == '_') {
		       len++;
		} else {
			break;
		}
	}
	
	l->pos += len;

	int tt = TOKEN_IDENTIFIER;

	for (size_t i = 0; 
	    i < (sizeof(keywords) / sizeof(struct token)); // maybe convert to macro if used somewhere else
	    i++) {

		if (len != keywords[i].length) { // len check for security purposes.
			continue;
		}

		if (strncmp(&l->buffer[start], keywords[i].lexeme, keywords[i].length) == 0) {
			tt = keywords[i].type;	
			break;
		}
	}

	return (struct token) {.type = tt, .length=len, .lexeme = &l->buffer[start]};
	 
}

struct token lex_next(struct lex *l)
{
	assert(l != NULL);

	if (l->buffer[l->pos] == '\0' || l->pos >= l->size) {
		return (struct token) {.type = TOKEN_EOP, .lexeme = ""};
	}

	lex_skipwhitespace(l);

	if (lex_is_digit(lex_seek(l, 0))) {
		return lex_number(l);
	}
	
	switch (lex_seek(l, 0)) {
	case '\n':
		l->pos++;
		l->line++;
		return TOKEN(TOKEN_NEWLINE, "\\n");
	case '#':
		return lex_directive(l);
	case '$':
		return lex_link_unit(l);
	case ';':
		l->pos++;
		return TOKEN(TOKEN_SEMICOLON, ";");
	case '{':
		l->pos++;
		return TOKEN(TOKEN_BRACES_OPEN, "{");
	case '}':
		l->pos++;
		return TOKEN(TOKEN_BRACES_CLOSE, "}");
	case '(':
		l->pos++;
		return TOKEN(TOKEN_PARAN_OPEN, "(");
	case ')':
		l->pos++;
		return TOKEN(TOKEN_PARAN_CLOSE, ")");
	case '=':
		l->pos++;
		return TOKEN(TOKEN_EQUALSIGN, "=");
	case 'a'...'z':
	case 'A'...'Z':
		return lex_keyword(l);	
	default:
		return TOKEN(TOKEN_INVALID, "INVALID");
	}
}
