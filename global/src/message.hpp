#pragma once

enum class MessageToServer : int
{
    INVALID = -1,
    ENTER_HUB,
    CREATE_LOBBY,
    CONNECT_TO_LOBBY,
    UPLOAD_CANVAS,
    UPLOAD_TEXT,
    START_ROUND,
};

enum class MessageToClient : int
{
    INVALID = -1,
    UPLOAD_LOBBIES,
    CONNECT_TO_LOBBY,
    INCORRECT_LOBBY_NAME,
    UPLOAD_CANVAS,
    UPLOAD_CHAT,
    PICK_PROMPT,
};

class Message
{
public:
    int type = -1;
    int size = 0;

    Message(int type, int size) : type(type), size(size) {}
    Message(int type) : type(type) {}
    Message() = default;
    ~Message() noexcept = default;
};