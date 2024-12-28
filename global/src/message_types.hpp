#pragma once

#include <iostream>

enum class MessageToServer : int
{
    INVALID = -1,
    REQUEST_LOBBIES,
    CREATE_LOBBY,
    CONNECT_TO_LOBBY,
    REQUEST_PLAYERS,
    REQUEST_GAMEMODE,
    UPLOAD_CANVAS,
    REQUEST_CANVAS,
    UPLOAD_TEXT,
    REQUEST_CHAT,
    START_GAME,
    START_ROUND,
};

enum class MessageToClient : int
{
    INVALID = -1,
    UPLOAD_LOBBIES,
    CONNECT,
    INCORRECT_LOBBY_NAME,
    INCORRECT_PLAYER_NAME,
    INCORRECT_PASSWORD,
    UPLOAD_PLAYERS,
    UPLOAD_GAMEMODE,
    UPLOAD_CANVAS,
    UPLOAD_CHAT,
    CONFIRM_GAME_START,
    PICK_PROMPT,
    UPLOAD_TIME,
};

std::ostream &operator<<(std::ostream &stream, const MessageToServer &msgType);
std::ostream &operator<<(std::ostream &stream, const MessageToClient &msgType);
