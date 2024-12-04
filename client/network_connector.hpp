#pragma once

#include <SDL2/SDL.h>
#include <iostream>

// Here I think we should handle all networking
class NetworkConnector
{
private:
    NetworkConnector(/* args */) {}
    /* data */
public:
    static NetworkConnector &getInstance()
    {
        static NetworkConnector INSTANCE;
        return INSTANCE;
    }
    ~NetworkConnector() {}

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