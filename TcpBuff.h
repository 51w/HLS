#ifndef _TCPBUFF_H_
#define _TCPBUFF_H_

class TcpBuff
{
public:
    TcpBuff();
    ~TcpBuff();
    
    int SetSize(int size);

public:
    int dataSize;
    int bufferSize;
    unsigned char* ptr;
};

TcpBuff::TcpBuff()
{
    dataSize = 0;
    bufferSize = 0;
    ptr = NULL;
}

TcpBuff::~TcpBuff()
{
    if(ptr != NULL)
      free(ptr);
  
    dataSize = 0;
    bufferSize = 0;
}

int TcpBuff::SetSize(int size)
{
    ptr = (unsigned char*)malloc(size);
    bufferSize = size;
    
    return 0;
}


class RecvBuff
{
public:
    RecvBuff();
    ~RecvBuff();

};

class SendBuff
{
public:
    SendBuff();
    ~SendBuff();
    
};

#endif