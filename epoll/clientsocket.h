#pragma once
#include <list>
#include <functional>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include "platform.h"

typedef std::function<void(const int&,const std::string&)> ClientSocketCallBack;
typedef std::function<void(const char*,const int& len)> ClientSocketReciveCallBack;

class CClientSocket {
  enum NetworkState {
    UnknownSate = 0,
    Connected = 1,
    Unconnected,
    HostLookup,
    Connecting,
    Bound,
    Listening,
    Closing,
    Disconnect,
    ManualDisconnect,
    WaitConnect,
};
public:
  CClientSocket(const ClientSocketCallBack& cb, const ClientSocketReciveCallBack& rcb);
  ~CClientSocket();

  static bool initSocket();
  static void unInitSocket();

  bool start();
  void stop();
  void setServerAddr(const std::string& ip,unsigned short nPort, std::list<unsigned short>& portList);
  bool sendData(const char* pdata,int len);

  void threadRun();
  static void threadCallBack(void* pData, CClientSocket* pThis);
private:
  int  createSocket();
  bool setBlock(bool isblock);
  bool connectToServer(const char *ip, unsigned short port , int sec);
  void closeSocket();

private:
  #ifdef WIN32
    SOCKET      m_socket=INVALID_SOCKET;
  #else
    int         m_socket=0;
  #endif

  int                       m_nTryCount = 0;
  bool                      m_bStop=false;
  bool                      m_bConnect=false;
  NetworkState              m_state=Unconnected;
  int                       m_iLastError = 0;
  bool                      m_bAutoReconnect=true;

  unsigned short            m_port;
  std::string               m_serverIp;
  std::string               m_connectIp;
  std::list<unsigned short> m_portList;

  std::mutex                   m_mutex;
  std::condition_variable      m_condition;
  std::shared_ptr<std::thread> m_thread;

  ClientSocketCallBack         m_connectCb;
  ClientSocketReciveCallBack   m_reciveCb;
};
