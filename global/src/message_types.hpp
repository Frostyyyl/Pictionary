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
    REQUEST_PROMPTS,
    PICK_PROMPT,
    REQUEST_TIME,
};

enum class MessageToClient : int
{
    INVALID = -1,
    UPLOAD_LOBBIES,
    CONFIRM_CONNECT,
    INCORRECT_LOBBY_NAME,
    INCORRECT_PLAYER_NAME,
    INCORRECT_PASSWORD,
    UPLOAD_PLAYERS,
    UPLOAD_GAMEMODE,
    UPLOAD_CANVAS,
    CONFIRM_TEXT_UPLOAD,
    UPLOAD_CHAT,
    CONFIRM_GAME_START,
    UPLOAD_PROMPTS,
    CONFIRM_PROMPT,
    UPLOAD_TIME,
};

std::ostream &operator<<(std::ostream &stream, const MessageToServer &msgType);
std::ostream &operator<<(std::ostream &stream, const MessageToClient &msgType);
