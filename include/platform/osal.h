#ifndef OSAL_H
#define OSAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

void* mem_alloc(size_t s);
void mem_free(void *p);

#define FILE_READ  (uint8_t) 0b00000001
#define FILE_WRITE (uint8_t) 0b00000010

struct file {
	const char *path;
	const char *buffer;
	const size_t size;
};

struct file open_file(const char *path, int flags);

void print(const char *fmt, ...);
void printv(const char *fmt, va_list l);
void exitc(int c);

#endif
