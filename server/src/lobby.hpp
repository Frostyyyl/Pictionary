#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>

#include "client.hpp"

// TODO: Handle player drawing disconnecting

class Player
{
public:
    Player(const std::string &name) : name(name) {}
    Player() = default;
    ~Player() noexcept = default;

    std::string GetName() { return name; }
    bool isReady() { return ready; }
    void SetIsReady(bool value) { ready = value; }

private:
    std::string name;
    bool ready = false;
};

class Lobby
{
public:
    static constexpr int ROUND_TIME_SEC = 180;

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
    std::vector<std::string> GetNames();

    bool hasPlayerName(const std::string &name);
    bool hasGameStarted() { return gameStarted; }
    bool hasRoundStarted() { return roundStarted; }
    bool isEveryoneReady();

    void AddMessage(const TextInfo &message) { chat.AddMessage(message); }
    ChatInfo &GetChat() { return chat; }

private:
    ChatInfo chat = {};
    std::string password;
    std::unordered_map<int, std::shared_ptr<Player>> players;

    std::vector<int> drawingHistory;
    bool playerDrawing = false;
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