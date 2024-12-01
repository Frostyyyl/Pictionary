#include "game_manager.hpp"
#include <iostream>

int main(int argc, char *args[])
{
    GameManager::getInstance().Init("Pictionary", 1000, 600);
    GameManager::getInstance().Run();
    GameManager::getInstance().Exit();

    return 0;
}
