///////////////////////////////////////
#ifndef _TCPBUFF_H_
#define _TCPBUFF_H_
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <map>

class TcpBuff
{
public:
    TcpBuff();
    ~TcpBuff();

    void reset();
    int SetSize(int num);

public:
    int fd;
    int id;

    int datasize;
    int offset;
    int size;
    uchar* ptr;
};

///////////////////////////////////////

class TcpConnect
{
public:
    TcpConnect();
    ~TcpConnect();

    bool OpenConnect(int fd);
    bool CloseConnect(int fd);
    bool IsConnect(int fd);

    ullong _ConnectID;

    std::map<int,    TcpBuff*> _FD2Conn; 
    std::map<ullong, TcpBuff*> _ID2Conn;
};

///////////////////////////////////////

#endif