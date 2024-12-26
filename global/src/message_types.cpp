#include "message_types.hpp"

std::ostream &operator<<(std::ostream &stream, const MessageToServer &msgType)
{
    switch (msgType)
    {
    case MessageToServer::REQUEST_LOBBIES:
        stream << "REQUEST_LOBBIES";
        break;
    case MessageToServer::CREATE_LOBBY:
        stream << "CREATE_LOBBY";
        break;
    case MessageToServer::CONNECT_TO_LOBBY:
        stream << "CONNECT_TO_LOBBY";
        break;
    case MessageToServer::REQUEST_PLAYERS:
        stream << "REQUEST_PLAYERS";
        break;
    case MessageToServer::UPLOAD_CANVAS:
        stream << "UPLOAD_CANVAS";
        break;
    case MessageToServer::REQUEST_CANVAS:
        stream << "REQUEST_CANVAS";
        break;
    case MessageToServer::UPLOAD_TEXT:
        stream << "UPLOAD_TEXT";
        break;
    case MessageToServer::REQUEST_CHAT:
        stream << "REQUEST_CHAT";
        break;
    case MessageToServer::START_ROUND:
        stream << "START_ROUND";
        break;

    default:
        break;
    }

    return stream;
}

std::ostream &operator<<(std::ostream &stream, const MessageToClient &msgType)
{
    switch (msgType)
    {
    case MessageToClient::UPLOAD_LOBBIES:
        stream << "UPLOAD_LOBBIES";
        break;
    case MessageToClient::CONNECT:
        stream << "CONNECT";
        break;
    case MessageToClient::INCORRECT_LOBBY_NAME:
        stream << "INCORRECT_LOBBY_NAME";
        break;
    case MessageToClient::INCORRECT_PLAYER_NAME:
        stream << "INCORRECT_PLAYER_NAME";
        break;
    case MessageToClient::INCORRECT_PASSWORD:
        stream << "INCORRECT_PASSWORD";
        break;
    case MessageToClient::UPLOAD_PLAYERS:
        stream << "UPLOAD_PLAYERS";
        break;
    case MessageToClient::UPLOAD_CANVAS:
        stream << "UPLOAD_CANVAS";
        break;
    case MessageToClient::UPLOAD_CHAT:
        stream << "UPLOAD_CHAT";
        break;
    case MessageToClient::PICK_PROMPT:
        stream << "PICK_PROMPT";
        break;

    default:
        break;
    }

    return stream;
}