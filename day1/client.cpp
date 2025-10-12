#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>

int main(){
    int sockfd=socket(AF_INET,SOCK_STREAM,0);//ipv4 数据传输方式 协议
    
    struct sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));//初始化结构体，用来表示地址
    serv_addr.sin_family=AF_INET;//ipv4
    serv_addr.sin_port=htons(8080);//端口号
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");//ip地址

    connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr));//连接服务器

    return 0;
}