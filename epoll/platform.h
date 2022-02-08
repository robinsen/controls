#pragma once
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/eventfd.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/epoll.h>
#include <sys/syscall.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/wait.h>
#endif
#include "log.h"