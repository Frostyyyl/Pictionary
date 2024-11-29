// #include "game.hpp"s
#include "game_manager.hpp"
#include <iostream>
// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *args[])
{
    std::cout << "test";
    std::cout.flush();
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        GameManager::getInstance().isRunning = false;
        std::cerr << "ERROR: Game init error" << std::endl;
        exit(1);
    }

    GameManager::getInstance().Init();
    GameManager::getInstance().Run();

    return 0;
}
