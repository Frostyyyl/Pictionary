#pragma once

#include <iostream>

enum class MessageToServer : int
{
    INVALID = -1,
    REQUEST_LOBBIES,
    CREATE_LOBBY,
    CONNECT_TO_LOBBY,
    REQUEST_PLAYERS,
    UPLOAD_CANVAS,
    REQUEST_CANVAS,
    UPLOAD_TEXT,
    REQUEST_CHAT,
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
    UPLOAD_CANVAS,
    UPLOAD_CHAT,
    PICK_PROMPT,
};

class Message
{
public:
    Message(int type, int size) : type(type), size(size) {}
    Message(int type) : type(type) {}
    Message() = default;
    ~Message() noexcept = default;

    int GetMessageType() { return type; }
    int GetSize() { return size; }

private:
    int type = -1;
    int size = 0;
};

std::ostream &operator<<(std::ostream &stream, const MessageToServer &msgType);
std::ostream &operator<<(std::ostream &stream, const MessageToClient &msgType);