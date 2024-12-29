#pragma once

#include <string>
#include <array>

#include "game_mode.hpp"

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

class ConnectInfo
{
public:
    static constexpr short MAX_LOBBY_NAME_SIZE = 16;
    static constexpr short MAX_LOBBY_PASSWORD_SIZE = 16;
    static constexpr short MAX_CLIENT_NAME_SIZE = 16;

    ConnectInfo(const std::string &lobby, const std::string &name, const std::string &password)
    {
        lobby.copy(this->lobby.data(), MAX_LOBBY_NAME_SIZE);
        this->lobby[MAX_LOBBY_NAME_SIZE] = '\0';

        password.copy(this->password.data(), MAX_LOBBY_PASSWORD_SIZE);
        this->password[MAX_LOBBY_PASSWORD_SIZE] = '\0';

        name.copy(this->name.data(), MAX_CLIENT_NAME_SIZE);
        this->name[MAX_CLIENT_NAME_SIZE] = '\0';
    }
    ConnectInfo() = default;
    ~ConnectInfo() noexcept = default;

    std::string GetLobbyName() const { return lobby.data(); }
    std::string GetPassword() const { return password.data(); }
    std::string GetPlayerName() const { return name.data(); }

private:
    std::array<char, MAX_LOBBY_NAME_SIZE + 1> lobby = {};
    std::array<char, MAX_LOBBY_PASSWORD_SIZE + 1> password = {};
    std::array<char, MAX_CLIENT_NAME_SIZE + 1> name = {};
};

class LobbyInfo
{
public:
    static constexpr short MAX_PLAYERS_PER_LOBBY = 8; // Currently fixed for all lobbies

    LobbyInfo(const std::string &name, int playerCount, const std::string &password)
        : playerCount(playerCount)
    {
        name.copy(this->name.data(), ConnectInfo::MAX_LOBBY_NAME_SIZE);
        this->name[ConnectInfo::MAX_LOBBY_NAME_SIZE] = '\0';

        if (password.empty())
        {
            this->password = false;
        }
        else
        {
            this->password = true;
        }
    }
    LobbyInfo() = default;
    ~LobbyInfo() noexcept = default;

    int GetPlayerCount() const { return playerCount; }
    std::string GetLobbyName() const { return name.data(); }
    bool hasPassword() const { return password; }

private:
    std::array<char, ConnectInfo::MAX_LOBBY_NAME_SIZE + 1> name = {};
    short playerCount = 0;
    bool password = false;
};

class LobbyInfoList
{
public:
    static constexpr short MAX_LOBBIES_PER_PAGE = 16;

    LobbyInfoList() = default;
    ~LobbyInfoList() noexcept = default;

    void AddLobbyInfo(const LobbyInfo &info) { list[size++] = info; }
    LobbyInfo &GetLobbyInfo(int index) { return list[index]; }
    int GetSize() const { return size; }

private:
    short size = 0;
    std::array<LobbyInfo, MAX_LOBBIES_PER_PAGE> list = {};
};

class PlayerInfo
{
public:
    PlayerInfo(const std::string &name)
    {
        name.copy(this->name.data(), ConnectInfo::MAX_CLIENT_NAME_SIZE);
        this->name[ConnectInfo::MAX_CLIENT_NAME_SIZE] = '\0';
    }
    PlayerInfo() = default;
    ~PlayerInfo() noexcept = default;

    std::string GetPlayerName() const { return name.data(); }

private:
    std::array<char, ConnectInfo::MAX_CLIENT_NAME_SIZE + 1> name = {};
};

class PlayerInfoList
{
public:
    PlayerInfoList() = default;
    ~PlayerInfoList() noexcept = default;

    void AddPlayerInfo(const PlayerInfo &info) { list[size++] = info; }
    PlayerInfo &GetPlayer(int index) { return list[index]; }
    int GetSize() const { return size; }

private:
    short size = 0;
    std::array<PlayerInfo, LobbyInfo::MAX_PLAYERS_PER_LOBBY> list = {};
};

class GameModeInfo
{
public:
    GameModeInfo(GameMode mode) : mode(mode) {};
    GameModeInfo() = default;
    ~GameModeInfo() noexcept = default;

    GameMode GetGameMode() { return mode; }

private:
    GameMode mode = GameMode::STANDBY;
};

class TextInfo
{
public:
    static constexpr short MAX_TEXT_SIZE = 19; // TODO: Update alongside the size of the chat window

    TextInfo(const std::string &name, const std::string &text)
    {
        name.copy(this->name.data(), ConnectInfo::MAX_CLIENT_NAME_SIZE);
        this->name[ConnectInfo::MAX_CLIENT_NAME_SIZE] = '\0';

        text.copy(this->text.data(), MAX_TEXT_SIZE);
        this->text[MAX_TEXT_SIZE] = '\0';
    };
    TextInfo() = default;
    ~TextInfo() noexcept = default;

    std::string GetText() { return text.data(); }
    std::string GetPlayerName() { return name.data(); }

private:
    std::array<char, MAX_TEXT_SIZE + 1> text = {};
    std::array<char, ConnectInfo::MAX_CLIENT_NAME_SIZE + 1> name = {};
};

class ChatInfo
{
public:
    static constexpr short MAX_TEXTS = 14; // TODO: Update alongside the size of the chat window

    ChatInfo() = default;
    ~ChatInfo() noexcept = default;

    void AddMessage(const TextInfo &message) 
    { 
        if (size == MAX_TEXTS)
        {
            for (int i = 0; i < size - 1; i++)
            {
                messages[i] = messages[i + 1];
            }

            size--;
        }

        messages[size++] = message; 

    }
    TextInfo &GetMessage(int index) { return messages[index]; }
    int GetSize() const { return size; }

private:
    short size = 0;
    std::array<TextInfo, MAX_TEXTS> messages = {};
};

#include <SDL2/SDL.h>

class CanvasInfo
{
public:
    static constexpr int CANVAS_WIDTH = 400;
    static constexpr int CANVAS_HEIGHT = 400;

    CanvasInfo()
    {
        canvas.fill(0);
    }

    CanvasInfo(const SDL_Texture* texture)
    {
        SDL_Renderer* renderer = SDL_CreateRenderer(SDL_CreateWindow("temp", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CANVAS_WIDTH, CANVAS_HEIGHT, 0), -1, 0);
        SDL_SetRenderTarget(renderer, const_cast<SDL_Texture*>(texture));
        SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA8888, canvas.data(), CANVAS_WIDTH * 4);
        SDL_DestroyRenderer(renderer);
    }

    ~CanvasInfo() noexcept = default;

    const std::array<Uint32, CANVAS_WIDTH * CANVAS_HEIGHT>& GetCanvas() const
    {
        return canvas;
    }

private:
    std::array<Uint32, CANVAS_WIDTH * CANVAS_HEIGHT> canvas;
};

class TimeInfo
{
public:
    TimeInfo(int time) : time(time) {};
    TimeInfo() = default;
    ~TimeInfo() noexcept = default;

    int GetTime() { return time; }

private:
    int time = 0;
};