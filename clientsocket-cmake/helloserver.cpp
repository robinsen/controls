#include <iostream>
#include "helloserver.h"

using namespace std;

CHelloServer::CHelloServer()
{
    CHelloServer::initSocket();
    start();
    // startNoSelect();
    startSelect();
}

CHelloServer::~CHelloServer()
{
    if (!m_bStop)
        stop();
    m_clientMap.erase(m_clientMap.begin(), m_clientMap.end());
    m_clientMap.clear();
    CHelloServer::unInitSocket();
}

bool CHelloServer::initSocket()
{
#ifdef WIN32
    int nErrCode = -1;
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2, 2);
    nErrCode = WSAStartup(wVersion, &wsaData);
    if (nErrCode != 0)
    {
        return false;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        return false;
    }
#else
#endif
    return true;
}

void CHelloServer::unInitSocket()
{
#ifdef WIN32
    WSACleanup();
#else

#endif
}

bool CHelloServer::start()
{
    if (createSocket() == -1)
        return false;
    m_bStop = false;
    m_state = NetworkState::Connecting;
    // bind
    struct sockaddr_in localAddress;
    localAddress.sin_family = AF_INET;                //地址族
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY); //
    localAddress.sin_port = htons(9191);
    int ret = bind(m_socket, (struct sockaddr *)&localAddress, sizeof(localAddress));
    if (ret == SOCKET_ERROR)
    {
        std::cout << " bind ip = " << m_serverIp << " port= " << m_port << " failed " << std::endl;
        return false;
    }
    ret = listen(m_socket, 5);
    if (ret != 0)
    {
        std::cout << " listen ip = " << m_serverIp << " port= " << m_port << " failed " << std::endl;
        return false;
    }

    return true;
}

void CHelloServer::startNoSelect()
{
    SOCKADDR_IN clientAddr = {};
    int nAddrLen = sizeof(SOCKADDR_IN);
    SOCKET ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(m_socket, (SOCKADDR *)&clientAddr, &nAddrLen);
    if (ClientSocket)
    {
        std::cout << " client connect ip=" << inet_ntoa(clientAddr.sin_addr) << " port = " << ntohs(clientAddr.sin_port) << std::endl;
    }
    const char msg[] = "client connect sucess";
    int ret = send(ClientSocket, msg, sizeof(msg), 0);
    if (ret)
        std::cout << "send msg " << std::endl;
    else
        std::cout << " send error " << std::endl;
    char recvbuf[1024];
    while (1)
    {
        memset(recvbuf, 0x0, sizeof(recvbuf));
        int recvLen = recv(ClientSocket, recvbuf, sizeof(recvbuf) - 1, 0);
        if (recvLen > 0)
        {
            std::cout << " recv = " << recvbuf << std::endl;
            if (string(recvbuf).compare("exit") == 0)
                break;
        }
        else
            std::cout << " recv error " << std::endl;
        Sleep(1000);
        string str(recvbuf);
        str = " server send back = " + str;
        ret = send(ClientSocket, str.c_str(), str.size(), 0);
        if (ret)
        {
            std::cout << "send msg " << std::endl;
        }
        else
            std::cout << " send error " << std::endl;
    }
    closesocket(ClientSocket);
    closesocket(m_socket);
    CHelloServer::unInitSocket();
}

