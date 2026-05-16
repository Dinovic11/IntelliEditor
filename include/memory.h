#ifndef INTELLIEDITOR_MEMORY_H
#define INTELLIEDITOR_MEMORY_H

#include <stddef.h>
#include <stdbool.h>

void memory_init(void);
void memory_shutdown(void);

void *memory_malloc_impl(size_t size, const char *file, int line);
void *memory_calloc_impl(size_t nmemb, size_t size, const char *file, int line);
void *memory_realloc_impl(void *ptr, size_t size, const char *file, int line);
void memory_free_impl(void *ptr, const char *file, int line);

void memory_report_leaks(void);
size_t memory_current_usage(void);
size_t memory_peak_usage(void);
size_t memory_active_allocations(void);

#define memory_malloc(size) memory_malloc_impl(size, __FILE__, __LINE__)
#define memory_calloc(nmemb, size) memory_calloc_impl(nmemb, size, __FILE__, __LINE__)
#define memory_realloc(ptr, size) memory_realloc_impl(ptr, size, __FILE__, __LINE__)
#define memory_free(ptr) memory_free_impl(ptr, __FILE__, __LINE__)

#endif 
