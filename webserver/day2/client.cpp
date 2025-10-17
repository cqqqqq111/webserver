#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "util.h"

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");//检查socket是否创建成功

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");//检查连接是否成功

    while (1) {
        char buf[1024];
        bzero(&buf, sizeof(buf));//清空缓冲区
        scanf("%s", buf);//输入数据到缓冲区
        ssize_t write_bytes = write(sockfd, buf, strlen(buf));//向服务器发送数据
        if (write_bytes == -1) {
            printf("socket already disconnected, can't write anymore\n");
            break;
        }
        bzero(&buf, sizeof(buf));//清空缓冲区
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));//从服务器读取数据
        if (read_bytes > 0) {//读取到数据
            printf("message from server: %s\n", buf);
        } else if (read_bytes == 0) {//服务器断开连接
            printf("server socket disconnected\n");
            break;
        } else if(read_bytes == -1){//读取出错
            close(sockfd);
            errif(1, "socket read error");
        }
    }

    close(sockfd);
    return 0;
}