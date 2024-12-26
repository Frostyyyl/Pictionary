#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

#include "lobby.hpp"
#include "client.hpp"
#include "../../global/src/messages.hpp"

class Server
{
private:
    Server();
    int SetNonBlocking(int socket);
    void SendLobbyList(int socket);
    void SendPlayerList(int socket);
    void CreateLobby(int socket, int message_size);
    void ConnectToLobby(int socket, int message_size);
    void Read(int socket);
    void Write(int socket);
    void ExitLobby(int socket);
    void EnterLobby(int socket, const std::string &lobby, const std::string &name);
    void Disconnect(int socket);
    void Accept();

    const int MAX_QUEUE_SIZE = 10;
    socklen_t size = sizeof(struct sockaddr_in);
    sockaddr_in serverAddr;
    int serverSocket;
    int maxSocket;
    timeval timeout;
    fd_set descriptors;
    fd_set reading;
    fd_set writing;

    LobbyManager lobbies;
    ClientManager clients;

public:
    static Server &getInstance();
    ~Server();

    void Init(int port);
    void Run();
};
