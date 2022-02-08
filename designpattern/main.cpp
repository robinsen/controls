#include <iostream>
#include <memory>
#include <cstring>
#include <vector>
#include <functional>
#include "graphics.h"
#include "graphicsimpl.h"
#include "drawstuff.h"
#include "dispatcher.h"
#include "responsibilitychain.h"
#include "strategy.h"

using namespace std;
using namespace DrawStuff;
using namespace Com;

typedef std::shared_ptr<Com::ChatMessage> ChatMessagePtr;
typedef std::shared_ptr<Com::LoginMessage> LoginMessagePtr;

//两种回调方式
class TestDispatch {
public:
    void onLogin( const LoginMessagePtr& msg,
                  int time)
    {
        msg->onResponse();
    }
    void onChat(const ChatMessagePtr& msg,
                int time)
    {
        msg->onResponse();
    }
    void onUnKonown(const MessagePtr& message,
                    int time)
    {
        std::cout<<" unkonwn msg  "<<std::endl;
    }
};

void onLoginMsg( const LoginMessagePtr& msg,
                  int time)
{
    std::cout<<" on login msg  "<<std::endl;
    msg->onResponse();
}

void onChatMsg(const ChatMessagePtr& msg,
                int time)
{
    std::cout<<" on chat msg  "<<std::endl;
    msg->onResponse();
}

void onUnKonwnMsgType(const MessagePtr& msg,
                      int time)
{
    std::cout<<" unkonwn msg type  "<<std::endl;
}

int main()
{
    //test swap
    {
        Graphics g1;
        Graphics g2;
        using std::swap;
        swap(g1, g2);
    }
    {
        TestTempl<Draw> test;
        TestTempl<Draw> test1;
        test.draw(1,2);
        using std::swap;
        swap(test,test1);
    }

    //消息分发
    {
        TestDispatch testDispatch;
        MessageDispatcher dispatch(std::bind(&TestDispatch::onUnKonown, &testDispatch, std::placeholders::_1, std::placeholders::_2));
        dispatch.registerMessageCallback<ChatMessage>(std::bind(&TestDispatch::onChat, &testDispatch, std::placeholders::_1, std::placeholders::_2));
        dispatch.registerMessageCallback<LoginMessage>(std::bind(&TestDispatch::onLogin, &testDispatch, std::placeholders::_1, std::placeholders::_2));
        dispatch.registerMessageCallback<ChatMessage>(onChatMsg);
        dispatch.registerMessageCallback<LoginMessage>(onLoginMsg);

        std::shared_ptr<ChatMessage> chat(new ChatMessage);
        std::shared_ptr<LoginMessage> login(new LoginMessage);
        dispatch.onProtobufMessage(chat, 11);
        dispatch.onProtobufMessage(login, 22);
    }

    //设计模式
    {
        //行为模式
        testStrategy();
        testResponsibilityChain();
    }
    system("pause");

    return 0;
}
