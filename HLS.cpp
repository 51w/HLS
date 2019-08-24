#include "HLS.h"
#include <string.h>

HLSServer::HLSServer()
{
    tcplisten.SetProc(this);
}

HLSServer::~HLSServer()
{

}

int HLSServer::ProcInput(TcpBuff* conn)
{
    LOG(INFO) << conn->id << "zzzzzzzzz\n" << conn->ptr;

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

    tcplisten.WriteNetData(sendbuf->fd, sendbuf);
    delete sendbuf;
}

int HLSServer::ProcOutput()
{
    
}

int HLSServer::StartServer()
{
    tcplisten.Listen(1060);
}

int HLSServer::StopServer()
{

}
