#include <passdbg.h>

#include <lib/arena.h>

#include <platform/osal.h>
#include <parser.h>

#include <assert.h>

static int indent_lvl = 0;

#define INDENT_PRINT(x)				 \
	for (int i = 0; i < indent_lvl + x; i++) \
		print(" ")

void passdbg_primary(void *n)
{
	assert(n != NULL);
	struct node_primary *primary = (struct node_primary *)n;

	if (primary->ntype != N_LITERAL) {
		print("passdbg_primary(): node not literal!");
		exitc(1);
	}

	INDENT_PRINT(0);
        print("Primary Node: (%s)\n", primary->is_number ? "Number" : "String");
	INDENT_PRINT(2);
	switch (primary->is_number) {
	case 0:
		print("Content: %s\n", primary->value.str);
		break;
	case 1:
		print("Content: %d\n", primary->value.number);
		break;
	default:
		print("error: unrecognized type for passdbg_primary()");
		exitc(1);
		break;
	}
}

void passdbg_unary(void *n)
{
	assert(n != NULL);

	struct node_unary *unary = (struct node_unary *)n;

	if (unary->ntype == N_LITERAL) {
		passdbg_primary(n);
		return;
	}

	if (unary->ntype != N_UNARY) {
		print("passdbg_unary(): Node not unary!");
		exitc(1);
	}

	INDENT_PRINT(0);	
	print("Unary Operator:\n");
	indent_lvl += 2;
	INDENT_PRINT(0);
	print("Operator: %s\n", unary->operator.lexeme);
	INDENT_PRINT(0);
	print("Expr:\n");
	indent_lvl += 2;
	passdbg_walk(unary->expr);
	indent_lvl -= 4;
}

void passdbg_binary(void *n)
{
	assert(n != NULL);

	struct node_binary *binary = (struct node_binary*)n;

	if (binary->ntype != N_BINARY) {
		print("passdbg_binary(): error node not binary!");
		exitc(1);
	}

	INDENT_PRINT(0);
	print("Binary Operator:\n");
	indent_lvl+=2;
	INDENT_PRINT(0);
	print("Operator: %s\n", binary->operator.lexeme);
	INDENT_PRINT(0);
	print("Left:\n");
	indent_lvl+=2;
	passdbg_walk(binary->left);
	indent_lvl-=2;
	INDENT_PRINT(0);
	print("Right:\n");
	indent_lvl+=2;
	passdbg_walk(binary->right);
	indent_lvl-=4;
}

#define __WALKER_CASE(type, func) case type: func; return

void passdbg_walk(void *n)
{
	struct node* node = (struct node*)n;

	switch (node->ntype) {
		__WALKER_CASE(N_LITERAL, passdbg_primary(n));
		__WALKER_CASE(N_UNARY, passdbg_unary(n));
		__WALKER_CASE(N_BINARY, passdbg_binary(n));
	default:
		print("passdbg_walk(): invalid node type!\n");
		exitc(1);
	}
}

