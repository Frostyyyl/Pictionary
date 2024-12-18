#pragma once

#include <string>

enum class MessageToServer {
    ENTER_HUB,
    CREATE_LOBBY,
    CONNECT_TO_LOBBY,
    UPLOAD_CANVAS,
    UPLOAD_TEXT,
    START_ROUND,
};

enum class MessageToClient {
    UPLOAD_LOBBIES,
    UPLOAD_CANVAS,
    UPLOAD_CHAT,
    PICK_PROMPT,
};

// TODO: Consider an alternative to current messages (perhaps a global message size higher than any message's actual size)