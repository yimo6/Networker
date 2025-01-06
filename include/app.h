#pragma once

#include "../include/public.h"

#define APP_NAME_SIZE 128

typedef struct Application {
    char name[APP_NAME_SIZE];
    long long lasttime; //上一次运行的时间(微秒)
    double timer; //定时器模块
    void* context; //上下文
    void (*onStart)(struct Application*); //初始化
    void (*onClose)(struct Application*); //关闭(用于内存回收)
    void (*callback)(struct Application*); //触发器
} Application;

Application* app_create(const char* name);
bool app_push(Application* app);
bool app_trigger(Application* app);
bool app_shutdown(Application* app);