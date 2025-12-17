#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(const char* message);

int main(int argc, char **argv)
{
    // check command line argument 
    if(argc != 2) {
        printf("Usage : %s <Port> n", argv[0]);
        exit(1);
    }

    // create receive socket and bind address
    int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));
    if(bind(recv_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        error_handling("bind() error");

    // receive multicast data 
    char buf[BUF_SIZE];
    int line = 0;
    while(1)
    {
        ssize_t read_len = recvfrom(recv_sock, buf, BUF_SIZE- 1, 0, NULL, 0);
        if(read_len < 0)
            break;
        buf[read_len] = 0;
        printf("[%d] %s", ++ line, buf);
    }

    close(recv_sock);
    return 0;
}

void error_handling(const char* message)
{
    perror(message);
    exit(1);
}