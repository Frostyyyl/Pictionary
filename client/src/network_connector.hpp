#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

class NetworkConnector
{
private:
    NetworkConnector();

    int lobbySocket; 
    int serverSocket;
public:
    static NetworkConnector &getInstance();
    ~NetworkConnector();

    bool Init();
    void Exit();
    bool Connect(std::string address, int port);
    void Disconnect();

    void HandleCanvasChange(SDL_Texture *tex)
    {
        // Here could be network canvas handling
        std::cout << "Network got updated texture" << std::endl;
    };

    void HandleNewMessage(std::string nickname, std::string message)
    {
        // Here could be network messages handling
        std::cout << "Network got new message from: " << nickname
                  << "; content: " << message << std::endl;
    }
};