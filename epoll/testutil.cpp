#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "util.h"
#include "testutil.h"

static const char* request = "GET http://localhost/index.html HTTP/1.1\r\nConnection: keep-alive\r\n\r\nxxxxxxxxxxxx";

bool write_nbytes( int sockfd, const char* buffer, int len )
{
    int bytes_write = 0;
    printf( "write out %d bytes to socket %d\n", len, sockfd );
    while( 1 ) 
    {   
        bytes_write = send( sockfd, buffer, len, 0 );
        if ( bytes_write == -1 )
        {   
            return false;
        }   
        else if ( bytes_write == 0 ) 
        {   
            return false;
        }   

        len -= bytes_write;
        buffer = buffer + bytes_write;
        if ( len <= 0 ) 
        {   
            return true;
        }   
    }   
}

bool read_once( int sockfd, char* buffer, int len )
{
    int bytes_read = 0;
    memset( buffer, '\0', len );
    bytes_read = recv( sockfd, buffer, len, 0 );
    if ( bytes_read == -1 )
    {
        return false;
    }
    else if ( bytes_read == 0 )
    {
        return false;
    }
	printf( "read in %d bytes from socket %d with content: %s\n", bytes_read, sockfd, buffer );

    return true;
}

void start_conn( int epoll_fd, int num, const char* ip, int port )
{
    int ret = 0;
    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    for ( int i = 0; i < num; ++i )
    {
        usleep( 1000 );
        int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
        printf( "create 1 sock\n" );
        if( sockfd < 0 )
        {
            continue;
        }

        if (  connect( sockfd, ( struct sockaddr* )&address, sizeof( address ) ) == 0  )
        {
            printf( "build connection %d\n", i );
            addfd( epoll_fd, sockfd, false, false);
        }
    }
}

void close_conn( int epoll_fd, int sockfd )
{
    epoll_ctl( epoll_fd, EPOLL_CTL_DEL, sockfd, 0 );
    close( sockfd );
}

int testWebServer(int num, uint32_t port, const char* ip)
{
    int epoll_fd = epoll_create( 100 );
    start_conn( epoll_fd, num, ip, port);
    epoll_event events[ 10000 ];
    char buffer[ 2048 ];
    while ( 1 )
    {
        int fds = epoll_wait( epoll_fd, events, 10000, 2);
        for ( int i = 0; i < fds; i++ )
        {   
            int sockfd = events[i].data.fd;
            if ( events[i].events & EPOLLIN )
            {   
                if ( ! read_once( sockfd, buffer, 2048 ) )
                {
                    close_conn( epoll_fd, sockfd );
                }
                struct epoll_event event;
                event.events = EPOLLOUT | EPOLLET | EPOLLERR;
                event.data.fd = sockfd;
                epoll_ctl( epoll_fd, EPOLL_CTL_MOD, sockfd, &event );
            }
            else if( events[i].events & EPOLLOUT ) 
            {
                if ( ! write_nbytes( sockfd, request, strlen( request ) ) )
                {
                    close_conn( epoll_fd, sockfd );
                }
                struct epoll_event event;
                event.events = EPOLLIN | EPOLLET | EPOLLERR;
                event.data.fd = sockfd;
                epoll_ctl( epoll_fd, EPOLL_CTL_MOD, sockfd, &event );
            }
            else if( events[i].events & EPOLLERR )
            {
                close_conn( epoll_fd, sockfd );
            }
        }
    }
}
