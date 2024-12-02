#pragma once

#include "interactable.hpp"
#include <memory>
#include <unordered_map>

class GameManager;

class InputManager
{
private:
    GameManager *gameManager;
    SDL_Event event;

    std::unordered_map<std::string, std::shared_ptr<Interactable>> interactables;

    bool isDrawing;
    bool isTyping;

public:
    InputManager(GameManager *manager);
    ~InputManager() {}

    void AddInteractable(std::string name, Interactable *object);
    void ClearInteractables();

    void HandleEvent();
};