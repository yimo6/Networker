#include <stdlib.h>
#include <string.h>
#include "../include/mem.h"
#include "../include/container.h"

//容器类

static Container* instance = NULL;

Container* container_get_instance(){
    if(instance == NULL){
        instance = (Container *)mem_malloc(sizeof(Container), NULL);
        instance -> pool = (MemPool *)mem_pool_create();
        instance -> size = 0;
    }
    return instance;
}

uint container_char2hash(const char* name){
    uint res = 0;
    size_t size = strlen(name);
    for(int i = 0;i < size;i++){
        res += name[i];
    }
    return res % CONTAINER_MAX_SIZE;
}

Context* container_import(const char* name){
    Container* c = container_get_instance();
    uint hash = container_char2hash(name);
    size_t size = c -> size;
    Context* tmp;
    while(1){
        tmp = c -> packs[hash++];
        if(tmp == NULL) continue;
        if(size == 0) break; //找不到任何一个返回NULL
        size--;
        if(strcmp(tmp -> name, name) == 0) return tmp;
    }
    return NULL;
}

bool container_export(const char* name, void* data){
    uint hash = container_char2hash(name);
    Container* c = container_get_instance();

    if(c -> size >= CONTAINER_MAX_SIZE) return false;

    MemBlock* block = mem_malloc(sizeof(Context), c -> pool);
    Context* context = (Context *)block -> ptr;
    context -> id = hash;
    context -> data = data;
    strcpy(context -> name, name);

    //冲突Hash解决方案
    while(c -> packs[hash] != NULL){
        hash++;
        if(hash >= CONTAINER_MAX_SIZE) hash %= CONTAINER_MAX_SIZE;
    }
    c -> packs[hash] = context;
    c -> size++;
    //printf("[Container]: export => %s\n", name);
    return true;
}