#pragma once

#include "../include/list.h"

typedef enum {
    EVENT_IO,
    EVENT_TIMER,
    EVENT
} EventType;

typedef struct {
    EventType type;
    int fd;
    void* data;
    void (*callback)(ListNode *);
    void* context;
} Event;

