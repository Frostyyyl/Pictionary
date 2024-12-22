#pragma once

#include <string>
#include <vector>
#include <array>
#include "../../global/src/objects.hpp"

class Lobby
{
public:
    std::vector<int> clients;
    std::string password = {};

    Lobby(const std::string& password) 
    {
        password.copy(this->password.data(), LobbyInfo::MAX_LOBBY_PASSWORD_SIZE);
    }
    Lobby() = default;
    ~Lobby() noexcept = default;
};