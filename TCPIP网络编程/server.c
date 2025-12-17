// Sender just need to create UDP socket and send data to multicast group :)
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

#define EPOLL_SIZE 50
#define BUF_SIZE 4 // 小 buffer 可以测试 epoll_wait 的默认水平触发
void error_handling(const char* message);
void setnonblockingmode(int fd); // 设置为非阻塞模式

int main(int argc, char **argv) 
{
    if(argc != 2) {
        printf("Usage : %s <Port>\n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    setnonblockingmode(serv_sock);

    int epfd = epoll_create(EPOLL_SIZE);
    struct epoll_event *ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    struct epoll_event event;
    
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    int idx = 1;

    while(1)
    {
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt == -1) {
            error_handling("epoll_wait() error");
        }
        printf("epoll wait [%d] \n", idx ++ );
        for(int i = 0; i < event_cnt; i ++ ) {
            if(ep_events[i].data.fd == serv_sock) {
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_sz = sizeof(clnt_addr);
                int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
                setnonblockingmode(clnt_sock); // 设置为非阻塞模式
                if(clnt_sock == -1) {
                    error_handling("accept() error");
                }
                printf("client connected: %d\n", clnt_sock);
                // set edge trigger
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
            }
            else {
                int clnt_sock = ep_events[i].data.fd;
                char buf[BUF_SIZE];
                while(1) { // 直到 read error 或者没有数据可读时(errno=EAGAIN)结束
                    ssize_t str_len = read(clnt_sock, buf, BUF_SIZE);
                    if(str_len == 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, clnt_sock, NULL);
                        close(clnt_sock);
                        printf("client disconnected: %d\n", clnt_sock);
                        break;
                    }
                    if(str_len < 0) {
                        // 非阻塞模式下，稍后再试
                        if(errno == EAGAIN || errno == EWOULDBLOCK) break; 
                        error_handling("read error");
                    }
                    else {
                        write(clnt_sock, buf, str_len);
                        buf[str_len] = 0;
                        printf("Received: %s \n", buf);
                    }
                }
            }
        }
    }
    
    close(epfd);
    close(serv_sock);
    return 0;
}

void error_handling(const char* message)
{
    perror(message);
    exit(1);
}

void setnonblockingmode(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}