#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<errno.h>
#include"util.h"

void setnonblocjing(int fd){
    fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK);
}

int main(){
    int sockfd=socket(AF_INET,SOCK_STREAM,0);//ipv4 数据传输方式 协议
    errif(sockfd==-1,"socket create error");
    
    struct sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));//初始化结构体，用来表示地址
    serv_addr.sin_family=AF_INET;//ipv4
    serv_addr.sin_port=htons(8080);//端口号
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");//ip地址
    
    errif(bind(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr))==-1,"socket bind error");
    
    errif(listen(sockfd,SOMAXCONN)==-1,"socket listen error");

    int epfd=epoll_create1(0);
    errif(epfd==-1,"epoll create error");
    
    struct epoll_event events[1024],ev;
    ev.data.fd=sockfd;
    ev.events=EPOLLIN|EPOLLET;
    setnonblocjing(sockfd);
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);
    
    while(true){
        int n=epoll_wait(epfd,events,1024,-1);//有n个fd事件发生
        errif(n==-1,"epoll wait error");
        for(int i=0;i<n;i++){
            if(events[i].data.fd==sockfd){//有新的客户端连接
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_len=sizeof(clnt_addr);
                bzero(&clnt_addr,sizeof(clnt_addr));
                
                int clnt_sockfd=accept(sockfd,(sockaddr*)&clnt_addr,&clnt_addr_len);//阻塞等待客户端连接
                errif(clnt_sockfd==-1,"socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
                
                ev.data.fd=clnt_sockfd;
                ev.events=EPOLLIN|EPOLLET;
                setnonblocjing(clnt_sockfd);//et搭配非阻塞
                epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sockfd,&ev);//将客户端socket加入epoll监听
            }else if(events[i].events&EPOLLIN){
                char buf[1024];
                while(true){//
                    bzero(&buf,sizeof(buf));
                    ssize_t read_bytes=read(events[i].data.fd,buf,sizeof(buf));
                    if(read_bytes>0){
                        printf("message from client fd %d: %s\n", events[i].data.fd,buf);
                        write(events[i].data.fd,buf,read_bytes);//保存到客户端
                    }else if(read_bytes==0){
                        printf("client fd %d closed connection\n", events[i].data.fd);
                        close(events[i].data.fd);
                    }else if(read_bytes==-1&&errno==EAGAIN){
                        printf("read later\n");
                        continue;
                    }else if(read_bytes==-1&&((errno==ECONNRESET)||(errno==EWOULDBLOCK))){
                        printf("finish reading once,errno=%d\n",errno);
                        break;
                    }
                }
            }else{
                printf("something else happened\n");
            }
        }      
    }
    close(sockfd);
    return 0;
}