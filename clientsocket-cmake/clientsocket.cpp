#include <iostream>
#include "clientsocket.h"

using namespace std;

CClientSocket::CClientSocket(const ClientSocketCallBack& cb, const ClientSocketReciveCallBack& rcb):
m_connectCb(cb),
m_reciveCb(rcb)
{

}

CClientSocket::~CClientSocket()
{
    if (!m_bStop)
        stop();
}

bool CClientSocket::initSocket()
{
#ifdef WIN32
    int nErrCode = -1;
    WSADATA wsaData;
    WORD wVersion=MAKEWORD(2,2);
    nErrCode=WSAStartup(wVersion,&wsaData);
    if(nErrCode!=0)
    {
        return false;
    }
    if(LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
    {

        WSACleanup();
        return false;
    }
#else
#endif
}

void CClientSocket::unInitSocket()
{
#ifdef WIN32
    WSACleanup();
#else
#endif
}

bool CClientSocket::start()
{
    if(m_serverIp.empty() || m_port == 0)
        return false;
    if(createSocket() == -1)
        return false;
    m_bStop = false;
    m_state = NetworkState::Connecting;
    if (m_thread.get())
    {
        if (m_thread->joinable())
            m_thread->join();
    }
    m_thread.reset(new std::thread(std::bind(&threadRun, this)));
    return true;
}

void CClientSocket::stop()
{
    m_bStop = true;
    {
        lock_guard<mutex> locker(m_mutex);
        m_condition.notify_all();
    }

    m_state = NetworkState::Unconnected;
    {
        lock_guard<mutex> locker(m_mutex);
        closesocket(m_socket);
        m_socket = -1;
    }
    if (m_thread.get())
    {
        if (m_thread->joinable())
            m_thread->join();
    }
    m_thread.reset();
}

void CClientSocket::setServerAddr(const std::string& ip,unsigned short nPort, std::list<unsigned short>& portList)
{
    m_serverIp = ip;
    m_port = nPort;
    m_portList = portList;
}

int CClientSocket::createSocket()
{
     //创建socket （TCP/IP协议 TCP）
    {
        lock_guard<mutex> locker(m_mutex);
        if(m_socket > 0)
        {
            closesocket(m_socket);
            m_socket = -1;
        }
        m_socket = socket(AF_INET, SOCK_STREAM, 0);    //直接创建socket返回给XTcp的成员函数
    }
    if (m_socket == -1)
    {
        cout<<"create tcp socket failed.\n";
        return -1;
    }
    else
    {
        cout<<"create tcp socket successed.\n";
        return m_socket;
    }
}

bool CClientSocket::setBlock(bool isblock)  //设置阻塞模式  （希望只有在connect的时候是非阻塞的，而接收数据时候是阻塞的）
{
    if (m_socket <= 0)
    {
        cout<<"set tcp socket block failed.\n";
        return false;
    }
#ifdef WIN32
    unsigned long ul = 0;
    if (!isblock) ul = 1;
    ioctlsocket(m_socket, FIONBIO, &ul);    //设置socket的模式(0 阻塞模式,1 非阻塞模式<connect就立即返回>)
#else
    int flags = fcntl(m_socket, F_GETFL, 0);  //获取socket的属性
    if (flags < 0)return false; //获取属性出错
    if (isblock)
    {
        flags = flags&~O_NONBLOCK;  //把非阻塞这位设为0
    }
    else
    {
        flags = flags | O_NONBLOCK; //把非阻塞这位设为1
    }
    if (fcntl(m_socket, F_SETFL, flags))return false;  //把标准位设回去
#endif

    if (isblock==0)
        cout<<"set tcp socket not block success.\n";
    if (isblock==1)
        cout<<"set tcp socket block success.\n";

    return true;
}

bool CClientSocket::connectToServer(const char *ip, unsigned short port , int sec)
{
    if (m_socket <= 0)
    {
        cout<<"ConnectToServer FAIL,m_socket <= 0\n";
        if(createSocket() == -1)
            return false;
    }

    sockaddr_in saddr;   //设置连接对象的结构体
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);  //字符串转整型

    setBlock(false);    //将socket改成非阻塞模式，此时它会立即返回  所以通过fd_set
    fd_set rfds, wfds;	    //文件句柄数组，在这个数组中，存放当前每个文件句柄的状态

    if (::connect(m_socket, (sockaddr*)&saddr, sizeof(saddr)) != 0)   //此时connect马上返回，状态为未成功连接
    {
        FD_ZERO(&rfds);  //首先把文件句柄的数组置空
        FD_ZERO(&wfds);
        FD_SET(m_socket, &rfds);   //把sock的网络句柄加入到该句柄数组中
        FD_SET(m_socket, &wfds);

        timeval tm;  //超时参数的结构体
        tm.tv_sec = sec;
        tm.tv_usec = 0;
        //（阻塞函数）(监听的文件句柄的最大值加1，可读序列文件列表，可写的序列文件列表，错误处理，超时)使用select监听文件序列set是否有可读可写，
        //这里监听set数组（里面只有sock），只要其中的句柄有一个变得可写（在这里是sock连接成功了以后就会变得可写，就返回），
        int selres = select(m_socket + 1, &rfds, &wfds, NULL, &tm);
        switch (selres)
        {
            case -1:
                cout<<"select error\n";
                return false;
            case 0:
                cout<<"select time out\n";
                return false;
            default:
                if (FD_ISSET(m_socket, &rfds) || FD_ISSET(m_socket, &wfds))
                {
                    ::connect(m_socket, (sockaddr*)&saddr, sizeof(saddr));    //再次连接一次进行确认
    #ifdef WIN32
                    m_iLastError = ::GetLastError();
                    if  ( m_iLastError == WSAEISCONN ||m_iLastError == WSAEINPROGRESS)     //已经连接到该套接字 或 套接字为非阻塞套接字，且连接请求没有立即完成

    #else
                    m_iLastError = errno;
                    if  ( m_iLastError == EISCONN||m_iLastError == EINPROGRESS)     //已经连接到该套接字 或 套接字为非阻塞套接字，且连接请求没有立即完成

    #endif
                    {
                        cout<<"connect= "<<ip<<" : port = "<<port<<"finished(success).\n";
                        setBlock(true);   //成功之后重新把sock改成阻塞模式，以便后面发送/接收数据
                        return true;
                    }
                    else
                    {
                        cout<<"connect= "<<ip<<" : port = "<<port<<"finished(failed)."<<" errno="<<errno<<endl;
                        // printf("FD_ISSET(sock_fd, &rfds): %d\n FD_ISSET(sock_fd, &wfds): %d\n", FD_ISSET(sock_fd, &rfds) , FD_ISSET(sock_fd, &wfds));
                        return false;
                    }
                }
                else
                {
                    cout<<"connect= "<<ip<<" : port = "<<port<<"finished(failed).\n";
                    return false;
                }
        }
    }
    else  //连接正常
    {
        setBlock(true);   //成功之后重新把sock改成阻塞模式，以便后面发送/接收数据
        cout<<"connect= "<<ip<<" : port = "<<port<<"finished(success).\n";
        return true;
    }
}

