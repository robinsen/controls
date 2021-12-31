#pragma once

#ifdef WIN32
#include <winsock2.h>
#else

#endif
#include <list>
#include <thread>
#include <memory>
#include <map>
#include <iostream>

typedef struct ClientEntity_t {
    int socket;
    int port;
    std::string ip;
    std::string recv;
    std::string write;
} ClientEntity;

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
  void stop();

  void setServerAddr(const std::string& ip,unsigned short nPort, std::list<unsigned short>& portList);

  bool sendData(char* pdata,int len);

private:
  int  createSocket();
  bool setBlock(bool isblock);
  void closeClient(const int& socket);

private:
  #ifdef WIN32
    SOCKET      m_socket=INVALID_SOCKET;
  #else

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

  std::map<int, ClientEntity*> m_clientMap;

};
