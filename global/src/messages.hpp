#pragma once

#include <string>
#include <array>

#include "message_types.hpp"

class LobbyConnectInfo 
{
public:
    static constexpr short MAX_LOBBY_NAME_SIZE = 16;
    static constexpr short MAX_LOBBY_PASSWORD_SIZE = 16;
    static constexpr short MAX_CLIENT_NAME_SIZE = 16;

    LobbyConnectInfo(const std::string& lobby, const std::string& name, const std::string& password)
    {
        lobby.copy(this->lobby.data(), MAX_LOBBY_NAME_SIZE);
        this->lobby[MAX_LOBBY_NAME_SIZE] = '\0';

        password.copy(this->password.data(), MAX_LOBBY_PASSWORD_SIZE);
        this->password[MAX_LOBBY_PASSWORD_SIZE] = '\0';

        name.copy(this->name.data(), MAX_CLIENT_NAME_SIZE);
        this->name[MAX_CLIENT_NAME_SIZE] = '\0';
    }
    LobbyConnectInfo() = default;
    ~LobbyConnectInfo() noexcept = default;

    std::string GetLobbyName() { return lobby.data(); }
    std::string GetPassword() { return password.data(); }
    std::string GetPlayerName() { return name.data(); }

private:
    std::array<char, MAX_LOBBY_NAME_SIZE + 1> lobby = {};
    std::array<char, MAX_LOBBY_PASSWORD_SIZE + 1> password = {};
    std::array<char, MAX_CLIENT_NAME_SIZE + 1> name = {};
};

class LobbyInfo
{
public:
    static constexpr short MAX_PLAYERS_PER_LOBBY = 8; // Currently fixed for all lobbies

    LobbyInfo(const std::string& name, int playerCount, std::string password) 
        : playerCount(playerCount) 
    {
        name.copy(this->name.data(), LobbyConnectInfo::MAX_LOBBY_NAME_SIZE);
        this->name[LobbyConnectInfo::MAX_LOBBY_NAME_SIZE] = '\0';

        if (password.empty()){
            this->password = false;
        } else {
            this->password = true;
        }
    }
    LobbyInfo() = default;
    ~LobbyInfo() noexcept = default;

    int GetPlayerCount() { return playerCount; }
    std::string GetLobbyName() { return name.data(); }
    bool hasPassword() { return password; }

private:
    std::array<char, LobbyConnectInfo::MAX_LOBBY_NAME_SIZE + 1> name = {};
    short playerCount = 0;
    bool password = false;
};

class LobbyInfoList
{
public:
    static constexpr short MAX_LOBBIES_PER_PAGE = 16;

    LobbyInfoList() = default;
    ~LobbyInfoList() noexcept = default;

    void AddLobbyInfo(LobbyInfo info) { list[size++] = info; }
    LobbyInfo& GetLobbyInfo(int index) { return list[index]; }
    int GetSize() const { return size; }

private:
    short size = 0;
    std::array<LobbyInfo, MAX_LOBBIES_PER_PAGE> list = {};
};