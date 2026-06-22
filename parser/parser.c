/**
 * @file parser.c
 * @brief 
 */
#include <parser.h>

#include <assert.h>

#include <platform/osal.h>
#include <lib/arena.h>
#include <lib/vector.h>
#include <lex.h>

#include <stdlib.h>

void parser_print_error(struct parser *p, const char *e)
{
	print("error: %s\n", e);
}

struct token* parser_previous(struct parser *p)
{
	if (p->pos < 1) {
		return &((struct token*)p->tokens->start)[0];
	}
	return &((struct token*)p->tokens->start)[p->pos - 1];
}

struct token* parser_advance(struct parser *p)
{
	if (p->pos >= p->size) {
		return &((struct token*)p->tokens->start)[p->size];
	}
	p->pos++;
	return &((struct token*)p->tokens->start)[p->pos];
}

struct token* parser_seek(struct parser *p, size_t o)
{
	if (p->pos + o >= p->size) {
		return &((struct token*)p->tokens->start)[p->size];
	}
	return &((struct token*)p->tokens->start)[p->pos + o];
}

int parser_expect(struct parser *p, int t, char *e)
{
	assert(p != NULL);

	if (parser_seek(p, 0)->type == t)
		return 0;

	p->error = 1;
	parser_print_error(p, e);
}

int parser_match(struct parser *p, size_t n, ...)
{
	assert(p != NULL);
	va_list args;
	va_start(args, p);

	int res = 0;
	
	struct token *t = parser_seek(p, 0);
	for (int i = 0; i < n; i++) {
		if (t->type == va_arg(args, int)) {
			res = 1;
			parser_advance(p);
			goto cleanup;
		}
	}

 cleanup:
	va_end(args);
	return res;
}

void * parser_parse_primary(struct parser *p)
{
	assert(p != NULL);

	struct node_primary* n = (struct node_primary*)region_alloc(p->buffer, sizeof(struct node_primary));

	n->ntype = N_LITERAL;
	
	struct token *tkn = parser_seek(p, 0);
	char * tkn_str = tkn->lexeme;
	
	switch (tkn->type) {
	case TOKEN_NUMBER:
		n->is_number = 1;
		n->value.number = atoi(tkn_str);
		break;
	case TOKEN_HEX:
		n->is_number = 1;
		print("TODO: to be implemented!!\n");
		break;
	case TOKEN_BINARY:
		print("TODO: to be implemented!\n");
		break;
	}

	parser_advance(p);
	return n;
}

void *parser_parse_unary(struct parser *p)
{
	if (parser_match(p, 2, TOKEN_MINUS, TOKEN_BANG)) {
		struct token *operator = parser_previous(p);
		void *unary = parser_parse_unary(p);
		
		struct node_unary *mem = region_alloc(p->buffer, sizeof(struct node_unary));
		*mem = (struct node_unary) {.ntype = N_UNARY, .operator=*operator, .expr=unary};
		return mem;
	}

	return parser_parse_primary(p);
}

void *parser_parse_term(struct parser *p)
{
	assert(p != NULL);

	void *expr = parser_parse_unary(p);

	while (parser_match(p, 2, TOKEN_STAR, TOKEN_SLASH)) {
		struct token *operator = parser_previous(p);
		void *right = parser_parse_unary(p);
		
		void *tmp = expr;
		expr = region_alloc(p->buffer, sizeof(struct node_binary));
		*((struct node_binary*)expr) = (struct node_binary) {.ntype = N_BINARY, .operator=*operator, .left = tmp, .right = right};
	}

	return expr;
}

void *parser_parse_factor(struct parser *p)
{
	assert(p != NULL);

	void *expr = parser_parse_term(p);

	while (parser_match(p, 2, TOKEN_PLUS, TOKEN_MINUS)) {
		struct token *operator = parser_previous(p);
		void *right = parser_parse_term(p);
		 
		void *tmp = expr;
		expr = region_alloc(p->buffer, sizeof(struct node_binary));
		*((struct node_binary*)expr) = (struct node_binary) {.ntype = N_BINARY, .operator=*operator, .left = tmp, .right = right};
	}

	return expr;
}

void *parser_parse_expression(struct parser *p)
{
	assert(p != NULL);
}

void *parser_parse_statement(struct parser *p)
{
	assert(p != NULL);
	parser_parse_primary(p);
	parser_expect(p, TOKEN_SEMICOLON, "A statement must end with a semicolon!");
}

void parser_parse(struct parser *p)
{
	assert(p != NULL);

	while (p->pos < p->size && (((struct token*)p->tokens->start) + p->pos) != NULL) {
		vector_emplace_back(p->result, parser_parse_factor(p));
	}
}
