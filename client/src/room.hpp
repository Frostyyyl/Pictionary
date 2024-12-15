#pragma once

#include "player.hpp"

#define MAX_PLAYERS 4

// Not sure if this class is needed but might be later
class Room
{
private:
    int numberOfPlayers;
    Player *players[MAX_PLAYERS];

public:
    Room()
    {
        // Here we should probably get info from server about number of players
        numberOfPlayers = 0;
    }
    ~Room() {}

    // I think this is not the best approach
    int AddPlayer(Player *player)
    {
        if (numberOfPlayers >= MAX_PLAYERS)
            return -1;
        players[numberOfPlayers] = player;
        numberOfPlayers++;
        return 0;
    }
};