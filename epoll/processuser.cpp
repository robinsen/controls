#include <iostream>
#include "define.h"
#include "processuser.h"

using namespace std;

CProcessUser::CProcessUser()
{
    
}

CProcessUser::~CProcessUser()
{
  
}

int CProcessUser::initSocket()
{
    m_listenfd = socket(PF_INET, SOCK_STREAM, 0); //直接创建socket返回给XTcp的成员函数
    if (m_listenfd == -1)
    {
        std::cout << "create tcp socket failed.\n";
        return -1;
    }
    else
    {
        std::cout << "create tcp socket successed "<<m_listenfd<<std::endl;
    }
    // bind
    struct sockaddr_in localAddress;
    localAddress.sin_family = AF_INET;                //地址族
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY); //轉換爲網絡字節序
    localAddress.sin_port = htons(9191);
    int ret = bind(m_listenfd, (struct sockaddr *)&localAddress, sizeof(localAddress));
    if (ret != 0)
    {
        return -1;
    }
    ret = listen(m_listenfd, 8);
    if (ret != 0)
    {
        return -1;
    }
    return m_listenfd;
}

void CProcessUser::init(int efd, int fd, int pid, struct sockaddr_in& connect)
{
    m_efd = efd;
    m_fd = fd;
    m_pid = pid;
    memset(&m_address, 0x0, sizeof(sockaddr_in));
    memcpy(&m_address, &connect, sizeof(sockaddr_in));

    m_port = ntohs(connect.sin_port);
    m_ip = std::string(inet_ntoa(connect.sin_addr));
}

void CProcessUser::process(const uint32_t& events)
{
    if (events&EPOLLIN)
    {
        readSocket(m_fd);
    }
    else if (events&EPOLLOUT)
    {
        writeSocket(m_fd);
    }
    else
    {

    }
}

void CProcessUser::readSocket(const int& fd)
{
    char szRecv[4096] = {};
    int recvLen = read(fd, szRecv, sizeof(szRecv));
    if (recvLen > 0)
    {
        m_readbuf += std::string(szRecv);
        std::string respMsg = " recv from  " + m_ip + " : " + std::string(szRecv);
        std::cout <<respMsg << std::endl;
        if (string(szRecv).compare("exit") == 0)
        {
           ::shutdown(m_fd, SHUT_WR);
           std::cout<<" client exit shutdowm write = "<<m_ip<<std::endl;
           return;
        }
        //注冊寫事件
        m_writebuf += m_readbuf;
        m_readbuf.clear();
        registerEvents(EPOLLIN|EPOLLOUT);
    }
    else if (recvLen == 0)
    {
        std::cout<<" client disconnect "<<m_ip<<std::endl;
        clear();
    }
}

void CProcessUser::writeSocket(const int& fd)
{
    std::string msg = "resp from " + m_ip + " " + m_writebuf;
    int sendLen = write(fd,msg.c_str(), msg.size());
    if (0 >= sendLen)
    {
        std::cout<<" send error ip= "<<m_ip<<std::endl;
    }else
    {
        std::cout<<" len = "<<sendLen<<std::endl;
        m_writebuf.clear();
    }
    registerEvents(EPOLLIN);
}

void CProcessUser::registerEvents(uint32_t events)
{
     //注冊寫事件
    struct epoll_event ev; //ev用于注册事件
    ev.data.fd = m_fd;
    ev.events = events; //监听读状态同时设置ET模式
    epoll_ctl(m_efd, EPOLL_CTL_MOD, m_fd, &ev); //注册epoll事件
}

void CProcessUser::clear()
{
    m_listenfd = -1;
    m_efd = -1;
    m_fd = -1;
    m_pid = -1;

    m_port = 0;
    m_ip.clear();
    m_readbuf.clear();
    m_writebuf.clear();

    memset(&m_address, 0x0, sizeof(sockaddr_in));
}