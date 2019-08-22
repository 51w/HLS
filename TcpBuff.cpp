///////////////////////////////////////
#include "TcpBuff.h"

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

bool TcpConnect::OpenConnect(int infd)
{
    ullong id = _ConnectID++;
    LOG(INFO) << "OpenConnect connection id " << id;

    TcpBuff *conn = new TcpBuff();
    conn->SetSize(1024);
    conn->fd =  infd;
    conn->id =  id;
    _ID2Conn.insert(std::make_pair(id,   conn));
    _FD2Conn.insert(std::make_pair(infd, conn));

    return (conn != NULL);
}

///////////////////////////////////////