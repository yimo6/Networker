#pragma once

#include <stdlib.h>
#include <string.h>
#include "mem.h"


#define STRING_EXT_BLOCK 2048

typedef struct Buffer {
    size_t size;
    size_t realsize;
    char*  data;
    MemBlock* raw;
} Buffer;

Buffer* buffer_create_size(uint size);
Buffer* buffer_create();
void buffer_delete(Buffer* buf);
Buffer* buffer_copy(Buffer* src);
bool buffer_isempty(Buffer* buf);
size_t buffer_add(Buffer* buf, const char* data);
void buffer_resize(Buffer* buf, size_t newSize);
void buffer_clear(Buffer* buf);
void buffer_clear_all(Buffer* buf);