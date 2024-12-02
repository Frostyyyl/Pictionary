#pragma once

#include "SDL2/SDL.h"
#include <iostream>

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
    GameMode gameMode;

public:
    Player(int id)
    {
        ID = id;
        gameMode = STANDBY;
    }
    ~Player() {}

    int getID()
    {
        return ID;
    }

    void ChangeGameMode(GameMode newGameMode)
    {
        gameMode = newGameMode;
    }

    void HandleCanvasChange(SDL_Texture *tex)
    {
        // Here could be network canvas handling
        std::cout << "Player got updated texture" << std::endl;
    };
};