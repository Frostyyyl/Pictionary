#include "input_manager.hpp"
#include "game_manager.hpp"

void InputManager::HandleEvents()
{
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            GameManager::getInstance().Exit();
        }
        for (auto &interactable : interactables)
        {
            interactable.get()->HandleEvent(event.type);
        }
    }
}