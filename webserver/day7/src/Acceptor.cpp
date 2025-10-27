#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Server.h"

Acceptor::Acceptor(EventLoop* _loop): loop(_loop){
    sock=new Socket();
    addr=new InetAddress("127.0.0.1",1111);
    sock->bind(addr);
    sock->listen();
    sock->setnonblocking();
    acceptChannel=new Channel(loop,sock->getFd());
    std::function<void()>cb=std::bind(&Acceptor::acceptConnection,this);
    acceptChannel->setCallback(cb);
    acceptChannel->enableReading();
}

Acceptor::~Acceptor(){
    delete sock;
    delete addr;
    delete acceptChannel;
}

void Acceptor::acceptConnection(){
    newConntionCallback(sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)>_cb){
    newConntionCallback=_cb;
}