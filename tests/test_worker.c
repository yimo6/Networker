#include "../libs/include/mem.h"
#include "../libs/include/buffer.h"
#include "../libs/include/list.h"
#include "../include/worker.h"

int main(){
    worker_init();
    Worker* worker = worker_create(WORKER_PROCOTOL_TCP, "127.0.0.1", 80);
    if(worker == NULL) return 1;
    return 0;
}
