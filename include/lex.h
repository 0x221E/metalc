#ifndef LEX_H
#define LEX_H

#include <stddef.h>

// Only to be used with compile-time known tokens.
#define TOKEN(tt, me) (struct token){.type=tt,.lexeme=me,.length=sizeof(me) - 1}

#define	TOKEN_INVALID		1
#define TOKEN_EOP		2
#define TOKEN_NUMBER		3
#define TOKEN_HEX		4
#define TOKEN_BINARY		5
#define TOKEN_SEMICOLON		6
#define TOKEN_NEWLINE		7
#define TOKEN_FUNCTION		8
#define TOKEN_G8		9
#define TOKEN_G16		10
#define TOKEN_G32		11
#define TOKEN_G64		12
#define TOKEN_IDENTIFIER	13
#define TOKEN_PACK		14
#define TOKEN_BRACES_OPEN	15
#define TOKEN_BRACES_CLOSE	16
#define TOKEN_PARAN_OPEN	17
#define TOKEN_PARAN_CLOSE	18
#define TOKEN_RETURN		19
#define TOKEN_EQUALSIGN         20
#define TOKEN_PLUS              21
#define TOKEN_MINUS             22
#define TOKEN_STAR              23
#define TOKEN_SLASH             24
#define TOKEN_BANG              25

const char* lex_get_token_name(int id);

#define LEX_NUMBER_ERR          0
#define LEX_HEX_ERR             1
#define LEX_BINARY_ERR          2


struct token {
	int type;
	size_t line;
	size_t length;
	char *lexeme;
};

struct lex {
	size_t pos;
	size_t size;
	char* buffer;
	const char* filepath;	
	size_t line;
	size_t linepos;
};

char lex_advance(struct lex *l, size_t o);
char lex_seek(struct lex *l, size_t o);

void lex_error(struct lex *l, int err_code, const char *s, ...);

struct token lex_number(struct lex *l);
struct token lex_directive(struct lex *l);
struct token lex_link_unit(struct lex *l);
struct token lex_next(struct lex *l);

#endif
