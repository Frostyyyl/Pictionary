#pragma once

#include <string>
#include <vector>

class Lobby
{
public:
    std::string password;
    std::vector<int> clients;

    Lobby(std::string password, int client) 
    {
        this->password = password;
        this->clients.push_back(client);
    }
    ~Lobby() {}
};