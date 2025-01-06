//一些worker示例
#include <stdio.h>
#include "../libs/include/mem.h"
#include "../include/public.h"
#include "../include/worker.h"

void worker_timeout_clear(Worker* worker, int err){
    mem_free(worker -> context, NULL);
}

void worker_timeout_tick(Worker* worker){
    AppTimer* timer = (AppTimer *)worker -> context;
    long long t = getMicroTime() - timer -> start;
    if(t >= timer -> timeout){
        printf("timeout(%lld)\n", t);
        worker_close(worker, -1);
    }
}

void worker_timeout_init(Worker* worker, uint timeout){
    MemBlock* block = mem_malloc(sizeof(AppTimer), NULL);
    AppTimer* timer = (AppTimer *)block -> ptr;
    timer -> start = getMicroTime();
    timer -> timeout = timeout;
    worker -> context = (void *)timer;
    worker -> onTick = worker_timeout_tick;
    worker -> onClose = worker_timeout_clear;
}

