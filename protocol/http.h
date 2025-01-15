#pragma once
#include "../include/worker.h"
#include "../libs/include/buffer.h"

#define HTTP_PACKET_VERSION "1.1"

typedef enum {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST
} HttpMethod;

typedef struct {
    uint statusCode;
    char status[256];
    char version[16];
    uint size; //header中的size
    Buffer* header;
    Buffer* body;
} HttpResponse;

typedef struct {
    uint realsize;
    HttpMethod method;
    char ip[256];
    char address[768];
    char host[256];
    char path[512];
    uint port;
    Buffer* header;
    Buffer* data;
    void (*hook)(Worker* worker, HttpResponse* res);
} HttpRequest;

void http_request_delete(HttpRequest* req);
HttpRequest* http_request_create(const char* address, uint port);
void http_request_method(HttpRequest* req, HttpMethod method);
bool http_request_header_add(HttpRequest* req, const char* key, const char* value);
Buffer* http_request_build(HttpRequest* req);

void http_response_delete(HttpResponse* res);
HttpResponse* http_response_create(char* data);

Worker* http_sender_create(const char* address, uint port);