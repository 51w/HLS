#ifndef _TCPLISTENER_H_
#define _TCPLISTENER_H_
#include "common.h"
#include "TcpBuff.h"

class TcpProc
{
public:
    virtual ~TcpProc() {}
    virtual int ProcInput(ullong id, uchar* input, int size) = 0;
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
    bool SetupConnection(int infd);
    void CloseConnection(TcpConnect* conn);
    void HandleInputEvent(TcpConnect* conn, uint& eventflag);
    void SetProc(TcpProc *proc);

    int tcpfd;
    int epollfd;
    //ullong g_NextSeqId;
    TcpProc *_proc;

    TcpConnect ConnCTL;
    //std::map<int,    TcpConnect*> g_FD2Conn; 
    //std::map<ullong, TcpConnect*> g_Seq2Conn;
};

#endif