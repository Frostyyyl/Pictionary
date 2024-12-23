#pragma once

#include <string>
#include <array>
#include <vector>
#include "message.hpp"

class LobbyInfo
{
public:
    static constexpr short MAX_PLAYERS_PER_LOBBY = 8; // Currently fixed for all lobbies

    LobbyInfo(const std::string& name, short playerCount, bool hasPassword) 
        : name(name), playerCount(playerCount), password(hasPassword) {}
    LobbyInfo() = default;
    ~LobbyInfo() noexcept = default;

    short getPlayerCount() const { return playerCount;}
    std::string getLobbyName() const { return name;}
    bool hasPassword() const { return password;}

private:
    std::string name = "";
    short playerCount = 0;
    bool password = false;
};

class LobbyInfoList
{
public:
    static constexpr short MAX_LOBBIES_PER_PAGE = 16;

    LobbyInfoList() = default;
    ~LobbyInfoList() noexcept = default;

    bool addLobbyInfo(LobbyInfo info) 
    { 
        if (list.size() < MAX_LOBBIES_PER_PAGE){
            list.push_back(info);

            return true;
        }
        return false;
    }
    LobbyInfo getLobbyInfo(int index) const
    { 
        return list[index];
    }
    int getSize() const { return list.size(); }

private:
    std::vector<LobbyInfo> list = {};
};

class LobbyConnectInfo 
{
public:
    static constexpr short MAX_LOBBY_NAME_SIZE = 32;
    static constexpr short MAX_LOBBY_PASSWORD_SIZE = 64;
    static constexpr short MAX_CLIENT_NAME_SIZE = 32;

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

    std::string getLobbyName() const { return lobby.data(); }
    std::string getPassword() const { return password.data(); }
    std::string getPlayerName() const { return name.data(); }

private:
    std::array<char, MAX_LOBBY_NAME_SIZE + 1> lobby = {};
    std::array<char, MAX_LOBBY_PASSWORD_SIZE + 1> password = {};
    std::array<char, MAX_CLIENT_NAME_SIZE + 1> name = {};
};
