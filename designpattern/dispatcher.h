#pragma once
#include <iostream>
#include <memory>
#include <functional>
#include <map>
#include <cassert>
#include <list>

namespace Com{
class Message;
typedef std::shared_ptr<Message> MessagePtr;

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}
template<typename T>
inline T* get_pointer(const std::shared_ptr<T>& ptr)
{
  return ptr.get();
}
template<typename To, typename From>
inline ::std::shared_ptr<To> down_pointer_cast(const ::std::shared_ptr<From>& f) {
  if (false)
  {
    implicit_cast<From*, To*>(0);
  }

#ifndef NDEBUG
  assert(f == NULL || dynamic_cast<To*>(get_pointer(f)) != NULL);
#endif
  return ::std::static_pointer_cast<To>(f);
}

class Message {
public:
    ~Message() = default;
    static int msgType() { return 0; }
    virtual int getType() { return 0; }
    virtual void onResponse() = 0;
};

class LoginMessage:public Message  {
public:
    static int msgType() { return 1; }
    virtual int getType() { return 1; }
    virtual void onResponse() {
        std::cout<<" login msg  "<<std::endl;
    }
};

class ChatMessage:public Message {
public:
    static int msgType() { return 2; }
    virtual int getType() { return 2; }
    virtual void onResponse() {
        std::cout<<" chat msg  "<<std::endl;
    }
};

class CallBack {
public:
    virtual ~CallBack() = default;
    virtual void onMessage(const MessagePtr& message,
                           int timestamp) const = 0;
};

//T型回调模板   核心就是通过这个模板完成自动类型转换
template<typename T>
class CallbackT: public CallBack {
static_assert(std::is_base_of<Message, T>::value,
              "T must be derived from gpb::Message.");
public:
//根据T型生成对应的回调函数类型   这个很重要
typedef std::function<void (const std::shared_ptr<T>& message,
                            int timestamp)> MessageTCallback;

CallbackT(const MessageTCallback& callback)
     :callback_(callback)
{

}

void onMessage(const MessagePtr& message,
               int timestamp) const override
{
    //对消息进行向下转换成自己的类型  不同的消息对应不同的回调
    std::shared_ptr<T> concrete = down_pointer_cast<T>(message);
    assert(concrete != NULL);
    callback_(concrete, timestamp);
}

private:
   MessageTCallback callback_;

};

//消息接收和分发
class MessageDispatcher
{
 public:
  typedef std::function<void (const MessagePtr& message,
                              int timestamp)> MessageCallback;

  explicit MessageDispatcher(const MessageCallback& defaultCb)
     : defaultCallback_(defaultCb)
  {
 
  }

  //收到消息后直接传入，不需要先转成Message类型，onMessage中会进行转换
  void onProtobufMessage(const MessagePtr& message,
                         int receiveTime) const
  {
    CallbackMap::const_iterator it = callbacks_.find(message->getType());
    if (it != callbacks_.end())
    {
        for (auto item: it->second)
          item->onMessage(message, receiveTime);
    }
    else
    {
        defaultCallback_(message, receiveTime);
    }
  }

//通过函数模板注册   每一种类型的消息可能注册了多个回调
//自动推导消息类型生成对应的 回调对象   在回调对象中会生成对应类型的回调函数  
  template<typename T>
  void registerMessageCallback(const typename CallbackT<T>::MessageTCallback& callback)
  {
     std::shared_ptr<CallbackT<T> > pd(new CallbackT<T>(callback));
     callbacks_[T::msgType()].emplace_back(pd);
  }

 private:
  typedef std::map<const int, std::list<std::shared_ptr<CallBack>>> CallbackMap;

  CallbackMap callbacks_;
  MessageCallback defaultCallback_;
};

}
