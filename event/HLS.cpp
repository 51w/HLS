#include "HLS.h"
#include <string.h>

HLSServer::HLSServer()
{
    tcpconnect = new TcpConnect();
    tcplisten  = new TcpListener();
    tcplisten->SetProc(this);
}

HLSServer::~HLSServer()
{
    delete tcplisten;
    delete tcpconnect;
}
int HLSServer::Proc_EPOLLERR(int fd)
{
    LOG(INFO) << "Proc_EPOLLERR";
    tcpconnect->CloseConnect(fd);
}
int HLSServer::Proc_EPOLLNEW(int fd)
{
    LOG(INFO) << "Proc_EPOLLNEW " << fd;
    tcpconnect->OpenConnect(fd);
    return 0;
}
int HLSServer::Proc_EPOLLIN(int fd)
{
static int zz=0;
if(zz<2)
{
    LOG(INFO) << "Proc_EPOLLIN " << fd;
    TcpBuff* conn = tcpconnect->_FD2Conn[fd];
    memset(conn->ptr, 0, conn->size);
    conn->datasize = 0;

    int ret = tcplisten->ReadNetData(fd, conn);
    LOG(INFO) << conn->datasize << " " << conn->ptr;

    if(ret == -1 || ret == 1)
    {
        tcpconnect->CloseConnect(fd);
        return -1;
    }

    for(int i=0; i<9; i++)
    {
        fprintf(stderr, "%d ", conn->ptr[i]);
    }
    fprintf(stderr, "\n\n");
    for(int i=9; i<9+764; i++)
    {
        fprintf(stderr, "%d ", conn->ptr[i]);
    }
    fprintf(stderr, "\n\n");
    for(int i=9+764; i<9+764+764; i++)
    {
        fprintf(stderr, "%d ", conn->ptr[i]);
    }
    for(int i=1; i<9; i++)
    {
        conn->ptr[i] = 0;
    }
    for(int i=0; i<1536; i++)
    {
        conn->ptr[1537+i] = i;
    }
    
    conn->datasize += 1536;
    tcplisten->WriteNetData(fd, conn);
zz++;
}
else
{
    TcpBuff* conn = tcpconnect->_FD2Conn[fd];
    memset(conn->ptr, 0, conn->size);
    conn->datasize = 0;

    int ret = tcplisten->ReadNetData(fd, conn);
    LOG(INFO) << conn->datasize << " " << conn->ptr;

    fprintf(stderr, "%s ", conn->ptr+10);
}

    return 0;
}
int HLSServer::Proc_EPOLLOUT(int fd)
{
    LOG(INFO) << "Proc_EPOLLOUT";
}

int HLSServer::StartServer()
{
    std::string port = "1060";
    tcplisten->Listen(port.c_str());
}

int HLSServer::StopServer()
{

}
