#pragma once
#include <SDL2/SDL.h>

class Interactable
{
private:
    /* data */
public:
    Interactable() {}
    ~Interactable() {}

    virtual void HandleEvent(Uint32 eventType) = 0;
};
