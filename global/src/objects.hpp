#pragma once

#include <vector>
#include <string>
#include "message.hpp"

class LobbyInfo
{
public:
    static const short MAX_LOBBY_SIZE = 8;
    static const short MAX_LOBBY_NAME_SIZE = 32;

    std::string name;
    short playerCount;
    bool hasPassword;

    LobbyInfo(std::string name, short playerCount, bool hasPassword)
    {
        this->name = name;
        this->playerCount = playerCount;
        this->hasPassword = hasPassword;
    }
    ~LobbyInfo() {}
};

class LobbiesList
{
public:
    static const int MAX_LOBBIES_PER_PAGE = 16;
    std::vector<LobbyInfo> lobbies;

    LobbiesList() {}
    ~LobbiesList() {}
};

class LobbyCreateInfo
{
public:
    static const short MAX_LOBBY_NAME_SIZE = LobbyInfo::MAX_LOBBY_NAME_SIZE;
    std::string name;
    std::string password;
};