///////////////////////////////////////////////
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include "TcpListener.h"
#define MAXEVENTS 64

TcpListener::TcpListener()
:tcpfd(-1), epollfd(-1), _proc(NULL)
{

}

TcpListener::~TcpListener()
{

}

int TcpListener::SetProc(TcpProc *proc)
{
    _proc = proc;
}

int TcpListener::AcceptSocket()
{
    struct sockaddr in_addr;
    socklen_t in_len = sizeof(in_addr);

    int infd = -1;
    infd = accept(tcpfd, &in_addr, &in_len);
    if(infd == -1)
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK || errno == ECONNABORTED || errno == EPROTO || errno != EINTR)
        {
            return -1;
        }
        else
        {
            LOG(ERROR) << "accept";
            return -1;
        }
    }
    int status = Set_NONBLOCK(infd);
    CHECK(status != -1) << "AcceptSocket Set_NONBLOCK ERROR.";
    
    struct epoll_event event;
    event.data.fd = infd;
    event.events = EPOLLIN | EPOLLET;
    status = epoll_ctl (epollfd, EPOLL_CTL_ADD, infd, &event);
    CHECK(status != -1) << "AcceptSocket epoll_ctl ERROR.";
    
    LOG(INFO) << "AcceptSocket Accept connection fd: " << infd;
    return infd;
}

int TcpListener::Set_NONBLOCK(int fd)
{
    int block = fcntl(fd, F_GETFL, 0);
    if (block == -1)
    {
        return -1;
    }
    block |= O_NONBLOCK;

    int set = fcntl(fd, F_SETFL, block);
    if (set == -1)
    {
        return -1;
    }
    
    return 0;
}

int TcpListener::Set_SOCKET(const char* port)
{
    int status, sfd;
    
    struct addrinfo hints;
    memset (&hints, 0, sizeof (struct addrinfo));
    hints.ai_family   = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM;   /* We want a TCP socket */
    hints.ai_flags    = AI_PASSIVE;    /* All interfaces */
    
    struct addrinfo *result, *rp;
    status = getaddrinfo(NULL, port, &hints, &result);
    if(status != 0)
    {
        LOG(ERROR) << "getaddrinfo: " << gai_strerror(status);
        return -1;
    }
    
    for(rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)  continue;

        int flag = 1;
        if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == 0)
        {
            status = bind(sfd, rp->ai_addr, rp->ai_addrlen);
            if(status == 0)
            {
                //LOG(INFO) << "Set SO_REUSEADDR True.";
                break;
            }
        }
        close (sfd);
    }
    CHECK(rp != NULL) << "Socket bind Error";
    freeaddrinfo (result);
    
    return sfd;
}

int TcpListener::ReadNetData(int fd, TcpBuff* data)
{
    char* buf = (char*)data->ptr + data->datasize;
    int bufSize = data->size - data->datasize;
    
    int offset = 0;
    while (offset < bufSize)
    {
        int nread = read(fd, buf + offset, bufSize - offset);
        if (nread > 0) //normal
        {
            offset += nread;
        }
        else if (nread == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) //no data
            {
                data->datasize += offset;
                return 0;
            }
            else if (errno == EINTR) //interrupt
            {
                continue;
            }
            else //other error
            {
                LOG(ERROR) << "ReadNetData read other error 0";
                return -1;
            }
        }
        else if (nread == 0) //remote close
        {
            LOG(WARNING) << "ReadNetData remote close";
            return -1;
        }
        else //other error , should never happen
        {
            LOG(WARNING) << "ReadNetData read other error 1";
            return -1;
        }
    }
    
    //data.dataSize = data.bufferSize;
    return 1; //buffer full
}

int TcpListener::WriteNetData(int fd, TcpBuff* data)
{
    char* buf = (char*)data->ptr + data->offset;
    int bufSize = data->datasize;
    
    int offset = 0;
    while(offset < bufSize)
    {
        int nwrite = write(fd, buf + offset, bufSize - offset);
        if (nwrite >= 0) //normal write
        {
            offset += nwrite;
        }
        else if (nwrite == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) // write block
            {
                //printf("EAGAIN\n");
                data->offset += offset;
                data->datasize -= offset;
                return 0;
            }
            else if (errno == EINTR) //interrupt
            {
                continue;
            }
            else //other error
            {
                return -1;
            }
        }
        else //other error , should never happen
        {
            printf("Write Failed!!!\n");
            return -1;
        }
    }
    
    data->offset = data->size;
    data->datasize   = 0;
    return 1;
}

///////////////////////////////////////////////
int TcpListener::ProcEvent(int num, struct epoll_event *events)
{
    CHECK(_proc != NULL) << "ProcEvent ERROR";

    for(int i = 0; i < num; i++)
	{
    int  ProcFD = events[i].data.fd;
    uint ProcLG = events[i].events;

    if(ProcFD == tcpfd)
    {
        while(1){
        int infd = AcceptSocket();
        if(infd == -1) break;

        int status = _proc->Proc_EPOLLNEW(infd);
        if(status != 0)
        {
            LOG(ERROR) << "ProcEvent Proc_EPOLLNEW False.";
            close(infd);
        }
        }
    }
    else if(ProcLG & EPOLLIN)
    {
        LOG(INFO) << "ProcEvent EPOLLIN.";
        int status = _proc->Proc_EPOLLIN(ProcFD);
        if(status != 0)
        {
            LOG(ERROR) << "ProcEvent EPOLLIN False.";
            close(ProcFD);
        }
    }
    else if(ProcLG & EPOLLOUT)
    {
        LOG(INFO) << "ProcEvent EPOLLOUT.";
        _proc->Proc_EPOLLOUT(ProcFD);
    }
    else if((ProcLG & EPOLLERR) || (ProcLG & EPOLLHUP))
    {
        LOG(INFO) << "ProcEvent EPOLLERR.";
        _proc->Proc_EPOLLERR(ProcFD);
    }
    else
    {
        LOG(ERROR) << "ProcEvent NotFound.";
    }
    }//ProcEvent//////////////////

    return 0;
}

int TcpListener::Init(const char* port)
{
    int status;

    tcpfd = Set_SOCKET(port);
    CHECK(tcpfd != -1) << "Set_SOCKET ERROR.";
    
    status = Set_NONBLOCK(tcpfd);
    CHECK(status != -1) << "Set_NONBLOCK ERROR.";

    status = ::listen(tcpfd, SOMAXCONN);
    CHECK(status != -1) << "Listen ERROR.";
    
    struct epoll_event event;
    epollfd = epoll_create1(0);
    CHECK(status != -1) << "epoll_create1 ERROR.";
    
    event.data.fd = tcpfd;
    event.events = EPOLLIN | EPOLLET;
    status = epoll_ctl (epollfd, EPOLL_CTL_ADD, tcpfd, &event);
    CHECK(status != -1) << "epoll_ctl ERROR.";
    
    return 0;
}

int TcpListener::Listen(const char* port)
{
    Init(port);
    LOG(INFO) << "TCP Listen Port:" << port;

    epoll_event events[MAXEVENTS];
    while(1)
    {
        int num = epoll_wait(epollfd, events, MAXEVENTS, 100);

        ProcEvent(num, events);
    }
}

///////////////////////////////////////////////