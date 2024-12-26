#pragma once

#include <SDL2/SDL.h>
#include <string>

class Component
{
protected:
    SDL_Rect rect;
    std::string name;

public:
    Component(const std::string &name) : name(name) {}
    virtual ~Component() {}
    virtual void Update() = 0;
    std::string GetName() { return name; }
};