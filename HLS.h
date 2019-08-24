#ifndef _HLS_H_
#define _HLS_H_
#include "TcpListener.h"

class HLSServer : public TcpProc
{
public:
    HLSServer();
    ~HLSServer();

    virtual int ProcInput(TcpBuff* conn);
    virtual int ProcOutput();

    int StartServer();
    int StopServer();

    TcpListener tcplisten;
};



#endif