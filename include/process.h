#pragma once

#include "../libs/include/list.h"
#include "public.h"
#include "app.h"

#define PROCESS_TOTAL_SLEEP_TIMER 1 * 100 //单位: 毫秒

#ifdef _WIN32
#define PROCESS_TOTAL_ACTIVE_TICK_TIMER 2500 //单位: 片(值越大网络延迟性能越高)
#else
#define PROCESS_TOTAL_ACTIVE_TIMER 400 //单位: 微秒(仅对Linux平台有效)
#endif

typedef struct {
    size_t size;
    List* list;
} Process;

bool process_push(Application* app);
bool process_runout(Application* app);
void process_tick();
void process_run();