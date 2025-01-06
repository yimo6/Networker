#include "../libs/include/buffer.h"
#include "../include/worker.h"
#include <stdio.h>

FILE* fp;

void closeEvent(Worker* worker, int err){
    size_t connect = *(size_t *)worker -> context;
    size_t new = connect <= 0 ? -1 : getMicroTime();
    printf("time used: %ldms [%d]\n", new - connect, worker -> sockid);
    if(err == 0) printf("Connection Close\n");
    else if(err == 10054) printf("Connection reset\n");
    else if(err == 10061) printf("Connection refused!\n");
    else printf("Connection fail! Error code: %d\n", err);
}

void start(Worker* worker){
    size_t* timer = (size_t *)worker -> context;
    *timer = getMicroTime();
    Buffer* buffer = buffer_create();
    buffer_add(buffer, "GET / HTTP/1.1\r\n");
    buffer_add(buffer, "Host: ");
    buffer_add(buffer, worker -> remote.ip);
    buffer_add(buffer, "\r\n");
    buffer_add(buffer, "Connection: close\r\n");
    buffer_add(buffer, "\r\n");
    int rec = worker_send(worker, buffer -> data);
    buffer_delete(buffer);
    fp = fopen("test.txt", "w+");
}

void message(Worker* worker){
    Buffer* buf = (Buffer *)worker -> buffer;
    fwrite(buf -> data, buf -> realsize, 1, fp);
    fflush(fp); //刷新一下防止没写完
}

Worker* mini_worker_create(const char* address, int port){
    Worker* worker = worker_create(WORKER_PROCOTOL_TCP, address, port);
    worker -> onClose = closeEvent;
    worker -> onConnect = start;
    worker -> onMessage = message;
    worker -> context = malloc(sizeof(size_t));
    return worker;
}