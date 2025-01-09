#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

#include "prompts_manager.hpp"
#include "lobby.hpp"
#include "client.hpp"
#include "../../global/src/messages.hpp"
#include "../../global/src/message_types.hpp"

class Server
{
private:
    Server();

    int SetNonBlocking(int socket);
    void Accept();
    void ExitLobby(int socket);
    void EnterLobby(int socket, const std::string &lobby, const std::string &name);
    void Disconnect(int socket);

    void SendIncorrectLobbyName(int socket);
    void SendIncorrectPlayerName(int socket);
    void SendIncorrectPassword(int socket);
    void SendMaxLobbiesReached(int socket);
    void SendConfirmConnect(int socket);
    void SendLobbyList(int socket);
    void SendPlayerList(int socket);
    void SendGameMode(int socket);
    void SendChat(int socket);
    void SendCanvasChanges(int socket);
    void SendTime(int socket);
    void SendPrompts(int socket);

    void UpdateChat(int socket, int message_size);
    void UpdateCanvas(int socket, int message_size);
    void UpdatePrompt(int socket, int message_size);
    void CreateLobby(int socket, int message_size);
    void ConnectToLobby(int socket, int message_size);
    
    template <typename T>
    bool ReadWithRetry(int socket, void *buffer, size_t size, const T& messageType);
    template <typename T>
    bool WriteWithRetry(int socket, const void *buffer, size_t size, const T& messageType);
    void Read(int socket);

    const int MAX_RETRIES = 3;
    const int MAX_PENDING_CONNECTIONS = 10;
    const int PORT = 1100;
    socklen_t clientAddrSize = sizeof(struct sockaddr_in);
    sockaddr_in serverAddr = {};
    int serverSocket = {};
    int maxSocket = {};
    timeval timeout = {};

    fd_set reading_list;
    fd_set writing_list;
    fd_set exception_list;

    bool isInit = false;

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
