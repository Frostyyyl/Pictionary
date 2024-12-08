#pragma once

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
    bool isClicked(SDL_Event event)
    {
        return (event.button.x >= rect.x && event.button.x <= rect.x + rect.w && event.button.y >= rect.y && event.button.y <= rect.y + rect.h);
    }
};