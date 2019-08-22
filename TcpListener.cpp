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

void TcpListener::SetProc(TcpProc *proc)
{
    _proc = proc;
}

int make_socket_non_blocking (int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        LOG(ERROR) << "fcntl F_GETFL.";
        return -1;
    }
    flags |= O_NONBLOCK;

    int set = fcntl(fd, F_SETFL, flags);
    if (set == -1)
    {
        LOG(ERROR) << "fcntl F_SETFL";
        return -1;
    }
    
    return 0;
}


int create_and_bind(int port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;
    char Tport[8];
    sprintf(Tport, "%d", port);
    
    memset (&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */
    
    s = getaddrinfo (NULL, Tport, &hints, &result);
    if (s != 0)
    {
        LOG(ERROR) << "getaddrinfo: " << gai_strerror(s);
        return -1;
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;
        
        int flag = 1;
        if( setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == 0)
        {
            LOG(INFO) << "create_and_bind set resuse succ";
            s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
            if (s == 0)
            {
                LOG(INFO) << "create_and_bind bind succ";
                /* We managed to bind successfully! */
                break;
            }
            else LOG(ERROR) << "create_and_bind bind fail " << errno;
        }
        else
            LOG(ERROR) << "create_and_bind set resuse fail " << errno;
        
        
        close (sfd);
    }
    
    if (rp == NULL)
    {
        LOG(ERROR) << "Could not bind";
        return -1;
    }
    
    freeaddrinfo (result);
    
    return sfd;
}

int ReadNetData(int fd, TcpConnect* data)
{
    char* buf = (char*)data->ptr + data->dataSize;
    int bufSize = data->bufferSize - data->dataSize;
    
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
                data->dataSize += offset;
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

int AcceptConnection(int lfd, int efd, int& infd)
{
    struct sockaddr in_addr;
    socklen_t in_len;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    struct epoll_event event;
    
    infd = -1;
    
    in_len = sizeof(in_addr);
    infd = accept (lfd, &in_addr, &in_len);
    if (infd == -1)
    {
        if ((errno == EAGAIN) ||
            (errno == EWOULDBLOCK) ||
            errno == ECONNABORTED || errno == EPROTO || errno != EINTR)
        {
            /* We have processed all incoming
             connections. */
            return -1;
        }
        else
        {
            LOG(ERROR) << "accept";
            return -1;
        }
    }
    
    /* Make the incoming socket non-blocking and add it to the
     list of fds to monitor. */
    int s = make_socket_non_blocking (infd);
    if (s == -1)
    {
        close(infd);
        infd = -1;
        return 0;
    }
    
    LOG(INFO) << "AcceptConnection Accept connection fd: " << infd;
    
    event.data.fd = infd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);
    if (s == -1)
    {
        LOG(WARNING) << "epoll_ctl";
        infd = -1;
        return 0;
    }
    
    return 0;
}

int TcpListener::NetInit(int port)
{
    int s;
    struct epoll_event event;
    
    //listen port
    tcpfd = create_and_bind (port);
    if (tcpfd == -1) return -1;
    
    s = make_socket_non_blocking (tcpfd);
    if (s == -1)
    {
        close(tcpfd);
        return -1;
    }
    
    s = listen(tcpfd, SOMAXCONN);
    if (s == -1)
    {
        perror ("listen");
        close(tcpfd);
        return -1;
    }
    
    //epoll
    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        LOG(ERROR) << "epoll_create";
        close(tcpfd);
        return -1;
    }
    
    event.data.fd = tcpfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl (epollfd, EPOLL_CTL_ADD, tcpfd, &event);
    if (s == -1)
    {
        LOG(ERROR) << "epoll_ctl";
        close(epollfd);
        close(tcpfd);
        return -1;
    }
    
    return 0;
}

void TcpListener::CloseConnection(TcpConnect* conn)
{
    close(conn->fd);

    g_FD2Conn.erase(conn->fd);
    g_Seq2Conn.erase(conn->id);
    delete conn;
}

void TcpListener::HandleInputEvent(TcpConnect* conn, uint& eventflag)
{
    if (eventflag & EPOLLIN)
    {
        int ret = ReadNetData(conn->fd, conn);
        if (ret == -1 || ret == 1) //drop connection when read error or query too long(treat as attack)
        {
            LOG(WARNING) << "HandleInputEvent Read Failed close " << conn->fd << "  " << conn->id;
            CloseConnection(conn);
            return;
        }

        if(_proc != NULL)
        {
            _proc->ProcInput(conn->id, conn->ptr, conn->dataSize);
        }

        //fprintf(stderr, "===============\n");
        //fprintf(stderr, "%s\n", conn->ptr);
        //fprintf(stderr, "===============\n");
        
        conn->dataSize = 0;
    }

    if (eventflag & EPOLLOUT) 
    {
        printf("EPOLLOUT\n");
    }
}

void TcpListener::ProcessInput(int lfd, int efd, int n, struct epoll_event *events)
{
    int retcode = -1;
    
    for (int i = 0; i < n; i++)
	{
        int eventFd = events[i].data.fd;
        uint eventflag = events[i].events;
        
        if ((eventflag & EPOLLERR) || (eventflag & EPOLLHUP))
        {
            printf("close matchFd %d when conn error\n", eventFd);
            if (g_FD2Conn.count(eventFd) != 0)
            {
                CloseConnection(g_FD2Conn[eventFd]);
            }
            else
            {
                close(eventFd); //should never happend, must be bug
            }

	    }
        else if (lfd == eventFd)
	    {
            printf("lfd == eventFd\n");
            while (1)
            {
                int infd = -1;
                retcode = AcceptConnection(lfd, efd, infd);
                if (retcode != 0)
                {
                    printf("AcceptConnection retcode != 0\n");
                    break;
                }
                
                if (infd != -1)
                {
                    printf("SetupConnection\n");
                    if (SetupConnection(infd) == false)
                    {
                        close(infd);
                    }
                }
            }
        }
        else
        {
            printf("lfd != eventFd\n");
            if (g_FD2Conn.count(eventFd) != 0)
            {
                fprintf(stderr, "HandleInputEvent\n");
                HandleInputEvent(g_FD2Conn[eventFd], eventflag);
            }
            else
            {
                fprintf(stderr, "HandleInputEvent bug ..\n");
                close(eventFd); //should never happen, must be bug.
            }
        }
    }
}

int TcpListener::Listen(int port)
{
    LOG(INFO) << "TCP Listen " << port;
    int retcode = NetInit(port);
    
    epoll_event events[MAXEVENTS];
    while(1)
    {
        int n = epoll_wait (epollfd, events, MAXEVENTS, 1000);
        ProcessInput(tcpfd, epollfd, n, events);
        //ProcessOutput(epollfd, g_WriteList, g_PendingOutputs);

        //LOG(INFO) << g_Seq2Conn.size();
    }
}