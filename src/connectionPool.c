#include "../include/public.h"
#include "../include/connectionPool.h"
#include "../include/worker.h"
#include "../libs/include/mem.h"
#include "../libs/include/buffer.h"
#include "../libs/include/list.h"

static ConnectionPool* connectionPool = NULL;

//List卸载事件
void connectionPool_event_delete(void* data){
    if(data == NULL) return;
    Worker* worker = (Worker *)data;
    worker_close(worker, -1);
}

ConnectionPool* connection_pool_instance(){
    if(connectionPool == NULL){
        connectionPool = (ConnectionPool *)mem_raw_malloc(sizeof(ConnectionPool));
        connectionPool -> size = 0;
        connectionPool -> connections = list_create();
        connectionPool -> connections -> eventFree = connectionPool_event_delete;
        //注册卸载事件
    }
    return connectionPool;
}

size_t connectiionPool_get_size(){
    ConnectionPool* pool = connection_pool_instance();
    return pool -> connections -> size;
}

//向连接池中删除节点
bool connectionPool_delete(Worker* worker){
    ConnectionPool* pool = connection_pool_instance();
    list_del(pool -> connections, (void *)worker);
}

//向连接池中推送Worker
bool connectionPool_push(Worker* worker){
    ConnectionPool* pool = connection_pool_instance();
    list_add(pool -> connections, (void *)worker);
    return true;
}

void connectionPool_tick(ListNode* node){
    Worker* worker = (Worker *)node -> data;
    int rec;
    long long timer = getMicroTime() - worker -> start;
    if(worker == NULL){
        printf("Error Worker\n");
        return;
    }
    if(worker -> status == WORKER_STATUS_CLOSED){
        connectionPool_delete(worker);
        return;
    }else if(worker -> status == WORKER_STATUS_CONNECTING){
        rec = worker_is_connected(worker);
        if(rec < 0){
            worker_close(worker, -1);
            return;
        }
        if(timer > worker -> timeout){
            if(worker -> onTimeout != NULL) (worker -> onTimeout)(worker);
            worker_close(worker, -1);
            return;
        }
        if(rec == 0){
            if(worker -> onTick != NULL) (worker -> onTick)(worker);
            return;
        }
        worker -> status = WORKER_STATUS_CONNECTED;
        if(worker -> onConnect != NULL) (worker -> onConnect)(worker);
        //检查超时
        return;
    }else if(worker -> status == WORKER_STATUS_CONNECTED){
        rec = worker_read(worker);
        if(rec == 0) return;
        if(rec > 0){
            (worker -> onMessage)(worker);
            buffer_clear(worker -> buffer);
            return;
        }
        worker_close(worker, GET_SOCKET_ERROR());
    }
}

void connectionPool_call(void (*event)(ListNode *)){
    ConnectionPool* pool = connection_pool_instance();
    if(pool -> connections -> size == 0){
        return;
    }
    if(event == NULL) event = connectionPool_tick;
    list_on(pool -> connections, event);
}
