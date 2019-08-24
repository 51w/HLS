#include "HLS.h"

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
