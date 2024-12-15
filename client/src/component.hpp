#pragma once

#include <SDL2/SDL.h>

class Component
{
protected:
    SDL_Rect rect;

public:
    Component() {}
    virtual ~Component() {}
    virtual void Update() = 0;
    virtual void Delete() = 0;
};