#include <iostream>

#include "game_manager.hpp"
#include "network_connector.hpp"

int main(int argc, char *args[])
{
    GameManager::getInstance().Init("Pictionary", 1000, 600);
    GameManager::getInstance().Run();

    return 0;
}