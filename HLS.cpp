#include "HLS.h"

HLSServer::HLSServer()
{
    tcplisten.SetProc(this);
}

HLSServer::~HLSServer()
{

}

int HLSServer::ProcInput(ullong id, uchar* input, int size)
{
    LOG(INFO) << id << "zzzzzzzzz\n" << input;

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
