#pragma once
#include <SDL2/SDL.h>
#include <iostream>

class Interactable
{
private:
public:
    Interactable() {}
    ~Interactable() {}

    virtual void HandleEvent(Uint32 eventType) = 0;
};