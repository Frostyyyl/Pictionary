#pragma once
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h> 
#include <unistd.h>
#include "lobby.hpp"
#include "../../global/src/objects.hpp"

class Server
{
private:
    Server();
    int SetNonBlocking(int socket);
    void SendLobbiesList(int socket);
    void Accept();
    void Read(int socket);
    void Write(int socket);
    
    const int MAX_QUEUE_SIZE = 10;
    const int BUFFER_SIZE = 1024;
    socklen_t size = sizeof(struct sockaddr_in);
    struct timeval timeout;
    struct sockaddr_in serverAddr;
    int serverSocket;
    int maxSocket;
    fd_set descriptors; 
    fd_set reading; 
    fd_set writing;

    std::map<std::string, Lobby> lobbies;
public:
    static Server &getInstance();
    ~Server();
    
    void Init(int port);
    void Run();
};
