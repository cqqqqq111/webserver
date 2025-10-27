#pragma once
#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor{
    private:
        EventLoop *loop;
        Socket *sock;
        InetAddress *addr;
        Channel *acceptChannel;
    public:
        Acceptor(EventLoop*Loop);
        ~Acceptor();
        void acceptConnection();
        std::function<void(Socket*)>newConntionCallback;
        void setNewConnectionCallback(std::function<void(Socket*)>);
};