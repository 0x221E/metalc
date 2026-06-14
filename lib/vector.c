#include <lib/vector.h>

#include <platform/osal.h>

#include <assert.h>
#include <string.h>

int vector_init(struct vector *v)
{
	assert(v != NULL);
	size_t initial = 50;
	v->size = 0;
	v->capacity = initial;
	v->buffer = mem_alloc(initial * sizeof(void*)); // add safety
	if (v->buffer == NULL)
		return -1;
	return 0;
}

int vector_reserve(struct vector *v, size_t nc)
{
	assert(v != NULL);
	void *nb = mem_alloc((nc) * sizeof(void*));

	if (nb == NULL) 
		return -1;

	memcpy(nb, v->buffer, v->size * sizeof(void*));
	v->capacity = nc;

	if (v->buffer == NULL) 
		return -1;
	
	mem_free(v->buffer);
	v->buffer = nb;
	return 0;
}

int vector_emplace_back(struct vector *v, void *p)
{
	assert(v != NULL);

	if (p == NULL) 
		return -1;

	if (v->size >= v->capacity)
		if (vector_reserve(v, v->capacity * 2) == -1) 
			return -1;


	v->buffer[v->size] = p;
	v->size++;	
	
	return 0;
}
