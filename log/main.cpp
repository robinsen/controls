#include <iostream>
#include "log.h"
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
    //test
    int m_close_log=0;
    Log::get_instance()->init("./ServerLog", 0, 2000, 800000, 0);
    LOG_INFO("%s", "adjust timer once");
    system("pause");

    return 0;
}
