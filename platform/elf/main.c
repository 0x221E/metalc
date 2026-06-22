#include <stdio.h>

#include <lex.h>
#include <parser.h>
#include <lib/arena.h>
#include <lib/vector.h>
#include <passdbg.h>

#include <platform/osal.h>

#include <stdlib.h>

int main(int argc, char **argv)
{
	printf("Platform set to 'linux-elf'.\n");

	if (argc == 1) {
		printf("You must specify a file to compile!\n");
		exit(1);
	}

	struct file fl = open_file(argv[1], FILE_READ);

	struct region tokens;
	
	if (region_init(&tokens) == -1) {
		printf("Region init error!");
		exit(1);
	}

	struct lex lex = {0};
	lex.line = 1;
	lex.buffer = fl.buffer;
	lex.filepath = fl.path;
	lex.size = fl.size;
	lex.pos = 0;
	
	struct token *tkn = NULL;
	
	size_t size = 0;

	do {
		struct token curr = lex_next(&lex);

		if (curr.type == TOKEN_NEWLINE)
			continue;

		if (curr.type == TOKEN_EOP)
			break;		
		
		tkn = region_alloc(&tokens, sizeof(struct token));

		if (tkn == NULL) {
			printf("error: token alloc");
			exit(1);
		}
		
		*tkn = curr;
		
		/* printf("Tokenized to mem %p! Current region size: %d, type: %d\n", */
		/*        tkn, */
		/*        (int)tokens.size, */
		/*        ((struct token*)tokens.start)[size].type); */

		size++;

	} while (tkn != NULL &&
		tkn->type != TOKEN_INVALID && 
		tkn->type != TOKEN_EOP);

	for (int i = 0; i < (tokens.size) / sizeof(struct token); i++) {
		struct token* curr = ((struct token*)tokens.start) + i;
		printf("Tokenized '%.*s' with type: %s\n",		       
		       (int)curr->length,
		       curr->lexeme,
		       lex_get_token_name((int)curr->type));
	}

	struct region parsed;
	region_init(&parsed);

	struct vector ast_tree;
	vector_init(&ast_tree);
	
	struct parser p;
	p.tokens = &tokens;
	p.buffer = &parsed;
	p.result = &ast_tree;
	p.size = tokens.size / sizeof(struct token);
	p.pos = 0;
	p.error = 0;
	
	parser_parse(&p);
	if (p.error)
		return 1;

	passdbg_walk(ast_tree.buffer[0]);

	vector_free(&ast_tree);
}
