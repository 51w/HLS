#include <iostream>
#include "TcpListener.h"
#include "common.h"

int main()
{
    
    TcpListener hls;
    hls.Listen(1060);
    
    return 0;
}