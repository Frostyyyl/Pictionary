#pragma once

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
};