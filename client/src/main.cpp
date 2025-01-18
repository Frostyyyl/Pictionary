#include <iostream>

#include "game_manager.hpp"
#include "network_connector.hpp"

int main(int argc, char *args[])
{
    if (argc < 3)
    {
        std::cerr << "ERROR: Insufficient arguments provided, aborting" << std::endl;
        exit(EXIT_FAILURE);
    }

    GameManager::getInstance().Init("Pictionary", 1000, 600, args[1], atoi(args[2]));
    GameManager::getInstance().Run();

    return 0;
}