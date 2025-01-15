#include <stdio.h>
#include <string.h>
#include "http.h"
#include "../libs/include/buffer.h"
#include "../libs/include/mem.h"
#include "../include/worker.h"

bool http_path_parser(HttpRequest* req, const char* address){
    char* first = strstr(address, "://");
    if(first == NULL){ //没写HTTP头
        return false;
    }
    char* pos = strstr(first + 3, "/");
    if(pos == NULL){
       strcpy(req -> address, address);
       strcpy(req -> host, first + 3);
       strcpy(req -> path, "/");
       return true;
    }
    uint length = pos - first - 3;
    strcpy(req -> address, address);
    strncpy(req -> host, first + 3, length);
    strncpy(req -> path, pos, strlen(pos));
    return true;
}

void http_request_delete(HttpRequest* req){
    buffer_delete(req -> header);
    buffer_delete(req -> data);
    mem_free(req, NULL);
}

HttpRequest* http_request_create(const char* address, uint port){
    MemBlock* block = mem_malloc(sizeof(HttpRequest), NULL);
    HttpRequest* req = block -> ptr;
    req -> method = HTTP_METHOD_GET;
    req -> port = port;
    req -> hook = NULL;
    memset(req -> address, '\0', sizeof(req -> address));
    memset(req -> host, '\0', sizeof(req -> host));
    memset(req -> path, '\0', sizeof(req -> path));

    if(!http_path_parser(req, address)){
        return NULL;
    }
    if(worker_isdomain(req -> host)){
        char* ip = worker_domain2ip(req -> host);
        strcpy(req -> ip, ip == NULL ? req -> host : ip);
    }else{
        strcpy(req -> ip, req -> host);
    }
    req -> realsize = 0;
    req -> header = buffer_create();
    req -> data = buffer_create();
    return req;
}

void http_request_method(HttpRequest* req, HttpMethod method){
    req -> method = method;
}

bool http_request_header_add(HttpRequest* req, const char* key, const char* value){
    Buffer* buf = req -> header;
    buffer_add(buf, key);
    buffer_add(buf, ": ");
    buffer_add(buf, value);
    buffer_add(buf, "\r\n");
    return true;
}

bool http_port2char(uint port, char* ptr){
    sprintf(ptr, "%d", port);
}

Buffer* http_request_build(HttpRequest* req){
    Buffer* buf = buffer_create();
    char porter[64];
    if(req -> method == HTTP_METHOD_GET){
        buffer_add(buf, "GET ");
    }else if(req -> method == HTTP_METHOD_POST){
        buffer_add(buf, "POST ");
    }
    buffer_add(buf, req -> path);
    buffer_add(buf, " HTTP/");
    buffer_add(buf, HTTP_PACKET_VERSION);
    buffer_add(buf, "\r\n");
    if(req -> port == 80){
        http_request_header_add(req, "Host", req -> host);
    }else{
        Buffer* tmp = buffer_create();
        buffer_add(tmp, req -> host);
        buffer_add(tmp, ":");
        http_port2char(req -> port, porter);
        buffer_add(tmp, porter);
        http_request_header_add(req, "Host", tmp -> data);
        buffer_delete(tmp);
    }

    buffer_add(buf, req -> header -> data);
    buffer_add(buf, "\r\n");
    http_request_header_add(req, "Connection", "Close");
    if(req -> method == HTTP_METHOD_POST && buffer_isempty(buf))
        buffer_add(buf, req -> data -> data);
    buffer_add(buf, "\r\n");
    
    return buf;
}

