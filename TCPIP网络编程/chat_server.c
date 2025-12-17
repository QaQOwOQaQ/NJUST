#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#define BUF_SIZE 128
#define MAX_CLNT 256

void *handler_clnt(void *arg);
void send_msg(const char *msg, int len);
void error_handling(const char *msg);

int clnt_cnt; // condition variable
int clnt_socks[MAX_CLNT]; // condition variable
pthread_mutex_t mutex;

#define PTHREAD_LOCK_MUTEX pthread_mutex_lock(&mutex)
#define PTHREAD_UNLOCK_MUTEX pthread_mutex_unlock(&mutex)

int main(int argc, char **argv)
{
    // check command line 
    if(argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    // create serv_sock, bind address and listen 
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    // create mutex 
    pthread_mutex_init(&mutex, NULL);
    
    // 
    while(1) {
        // accept 
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_sz = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
        
        // client limit
        PTHREAD_LOCK_MUTEX;
        if (clnt_cnt >= MAX_CLNT) {
            close(clnt_sock);
            pthread_mutex_unlock(&mutex);
            continue;
        }
        clnt_socks[clnt_cnt++] = clnt_sock;
        PTHREAD_UNLOCK_MUTEX;
        
        // assign a new thread to this client
        pthread_t t_id;
            // convert int to pointer, but in the most of time, INT is 4B and POINTER is 8B
            // so it may be not saft convert INT(4B) to POINTER(8B) directly
            // LONG has the same size to POINTER, so we need to covert INT to LONG first
        pthread_create(&t_id, NULL, handler_clnt, (void*)(long)clnt_sock);
        pthread_detach(t_id);
        printf("Connected client IP: %s\n", inet_ntoa(clnt_addr.sin_addr));
    }
    close(serv_sock);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void *handler_clnt(void *arg)
{
    int clnt_sock = (long)arg;
    ssize_t str_len = 0;
    char msg[BUF_SIZE];
    while((str_len = read(clnt_sock, msg, sizeof(msg))) != 0) {
        send_msg(msg, str_len);
    }
    // remove this client socket 
    PTHREAD_LOCK_MUTEX;
    for(int i = 0; i < clnt_cnt; i ++ ) {
        if(clnt_sock == clnt_socks[i]) {
            clnt_socks[i] = clnt_socks[clnt_cnt - 1];
            clnt_cnt -- ;
            break;
        }
    }
    PTHREAD_UNLOCK_MUTEX;
    close(clnt_sock);
    return NULL;
}

void send_msg(const char *msg, int len)
{
    PTHREAD_LOCK_MUTEX;
    for(int i = 0; i < clnt_cnt; i ++ ) 
        write(clnt_socks[i], msg, len);
    PTHREAD_UNLOCK_MUTEX;
}

void error_handling(const char *msg)
{
    perror(msg);
    exit(1);
}