#pragma once

#include <netinet/in.h>

class Client
{
public:
    Client(struct sockaddr_in address) 
    {
        this->address = address;
    }
    ~Client() {}

    struct sockaddr_in address;
    int lobby;
};