#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 1024
void error_handling(const char *msg);

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }

    printf("client connecting...\n");

    FILE *read_fd = fdopen(sock, "r");
    FILE *write_fp = fdopen(dup(sock), "w");

    char buf[BUF_SIZE];
    while(1)
    {   
        printf("Input message(q to quit): ");
        fgets(buf, BUF_SIZE, stdin);
        if(strcmp(buf, "q\n") == 0 || strcmp(buf, "Q\n") == 0) {
            printf("client quitting...\n");
            break;
        }
        fputs(buf, write_fp);
        fflush(write_fp);
        fgets(buf, BUF_SIZE, read_fd);
        printf("Message received: %s\n", buf);
    }

    fclose(read_fd);
    fclose(write_fp);

    return 0;
}

void error_handling(const char *msg)
{
    perror(msg);
    exit(1);
}