#include "../include/worker.h"
int worker_init_flag = 0;

void worker_init(){
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(result != 0){
        printf("Windows dll cannot import!\n");
        exit(0);
    }
#endif
    worker_init_flag = 1;
}

void worker_clear(){
#ifdef _WIN32
    WSACleanup();
#endif
    worker_init_flag = 0;
}

#include "../libs/include/mem.h"
#include "../libs/include/buffer.h"
#include "../include/public.h"
#include "../include/connectionPool.h"

bool worker_isdomain(const char* data){
    uint max = strlen(data);
    for(uint i = 0;i < max;i++)
        if(('9' < data[i] || data[i] < '0') && data[i] != '.') return true;
    return false;
}

char* worker_domain2ip(const char* domain){
    int rec;
    struct hostent* info;
    struct in_addr* address;
    info = gethostbyname(domain);
    if(info == NULL) return NULL;
    address = (struct in_addr* )info -> h_addr_list[0];
    return inet_ntoa(*address);
}

Worker* worker_create(int protocol, const char* address, uint port){
    if(!worker_init_flag) return NULL;
    MemBlock* block = mem_malloc(sizeof(Worker), NULL);
    Worker* worker = (Worker *)block -> ptr;
    worker -> memBlock   = (void *)block;
    worker -> context    = NULL;
    worker -> start      = getMicroTime();
    worker -> timeout    = WORKER_DEFAULT_TIMEOUT * 1000;
    worker -> status     = WORKER_STATUS_WAITTING;
    worker -> bufferSize = WORKER_BUFFER_SIZE; //默认大小
    worker -> buffer     = (void *)buffer_create_size(WORKER_BUFFER_SIZE * sizeof(char));
    worker -> onClose    = NULL;
    worker -> onConnect  = NULL;
    worker -> onMessage  = NULL;
    worker -> onTimeout  = NULL;
    worker -> onStart    = NULL;
    worker -> onTick     = NULL;
    buffer_clear(worker -> buffer);

    if(protocol == WORKER_PROCOTOL_TCP){
        char* ptr = (char *)address;
        if(worker_isdomain(address)){
            ptr = worker_domain2ip(address);
            if(ptr == NULL){
                printf("[Worker]: Cannot create the invaild address parser\n");
                return NULL;
            }
        }
        worker -> sockid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(worker -> sockid == WORKER_SOCKET_ERROR){
            return NULL;
        }
        strcpy(worker -> remote.address, address);
        strcpy(worker -> remote.ip, ptr);
        worker -> remote.port = port;
        (worker -> remote).sockaddr.sin_family = AF_INET;
        (worker -> remote).sockaddr.sin_port = htons(port);
        (worker -> remote).sockaddr.sin_addr.s_addr = inet_addr(ptr);
        return worker;
    }
}

int worker_is_connected(Worker* worker){
    if(worker -> status == WORKER_STATUS_CONNECTED) return true;
    int rec, len;
#if _WIN32
    DWORD timer;
    len = sizeof(timer);
    rec = getsockopt(worker -> sockid, SOL_SOCKET, SO_CONNECT_TIME, (char *)&timer, &len);
    if(rec < 0) return -1;
    return timer != 0xffffffff;
#else
    struct tcp_info info;
    socklen_t slen = sizeof(info);
    rec = getsockopt(worker -> sockid, SOL_TCP, TCP_INFO, (char *)&info, &slen);
    if(rec < 0) return -1;
    return info.tcpi_state == TCP_ESTABLISHED;
#endif
}

void worker_set_buffer_size(Worker* worker, size_t newSize){
    buffer_resize(worker -> buffer, newSize);
}

void worker_close(Worker* worker, int err){
    if(worker -> status == WORKER_STATUS_CLOSED) return;
    worker -> status = WORKER_STATUS_CLOSED;

    if(!buffer_isempty(worker -> buffer)) (worker -> onMessage)(worker);
    if(worker -> onClose != NULL) (worker -> onClose)(worker, err);
    buffer_delete((Buffer *)worker -> buffer);
    MemBlock* block = (MemBlock *)worker -> context;
    mem_free(block -> ptr, NULL);
    WORKER_SOCKET_CLOSE(worker -> sockid);
}

int worker_send(Worker* worker, const char* data){
    if(worker -> status == WORKER_STATUS_CLOSED) return -1;
    uint length = strlen(data);
    return send(worker -> sockid, data, length, 0);
}

int worker_read(Worker* worker){
    if(worker -> status == WORKER_STATUS_CLOSED || worker == NULL) return -1;
    Buffer* buf = (Buffer *)worker -> buffer;
    buffer_clear(buf);
    int rec = recv(worker -> sockid, buf -> data, worker -> bufferSize, 0);
    if(rec > 0){
        buf -> realsize = rec;
        return rec;
    }
    if(rec == 0){
        worker_close(worker, GET_SOCKET_ERROR());
        return -1;
    }else if(rec < 0){
        #ifdef _WIN32
        if(GET_SOCKET_ERROR() == WSAEWOULDBLOCK) return 0;
        #else
        if(GET_SOCKET_ERROR() == EAGAIN || 
           GET_SOCKET_ERROR() == EWOULDBLOCK) return 0;
        #endif
        worker_close(worker, GET_SOCKET_ERROR());
        return -1;
    }
}

//非阻塞模式
bool worker_set_noblocking(SOCKET sock){
#ifdef _WIN32
    u_long mode = 1;
    if(ioctlsocket(sock, FIONBIO, &mode) != 0) return false;
#else
    int flags = fcntl(sock, F_GETFL, 0);
    if(flags == -1) return false;
    if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
        return false;
#endif
    return true;
}

bool worker_connect(Worker* worker, bool async){
    if(worker == NULL || worker -> status == WORKER_STATUS_CONNECTED
                      || worker -> status == WORKER_STATUS_CONNECTING) return false;
    SOCKET client = worker -> sockid;
    if(async){
        worker_set_noblocking(client);
        int rec = connect(client, (struct sockaddr*)&worker -> remote.sockaddr, sizeof(worker -> remote.sockaddr));
        #if _WIN32
        int flag = GET_SOCKET_ERROR() != WSAEWOULDBLOCK;
        #else
        int flag = GET_SOCKET_ERROR() != EINPROGRESS;
        #endif
        if(rec < 0 && flag) return false;
        worker -> status = WORKER_STATUS_CONNECTING;
        connectionPool_push(worker);
        return true;
    }
    int rec = connect(client, (struct sockaddr*)&worker -> remote.sockaddr, sizeof(worker -> remote.sockaddr));
    if(rec == -1){
        worker_close(worker, GET_SOCKET_ERROR());
        return false;
    }
    if(worker -> onConnect != NULL) (worker -> onConnect)(worker);
    worker -> status = WORKER_STATUS_CONNECTED;
    int flag = worker_set_noblocking(client);
    long long timer = 0;
    while(1){
        if(worker -> status == WORKER_STATUS_CLOSED) break;
        timer = getMicroTime() - worker -> start;
        if(timer > worker -> timeout){
            if(worker -> onTimeout != NULL) (worker -> onTimeout)(worker);
            worker_close(worker, -1);
        }
        rec = worker_read(worker);
        if(rec == 0) continue;
        if(rec > 0 && worker -> onMessage != NULL){
            (worker -> onMessage)(worker);
            buffer_clear(worker -> buffer);
        }
        usleep(WORKER_SYNC_TIMER);
    }
    return true;
}
