#pragma once

#include <iostream>

enum class MessageToServer : int
{
    INVALID = -1,
    CREATE_LOBBY,
    CONNECT_TO_LOBBY,
    REQUEST_LOBBIES,
    REQUEST_PLAYERS,
    REQUEST_GAMEMODE,
    REQUEST_CANVAS,
    REQUEST_CHAT,
    REQUEST_PROMPTS,
    REQUEST_TIME,
    UPLOAD_CANVAS,
    UPLOAD_TEXT,
    UPLOAD_PROMPT,
    START_GAME,
};

enum class MessageToClient : int
{
    INVALID = -1,
    CONFIRM_CONNECT,
    INCORRECT_LOBBY_NAME,
    INCORRECT_PLAYER_NAME,
    INCORRECT_PASSWORD,
    UPLOAD_LOBBIES,
    UPLOAD_PLAYERS,
    UPLOAD_GAMEMODE,
    UPLOAD_CANVAS,
    UPLOAD_CHAT,
    UPLOAD_PROMPTS,
    UPLOAD_TIME,
};

std::ostream &operator<<(std::ostream &stream, const MessageToServer &msgType);
std::ostream &operator<<(std::ostream &stream, const MessageToClient &msgType);
