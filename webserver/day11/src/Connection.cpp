#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "util.h"
#include "Buffer.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


Connection::Connection(EventLoop *_loop,Socket *_sock):loop(_loop),sock(_sock),channel(nullptr){
    channel=new Channel(loop,sock->getFd());
    std::function<void()> cb=std::bind(&Connection::echo,this,sock->getFd());
    channel->setCallback(cb);
    channel->enableReading();
    readBuffer=new Buffer();
}

Connection::~Connection(){
    delete channel;
    delete sock;
}

void Connection::echo(int sockfd){
    char buf[1024];
    while(true){
        bzero(&buf,sizeof(buf));
        ssize_t bytes_read=read(sockfd,buf,sizeof(buf));
        if(bytes_read>0){
            readBuffer->append(buf,bytes_read);
        }
        else if(bytes_read==-1&&errno==EINTR){
            printf("continue reading");
            continue;
        }
        else if(bytes_read==-1&&((errno==EAGAIN)||(errno==EWOULDBLOCK))){
            printf("finish reading once\n");
            printf ("message from client fd %d: %s\n",sockfd,readBuffer->c_str());
            errif (write(sockfd,readBuffer->c_str(),readBuffer->size())==-1,"Socket write error");
            readBuffer->clear();
            break;
        }
        else if(bytes_read==0){
            printf("EOF,client fd %d disconnected\n",sockfd);
            deleteConnectionCallback(sock);
            break;
        }
    }
}
void Connection::setDeleteConnectionCallback(std::function<void(Socket*)>_cb){
    deleteConnectionCallback=_cb;
}