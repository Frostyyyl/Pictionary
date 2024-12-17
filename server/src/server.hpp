#pragma once
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h> 
#include <unistd.h>
#include "client.hpp"

class Server
{
private:
    Server();
    int SetNonBlocking(int socket);
    void Accept();
    void Read(int socket);
    void Write(int socket);
    
    const int MAX_QUEUE_SIZE = 10;
    const int BUFFER_SIZE = 1024;
    socklen_t SIZE = sizeof(struct sockaddr_in);
    struct timeval timeout;
    std::map<int, Client> clients;
    struct sockaddr_in serverAddr;
    int serverSocket;
    int maxSocket;
    fd_set descriptors; 
    fd_set reading; 
    fd_set writing;
public:
    static Server &getInstance();
    ~Server();
    
    void Init(int port);
    void Run();
};
