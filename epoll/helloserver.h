#pragma once
#include <list>
#include <thread>
#include <memory>
#include <map>
#include <iostream>
#include "define.h"

class CHelloServer {
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
  CHelloServer();
  ~CHelloServer();

  static bool initSocket();
  static void unInitSocket();

  bool start();
  void startNoSelect();
  void startSelect();
  void startEpoll();
  void stop();

  void setServerAddr(const std::string& ip,unsigned short nPort, std::list<unsigned short>& portList);

  bool sendData(char* pdata,int len);

private:
  int  createSocket();
  bool setBlock(const int& fd, bool isblock);
  void readAndResponseSocket(const int& fd);
  void closeClient(const int& socket);
  void closeSocket(const int& fd);
  void addEpollfd(const int& efd, const int& fd);


private:
  #ifdef WIN32
    SOCKET      m_socket=INVALID_SOCKET;
  #else
    int         m_socket = -1;
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

  typedef std::shared_ptr<ClientEntity> ClientEntityPtr;
  std::map<int, ClientEntityPtr> m_clientMap;
  int m_close_log=0;
};
