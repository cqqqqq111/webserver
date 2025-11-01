#pragma once
#include <functional>

class EventLoop;
class Socket;
class Channel;
class Acceptor{
    private:
        EventLoop *loop;
        Socket *sock;
        Channel *acceptChannel;
        std::function<void(Socket*)>newConnectionCallback;
    public:
        Acceptor(EventLoop*Loop);
        ~Acceptor();
        void acceptConnection();
        void setNewConnectionCallback(std::function<void(Socket*)>);
};