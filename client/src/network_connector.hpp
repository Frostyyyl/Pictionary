#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "../../global/src/message_types.hpp"
#include "../../global/src/messages.hpp"

class NetworkConnector
{
private:
    NetworkConnector();

    bool isInit;
    int mySocket;
    std::string errorMessage;

    const int PORT = 1100;
    const std::string ADDRESS = "127.0.0.1";

public:
    static NetworkConnector &getInstance();
    ~NetworkConnector();

    bool isInitialized() { return isInit; }
    void Init();
    void Exit();
    std::string GetError();
    LobbyInfoList RequestLobbies();
    bool ValidateData(const std::string &lobby, const std::string &name, const std::string &password);
    bool CreateLobby(const std::string &lobby, const std::string &name, const std::string &password);
    bool ConnectToLobby(const std::string &lobby, const std::string &name, const std::string &password);
    PlayerInfoList RequestPlayers();

    void HandleCanvasChange(SDL_Texture *tex)
    {
        // Here could be network canvas handling
        // std::cout << "Network got updated texture" << std::endl;
    }

    void HandleNewMessage(std::string message)
    {
        // Here could be network messages handling
        // std::cout << "Network got new message from: " << nickname
        //          << "; content: " << message << std::endl;
    }
};
