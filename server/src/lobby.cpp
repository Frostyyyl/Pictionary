#include <algorithm>

#include "lobby.hpp"

int Lobby::GetPlayerDrawing()
{
    if (playerDrawing)
    {
        return -1;
    }
 
    int player = -1;
    while (!drawingHistory.empty())
    {
        int lastDrawingPlayer = drawingHistory.back();
        auto it = players.find(lastDrawingPlayer);

        // If the last drawing player disconnected remove it from the drawing history
        if (it == players.end())
        {
            drawingHistory.pop_back();
        }
        // If the last drawing player was the last player to connect clear the drawing history
        else if (players.begin()->first == lastDrawingPlayer)
        {
            drawingHistory.clear();
        }
        // Find the player who joined the lobby after the last drawing player
        else
        {
            for (auto it = players.begin(); it != players.end(); ++it)
            {
                if (it->first == lastDrawingPlayer)
                {
                    break;
                }
                player = it->first;
            }

            return player;
        }
    }

    // Find the player who joined the lobby first
    for (auto it = players.begin(); it != players.end(); ++it)
    {
        player = it->first;
    }

    return player;
}

void Lobby::SetPlayerDrawing(int socket)
{
    playerDrawing = true;
    drawingHistory.push_back(socket);
}

std::shared_ptr<Player> Lobby::GetPlayer(int socket)
{
    if (players.count(socket))
    {
        return players.at(socket);
    }
    return nullptr;
}

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
    for (const auto &pair : players)
    {
        tmp.push_back(pair.second->GetName());
    }

    std::reverse(tmp.begin(), tmp.end());
    return tmp;
}

void Lobby::StartRound()
{
    roundStarted = true;
    time = std::chrono::steady_clock::now();
}

void Lobby::EndRound()
{
    roundStarted = false;
    playerDrawing = false;
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
    for (const auto &pair : players)
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
