#ifndef _HLS_H_
#define _HLS_H_
#include "TcpListener.h"

class HLSServer : public TcpProc
{
public:
    HLSServer();
    ~HLSServer();

    virtual int ProcInput(ullong id, uchar* input, int size);
    virtual int ProcOutput();

    int StartServer();
    int StopServer();

    TcpListener tcplisten;
};



#endif