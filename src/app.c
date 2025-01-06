#include <stdio.h>
#include <string.h>
#include "../libs/include/mem.h"
#include "../include/public.h"
#include "../include/app.h"
#include "../include/process.h"

Application* app_create(const char* name){
    MemBlock* block = (MemBlock *)mem_malloc(sizeof(Application), NULL);
    Application* app = block -> ptr;
    app -> context  = NULL;
    app -> onStart  = NULL;
    app -> onClose  = NULL;
    app -> callback = NULL;
    app -> timer    = 0.0;
    app -> lasttime = 0.0;
    strncpy(app -> name, name, APP_NAME_SIZE - 1);
    return app;
}

bool app_push(Application* app){
    if(!process_push(app)){
        printf("[%s] An error occurred while pushing data to the process.\n", app -> name);
    }
    return true;
}

//用于联合触发等操作
bool app_trigger(Application* app){
    if(app -> timer > 0){
        long long tick = (getMicroTime() - app -> lasttime);
        if(tick < app -> timer) return false;
        app -> lasttime = getMicroTime();
    }
    if(app -> callback != NULL){
        (app -> callback)(app);
        app -> lasttime = getMicroTime();
        return true;
    }
    return false;
}

bool app_shutdown(Application* app){
    if(app == NULL) return false;
    process_runout(app);
    mem_free(app, NULL);
}

