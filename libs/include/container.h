#pragma once
#include <stdlib.h>

#define CONTAINER_MAX_SIZE 8192 //最大容器数
#define CONTAINER_CONTEXT_NAME_LENGTH 128 //最大容器数

//容器上下文
typedef struct {
    unsigned int id; //实际值为Hash
    char name[CONTAINER_CONTEXT_NAME_LENGTH];
    void* data;
} Context;

typedef struct {
    size_t size;
    MemPool* pool;
    Context* packs[CONTAINER_MAX_SIZE];
} Container;

Container* container_get_instance();
Context* container_import(const char* name);
bool container_export(const char* name, void* data);