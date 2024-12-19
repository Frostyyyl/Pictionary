#pragma once

enum class MessageToServer 
{
    ENTER_HUB,
    CREATE_LOBBY,
    CONNECT_TO_LOBBY,
    UPLOAD_CANVAS,
    UPLOAD_TEXT,
    START_ROUND,
};

enum class MessageToClient 
{
    UPLOAD_LOBBIES,
    UPLOAD_CANVAS,
    UPLOAD_CHAT,
    PICK_PROMPT,
};

class Message
{
public:
    int type;
    int size;

    Message(int type, int size)
    {
        this->type = type;
        this->size = size;
    }
    Message() {}
    ~Message() {}
};