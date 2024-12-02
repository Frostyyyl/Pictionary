#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "component.hpp"

class Interactable : public Component
{
public:
    Interactable() {}
    ~Interactable() {}

    virtual void HandleEvent(SDL_Event event) = 0;
    virtual void Update() = 0;
    virtual void Delete() = 0;
};