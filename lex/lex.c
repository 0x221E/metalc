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

#define __GET_TKN_NAME(TKN) case TKN: return #TKN

const char *lex_get_token_name(int id)
{
	switch (id) {
	__GET_TKN_NAME(TOKEN_INVALID);
	__GET_TKN_NAME(TOKEN_EOP);
	__GET_TKN_NAME(TOKEN_NUMBER);
	__GET_TKN_NAME(TOKEN_HEX);
	__GET_TKN_NAME(TOKEN_BINARY);
	__GET_TKN_NAME(TOKEN_FUNCTION);
	__GET_TKN_NAME(TOKEN_SEMICOLON);
	__GET_TKN_NAME(TOKEN_NEWLINE);
	__GET_TKN_NAME(TOKEN_G8);		
	__GET_TKN_NAME(TOKEN_G16);		
	__GET_TKN_NAME(TOKEN_G32);
	__GET_TKN_NAME(TOKEN_G64);		
	__GET_TKN_NAME(TOKEN_IDENTIFIER);	
	__GET_TKN_NAME(TOKEN_PACK);
	__GET_TKN_NAME(TOKEN_BRACES_OPEN);
	__GET_TKN_NAME(TOKEN_BRACES_CLOSE);
	__GET_TKN_NAME(TOKEN_PARAN_OPEN);
	__GET_TKN_NAME(TOKEN_PARAN_CLOSE);
	__GET_TKN_NAME(TOKEN_RETURN);
	__GET_TKN_NAME(TOKEN_EQUALSIGN);
	__GET_TKN_NAME(TOKEN_PLUS);
	__GET_TKN_NAME(TOKEN_MINUS);
	__GET_TKN_NAME(TOKEN_STAR);
	__GET_TKN_NAME(TOKEN_SLASH);
	default:
		print("lex_get_token_name: token not found: %d!", id);
		exitc(1);
	}
}

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

char lex_seek(struct lex *l, size_t o)
{
	assert(l != NULL);	
	if (l->buffer[l->pos + o])
		return l->buffer[l->pos + o];
	return 0;
}

char lex_advance(struct lex *l, size_t o)
{
	assert(l != NULL);

	if (l->buffer[l->pos + o] == 0 ||
	    l->pos + o >= l->size) {
		l->pos = l->size;
		return 0;
	}
	
	l->pos += o;
	return lex_seek(l, o);
}

struct token lex_token_create(int tt, char* lexeme, size_t s)
{
	return (struct token) {.type = tt, .length = s, .lexeme = lexeme};
}

void lex_error(struct lex *l, int err_code, const char *s, ...)
{
	assert(l != NULL);
	va_list args;
	va_start(args, s);
	print("%s:%d: error: ", l->filepath, l->line);
	printv(s, args);
	print("\n");

	int start = 0;
	int end = 0;

	int pos = 0;
	int curr_line = 0;

	// inefficient, but safe for now
	
	while (l->buffer[pos]) {
		if (l->buffer[pos] == '\n') {
			curr_line+=1;
		}
		pos++;

		if (curr_line == l->line - 1) {
			start = pos;

			while (l->buffer[pos]) {
				if (l->buffer[pos] == '\n')
					break;
				else
					pos++;
			}
			end = pos;
		}
	}
       
	int size = (int)(end - start);

	print("%d |    %.*s\n", l->line, size, &l->buffer[start]);

	print("  |");
	size_t cursor_loc = (l->pos - start) + 4;
	for (size_t i = 0; i < cursor_loc; i++) {
		print(" ");
	}
	print("^\n");
	
	va_end(args);
	exitc(1);
}

struct token lex_number(struct lex *l)
{
	assert(l != NULL);

	int state = 0;

	char current_tkn = lex_seek(l, 0);		
	char next_tkn = lex_seek(l, 1);

	if (current_tkn == '0') {
		
		if (next_tkn == 'x') {
			state = 1;
			lex_advance(l, 2);
		}

		if (next_tkn == 'b') {
			state = 2;
			lex_advance(l, 2);
		}
	}

	int start = l->pos;
	int len = 0;

	int tt;

	switch (state) {
	case 0:
		tt = TOKEN_NUMBER;
		while (lex_seek(l, len)) {
			if (lex_is_digit(lex_seek(l, len))) {
				len++;
			} else if (lex_is_char(lex_seek(l, len))) {
				lex_error(l, LEX_NUMBER_ERR, "A number literal must not contain any non-digit characters.");
			} else {
				break;
			}
		}
		break;
	case 1:
		tt = TOKEN_HEX;
		while (lex_seek(l, len) && (lex_is_digit(lex_seek(l, len)) || lex_is_char(lex_seek(l, len))))
			len++;
		break;
	case 2:
		tt = TOKEN_BINARY;
		while (lex_seek(l, len)) {
			char curr = lex_seek(l, len);

			if ((curr == '0') ||
			    (curr == '1')) {
				len++;
				continue;
			} else if (lex_is_digit(curr)) {
				lex_error(l, LEX_BINARY_ERR, "A binary number should not have digits other than 0 or 1.");
			} else if (lex_is_char(curr)) {
				lex_error(l, LEX_BINARY_ERR,"A binary number should not contain alphanumeric characters.");
			}

			break;
		}
		break;
	default:
		return (struct token) {.type = TOKEN_INVALID, .lexeme = "number"};
	}	

	lex_advance(l, len);

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

       	lex_advance(l, len); 

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

	// so much repetition, ugly, but can stay
	switch (lex_seek(l, 0)) {
	case '\n':
		lex_advance(l, 1);
		l->line++;
		return TOKEN(TOKEN_NEWLINE, "\\n");
	case '#':
		return lex_directive(l);
	case '$':
		return lex_link_unit(l);
	case ';':
		lex_advance(l,1);
		return TOKEN(TOKEN_SEMICOLON, ";");
	case '{':
		lex_advance(l, 1);
		return TOKEN(TOKEN_BRACES_OPEN, "{");
	case '}':
		lex_advance(l, 1);
		return TOKEN(TOKEN_BRACES_CLOSE, "}");
	case '(':
		lex_advance(l, 1);
		return TOKEN(TOKEN_PARAN_OPEN, "(");
	case ')':
		lex_advance(l, 1);
		return TOKEN(TOKEN_PARAN_CLOSE, ")");
	case '=':
		lex_advance(l, 1);
		return TOKEN(TOKEN_EQUALSIGN, "=");
	case '*':
		lex_advance(l, 1);
		return TOKEN(TOKEN_STAR, "*");
	case '+':
		lex_advance(l, 1);
		return TOKEN(TOKEN_PLUS, "+");
	case '-':
		lex_advance(l, 1);
		return TOKEN(TOKEN_MINUS, "-");
	case '/':
		lex_advance(l, 1);
		return TOKEN(TOKEN_SLASH, "/");
	default:
		char curr = lex_seek(l, 0);
		if (lex_is_char(curr) || (curr == '_')) {
			return lex_keyword(l);
		}
		return TOKEN(TOKEN_INVALID, "INVALID");
	}
}
