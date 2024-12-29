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

    void ExitError();

public:
    static NetworkConnector &getInstance();
    ~NetworkConnector();

    bool isInitialized() { return isInit; }
    void Init();
    void Exit();
    std::string GetError() { return errorMessage; };
    bool hasPlayerCreatedLobby() { return hasCreated; };
    LobbyInfoList RequestLobbies();
    bool ValidateData(const std::string &lobby, const std::string &name, const std::string &password);
    bool CreateLobby(const std::string &lobby, const std::string &name, const std::string &password);
    bool ConnectToLobby(const std::string &lobby, const std::string &name, const std::string &password);
    PlayerInfoList RequestPlayers();
    GameMode RequestGameMode();
    ChatInfo RequestChat();
    bool StartGame();

    void HandleCanvasChange(SDL_Texture *tex)
    {
        // Here could be network canvas handling
        // std::cout << "Network got updated texture" << std::endl;
    }

    bool UploadText(const std::string& player, const std::string& text)
    {
        TextInfo info = TextInfo(player, text);
        Message message = Message(static_cast<int>(MessageToServer::UPLOAD_TEXT), sizeof(info));

        // Send the message type
        int rv = write(mySocket, &message, sizeof(Message));

        // Handle errors
        if (rv == -1)
        {
            std::cerr << "ERROR: Failed to send message of type: " << MessageToServer::UPLOAD_TEXT << std::endl;
            return false;
        }

        // Send the text information
        rv = write(mySocket, &info, sizeof(info));

        // Handle errors
        if (rv == -1)
        {
            std::cerr << "ERROR: While uploading text failed to send: TextInfo" << std::endl;
            return false;
        }

        // Receive the response
        rv = read(mySocket, &message, sizeof(Message));

        // Handle errors
        if (rv == -1)
        {
            std::cerr << "ERROR: Failed to receive message in method: UploadText" << std::endl;
            return false;
        }

        // Handle based on response
        switch (static_cast<MessageToClient>(message.GetMessageType()))
        {
        case MessageToClient::CONFIRM_TEXT_UPLOAD:
            return true;
            break;

        case MessageToClient::INVALID:
            ExitError();
            break;
        default:
            std::cerr << "ERROR: While uploading text received unexpected message type" << std::endl;
            ExitError();
            break;
        }

        return false;
    }
};
