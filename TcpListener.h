#ifndef _TCPLISTENER_H_
#define _TCPLISTENER_H_


class TcpListener
{
public:
    TcpListener();
    ~TcpListener();
    
    int Listen(int port);
    int NetInit(int port);
    
    int tcpfd;
    int epollfd;

};

#endif