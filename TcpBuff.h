#ifndef _TCPBUFF_H_
#define _TCPBUFF_H_
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

/////////////////////////////

class TcpBuff
{
public:
    TcpBuff();
    ~TcpBuff();
    
    int SetSize(int size);

public:
    int dataSize;
    int bufferSize;
    uchar* ptr;
};

/////////////////////////////

class TcpConnect : public TcpBuff
{
public:
    TcpConnect();
    ~TcpConnect();

    int fd;
    int id;
    //TcpBuff recvBuff;
};

/////////////////////////////

#endif