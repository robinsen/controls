#pragma once
#include <list>
#include <thread>
#include <memory>
#include <map>
#include <iostream>

class time_wheel;

class CWebServer {
public:
  CWebServer(uint32_t port, const char* ip);
  ~CWebServer();

  int run();

private:
  void addInTimerWheel(const int& fd, const struct sockaddr_in& address);

private:
  uint16_t     m_port = 0;
  const char*  m_ip;

  typedef shared_ptr<time_wheel> time_wheelPtr;
  time_wheelPtr m_pTwPtr;
};
