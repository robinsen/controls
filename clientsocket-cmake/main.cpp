#include <iostream>
#include "clientsocket.h"
using namespace std;

class test {
    public:
    void connectCallBack(const int& state, const std::string& msg)
    {
        m_iconnect = state;
        std::cout<<"state ="<<state<<"  msg="<<msg<<std::endl;
    }
    void receiveCallBack(const char* buf, const int& len)
    {
        string rc = string(buf);
        std::cout<<"receive ="<<rc<<"  len="<<len<<std::endl;
    }
    int getState();
    private:
        int m_iconnect=0;
};

int main()
{
    string ip = "10.0.2.123";
    unsigned short port = 8080;
    list<unsigned short> portlist;
    portlist.push_back(port);

    CClientSocket::initSocket();

    test t;
    CClientSocket sc(std::bind(&test::connectCallBack, &t, std::placeholders::_1, std::placeholders::_2),
                    std::bind(&test::receiveCallBack, &t, std::placeholders::_1, std::placeholders::_2));
    sc.setServerAddr(ip, port, portlist);
    sc.start();

    system("pause");

    CClientSocket::unInitSocket();
    return 0;
}
