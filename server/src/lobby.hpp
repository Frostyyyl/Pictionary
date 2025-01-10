#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <deque>
#include <unordered_map>

#include "../../global/src/messages.hpp"

class Player
{
public:
    Player(const std::string &name) : name(name) {}
    Player() = default;
    ~Player() noexcept = default;

    std::string GetName() { return name; }
    void AddPoints(int points) { this->points += points; }
    int GetPoints() { return points; }
    bool isReady() { return ready; }
    void SetIsReady(bool value) { ready = value; }
    void SetLastReadChange(int value) { lastReadChange = value; }
    int GetLastReadChange() { return lastReadChange; }

private:
    std::string name;
    int points = 0;
    bool ready = false;
    int lastReadChange = -1;
};

class Lobby
{
public:
    static constexpr int ROUND_TIME_SEC = 60;

    Lobby(const std::string &password) : password(password) {}
    Lobby() = default;
    ~Lobby() noexcept = default;

    void AddPlayer(int socket, std::string name) { players.insert({socket, std::make_shared<Player>(name)}); }
    void RemovePlayer(int socket) { players.erase(socket); }
    std::shared_ptr<Player> GetPlayer(int socket); // Return nullptr if not found

    int GetPlayerDrawing();
    void SetPlayerDrawing(int socket);

    std::string GetPassword() { return password; }
    int GetSize() { return players.size(); }
    int GetTime();
    void StartGame() { gameStarted = true; }
    void StartRound();
    void EndRound();

    std::vector<int> GetSockets();
    std::vector<std::pair<std::string, int>> GetPlayerInfos();

    bool hasPlayerName(const std::string &name);
    bool hasGameStarted() { return gameStarted; }
    bool hasRoundStarted() { return roundStarted; }
    bool isEveryoneReady();

    void AddMessage(const TextInfo &message) { chat.AddMessage(message); }
    ChatInfo &GetChat() { return chat; }
    void AddCanvasChange(const CanvasChangeInfo &change) { canvasChanges.push_back(change); }
    CanvasChangeInfoList GetCanvasChanges(int socket);
    void SetPrompt(const std::string &prompt) { this->prompt = prompt; }
    std::string GetPrompt() { return prompt; }
    void SetCreator(int socket) { this->creator = socket; }
    int GetCreator() { return creator; }

private:
    ChatInfo chat = {};
    std::deque<CanvasChangeInfo> canvasChanges;
    std::string prompt;
    int creator;
    std::string password;
    std::unordered_map<int, std::shared_ptr<Player>> players;

    std::vector<int> drawingHistory;
    int playerDrawing = -1;
    bool gameStarted = false;
    bool roundStarted = false;
    std::chrono::steady_clock::time_point time = {};
};

class LobbyManager
{
public:
    static LobbyManager &getInstance()
    {
        static LobbyManager instance;
        return instance;
    }

    std::shared_ptr<Lobby> GetLobby(const std::string &name); // Return nullptr if not found
    void AddLobby(const std::string &name, const std::string &password) { lobbies.insert({name, std::make_shared<Lobby>(password)}); }
    void RemoveLobby(const std::string &name) { lobbies.erase(name); }
    std::vector<std::string> GetLobbyNames(int count);
    bool hasLobby(const std::string &name) { return lobbies.count(name) > 0; }

private:
    LobbyManager() = default;
    ~LobbyManager() noexcept = default;
    LobbyManager(const LobbyManager &) = delete;
    LobbyManager &operator=(const LobbyManager &) = delete;

    std::map<std::string, std::shared_ptr<Lobby>> lobbies;
};