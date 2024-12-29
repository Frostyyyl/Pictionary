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
    case MessageToServer::REQUEST_GAMEMODE:
        stream << "REQUEST_GAMEMODE";
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
    case MessageToServer::START_GAME:
        stream << "START_GAME";
        break;
    case MessageToServer::REQUEST_PROMPTS:
        stream << "REQUEST_PROMPTS";
        break;
    case MessageToServer::PICK_PROMPT:
        stream << "PICK_PROMPT";
        break;
    case MessageToServer::REQUEST_TIME:
        stream << "REQUEST_TIME";
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
    case MessageToClient::CONFIRM_CONNECT:
        stream << "CONFIRM_CONNECT";
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
    case MessageToClient::UPLOAD_GAMEMODE:
        stream << "UPLOAD_GAMEMODE";
        break;
    case MessageToClient::UPLOAD_CANVAS:
        stream << "UPLOAD_CANVAS";
        break;
    case MessageToClient::CONFIRM_TEXT_UPLOAD:
        stream << "CONFIRM_TEXT_UPLOAD";
        break;
    case MessageToClient::UPLOAD_CHAT:
        stream << "UPLOAD_CHAT";
        break;
    case MessageToClient::CONFIRM_GAME_START:
        stream << "CONFIRM_GAME_START";
        break;
    case MessageToClient::UPLOAD_PROMPTS:
        stream << "UPLOAD_PROMPTS";
        break;
    case MessageToClient::CONFIRM_PROMPT:
        stream << "CONFIRM_PROMPT";
        break;
    case MessageToClient::UPLOAD_TIME:
        stream << "UPLOAD_TIME";
        break;

    default:
        break;
    }

    return stream;
}