#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "../../global/src/message_types.hpp"
#include "../../global/src/messages.hpp"

class NetworkConnector
{
private:
    NetworkConnector();

    bool isInit;
    int mySocket;
    std::string errorMessage;
    bool hasCreated;

    const int PORT = 1100;
    const std::string ADDRESS = "127.0.0.1";
    const int MAX_RETRIES = 3;
    const int RETRY_DELAY_SEC = 0.010;

    void ExitError();
    template <typename T>
    bool WriteWithRetry(int socket, const void *buffer, size_t size, const T& messageType);
    template <typename T>
    bool ReadWithRetry(int socket, void *buffer, size_t size, const T& messageType);
    bool ValidateData(const std::string &lobby, const std::string &name, const std::string &password);

public:
    static NetworkConnector &getInstance();
    ~NetworkConnector();

    bool isInitialized() { return isInit; }
    std::string GetError() { return errorMessage; };
    bool hasPlayerCreatedLobby() { return hasCreated; };

    void Init();
    void Exit();
    LobbyInfoList RequestLobbies();
    bool CreateLobby(const std::string &lobby, const std::string &name, const std::string &password);
    bool ConnectToLobby(const std::string &lobby, const std::string &name, const std::string &password);
    PlayerInfoList RequestPlayers();
    GameMode RequestGameMode();
    ChatInfo RequestChat();

    void StartGame();
    void HandleCanvasChange(SDL_Texture *tex) {}
    void UploadText(const std::string& player, const std::string& text);
};
