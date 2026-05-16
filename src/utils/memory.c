#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_MAGIC 0xDEADBEEF

typedef struct MemoryBlock {
    unsigned int magic;
    size_t size;
    const char *file;
    int line;
    struct MemoryBlock *prev;
    struct MemoryBlock *next;
} MemoryBlock;

static MemoryBlock *memory_head = NULL;
static size_t memory_used = 0;
static size_t memory_peak = 0;
static size_t memory_alloc_count = 0;
static size_t memory_free_count = 0;
static bool memory_initialized = false;

static void memory_register_block(MemoryBlock *block) {
    block->prev = NULL;
    block->next = memory_head;
    if (memory_head) {
        memory_head->prev = block;
    }
    memory_head = block;
    memory_used += block->size;
    if (memory_used > memory_peak) {
        memory_peak = memory_used;
    }
    memory_alloc_count += 1;
}

static void memory_unregister_block(MemoryBlock *block) {
    if (!block) {
        return;
    }
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        memory_head = block->next;
    }
    if (block->next) {
        block->next->prev = block->prev;
    }
    if (memory_used >= block->size) {
        memory_used -= block->size;
    } else {
        memory_used = 0;
    }
    memory_free_count += 1;
}

void memory_init(void) {
    if (memory_initialized) {
        return;
    }
    memory_initialized = true;
    atexit(memory_report_leaks);
}

void memory_shutdown(void) {
    memory_report_leaks();
}

void *memory_malloc_impl(size_t size, const char *file, int line) {
    if (!memory_initialized) {
        memory_init();
    }
    size_t total = sizeof(MemoryBlock) + size;
    MemoryBlock *block = malloc(total);
    if (!block) {
        return NULL;
    }
    block->magic = MEMORY_MAGIC;
    block->size = size;
    block->file = file;
    block->line = line;
    memory_register_block(block);
    return (void *)(block + 1);
}

void *memory_calloc_impl(size_t nmemb, size_t size, const char *file, int line) {
    if (!memory_initialized) {
        memory_init();
    }
    size_t total_data = nmemb * size;
    size_t total = sizeof(MemoryBlock) + total_data;
    MemoryBlock *block = malloc(total);
    if (!block) {
        return NULL;
    }
    block->magic = MEMORY_MAGIC;
    block->size = total_data;
    block->file = file;
    block->line = line;
    void *payload = (void *)(block + 1);
    memset(payload, 0, total_data);
    memory_register_block(block);
    return payload;
}

void *memory_realloc_impl(void *ptr, size_t size, const char *file, int line) {
    if (!memory_initialized) {
        memory_init();
    }
    if (!ptr) {
        return memory_malloc_impl(size, file, line);
    }
    MemoryBlock *old_block = (MemoryBlock *)ptr - 1;
    if (old_block->magic != MEMORY_MAGIC) {
        fprintf(stderr, "Memory realloc error: invalid pointer at %s:%d\n", file, line);
        return NULL;
    }
    size_t old_size = old_block->size;
    size_t total = sizeof(MemoryBlock) + size;
    MemoryBlock *new_block = realloc(old_block, total);
    if (!new_block) {
        return NULL;
    }
    new_block->magic = MEMORY_MAGIC;
    new_block->size = size;
    new_block->file = file;
    new_block->line = line;
    if (new_block != old_block) {
        if (new_block->prev) {
            new_block->prev->next = new_block;
        } else {
            memory_head = new_block;
        }
        if (new_block->next) {
            new_block->next->prev = new_block;
        }
    }
    if (memory_used >= old_size) {
        memory_used -= old_size;
    } else {
        memory_used = 0;
    }
    memory_used += size;
    if (memory_used > memory_peak) {
        memory_peak = memory_used;
    }
    return (void *)(new_block + 1);
}

void memory_free_impl(void *ptr, const char *file, int line) {
    if (!ptr) {
        return;
    }
    MemoryBlock *block = (MemoryBlock *)ptr - 1;
    if (block->magic != MEMORY_MAGIC) {
        fprintf(stderr, "Memory free error: invalid pointer at %s:%d\n", file, line);
        return;
    }
    memory_unregister_block(block);
    block->magic = 0;
    free(block);
}

void memory_report_leaks(void) {
    if (!memory_initialized) {
        return;
    }
    if (memory_head) {
        fprintf(stderr, "Memory leak report: %zu active allocations, current usage %zu bytes, peak %zu bytes\n",
                memory_alloc_count - memory_free_count,
                memory_used,
                memory_peak);
        MemoryBlock *block = memory_head;
        while (block) {
            fprintf(stderr, "  leak %zu bytes allocated at %s:%d\n",
                    block->size,
                    block->file ? block->file : "unknown",
                    block->line);
            block = block->next;
        }
    } else {
        fprintf(stderr, "Memory check: no leaks, total allocations %zu, frees %zu, peak usage %zu bytes\n",
                memory_alloc_count,
                memory_free_count,
                memory_peak);
    }
}

size_t memory_current_usage(void) {
    return memory_used;
}

size_t memory_peak_usage(void) {
    return memory_peak;
}

size_t memory_active_allocations(void) {
    return memory_alloc_count > memory_free_count ? memory_alloc_count - memory_free_count : 0;
}
