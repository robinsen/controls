#pragma once
#include <list>
#include <thread>
#include <memory>
#include <map>
#include <iostream>

class CProcessUser {
public:
  CProcessUser();
  ~CProcessUser();

  int initSocket();

  void init(int efd, int fd, int pid, struct sockaddr_in& connect);
  void process(const uint32_t& events);

private:
  void readSocket(const int& fd);
  void writeSocket(const int& fd);
  void registerEvents(uint32_t events);
  void clear();
private:
  int m_listenfd = -1;

  int m_efd = -1;
  int m_fd = -1;
  int m_pid = -1;
  struct sockaddr_in m_address;

  uint16_t     m_port = 0;
  std::string  m_ip;

  std::string  m_readbuf;
  std::string  m_writebuf;

};
