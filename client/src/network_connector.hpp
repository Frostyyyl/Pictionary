#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../../global/src/objects.hpp"

class NetworkConnector
{
private:
    NetworkConnector();

    bool isInit;
    int serverSocket;
public:

    static NetworkConnector &getInstance();
    ~NetworkConnector();

    bool isInitialized() { return isInit; }
    void Init(int port, std::string address);
    void Exit();
    LobbyInfoList RequestLobbies();
    bool ValidateData(const std::string& lobby, const std::string& name, const std::string& password);
    bool CreateLobby(const std::string& lobby, const std::string& name, const std::string& password);
    bool ConnectToLobby(const std::string& lobby, const std::string& name, const std::string& password);

    void HandleCanvasChange(SDL_Texture *tex)
    {
        // Here could be network canvas handling
        std::cout << "Network got updated texture" << std::endl;
    }

    void HandleNewMessage(std::string nickname, std::string message)
    {
        // Here could be network messages handling
        std::cout << "Network got new message from: " << nickname
                  << "; content: " << message << std::endl;
    }
};