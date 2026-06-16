#include <platform/osal.h>

#include <stdlib.h>
#include <stdio.h>

#include <stdarg.h>

#include <assert.h>

void *mem_alloc(size_t s)
{
	return malloc(s);
}

void mem_free(void *p)
{
	free(p);
}

struct file open_file(const char *path, int flags)
{
	assert(path != NULL);
	
	FILE *file;

	if (flags & FILE_READ) {
		file = fopen(path, "rb");
	} else {
		printf("Read option not specified for file read!");
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *content = malloc(fsize + 1);
	fread(content, fsize, 1, file);
	fclose(file);

	content[fsize] = 0;

	return (struct file) {.path=path, .buffer=content, .size=fsize};
}

void print(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void printv(const char *fmt, va_list l)
{
	vprintf(fmt, l);
}

void exitc(int c)
{
	exit(c);
}