void CClientSocket::threadRun()
{
    m_nTryCount = 0;
    m_connectIp = "";
    m_iLastError = 0;
    char dataBuf[1024]={0};

    while(!m_bStop)
    {
        struct hostent* ht = gethostbyname(m_serverIp.c_str());
        if(ht && ht->h_addr_list)
        {
            char** pptr =  ht->h_addr_list;
            for(;*pptr != NULL && !m_bStop;pptr++)
            {
                list<unsigned short> portList = m_portList;
                do
                {
                    if(portList.size() > 0)
                      portList.remove(m_port);
                    std::string ip =  inet_ntoa( *((struct in_addr*)*pptr));
                    m_connectIp = ip;
                    // if(m_nTryCount > 1) //第一次尝试不显示
                    //     sendConnectStatePacket(NetworkState::Connecting);
                    m_bConnect = connectToServer(ip.c_str(),m_port,10);
                    if(m_bConnect)
                    {
                        m_nTryCount = 0;
                        m_state = NetworkState::Connected;
                        if (m_connectCb)
                            m_connectCb(m_state, "connect succ\n");
                        while(!m_bStop)
                        {
                            int datalen = recv(m_socket,dataBuf,1024,0);
                            if(datalen <= 0)
                            {
    #ifdef WIN32
                                m_iLastError = ::GetLastError();
    #else
                                m_iLastError = errno;
    #endif
                                cout<<"get lasterror:"<<m_iLastError<<endl;
                                m_bConnect = false;
                                closesocket(m_socket);
                                m_socket = -1;
                                m_state = NetworkState::Unconnected;
                                 if (m_connectCb)
                                    m_connectCb(m_state, "disconnect\n");
                                break;
                            }
                            else
                            {
                                if(m_reciveCb)
                                    m_reciveCb(dataBuf,datalen);
                            }
                        }
                        m_bConnect = false;
                    }
                    else
                    {
                         if (m_connectCb)
                            m_connectCb(NetworkState::Unconnected, "connect error TCP_ERROR_UNSOLVE_IP\n");
                    }
                    if(portList.size() > 0)
                    {
                        m_port = portList.front();
                    }
                }while(portList.size() > 0 && !m_bStop);
            }//end for
        }
        else
        {
            string msg = "connect error unsole ip\n";
            if (m_connectCb)
                m_connectCb(NetworkState::Unconnected, msg);
        }
        if(m_bAutoReconnect && !m_bStop)
        {
            {
                unique_lock<mutex> locker(m_mutex);
                if(m_bStop)
                {
                    break;
                }
                auto start = std::chrono::steady_clock::now();
                m_condition.wait_until(locker,start + std::chrono::seconds(3));
            }
            m_nTryCount++;
            continue;
        }
        else
            break;
    }
    if(m_socket > 0) //连接成功后，有可能主线程之间停止该线程
    {
        {
            lock_guard<mutex> locker(m_mutex);
            closesocket(m_socket);
            m_socket = -1;
        }
    }
    m_bConnect = false;
    m_state = NetworkState::Unconnected;
    cout<<"client socket runloop exit\n";
}

 bool CClientSocket::sendData(char* pdata,int len)
 {
     if(!m_bConnect || m_socket == -1)
         return false;
    int ret = send(m_socket,pdata,len,0);
    return (ret > 0);
 }
