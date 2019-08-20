#ifndef _TCPLISTENER_H_
#define _TCPLISTENER_H_
#include "common.h"
#include "TcpBuff.h"
#include <map>

class TcpListener
{
public:
    TcpListener();
    ~TcpListener();

    int Listen(int port);
    int NetInit(int port);

    void ProcessInput(int lfd, int efd, int n, struct epoll_event *events);
    bool SetupConnection(int infd);
    void CloseConnection(TcpConnect* conn);
    void HandleInputEvent(TcpConnect* conn, uint& eventflag);

    int tcpfd;
    int epollfd;
    ullong g_NextSeqId;

    std::map<int,    TcpConnect*> g_FD2Conn; 
    std::map<ullong, TcpConnect*> g_Seq2Conn;
};

#endif