#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libs/include/buffer.h"
#include "../include/worker.h"
#include "../include/app.h"
#include "../include/process.h"

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
}

void message(Worker* worker){
    Buffer* buf = (Buffer *)worker -> buffer;
    printf("%s\n", buf -> data);
    fflush(stdout);
}

Worker* mini_worker_create(const char* address, int port){
    Worker* worker = worker_create(WORKER_PROCOTOL_TCP, address, port);
    worker -> onClose = closeEvent;
    worker -> onConnect = start;
    worker -> onMessage = message;
    worker -> context = malloc(sizeof(size_t));
    return worker;
}

void main_test(Application* app){
    Worker* worker = (Worker *)app -> context;
    if(worker -> status == WORKER_STATUS_CONNECTED)
        printf("Alive\n");
    else if(worker -> status == WORKER_STATUS_CLOSED){
        printf("Close\n");
        app_shutdown(app);
    }else if(worker -> status == WORKER_STATUS_CONNECTED)
        printf("Connecting...\n");
    else printf("Unkown (%d)?\n", worker -> status);
}

void main_start(Application* app){
    Worker* worker = mini_worker_create("127.0.0.1", 80);
    app -> context = (void *)worker;
    worker_connect(worker, true);
    printf("[%s] Init\n", app -> name);
}

int main(){
    worker_init();
    
    Application* main = app_create("testing");
    main -> onStart = main_start;
    main -> callback = main_test;
    main -> timer = 1000; //ms
    app_push(main);

    process_run();
}
