#pragma once

#include <string>
#include <array>
#include <vector>
#include <map>
#include "client.hpp"

class Lobby
{
public:
    Lobby(const std::string& password) : password(password) {}
    Lobby() {}
    ~Lobby() noexcept = default;

    void addPlayer(int socket, std::string name) { players.insert({socket, name}); }
    void removePlayer(int socket) { players.erase(socket); }
    std::vector<int> getSockets()
    { 
        std::vector<int> tmp;
    
        for (const auto& pair : players) {
            tmp.push_back(pair.first);
        }

        return tmp; 
    }
    std::vector<std::string> getPlayerNames() 
    { 
        std::vector<std::string> tmp;
    
        for (const auto& pair : players) {
            tmp.push_back(pair.second);
        }

        return tmp; 
    }
    std::string getPlayerName(int socket) { return players[socket]; }
    std::string getPassword() { return password; }
    int getSize(){ return players.size(); }
    bool hasPassword() { return password.empty() ? false : true; }
    bool hasPlayerName(const std::string& name)
    {
        for (const auto& pair : players) {
            if (pair.second == name) {
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

    Lobby getLobby(const std::string& name) { return lobbies[name]; }
    void addLobby(const std::string& name, Lobby lobby) { lobbies.insert({name, lobby}).second; }
    void removeLobby(const std::string& name) { lobbies.erase(name); }
    /**
     * Return 'count' of lobby names
     */
    std::vector<std::string> getLobbyNames(int count)
    { 
        std::vector<std::string> names;
        int counter = 0;

        for (const auto& pair : lobbies) {
            if (counter < count){
                names.push_back(pair.first);
                counter++;
            }
        }

        return names; 
    }
    bool hasLobby(const std::string& name)
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