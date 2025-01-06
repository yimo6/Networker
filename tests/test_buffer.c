#include <string.h>
#include "../libs/include/mem.h"
#include "../libs/include/buffer.h"

int main(){
    Buffer* buffer = buffer_create();
    buffer_add(buffer, "hello");
    buffer_add(buffer, " ");
    buffer_add(buffer, "world");
    if(strcmp(buffer -> data, "hello world") != 0) return 1;
    buffer_clear(buffer);
    if(strlen(buffer -> data) > 0) return 1;

    return 0;
}
