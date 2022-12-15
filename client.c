#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


int main() {

    int fd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_port = htons(9999);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr.s_addr));

    int ret = connect(fd, (struct sockaddr*)&addr, sizeof addr);
    if (ret == -1) {
        perror("connect");
        exit(-1);
    }
    
    char buf[5];

    while (1) {
        bzero(buf, sizeof buf);
        fgets(buf, sizeof buf, stdin);
        ret = write(fd, buf, strlen(buf) + 1);
        if (ret == -1) {
            perror("write");
            exit(-1);
        }
        int len = read(fd, buf, sizeof buf);
        if (len == -1) {
            perror("read");
            exit(-1);
        }
        else if (len == 0) {
            printf("server lose ...\n");
        }
        else {
            printf("Data : %s", buf);
        }
    }
    close(fd);
    return 0;
}