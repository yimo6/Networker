#include <stdio.h>
#include <stdlib.h>
#include "../include/mem.h"

MemPool* _memory_pool_instance;

void* mem_raw_malloc(size_t size){
    void* ptr = malloc(size);
    if(ptr == NULL){
        printf("[内存分配失败]");
        exit(0);
    }
    return ptr;
}

void* mem_raw_realloc(void* ptr, size_t size){
    void* newPtr = realloc(ptr, size);
    if(newPtr == NULL){
        printf("[内存分配失败]");
        exit(0);
    }
    return newPtr;
}

MemPool* mem_pool_create(){
    MemPool* pool = (MemPool *)mem_raw_malloc(sizeof(MemPool));
    pool -> freeSize = 0;
    pool -> usedSize = 0;
    pool -> memSize  = 0;
    pool -> size     = 0;
    pool -> free     = (MemBlock *)mem_raw_malloc(sizeof(MemBlock));
    pool -> used     = (MemBlock *)mem_raw_malloc(sizeof(MemBlock));
    pool -> free -> id   = 0;
    pool -> free -> id   = 0;
    pool -> used -> size = 0;
    pool -> used -> size = 0;
    pool -> free -> ptr  = NULL;
    pool -> used -> ptr  = NULL;
    pool -> free -> next = NULL;
    pool -> used -> next = NULL;
    return pool;
}

//内存池句柄
MemPool* mem_pool_instance(){
    if(_memory_pool_instance == NULL){
        _memory_pool_instance = (MemPool *)mem_pool_create(sizeof(MemPool));
    }
    return _memory_pool_instance;
}

//[内部操作] 内存节点释放
bool mem_pool_node_free(MemBlock* root, MemPool* pool){
    if(root == NULL || root -> id != 0) return false;
    MemBlock* node = root -> next, *last = root;
    while(node != NULL){
        if(node -> ptr != NULL) free(node -> ptr);
        node -> next = NULL;
        node -> ptr = NULL;
        pool -> memSize -= node -> size;
        free(node);
        last = node;
        node = node -> next;
    }
    return true;
}

void mem_realloc(size_t size, MemBlock* src){
    src -> ptr = mem_raw_realloc(src -> ptr, size);
    src -> size = size;
}

bool mem_block_add(MemBlock* root, MemBlock* new){
    if(root == NULL || root -> id != 0) return false;
    MemBlock* node = root;
    while(node -> next != NULL){
        node = node -> next;
    }
    new -> next = NULL;
    node -> next = new;
}

//释放所有占用的内存空间
void mem_pool_free(MemPool* pool){
    if(pool == NULL) pool = mem_pool_instance();
    printf("pool-free: %d/%d\n", pool -> usedSize, pool -> freeSize);
    if(pool -> usedSize) mem_pool_node_free(pool -> used, pool);
    if(pool -> freeSize) mem_pool_node_free(pool -> free, pool);
    free(pool);
}

bool mem_free(void* ptr, MemPool* pool){
    if(pool == NULL) pool = mem_pool_instance();
    if(pool -> usedSize == 0) return false;
    MemBlock* node = pool -> used -> next, *last = pool -> used;
    while(node != NULL){
        if(node -> ptr == ptr){
            last -> next = node -> next;
            node -> next = NULL;
            mem_block_add(pool -> free, node);
            pool -> usedSize--;
            pool -> freeSize++;
            return true;
        }
        last = node;
        node = node -> next;
    }
    return false;
}

MemBlock* mem_malloc(size_t size, MemPool* pool){
    if(pool == NULL) pool = mem_pool_instance();
    MemBlock* node = pool -> free -> next, *last = pool -> free;

    //使用空闲块
    while(node != NULL){
        if(node -> size >= size){
            last -> next = node -> next;
            node -> next = NULL;
            mem_block_add(pool -> used, node);
            pool -> usedSize++;
            pool -> freeSize--;
            return node;
        }
        last = node;
        node = node -> next;
    }

    MemBlock* block = (MemBlock *)mem_raw_malloc(sizeof(MemBlock));
    block -> id = ++pool -> size;
    block -> size = size;
    block -> ptr = (void *)mem_raw_malloc(size);
    block -> next = NULL;
    pool -> memSize += size;
    pool -> usedSize++;
    mem_block_add(pool -> used, block);
    return block;
}