void CHelloServer::startSelect()
{
    //设置非阻塞模式
    unsigned long ul = 0;
    ioctlsocket(m_socket, FIONBIO, &ul); //设置socket的模式(0 阻塞模式,1 非阻塞模式<connect就立即返回>)

    std::cout << " start select " << std::endl;
    fd_set rfds, wfds;
    std::list<int> readlist;
    std::list<int> writelist;
    while (1)
    {
        if (m_bStop)
            break;
        FD_ZERO(&rfds); //首先把文件句柄的数组置空
        FD_ZERO(&wfds);
        FD_SET(m_socket, &rfds); //把sock的网络句柄加入到该句柄数组中
        FD_SET(m_socket, &wfds);
        int maxSocket = m_socket;
        readlist.push_back(m_socket);
        writelist.push_back(m_socket);
        for (const auto &client : m_clientMap)
        {
            int clientSocket = client.second->socket;
            FD_SET(clientSocket, &rfds);
            FD_SET(clientSocket, &wfds);
            maxSocket = maxSocket > clientSocket ? maxSocket : clientSocket;
            readlist.push_back(clientSocket);
            writelist.push_back(clientSocket);
        }

        timeval tm; //超时参数的结构体
        tm.tv_sec = 0;
        tm.tv_usec = 100;
        int selres = select(maxSocket + 1, &rfds, &wfds, NULL, &tm);
        if (selres > 0)
        {
            //是否有新连接
            if (FD_ISSET(m_socket, &rfds))
            {
                --selres;
                SOCKADDR_IN clientAddr = {};
                int nAddrLen = sizeof(SOCKADDR_IN);
                SOCKET ClientSocket = INVALID_SOCKET;
                ClientSocket = accept(m_socket, (SOCKADDR *)&clientAddr, &nAddrLen);
                if (INVALID_SOCKET == ClientSocket)
                {
                    std::cout << "accept() failed" << std::endl;
                    break;
                }
                else
                {
                    ClientEntity *pEntity = new ClientEntity;
                    pEntity->socket = ClientSocket;
                    pEntity->port = ntohs(clientAddr.sin_port);
                    pEntity->ip = inet_ntoa(clientAddr.sin_addr);
                    m_clientMap.insert(std::pair<int, ClientEntity *>(ClientSocket, pEntity));
                    unsigned long ul = 1;
                    ioctlsocket(ClientSocket, FIONBIO, &ul);
                    // 客户端连接成功，则显示客户端连接的IP地址和端口号
                    std::cout << "new client connect, ip:" << inet_ntoa(clientAddr.sin_addr) << "  port:" << ntohs(clientAddr.sin_port) << std::endl;
                }
            }
            ///有数据要读取的连接
            if (selres > 0)
            {
                for (int socket : readlist)
                {
                    if (FD_ISSET(socket, &rfds))
                    {
                        --selres;
                        char szRecv[4096] = {};
                        int recvLen = recv(socket, szRecv, sizeof(szRecv), 0);
                        if (recvLen > 0)
                        {
                            auto ite = m_clientMap.find(socket);
                            ite->second->recv += std::string(szRecv);
                            std::string respMsg = " recv from  " + ite->second->ip + " : " + std::string(szRecv);
                            std::cout <<respMsg << std::endl;
                            // send(socket, respMsg.c_str(), respMsg.size(), 0);
                            if (string(szRecv).compare("exit") == 0)
                            {
                                closeClient(socket);
                            }
                        }
                        else if (0 == recvLen)
                        {
                            closeClient(socket);
                        }
                    }
                }
            }
            //可写的连接
            if (selres > 0)
            {
                for (int socket: writelist)
                {
                    auto ite = m_clientMap.find(socket);
                    if (ite != m_clientMap.end())
                    {
                        if (ite->second->recv.size() > 0)
                        {
                            int sendLen = send(socket, ite->second->recv.c_str(), ite->second->recv.size(), 0);
                            if (0 >= sendLen)
                                std::cout<<" send error ip= "<<ite->second->ip<<std::endl;
                            else
                                ite->second->recv.clear();
                        }
                    }
                }
            }
        }
    }
}

void CHelloServer::stop()
{
    m_bStop = true;
}

void CHelloServer::setServerAddr(const std::string &ip, unsigned short nPort, std::list<unsigned short> &portList)
{
    m_serverIp = ip;
    m_port = nPort;
    m_portList = portList;
}

int CHelloServer::createSocket()
{
    //创建socket （TCP/IP协议 TCP）
    if (m_socket > 0)
    {
        closesocket(m_socket);
        m_socket = -1;
    }
    m_socket = socket(PF_INET, SOCK_STREAM, 0); //直接创建socket返回给XTcp的成员函数
    if (m_socket == -1)
    {
        cout << "create tcp socket failed.\n";
        return -1;
    }
    else
    {
        cout << "create tcp socket successed.\n";
        return m_socket;
    }
}

bool CHelloServer::setBlock(bool isblock) //设置阻塞模式  （希望只有在connect的时候是非阻塞的，而接收数据时候是阻塞的）
{
    if (m_socket <= 0)
    {
        cout << "set tcp socket block failed.\n";
        return false;
    }
#ifdef WIN32
    unsigned long ul = 0;
    if (!isblock)
        ul = 1;
    ioctlsocket(m_socket, FIONBIO, &ul); //设置socket的模式(0 阻塞模式,1 非阻塞模式<connect就立即返回>)
#else
    int flags = fcntl(m_socket, F_GETFL, 0); //获取socket的属性
    if (flags < 0)
        return false; //获取属性出错
    if (isblock)
    {
        flags = flags & ~O_NONBLOCK; //把非阻塞这位设为0
    }
    else
    {
        flags = flags | O_NONBLOCK; //把非阻塞这位设为1
    }
    if (fcntl(m_socket, F_SETFL, flags))
        return false; //把标准位设回去
#endif

    if (isblock == 0)
        cout << "set tcp socket not block success.\n";
    if (isblock == 1)
        cout << "set tcp socket block success.\n";

    return true;
}

void CHelloServer::closeClient(const int &socket)
{
    std::cout << "client exit socket = " << socket << std::endl;
    closesocket(socket);
    auto client = m_clientMap.find(socket);
    if (client != m_clientMap.end())
        m_clientMap.erase(client);
}