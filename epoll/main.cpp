#include <iostream>
#include <exception>
#include "clientsocket.h"
#include "processuser.h"
#include "processpoolserver.h"
#include "helloserver.h"
#include "webserver.h"
#include "testutil.h"

class test
{
public:
    void connectCallBack(const int &state, const std::string &msg)
    {
        m_iconnect = state;
        std::cout << "state =" << state << "  msg=" << msg << std::endl;
    }
    void receiveCallBack(const char *buf, const int &len)
    {
        string rc = string(buf);
        std::cout << "receive =" << rc << "  len=" << len << std::endl;
    }
    int getState();

private:
    int m_iconnect = 0;
};

static void testClient()
{
    //客户端
    string ip = "127.0.0.1";
    unsigned short port = 9191;
    list<unsigned short> portlist;
    portlist.push_back(port);

    CClientSocket::initSocket();

    test t;
    CClientSocket sc(std::bind(&test::connectCallBack, &t, std::placeholders::_1, std::placeholders::_2),
                     std::bind(&test::receiveCallBack, &t, std::placeholders::_1, std::placeholders::_2));
    sc.setServerAddr(ip, port, portlist);
    sc.start();

    const char *exit = "exit";
    std::string cmd;
    while (1)
    {
        std::getline(std::cin, cmd);
        if (cmd.compare(exit) == 0)
        {
            sc.sendData(cmd.c_str(), cmd.length());
            #ifdef WIN32
            Sleep(1000);
            #else
            sleep(1);
            #endif
            break;
        }
        sc.sendData(cmd.c_str(), cmd.length());
    }
    CClientSocket::unInitSocket();
}

static void testLoaclClient()
{
    const char *ip = "127.0.0.1";
    unsigned short port = 9191;
    CClientSocket::initSocket();
    int m_socket = socket(AF_INET, SOCK_STREAM, 0); //直接创建socket返回给XTcp的成员函数
    sockaddr_in saddr;                              //设置连接对象的结构体
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);                                      //字符串转整型
    if (::connect(m_socket, (sockaddr *)&saddr, sizeof(saddr)) == -1) //再次连接一次进行确认
        std::cout << " client connect error " << std::endl;
    while (1)
    {
        char buf[1024] = {};
        int strLen = recv(m_socket, buf, sizeof(buf), 0);
        if (strLen > 0)
            std::cout << " recv msg = " << buf << std::endl;
        else
            std::cout << " recv error" << std::endl;
        std::string cmd;
        std::getline(std::cin, cmd);
        if (cmd.compare("exit") == 0)
        {
            char msg[] = "exit";
            send(m_socket, msg, sizeof(msg), 0);
            break;
        }
        send(m_socket, cmd.c_str(), cmd.size(), 0);
    }
#ifdef WIN32
    closesocket(m_socket);
#else
    close(m_socket);
#endif
    CClientSocket::unInitSocket();
}

int main(int argc, char *argv[])
{
    int m_close_log=0;
    //這邊的實現環境都是本地通信
    Log::get_instance()->init("./ServerLog", 0, 2000, 800000, 0);
    if (argc > 1)
    {
        int i = atoi(argv[1]);
        if (i == 0) //通用客戶端測試select和epoll模式
        {
            testClient();
        }
        else if (i == 1) //實現簡單的select和epoll模式，收到消息後直接發回給客戶端
        {
            CHelloServer hs;
        }
        else if (i == 2) //進程池實現的epoll模式
        {
            CProcessUser user;
            int lfd = user.initSocket();
            processpool<CProcessUser>* process = processpool<CProcessUser>::create(lfd);
            process->run();
        }
        else if (i == 3) //測試weberver
        {
            CWebServer webserver(9191, "127.0.0.1");
            webserver.run();
        }
        else if (i == 4) //測試webserver的客戶端
        {
            testWebServer(5, 9191, "127.0.0.1");
        }
    }
    return 0;
}
