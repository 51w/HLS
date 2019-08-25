#ifndef _TCPLISTENER_H_
#define _TCPLISTENER_H_
#include "common.h"
#include "TcpBuff.h"

class TcpProc
{
public:
    virtual ~TcpProc() {}
    virtual int Proc_EPOLLERR(int fd) = 0;
    virtual int Proc_EPOLLNEW(int fd) = 0;
    virtual int Proc_EPOLLIN( int fd) = 0;
    virtual int Proc_EPOLLOUT(int fd) = 0;

    virtual int StartServer() = 0;
    virtual int StopServer()  = 0;
};

class TcpListener
{
public:
    TcpListener();
    ~TcpListener();
    int SetProc(TcpProc *proc);
    int Listen(const char* port);
    int Init(const char* port);

    int WriteNetData(int fd, TcpBuff* data);
    int ReadNetData(int fd, TcpBuff* data);

    int Set_SOCKET(const char* port);
    int AcceptSocket();
    int Set_NONBLOCK(int fd);
    int ProcEvent(int num, struct epoll_event *events);

private:
    TcpProc *_proc;
    int tcpfd;
    int epollfd;
};

#endif