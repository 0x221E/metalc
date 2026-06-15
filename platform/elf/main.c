#include <stdio.h>

#include <lex.h>
#include <lib/vector.h>

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

	struct vector token_storage = {0};
	if (vector_init(&token_storage)) {
		printf("error: vector_init()");
		exit(1);
	}

	struct lex lex = {0};
	lex.line = 0;
	lex.buffer = fl.buffer;
	lex.size = fl.size;
	lex.pos = 0;
	
	struct token *tkn = NULL;
	
	size_t size = 0;

	do {
		tkn = mem_alloc(sizeof(struct token)); // Change mem alloc with arena alloc

		if (tkn == NULL) {
			printf("error: token alloc");
			exit(1);
		}

		*tkn = lex_next(&lex);

		//		printf("Tokenized: %.*s, with length: %d.\n", (int)tkn->length, tkn->lexeme, (int)tkn->type);

		if (vector_emplace_back(&token_storage, tkn) == -1) {
			printf("error: vector_emplace_back()");
			exit(1);
		}

	} while (tkn != NULL &&
		tkn->type != TOKEN_INVALID && 
		tkn->type != TOKEN_EOP);

	for (int i = 0; i < token_storage.size; i++) {
		if (token_storage.buffer[i] == NULL)
			break;

		printf("Tokenized '%.*s' with type: %s\n", //, on line: %d at position: %d\n", 
		       (int)((struct token*) token_storage.buffer[i])->length,
		       ((struct token*)token_storage.buffer[i])->lexeme,
		       lex_get_token_name((int)((struct token*)token_storage.buffer[i])->type)
//				((struct token*)token_storage.buffer[i])->line,
//				((struct token*)token_storage.buffer[i])->pos
				);
	}

	return 0;
}
