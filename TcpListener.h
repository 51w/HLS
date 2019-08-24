#ifndef _TCPLISTENER_H_
#define _TCPLISTENER_H_
#include "common.h"
#include "TcpBuff.h"

class TcpProc
{
public:
    virtual ~TcpProc() {}
    virtual int ProcInput(TcpBuff* conn) = 0;
    virtual int ProcOutput()  = 0;

    virtual int StartServer() = 0;
    virtual int StopServer()  = 0;
};

class TcpListener
{
public:
    TcpListener();
    ~TcpListener();

    int Listen(int port);
    int NetInit(int port);

    void ProcessInput(int lfd, int efd, int n, struct epoll_event *events);
    void HandleInputEvent(TcpBuff* conn, uint& eventflag);
    void SetProc(TcpProc *proc);

    int tcpfd;
    int epollfd;

    TcpProc *_proc;
    TcpConnect ConnCTL;
};

#endif