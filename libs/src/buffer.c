#include <string.h>
#include <stdlib.h>
#include "../include/mem.h"
#include "../include/buffer.h"

static MemPool* buffer_pool;

MemPool* buffer_pool_instance(){
    if(buffer_pool == NULL){
        buffer_pool = (MemPool *)mem_pool_create();
    }
    return buffer_pool;
}

Buffer* buffer_create_size(uint size){
    MemPool* pool = buffer_pool_instance();
    size_t length = sizeof(char) * size;
    Buffer* buf = (Buffer *)mem_raw_malloc(sizeof(Buffer));
    MemBlock* block = mem_malloc(length, pool);

    buf -> size = length;
    buf -> raw = block; //保存原始内存区块信息
    buf -> data = (char *)block -> ptr;
    buf -> realsize = 0;
    memset(buf -> data, 0, length);
    return buf;
}

Buffer* buffer_create(){
    return buffer_create_size(STRING_EXT_BLOCK);
}

void buffer_delete(Buffer* buf){
    if(buf == NULL || buf -> data == NULL) return;
    MemPool* pool = buffer_pool_instance();
    mem_free(buf -> raw -> ptr, pool);
}

bool buffer_isempty(Buffer* buf){
    return buf -> realsize == 0;
}

void buffer_clear(Buffer* buf){
    if(buf -> realsize == 0) return;
    memset(buf -> data, '\0', buf -> realsize);
    buf -> realsize = 0;
}

//完全清理整个缓存块
void buffer_clear_all(Buffer* buf){
    if(buf -> realsize == 0) return;
    memset(buf -> data, '\0', buf -> size);
    buf -> realsize = 0;
}

Buffer* buffer_copy(Buffer* src){
    Buffer* buf = buffer_create();
    buf -> size = src -> size;
    buf -> realsize = src -> realsize;
    strcpy(buf -> data, src -> data);
    return buf;
}

void buffer_resize(Buffer* buf, size_t newSize){
    mem_realloc(newSize, buf -> raw);
    buf -> size = newSize;
    buf -> data = (char *)buf -> raw -> ptr;
}

void buffer_check(Buffer* buf, size_t newSize){
    //因为补充\0的原因,不能严格=
    if(buf -> size > newSize) return;
    uint ext = ((newSize / STRING_EXT_BLOCK) + 1) * sizeof(char);
    size_t new = buf -> size + ext * STRING_EXT_BLOCK;

    mem_realloc(new, buf -> raw);
    buf -> size = new;
    buf -> data = (char *)buf -> raw -> ptr;
}

size_t buffer_add(Buffer* buf, const char* data){
    size_t dataLen = strlen(data);
    size_t newSize = buf -> realsize + dataLen;
    buffer_check(buf, newSize);
    strncat(buf -> data, data, dataLen);
    buf -> realsize = newSize;
    buf -> data[buf -> realsize] = '\0';
    return dataLen;
}
