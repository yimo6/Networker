#include "../libs/include/buffer.h"
#include "../include/worker.h"
#include "../protocol/http.h"

void http_sender_init_hook(Worker* worker){
    HttpRequest* req = (HttpRequest *)worker -> context;
    Buffer* packet = http_request_build(req);
    worker_send(worker, packet -> data);
    buffer_delete(packet);
}

void http_sender_message_hook(Worker* worker){
    HttpRequest* req = (HttpRequest *)worker -> context;
    Buffer* buf = worker -> buffer;
    HttpResponse* res = http_response_create(buf -> data);
    if(res == NULL) return;
    (req -> hook)(worker, res);
    http_response_delete(res);
}

void http_sender_close_hook(Worker* worker, int err){
    HttpRequest* req = (HttpRequest *)worker -> context;
    http_request_delete(req);
}

Worker* http_sender_create(const char* address, uint port){
    HttpRequest* req = http_request_create(address, port);
    Worker* worker = worker_create(WORKER_PROCOTOL_TCP, req -> host, port);
    if(worker == NULL || req == NULL) return false;
    worker -> context    = (void *)req;
    worker -> onConnect  = http_sender_init_hook;
    worker -> onClose    = http_sender_close_hook;
    worker -> onMessage  = http_sender_message_hook;
    return worker;
}

