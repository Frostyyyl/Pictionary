#pragma once
#include <iostream>
#include "SDL2/SDL.h"

struct Text
{
    std::string text;
    SDL_Rect dest = {0, 0, 0, 0};

    Text() {}
};