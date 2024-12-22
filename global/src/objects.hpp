#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <array>
#include "message.hpp"

class LobbyInfo
{
public:
    static constexpr short MAX_PLAYERS_PER_LOBBY = 8; // Currently fixed for all lobbies
    static constexpr short MAX_LOBBY_NAME_SIZE = 32;
    static constexpr short MAX_LOBBY_PASSWORD_SIZE = 64;

    std::array<char, MAX_LOBBY_NAME_SIZE> name = {};
    short playerCount = 0;
    bool hasPassword = false;


    LobbyInfo(const std::string& name, short playerCount, bool hasPassword) 
        : playerCount(playerCount), hasPassword(hasPassword) 
    {
        name.copy(this->name.data(), MAX_LOBBY_NAME_SIZE);
    }
    LobbyInfo() = default;
    ~LobbyInfo() noexcept = default;
};

class LobbiesList
{
public:
    static constexpr short MAX_PLAYERS_PER_LOBBY = LobbyInfo::MAX_PLAYERS_PER_LOBBY;
    static constexpr int MAX_LOBBIES_PER_PAGE = 16;
    std::array<LobbyInfo, MAX_LOBBIES_PER_PAGE> lobbies = {};
    int size = 0;


    LobbiesList() = default;
    ~LobbiesList() noexcept = default;
};

class LobbyCreateInfo {
public:
    static constexpr short MAX_LOBBY_NAME_SIZE = LobbyInfo::MAX_LOBBY_NAME_SIZE;
    static constexpr short MAX_LOBBY_PASSWORD_SIZE = LobbyInfo::MAX_LOBBY_PASSWORD_SIZE;

    std::array<char, MAX_LOBBY_NAME_SIZE> name = {};
    std::array<char, MAX_LOBBY_PASSWORD_SIZE> password = {};


    LobbyCreateInfo(const std::string& name, const std::string& password) 
    {
        name.copy(this->name.data(), MAX_LOBBY_NAME_SIZE);
        password.copy(this->password.data(), MAX_LOBBY_PASSWORD_SIZE);
    }
    LobbyCreateInfo() = default;
    ~LobbyCreateInfo() noexcept = default;
};