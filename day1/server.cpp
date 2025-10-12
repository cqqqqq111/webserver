#include<stdio.h>
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
    
    bind(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr));//绑定
    
    listen(sockfd,SOMAXCONN);
    
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len=sizeof(clnt_addr);
    bzero(&clnt_addr,sizeof(clnt_addr));
    
    int clnt_sockfd=accept(sockfd,(sockaddr*)&clnt_addr,&clnt_addr_len);//阻塞等待客户端连接
    
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
}