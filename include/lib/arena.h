/**
 * @file arena.h
 * @author x221e
 * @brief linear arena-allocation without relocation
 * @description
 * This will be a linear arena allocation,
 * no free functionality exists right now.
 *
 * @todo create a larger arena structure that would hold regions
 */

#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

// 2 MiB regions
#define REGION_MAX_CAPACITY    1024*1024*2 

struct region { // or arena?
	size_t capacity;
	size_t size;
	void *end;
	void *start;
};

int region_init(struct region *r);
void *region_alloc(struct region *r, size_t b);
int region_pop(struct region *r, size_t b);

#endif
