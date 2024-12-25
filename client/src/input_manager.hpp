#pragma once

#include <memory>
#include <unordered_map>

#include "interactable.hpp"

class GameManager;

class InputManager
{
private:
    GameManager *gameManager;
    SDL_Event event;

    std::unordered_map<std::string, std::weak_ptr<Interactable>> interactables;

    bool isDrawing;
    bool isTyping;

public:
    InputManager(GameManager *manager);
    ~InputManager() {}

    void AddInteractable(std::string name, std::weak_ptr<Interactable> object);
    void ClearInteractables();

    void HandleEvent();
};