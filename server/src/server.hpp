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
#include "../../global/src/message_types.hpp"

class Server
{
private:
    Server();
    int SetNonBlocking(int socket);
    template <typename T>
    void DisplaySendError(const T &message);
    template <typename T>
    void DisplayRecvError(const T &message);
    void SendIncorrectLobbyName(int socket);
    void SendIncorrectPlayerName(int socket);
    void SendIncorrectPassword(int socket);
    void ConfirmConnect(int socket);
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

    const int MAX_PENDING_CONNECTIONS = 10;
    const int PORT = 1100;
    bool isInit = false;
    socklen_t size = sizeof(struct sockaddr_in);
    sockaddr_in serverAddr = {};
    int serverSocket = {};
    int maxSocket = {};
    timeval timeout = {};

    fd_set reading_list;
    fd_set writing_list;
    fd_set exception_list;

    LobbyManager lobbies = {};
    ClientManager clients = {};

public:
    static Server &getInstance();
    ~Server();

    void Init();
    void Run();
    void Exit();

    static void Exit(int signal)
    {
        Server::getInstance().Exit();
    }
};
