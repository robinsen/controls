#pragma once
#include "platform.h"

typedef struct ClientEntity_t {
    int socket;
    int port;
    std::string ip;
    std::string recv;
    std::string write;
} ClientEntity;
