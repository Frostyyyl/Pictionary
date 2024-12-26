#pragma once

#include <string>
#include <vector>
#include <map>

#include "client.hpp"

class Lobby
{
public:
    Lobby(const std::string &password) : password(password) {}
    Lobby() {}
    ~Lobby() noexcept = default;

    void AddPlayer(int socket, std::string name) { players.insert({socket, name}); }
    void RemovePlayer(int socket) { players.erase(socket); }
    std::vector<int> GetSockets()
    {
        std::vector<int> tmp;

        for (const auto &pair : players)
        {
            tmp.push_back(pair.first);
        }

        return tmp;
    }
    std::vector<std::string> GetNames()
    {
        std::vector<std::string> tmp;

        for (const auto &pair : players)
        {
            tmp.push_back(pair.second);
        }

        return tmp;
    }
    std::string GetPlayerName(int socket) { return players[socket]; }
    std::string GetPassword() { return password; }
    int GetSize() { return players.size(); }
    bool hasPlayerName(const std::string &name)
    {
        for (const auto &pair : players)
        {
            if (pair.second == name)
            {
                return true;
            }
        }
        return false;
    }

private:
    std::string password = {};
    std::map<int, std::string> players = {};
};

class LobbyManager
{
public:
    LobbyManager() = default;
    ~LobbyManager() noexcept = default;

    Lobby &GetLobby(const std::string &name) { return lobbies[name]; }
    void AddLobby(const std::string &name, Lobby lobby) { lobbies.insert({name, lobby}).second; }
    void RemoveLobby(const std::string &name) { lobbies.erase(name); }
    /**
     * Return 'count' of lobby names
     */
    std::vector<std::string> GetLobbyNames(int count)
    {
        std::vector<std::string> names;
        int counter = 0;

        for (const auto &pair : lobbies)
        {
            if (counter < count)
            {
                names.push_back(pair.first);
                counter++;
            }
            else
            {
                break;
            }
        }

        return names;
    }
    bool hasLobby(const std::string &name)
    {
        std::map<std::string, Lobby>::iterator it = lobbies.find(name);

        if (it != lobbies.end())
        {
            return true;
        }
        return false;
    }

private:
    std::map<std::string, Lobby> lobbies = {};
};