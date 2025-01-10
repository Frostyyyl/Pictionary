#include "input_manager.hpp"
#include "game_manager.hpp"
#include "components.hpp"

InputManager::InputManager(GameManager *manager)
{
    gameManager = manager;
    isDrawing = false;
    isTyping = false;
}

void InputManager::AddInteractable(const std::string &name, std::weak_ptr<Interactable> object)
{
    interactables[name] = object;

    if (name == "Canvas")
    {
        std::shared_ptr<Interactable> sharedObject = object.lock();
        if (sharedObject)
        {
            std::shared_ptr<Canvas> sharedCanvas = std::static_pointer_cast<Canvas>(sharedObject);
            if (sharedCanvas)
                canvas = sharedCanvas;
        }
    }
}

void InputManager::DeleteInteractable(const std::string &name)
{
    interactables.erase(name);
}

void InputManager::ClearInteractables()
{
    interactables.clear();
}

void InputManager::StopDrawing()
{
    // NOTE: This is quite junky but oh well
    if (isDrawing)
    {
        isDrawing = false;
        SDL_Event ev;
        ev.type = SDL_MOUSEBUTTONUP;
        canvas.lock().get()->HandleEvent(ev);
    }
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
            if (name == "Canvas")
            {
                isDrawing = true;
            }
            obj.lock().get()->HandleEvent(event);
            return;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (interactables.find("Canvas") != interactables.end())
        {
            isDrawing = false;
            canvas.lock().get()->HandleEvent(event);
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        if (isDrawing && interactables.find("Canvas") != interactables.end())
        {
            canvas.lock().get()->HandleEvent(event);
        }
    }
    else if (event.type == SDL_TEXTINPUT ||
             (event.type == SDL_KEYDOWN &&
              (event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_RETURN)))
    {
        if (interactables.find(GameManager::getInstance().GetCurrentTextInput()) != interactables.end())
        {
            interactables[GameManager::getInstance().GetCurrentTextInput()].lock().get()->HandleEvent(event);
        }
    }
}