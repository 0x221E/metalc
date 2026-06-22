#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

/**
 * A simple pointer-only C++-style vector.
 */

struct vector {
	size_t size;
	size_t capacity;
	void** buffer;
};

int vector_init(struct vector *v);
int vector_resize(struct vector *v, size_t nc);
int vector_emplace_back(struct vector *v, void *p);
int vector_free(struct vector *v);

#endif
