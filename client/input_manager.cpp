#include "input_manager.hpp"
#include "game_manager.hpp"
#include "components.hpp"

InputManager::InputManager(GameManager *manager)
{
    gameManager = manager;
    isDrawing = false;
    isTyping = false;
}

void InputManager::AddInteractable(std::string name, std::shared_ptr<Interactable> object)
{
    interactables[name] = object;
}

void InputManager::ClearInteractables()
{
    interactables.clear();
}

void InputManager::HandleEvent()
{
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT)
    {
        gameManager->Exit();
        return;
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        for (auto &[name, obj] : interactables)
        {
            if (!obj.get()->isClicked(event))
                continue;
            if (name == "canvas")
            {
                isDrawing = true;
            }
            obj.get()->HandleEvent(event);
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (interactables.find("canvas") != interactables.end())
        {
            isDrawing = false;
            interactables["canvas"].get()->HandleEvent(event);
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        if (isDrawing) // probably won't give any error that canvas does not exist
        {
            interactables["canvas"].get()->HandleEvent(event);
        }
    }
    else if (event.type == SDL_TEXTINPUT ||
             (event.type == SDL_KEYDOWN &&
              (event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_RETURN)))
    {
        if (interactables.find("textInput") != interactables.end())
        {
            interactables["textInput"].get()->HandleEvent(event);
        }
    }
}