#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
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

    const int MAX_RETRIES = 3;
    const int RETRY_DELAY_SEC = 0.010;
    const int AWAIT_FULL_MESSAGE_SEC = 0.0001;

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

    void Init(const std::string &address, int port);
    void Exit();
    bool CreateLobby(const std::string &lobby, const std::string &name, const std::string &password);
    bool ConnectToLobby(const std::string &lobby, const std::string &name, const std::string &password);
    LobbyInfoList RequestLobbies();
    PlayerInfoList RequestPlayers();
    GameModeInfo RequestGameMode();
    ChatInfo RequestChat();
    CanvasChangeInfoList RequestCanvasChange();
    PromptsInfoList RequestPrompts();
    TimeInfo RequestTime();

    void StartGame();
    void UploadCanvasChanges(const CanvasChangeInfoList& canvas);
    void UploadText(const std::string& player, const std::string& text);
    void UploadPrompt(const std::string& prompt);
    void UploadLobbyExit();
};
