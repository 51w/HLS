#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include "TcpListener.h"
#include "TcpConnect.h"
#include "TcpBuff.h"
#include "common.h"

#define MAXEVENTS 64

TcpListener::TcpListener()
{

}

TcpListener::~TcpListener()
{

}

int make_socket_non_blocking (int sfd)
{
    int flags, s;
    
    flags = fcntl (sfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror ("fcntl");
        return -1;
    }
    
    flags |= O_NONBLOCK;
    s = fcntl (sfd, F_SETFL, flags);
    if (s == -1)
    {
        perror ("fcntl");
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
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
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
            Logd("create_and_bind set resuse succ");
            s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
            if (s == 0)
            {
                Logd("create_and_bind bind succ");
                /* We managed to bind successfully! */
                break;
            }
            else Loge("create_and_bind bind fail %d", errno);
        }
        else
            Loge("create_and_bind set resuse fail %d", errno);
        
        
        close (sfd);
    }
    
    if (rp == NULL)
    {
        fprintf (stderr, "Could not bind\n");
        return -1;
    }
    
    freeaddrinfo (result);
    
    return sfd;
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
        perror ("epoll_create");
        close(tcpfd);
        return -1;
    }
    
    event.data.fd = tcpfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl (epollfd, EPOLL_CTL_ADD, tcpfd, &event);
    if (s == -1)
    {
        perror ("epoll_ctl");
        close(epollfd);
        close(tcpfd);
        return -1;
    }
    
    return 0;
}

void ProcessInput(int lfd, int efd, int n, struct epoll_event *events)
{
    int retcode = -1;
    
    for (int i = 0; i < n; i++)
	{
        int eventFd = events[i].data.fd;
        uint32_t eventflag = events[i].events;
        
        if ((eventflag & EPOLLERR) || (eventflag & EPOLLHUP))
        {
            printf("close matchFd %d when conn error\n", eventFd);

	    }
        else if (lfd == eventFd)
	    {
            printf("lfd == eventFd\n");
        }
        else
        {
            printf("lfd != eventFd\n");
        }
    }
}

int TcpListener::Listen(int port)
{
    int retcode = NetInit(port);
    
    epoll_event events[MAXEVENTS];
    while(1)
    {
        int n = epoll_wait (epollfd, events, MAXEVENTS, 100);
        ProcessInput(tcpfd, epollfd, n, events);
        //ProcessOutput(epollfd, g_WriteList, g_PendingOutputs);
    }
}