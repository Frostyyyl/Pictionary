#include "lobby.hpp"
#include <iostream>

std::vector<int> Lobby::GetSockets()
{
    std::vector<int> tmp;
    for (const auto &pair : players)
    {
        tmp.push_back(pair.first);
    }
    return tmp;
}

std::vector<std::string> Lobby::GetNames()
{
    std::vector<std::string> tmp;
    for (auto &pair : players)
    {
        tmp.push_back(pair.second->GetName());
    }
    return tmp;
}

void Lobby::StartRound()
{
    roundStarted = true;
    time = std::chrono::steady_clock::now();
}

bool Lobby::isEveryoneReady()
{
    for (const auto &pair : players)
    {
        if (!pair.second->isReady())
        {
            return false;
        }
    }
    return true;
}

int Lobby::GetTime()
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    return ROUND_TIME_SEC - std::chrono::duration_cast<std::chrono::seconds>(now - time).count();
}

bool Lobby::hasPlayerName(const std::string &name)
{
    for (auto &pair : players)
    {
        if (pair.second->GetName() == name)
        {
            return true;
        }
    }
    return false;
}

std::shared_ptr<Lobby> LobbyManager::GetLobby(const std::string &name)
{
    if (lobbies.count(name))
    {
        return lobbies.at(name);
    }
    return nullptr; 
}

std::vector<std::string> LobbyManager::GetLobbyNames(int count)
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
