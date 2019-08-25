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
    LOG(INFO) << "Proc_EPOLLIN " << fd;
    TcpBuff* conn = tcpconnect->_FD2Conn[fd];
    memset(conn->ptr, 0, conn->size);
    int ret = tcplisten->ReadNetData(fd, conn);
    LOG(INFO) << conn->id << "zzzzzzzzz" << conn->ptr;

    if(ret == -1 || ret == 1)
    {
        tcpconnect->CloseConnect(fd);
        return -1;
    }

static int iii = 0;
if(iii == 0)
{
    TcpBuff* sendbuf = new TcpBuff();
    sendbuf->SetSize(1024);
    sendbuf->fd = conn->fd;
    sendbuf->id = conn->id;
    
    std::string buf = "HTTP/1.1 200 OK\r\nContent-Type: audio/x-mpegurl\r\nServer: FlashCom/3.5.4\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, PUT, POST, DELETE, OPTIONS\r\nContent-Length: 453\r\n\r\n";
    buf += "#EXTM3U\r\n";
    buf += "#EXT-X-TARGETDURATION:3\r\n";
    buf += "#EXT-X-VERSION:3\r\n";
    buf += "#EXT-X-MEDIA-SEQUENCE:0\r\n";
    buf += "#EXT-X-PLAYLIST-TYPE:VOD\r\n";
    buf += "#EXTINF:3.3,\r\n";
    buf += "media_0.ts?proxysessionid=1804289438,bandwidth=1647992\r\n";
    buf += "#EXTINF:3.0,\r\n";
    buf += "media_1.ts?proxysessionid=1804289438,bandwidth=1647992\r\n";
    buf += "#EXTINF:3.0,\r\n";
    buf += "media_2.ts?proxysessionid=1804289438,bandwidth=1647992\r\n";
    buf += "#EXTINF:3.0,\r\n";
    buf += "media_3.ts?proxysessionid=1804289438,bandwidth=1647992\r\n";
    buf += "#EXTINF:1.0,\r\n";
    buf += "media_4.ts?proxysessionid=1804289438,bandwidth=1647992\r\n";
    buf += "EXT-X-ENDLIST\r\n";

    memcpy(sendbuf->ptr, buf.c_str(), buf.length());
    sendbuf->datasize = buf.length();

    tcplisten->WriteNetData(sendbuf->fd, sendbuf);
    delete sendbuf;
iii = 1;
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
