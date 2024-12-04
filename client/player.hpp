#pragma once

#include "SDL2/SDL.h"
#include <iostream>
#include <string>

enum GameMode
{
    STANDBY,
    DRAW,
    GUESS
};

class Player
{
private:
    int ID;
    std::string nickname;
    GameMode gameMode;

public:
    Player(int id, std::string name)
    {
        ID = id;
        nickname = name;
        gameMode = STANDBY;
    }
    ~Player() {}

    int getID()
    {
        return ID;
    }

    std::string GetNickname()
    {
        return nickname;
    }

    void ChangeGameMode(GameMode newGameMode)
    {
        gameMode = newGameMode;
    }
};