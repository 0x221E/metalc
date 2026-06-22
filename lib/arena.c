#include <lib/arena.h>

#include <platform/osal.h>

#include <sys/mman.h>

#include <string.h>
#include <errno.h>
#include <assert.h>

int region_init(struct region *r)
{
	void * mem = mmap(NULL, REGION_MAX_CAPACITY, PROT_READ | PROT_WRITE,
			  MAP_PRIVATE | MAP_ANONYMOUS, 0, 0); // Abstract this under platform

	if (mem == NULL) {
		print("errno: %s\n", strerror(errno));
		return -1;
	}

	r->capacity = REGION_MAX_CAPACITY;
	r->start = mem;
	r->end = r->start;
	r->size = 0;

	return 0;		
}

void *region_alloc(struct region *r, size_t b)
{
	if (r->capacity <= r->size + b) {
		print("compiler error: region memory exceeded trying to allocate %zu bytes!", b); // Make this debug error, only present with verbose flag.
		return NULL;
	}
       
	void* ptr = r->end;
	r->end = (char*)r->end + b;
	r->size += b;
	return ptr;
}

// such a dumb workaround, fix this.
int region_pop(struct region *r, size_t b)
{
	assert(r != NULL);

	if (r->size > b) {
		return -1;
	}

	r->end = (char*)r->end - b;
	r->size -= b;
	return 0;
}
