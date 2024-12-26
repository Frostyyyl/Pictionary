#pragma once

#include <string>
#include <array>

class ConnectInfo
{
public:
    static constexpr short MAX_LOBBY_NAME_SIZE = 16;
    static constexpr short MAX_LOBBY_PASSWORD_SIZE = 16;
    static constexpr short MAX_CLIENT_NAME_SIZE = 16;

    ConnectInfo(const std::string &lobby, const std::string &name, const std::string &password)
    {
        lobby.copy(this->lobby.data(), MAX_LOBBY_NAME_SIZE);
        this->lobby[MAX_LOBBY_NAME_SIZE] = '\0';

        password.copy(this->password.data(), MAX_LOBBY_PASSWORD_SIZE);
        this->password[MAX_LOBBY_PASSWORD_SIZE] = '\0';

        name.copy(this->name.data(), MAX_CLIENT_NAME_SIZE);
        this->name[MAX_CLIENT_NAME_SIZE] = '\0';
    }
    ConnectInfo() = default;
    ~ConnectInfo() noexcept = default;

    std::string GetLobbyName() const { return lobby.data(); }
    std::string GetPassword() const { return password.data(); }
    std::string GetPlayerName() const { return name.data(); }

private:
    std::array<char, MAX_LOBBY_NAME_SIZE + 1> lobby = {};
    std::array<char, MAX_LOBBY_PASSWORD_SIZE + 1> password = {};
    std::array<char, MAX_CLIENT_NAME_SIZE + 1> name = {};
};

class LobbyInfo
{
public:
    static constexpr short MAX_PLAYERS_PER_LOBBY = 8; // Currently fixed for all lobbies

    LobbyInfo(const std::string &name, int playerCount, std::string password)
        : playerCount(playerCount)
    {
        name.copy(this->name.data(), ConnectInfo::MAX_LOBBY_NAME_SIZE);
        this->name[ConnectInfo::MAX_LOBBY_NAME_SIZE] = '\0';

        if (password.empty())
        {
            this->password = false;
        }
        else
        {
            this->password = true;
        }
    }
    LobbyInfo() = default;
    ~LobbyInfo() noexcept = default;

    int GetPlayerCount() const { return playerCount; }
    std::string GetLobbyName() const { return name.data(); }
    bool hasPassword() const { return password; }

private:
    std::array<char, ConnectInfo::MAX_LOBBY_NAME_SIZE + 1> name = {};
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
    LobbyInfo &GetLobbyInfo(int index) { return list[index]; }
    int GetSize() const { return size; }

private:
    short size = 0;
    std::array<LobbyInfo, MAX_LOBBIES_PER_PAGE> list = {};
};

class PlayerInfo
{
public:
    PlayerInfo(const std::string &name)
    {
        name.copy(this->name.data(), ConnectInfo::MAX_CLIENT_NAME_SIZE);
        this->name[ConnectInfo::MAX_CLIENT_NAME_SIZE] = '\0';
    }
    PlayerInfo() = default;
    ~PlayerInfo() noexcept = default;

    std::string GetPlayerName() const { return name.data(); }

private:
    std::array<char, ConnectInfo::MAX_CLIENT_NAME_SIZE + 1> name = {};
};

class PlayerInfoList
{
public:
    PlayerInfoList() = default;
    ~PlayerInfoList() noexcept = default;

    void AddPlayerInfo(PlayerInfo info) { list[size++] = info; }
    PlayerInfo &GetPlayer(int index) { return list[index]; }
    int GetSize() const { return size; }

private:
    short size = 0;
    std::array<PlayerInfo, LobbyInfo::MAX_PLAYERS_PER_LOBBY> list = {};
};