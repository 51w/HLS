#include "TcpBuff.h"

TcpBuff::TcpBuff()
{
    dataSize = 0;
    bufferSize = 0;
    ptr = NULL;
}

TcpBuff::~TcpBuff()
{
    if(ptr != NULL)
    {
        free(ptr);
        fprintf(stderr, "======~TcpBuff\n");
    }
      
  
    dataSize = 0;
    bufferSize = 0;
}

int TcpBuff::SetSize(int size)
{
    ptr = (unsigned char*)malloc(size);
    bufferSize = size;
    
    return 0;
}


TcpConnect::TcpConnect()
{

}

TcpConnect::~TcpConnect()
{

}