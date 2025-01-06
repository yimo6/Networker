#pragma once

#include <stdlib.h>
#include "public.h"
#include "worker.h"
#include "../libs/include/list.h"

typedef struct {
    size_t size;
    List* connections;
} ConnectionPool;

ConnectionPool* connection_pool_instance();
bool connectionPool_delete(Worker* worker);
bool connectionPool_push(Worker* worker);
void connectionPool_call();
void connectionPool_tick(ListNode* node);
size_t connectiionPool_get_size();