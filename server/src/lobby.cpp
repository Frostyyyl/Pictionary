#include <algorithm>

#include "lobby.hpp"

int Lobby::GetPlayerDrawing()
{
    if (playerDrawing > -1)
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
    playerDrawing = socket;
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

std::vector<std::pair<std::string, int>> Lobby::GetPlayerInfos()
{
    std::vector<std::pair<std::string, int>> tmp;
    for (const auto &pair : players)
    {
        tmp.push_back({pair.second->GetName(), pair.second->GetPoints()});
    }

    std::reverse(tmp.begin(), tmp.end());
    return tmp;
}

void Lobby::StartRound()
{
    roundStarted = true;
    time = std::chrono::steady_clock::now();

    canvasChanges.clear();
}

void Lobby::EndRound()
{
    roundStarted = false;
    playerDrawing = -1;

    canvasChanges.clear();
    prompt = "";

    for (const auto &pair : players)
    {
        pair.second->SetIsReady(false);
    }

    for (const auto &pair : players)
    {
        pair.second->SetLastReadChange(-1);
    }
}

bool Lobby::isEveryoneReady()
{
    bool val = true;

    for (const auto &pair : players)
    {
        if (!pair.second->isReady())
        {
            val =  false;
        }
    }

    return val;
}

int Lobby::GetTime()
{
    if (!roundStarted)
    {
        return ROUND_TIME_SEC;
    }

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    int timeLeft = ROUND_TIME_SEC - std::chrono::duration_cast<std::chrono::seconds>(now - time).count();

    if (timeLeft < 0)
    {
        EndRound();
    }

    return timeLeft;
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

CanvasChangeInfoList Lobby::GetCanvasChanges(int socket)
{
    CanvasChangeInfoList changes;

    if (!hasRoundStarted())
    {
        return changes;
    }

    int next = players[socket]->GetLastReadChange() + 1;

    for (int i = next; i < next + CanvasChangeInfoList::MAX_CANVAS_CHANGES; i++)
    {
        if (static_cast<int>(canvasChanges.size()) > i)
        {
            changes.AddCanvasChange(canvasChanges[i]);
        }
        else
        {
            break;
        }
    }

    players[socket]->SetLastReadChange(next + changes.GetSize() - 1);
    
    // Get the minimum last read change
    int min = players[socket]->GetLastReadChange();
    for (const auto &pair : players)
    {
        if (pair.first != playerDrawing && pair.second->GetLastReadChange() < min)
        {
            min = pair.second->GetLastReadChange();
        }
    }
    min++;

    // Adjust the last read change for all players
    for (const auto &pair : players)
    {
        if (pair.first != playerDrawing)
        {
            pair.second->SetLastReadChange(pair.second->GetLastReadChange() - min);
        }
    }

    // Remove the changes read by all players
    for (int i = 0; i < min; i++)
    {
        canvasChanges.pop_front();
    }

    return changes;
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
