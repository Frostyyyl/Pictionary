#pragma once

#include <memory>
#include <unordered_map>

#include "interactable.hpp"
#include "components.hpp"

class GameManager;

class InputManager
{
private:
    GameManager *gameManager;
    SDL_Event event;
    std::weak_ptr<Canvas> canvas;

    std::unordered_map<std::string, std::weak_ptr<Interactable>> interactables;

    bool isDrawing;
    bool isTyping;

public:
    InputManager(GameManager *manager);
    ~InputManager() {}

    void AddInteractable(const std::string &name, std::weak_ptr<Interactable> object);
    void DeleteInteractable(const std::string &name);
    void ClearInteractables();

    void StopDrawing();
    void HandleEvent();
};