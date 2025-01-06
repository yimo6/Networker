#include "../include/mem.h"
#include "../include/event.h"

Event* event_create(EventType type){
    Event* event = (Event *)mem_malloc(sizeof(Event), NULL);
    event -> type = type;
    event -> fd = -1;
    event -> data = NULL;
    event -> callback = NULL;
    event -> context = NULL;
    return event;
}
