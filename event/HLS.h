#ifndef _HLS_H_
#define _HLS_H_
#include "TcpListener.h"

class HLSServer : public TcpProc
{
public:
    HLSServer();
    ~HLSServer();

    virtual int Proc_EPOLLERR(int fd);
    virtual int Proc_EPOLLNEW(int fd);
    virtual int Proc_EPOLLIN( int fd);
    virtual int Proc_EPOLLOUT(int fd);

    int StartServer();
    int StopServer();

    TcpConnect  *tcpconnect;
    TcpListener *tcplisten;
};



#endif