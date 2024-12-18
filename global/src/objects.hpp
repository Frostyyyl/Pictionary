#pragma once

#include <vector>
#include "message.hpp"

class LobbyInfo
{
public:
    static const short MAX_LOBBY_SIZE = 8;
    static const short MAX_LOBBY_NAME_SIZE = 32;

    char name[MAX_LOBBY_NAME_SIZE];
    short playerCount;
    bool hasPassword;
};

class MessageLobbiesList
{
public:
    static const int MAX_LOBBIES = 16;
    std::vector<LobbyInfo> lobbies;

    MessageLobbiesList() { lobbies.reserve(MAX_LOBBIES); }
    ~MessageLobbiesList() {}

    static int getSize()
    { 
        int size = sizeof(MessageLobbiesList);
        size += sizeof(std::vector<LobbyInfo>);
        size += MAX_LOBBIES * sizeof(LobbyInfo);

        return size; 
    }
    static int getMessageType() { return static_cast<int>(MessageToClient::UPLOAD_LOBBIES); }
};
