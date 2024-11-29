#pragma once
// #include "mouse.hpp"
#include "components.hpp"
#include <set>
#include <memory>

class InputManager
{
private:
    // Mouse mouse;
    std::set<std::shared_ptr<Interactable>> interactables;
    SDL_Event event;
    InputManager()
    {
        // mouse = Mouse();
    }

public:
    static InputManager &getInstance()
    {
        static InputManager INSTANCE;
        return INSTANCE;
    }

    ~InputManager() {}

    void HandleEvents();
};