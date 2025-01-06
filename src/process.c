#include <time.h>
#include "../include/public.h"
#include "../include/connectionPool.h"
#include "../include/process.h"
#include "../include/app.h"
#include "../libs/include/mem.h"

#ifdef _WIN32
#include <windows.h>

//Windows: 时间切片最小调度单位: 1毫秒
//通过对1毫秒分片进行模拟微秒级延迟
int active_tick = 1;

void process_delay(unsigned long start){
    unsigned long end = getMicroTime();
    double error_timer = (end - start), delay = 0.0;
    if(connectiionPool_get_size() == 0){
        delay = PROCESS_TOTAL_SLEEP_TIMER - error_timer;
    }else{
        if(active_tick % PROCESS_TOTAL_ACTIVE_TICK_TIMER == 0){
            active_tick = 1;
            delay = 1.0 - error_timer;
        }else{
            active_tick++;
        }
    }
    if(delay > 0) Sleep(delay);
}

#else
#include <unistd.h>
void process_delay(unsigned long start){
    double error_timer = getMicroTime() - start, delay = 0.0;
    if(connectiionPool_get_size() == 0){
        delay = PROCESS_TOTAL_SLEEP_TIMER - error_timer;
    }else{
        delay = PROCESS_TOTAL_ACTIVE_TIMER - error_timer;
    }
    if(delay > 0) usleep(delay);
}
#endif

static Process* _process_instance = NULL;

Process* process_instance(){
    if(_process_instance == NULL){
        _process_instance = (Process *)mem_malloc(sizeof(Process), NULL);
        _process_instance -> list = list_create();
    }
    return _process_instance;
}

bool process_push(Application* app){
    Process* process = process_instance();
    if(app -> onStart != NULL) (app -> onStart)(app);
    return list_add(process -> list, (void *)app);
}

bool process_runout(Application* app){
    Process* process = process_instance();
    if(app -> onClose != NULL) (app -> onClose)(app);
    return list_del(process -> list, (void *)app);
}

void process_node_tick(ListNode* node){
    Application* app = (Application *)node -> data;
    app_trigger(app);
}

void process_tick(){
    unsigned long start = getMicroTime();
    Process* process = process_instance();
    //任务列表
    list_on(process -> list, process_node_tick);
    connectionPool_call(NULL);
    process_delay(start);
}

void process_run(){
    while(1){
        process_tick();
    }
}