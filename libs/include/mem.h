#pragma once
#include <stdlib.h>
#include "../../include/public.h"

typedef struct MemBlock {
    size_t id;
    size_t size;
    void* ptr;
    struct MemBlock* next;
} MemBlock;

typedef struct {
    uint size;
    uint freeSize;
    uint usedSize;
    MemBlock* free;
    MemBlock* used;
    size_t memSize;
} MemPool;

//抽象内存接口
typedef struct {
    MemBlock* (* alloc)(size_t size); //分配接口
    void (* free)(MemBlock* block); //释放接口
} MemoryApi;

void* mem_raw_malloc(size_t size);
void* mem_raw_realloc(void* ptr, size_t size);
void mem_realloc(size_t size, MemBlock* src);
void mem_pool_free(MemPool* pool);
bool mem_free(void* ptr, MemPool* pool);
MemBlock* mem_malloc(size_t size, MemPool* pool);
MemPool* mem_pool_create();
MemPool* mem_pool_instance();