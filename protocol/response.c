#include <stdio.h>
#include <string.h>
#include "http.h"
#include "../libs/include/buffer.h"

void http_response_delete(HttpResponse* res){
    buffer_delete(res -> header);
    buffer_delete(res -> body);
    mem_free(res, NULL);
}

HttpResponse* http_response_create(char* data){
    MemBlock* block = mem_malloc(sizeof(HttpResponse), NULL);
    HttpResponse* res = (HttpResponse *)block -> ptr;
    res -> size = 0;
    res -> header = buffer_create();
    res -> body   = buffer_create();
    Buffer* tmp   = buffer_create();

    char* ptr  = strstr(data, "\r\n");
    buffer_nadd(tmp, data, ptr - data);
    char* body = strstr(ptr, "\r\n\r\n");
    if(ptr == NULL || body == NULL) return NULL;
    if(strncmp(tmp -> data, "HTTP", 4) != 0) return NULL; //判断是否为合法HTTP包
    sscanf(tmp -> data, "%s %d %s", res -> version, &res -> statusCode, res -> status);
    buffer_clear(tmp);

    buffer_nadd(res -> header, data, body - data);
    buffer_add(res -> body, body + 4);
    buffer_delete(tmp);

    return res;
}

