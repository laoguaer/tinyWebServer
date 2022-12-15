#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

struct cliInfo {
    int fd;
    struct sockaddr_in addr;
    pthread_t pid;
} cliarr[128];

void * working(void* arg) {
    struct cliInfo* info = (struct cliInfo*)arg;
    char cliIP[16];
    inet_ntop(AF_INET, &info->addr.sin_addr.s_addr, cliIP, sizeof cliIP);
    unsigned short port = ntohs(info->addr.sin_port);
    printf("ip : %s   port : %d\n", cliIP, port);

    char buf[1024];
    while (1) {
        memset(buf, 0, sizeof buf);
        int len = read(info->fd, buf, sizeof buf);
        if (len == -1) {
            perror("read");
            exit(-1);
        }
        else if (len > 0) {
            printf("Data : %s", buf);
        }
        else {
            printf("connect lose ...\n");
            break;
        }
        write(info->fd, buf, strlen(buf) + 1);
    }
    close(info->fd);
    printf("close fd = %d\n", info->fd);
    info->fd = -1;
}

int main() {    

    for (int i = 0; i < sizeof cliarr / sizeof cliarr[0]; ++i) {
        bzero(&cliarr[i], sizeof cliarr[i]);
        cliarr[i].fd = -1;
    }

    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    int ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    ret =  listen(lfd, 8);
    if (ret == -1) {
        perror("listen");
        exit(-1);
    }

    while (1) {
        struct sockaddr_in cliaddr;
        int len = sizeof cliaddr;


        for (int i = 0; i < 128; ++i) {
            if (cliarr[i].fd == -1) {
                cliarr[i].fd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
                if (cliarr[i].fd == -1) {
                    perror("accpet");
                    exit(-1);
                }
                cliarr[i].addr = cliaddr;
                pthread_create(&cliarr[i].pid, NULL, working, &cliarr[i]);
                pthread_detach(cliarr[i].pid);
            }
            if (i == 127) i = 0;
        }
        
        // pid_t pid;
        // pid = fork();
        // if (pid == 0) {
        //     char cliIP[16];
        //     inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, cliIP, sizeof cliIP);
        //     unsigned short port = ntohs(cliaddr.sin_port);
        //     printf("ip : %s   port : %d\n", cliIP, port);

        //     char buf[1024];
        //     while (1) {
        //         memset(buf, 0, sizeof buf);
        //         int len = read(cfd, buf, sizeof buf);
        //         if (len == -1) {
        //             perror("read");
        //             exit(-1);
        //         }
        //         else if (len > 0) {
        //             printf("Data : %s", buf);
        //         }
        //         else {
        //             printf("connect lose ...\n");
        //             break;
        //         }

        //         write(cfd, buf, strlen(buf) + 1);
        //     }
        //     close(cfd);
        // }

    }

    close(lfd);

    return 0;
}