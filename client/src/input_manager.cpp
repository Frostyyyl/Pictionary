#include "input_manager.hpp"
#include "game_manager.hpp"
#include "components.hpp"

InputManager::InputManager(GameManager *manager)
{
    gameManager = manager;
    isDrawing = false;
    isTyping = false;
}

void InputManager::AddInteractable(std::string name, std::weak_ptr<Interactable> object)
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
        if (interactables.empty())
            return;
        for (auto &[name, obj] : interactables)
        {
            if (gameManager->wasSceneChanged)
                return;
            if (!obj.lock())
                continue;
            if (!obj.lock().get()->isClicked(event))
                continue;
            std::cout << name << std::endl;
            if (name == "canvas")
            {
                isDrawing = true;
            }
            obj.lock().get()->HandleEvent(event);
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (interactables.find("canvas") != interactables.end())
        {
            isDrawing = false;
            interactables["canvas"].lock().get()->HandleEvent(event);
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        if (isDrawing) // probably won't give any error that canvas does not exist
        {
            interactables["canvas"].lock().get()->HandleEvent(event);
        }
    }
    else if (event.type == SDL_TEXTINPUT ||
             (event.type == SDL_KEYDOWN &&
              (event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_RETURN)))
    {
        if (interactables.find("textInput") != interactables.end())
        {
            interactables["textInput"].lock().get()->HandleEvent(event);
        }
    }
}