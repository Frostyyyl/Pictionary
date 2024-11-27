#include <stdio.h>
#include "game.hpp"

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *args[])
{
    Game game = Game(600, 600, "Pictionary");
    if (game.init() < 0)
    {
        printf("%s\n", SDL_GetError());
        return 1;
    }
    game.run();
    game.quit();
    return 0;
}
