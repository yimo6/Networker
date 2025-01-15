#pragma once

#include <unistd.h>
#include "public.h"
#ifdef _WIN32
//Win
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>

#ifndef SO_CONNECT_TIME
#define SO_CONNECT_TIME 0x700C
#endif

#define WORKER_SOCKET_ERROR INVALID_SOCKET
#define GET_SOCKET_ERROR() WSAGetLastError()
#define WORKER_SOCKET_CLOSE(sock) closesocket(sock)

#else
//Linux
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
typedef int SOCKET;
#define WORKER_SOCKET_ERROR -1
#define GET_SOCKET_ERROR() errno
#define WORKER_SOCKET_CLOSE(sock) close(sock)

#endif

#define WORKER_MAX_CLIENTS 1024 //单个最大连接数
#define WORKER_BUFFER_SIZE 8192 //默认缓存容量
#define WORKER_DEFAULT_TIMEOUT 30 //默认超时时间(单位秒)
#define WORKER_SYNC_TIMER 250 //非异步模式下响应tick(单位微秒)

#define WORKER_PROCOTOL_TCP 1

typedef struct {
    SOCKET sockid;
    struct sockaddr_in sockaddr;
    void* context; //上下文
    char ip[128];
    char address[256];
    uint port;
} Client;

typedef enum {
    WORKER_STATUS_WAITTING, //正在等待
    WORKER_STATUS_CONNECTING, //连接中
    WORKER_STATUS_CONNECTED, //已连接
    WORKER_STATUS_CLOSED, //已关闭
} WorkerStatus;

typedef struct Worker {
    SOCKET sockid;
    WorkerStatus status;
    Client remote;
    long long start; //开始时间
    uint timeout; //超时时间(-1为禁用)
    void* buffer; //缓存
    void* memBlock; //内存上下文

    size_t bufferSize; //单次最大缓存量
    void* context; //上下文(用户态自定义)
    void (*onStart)(struct Worker *);
    void (*onConnect)(struct Worker *);
    void (*onMessage)(struct Worker *);
    void (*onTimeout)(struct Worker *); //执行后还是会执行onClose
    void (*onTick)(struct Worker *); //连接中事件(仅Async模式有效)
    void (*onClose)(struct Worker *, int err);
} Worker;

void worker_init();
void worker_clear();

Worker* worker_create(int protocol, const char* address, uint port);
int worker_is_connected(Worker* worker);
void worker_set_buffer_size(Worker* worker, size_t newSize);
void worker_close(Worker* worker, int err);
int worker_send(Worker* worker, const char* data);
int worker_read(Worker* worker);
bool worker_set_noblocking(SOCKET sock);
bool worker_connect(Worker* worker, bool async);
bool worker_isdomain(const char* data);
char* worker_domain2ip(const char* domain);
