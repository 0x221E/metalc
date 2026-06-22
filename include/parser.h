/**
 * @file parser.h
 * @author x221e
 * @brief parser
 * @description
 * I am opting-in to AST producing compiler design. It is not strictly
 * needed in this context, however, it will produce optimization benefits
 * in the future.
 *
 * @warning DEFINETLY NEEDS ARENA ALLOC IMPL // prototyping one under lib/
 */

#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#include <lex.h>

typedef struct vector vector;

struct parser {
	struct region *tokens;
	struct vector *result; //TODO: replace with arena,, temporary
	struct region *buffer;
	size_t size;
	size_t pos;

	int error;
};

enum node_type { // Or should I use macros???
	N_INVALID = -1,
	N_EXPR = 0,
	N_LITERAL = 1,
	N_UNARY,
	N_BINARY,
};

enum data_type {
	DT_INVALID = -1,
	DT_UNSIGNED64,
	DT_UNSIGNED32,
	DT_UNSIGNED16,
	DT_UNSIGNED8,
};

struct node_primary {
	enum node_type ntype;
	enum data_type type;

	int is_number;
	union {
		size_t number;
		char* str;
	} value;
};

struct node_unary {
	enum node_type ntype;

	struct token operator;
	void *expr;
};

struct node_binary {
	enum node_type ntype;
	struct token operator;
	void *left;
	void *right;
};

struct node {
	enum node_type ntype;
};

void parser_parse(struct parser *p);

#endif
