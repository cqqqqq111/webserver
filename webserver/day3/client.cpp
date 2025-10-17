#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "util.h"

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    errif(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");

    while (1) {
        char buf[1024];
        bzero(buf, sizeof(buf));
        if (scanf("%1023s", buf) != 1) break;
        ssize_t write_bytes = write(sockfd, buf, strlen(buf));
        if (write_bytes == -1) {
            printf("socket already disconnected, can't write anymore\n");
            break;
        }

        bzero(buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("message from server: %s\n", buf);
        } else if (read_bytes == 0) {
            printf("server socket disconnected\n");
            break;
        } else {
            close(sockfd);
            errif(1, "socket read error");
        }
    }

    close(sockfd);
    return 0;
}