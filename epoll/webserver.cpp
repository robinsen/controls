#include <iostream>
#include "define.h"
#include "webserver.h"
#include "locker.h"
#include "http_conn.h"
#include "threadpool.h"
#include "processpoolserver.h"
#include "util.h"
#include "timerwheel.h"

#define TIMEOUT  5
#define TIMESLOT 1
#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000

using namespace std;

int http_conn::m_user_count = 0;
int http_conn::m_epollfd = -1;

void show_error( int connfd, const char* info )
{
    printf( "%s", info );
    send( connfd, info, strlen( info ), 0 );
    close( connfd );
}

void timer_handler(time_wheel* pw)
{
    if (pw)
        pw->tick();
    alarm( TIMESLOT );
}

void cb_func( client_data* user_data )
{
    epoll_ctl( user_data->epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0 );
    assert( user_data );
    close( user_data->sockfd );
    printf( "close fd %d\n", user_data->sockfd );
}

CWebServer::CWebServer(uint32_t port, const char* ip):
    m_port(port),
    m_ip(ip)
{
    std::cout<<" server ip = "<<std::string(m_ip)<<std::endl;
    m_pTwPtr.reset(new time_wheel());
}

CWebServer::~CWebServer()
{
  
}

int CWebServer::run()
{
    addsig( SIGPIPE, SIG_IGN );

    threadpool< http_conn >* pool = NULL;
    try
    {
        pool = new threadpool< http_conn >;
    }
    catch( ... )
    {
        return 1;
    }

    http_conn* users = new http_conn[ MAX_FD ];
    assert( users );
    int user_count = 0;

    int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( listenfd >= 0 );
    struct linger tmp = { 1, 0 };
    setsockopt( listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof( tmp ) );

    int ret = 0;
    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, m_ip, &address.sin_addr );
    address.sin_port = htons( m_port );

    ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    assert( ret >= 0 );

    ret = listen( listenfd, 5 );
    assert( ret >= 0 );

    epoll_event events[ MAX_EVENT_NUMBER ];
    int epollfd = epoll_create( 5 );
    assert( epollfd != -1 );
    addfd( epollfd, listenfd, false);
    http_conn::m_epollfd = epollfd;

    //添加管道   
    ret = socketpair( PF_UNIX, SOCK_STREAM, 0, sig_pipefd );
    assert( ret != -1 );
    setnonblocking( sig_pipefd[1] );
    addfd( epollfd, sig_pipefd[0] );

    // add all the interesting signals here
    addsig( SIGALRM, sig_handler);
    addsig( SIGTERM, sig_handler);
    bool stop_server = false;

    bool timeout = false;
    alarm( TIMESLOT );

    while( !stop_server )
    {
        int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
        if ( ( number < 0 ) && ( errno != EINTR ) )
        {
            printf( "epoll failure\n" );
            break;
        }

        for ( int i = 0; i < number; i++ )
        {
            int sockfd = events[i].data.fd;
            if( sockfd == listenfd )
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                if ( connfd < 0 )
                {
                    printf( "errno is: %d\n", errno );
                    continue;
                }
                if( http_conn::m_user_count >= MAX_FD )
                {
                    show_error( connfd, "Internal server busy" );
                    continue;
                }
                std::cout<<" new connection fd = "<<connfd<<std::endl;
                users[connfd].init( connfd, client_address );
                //添加到定時中  5秒沒收到數據就斷開這個連接
                tw_timer* pTimer = m_pTwPtr->add_timer(TIMEOUT);
                pTimer->user_data = new client_data;
                pTimer->user_data->epollfd = epollfd;
                pTimer->user_data->sockfd = connfd;
                pTimer->user_data->timer = pTimer;
                pTimer->cb_func = cb_func;
                users[connfd].setTimer(pTimer);
            }
            else if( ( sockfd == sig_pipefd[0] ) && ( events[i].events & EPOLLIN ) )
            {
                int sig;
                char signals[1024];
                ret = recv( sig_pipefd[0], signals, sizeof( signals ), 0 );
                if( ret == -1 )
                {
                    // handle the error
                    continue;
                }
                else if( ret == 0 )
                {
                    continue;
                }
                else
                {
                    for( int i = 0; i < ret; ++i )
                    {
                        switch( signals[i] )
                        {
                            case SIGALRM:
                            {
                                timeout = true;
                                break;
                            }
                            case SIGTERM:
                            {
                                stop_server = true;
                            }
                        }
                    }
                }
            }
            else if( events[i].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) )
            {
                users[sockfd].close_conn();
            }
            else if( events[i].events & EPOLLIN )
            {
                if( users[sockfd].read() )
                {
                    pool->append( users + sockfd );
                    tw_timer* p = users[sockfd].getTimer();
                    if (p)
                    {
                        m_pTwPtr->adjust_timer(TIMEOUT, p);
                    }
                }
                else
                {
                    tw_timer* p = users[sockfd].getTimer();
                    if (p)
                    {
                        cb_func(p->user_data);
                        m_pTwPtr->del_timer(p);
                        users[sockfd].setTimer(nullptr);
                    }
                    users[sockfd].close_conn();
                }
            }
            else if( events[i].events & EPOLLOUT )
            {
                if( !users[sockfd].write() )
                {
                    tw_timer* p = users[sockfd].getTimer();
                    if (p)
                    {
                        cb_func(p->user_data);
                        m_pTwPtr->del_timer(p);
                        users[sockfd].setTimer(nullptr);
                    }
                    users[sockfd].close_conn();
                }
            }
            else
            {

            }
            //定時時間到  時間輪移動一格
            if (timeout)
            {
                timer_handler(m_pTwPtr.get());
                timeout = false;
            }
        }
    }

    close( epollfd );
    close( listenfd );
    delete [] users;
    delete pool;
}

void CWebServer::addInTimerWheel(const int& fd, const struct sockaddr_in& address)
{
    
}