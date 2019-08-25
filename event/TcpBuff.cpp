///////////////////////////////////////
#include "TcpBuff.h"
#include <stdio.h>
#include <unistd.h>

TcpBuff::TcpBuff()
:fd(-1), id(-1), datasize(0), offset(0), size(0)
{
    ptr = NULL;
}

TcpBuff::~TcpBuff()
{
    if(ptr != NULL)
    {
        free(ptr);
        fprintf(stderr, "======~TcpBuff\n");
    }
  
    reset();
}

void TcpBuff::reset()
{
    fd = -1;
    id = -1;
    datasize = 0;
    offset = 0;
    size = 0;
}

int TcpBuff::SetSize(int num)
{
    ptr = (uchar*)malloc(num);
    size = num;
    
    return 0;
}

///////////////////////////////////////

TcpConnect::TcpConnect()
:_ConnectID(100)
{

}

TcpConnect::~TcpConnect()
{

}

bool TcpConnect::OpenConnect(int fd)
{
    ullong id = _ConnectID++;
    LOG(INFO) << "OpenConnect connection id " << id;

    TcpBuff *conn = new TcpBuff();
    conn->SetSize(1024);
    conn->fd =  fd;
    conn->id =  id;

    _FD2Conn.insert(std::make_pair(fd, conn));
    _ID2Conn.insert(std::make_pair(id, conn));

    return (conn != NULL);
}

bool TcpConnect::IsConnect(int fd)
{
    return _FD2Conn.count(fd);
}

bool TcpConnect::CloseConnect(int fd)
{
    int id = _FD2Conn[fd]->id;
    close(fd);

    delete _FD2Conn[fd];

    _FD2Conn.erase(fd);
    _ID2Conn.erase(id);

    return true;
}

///////////////////////////////////////