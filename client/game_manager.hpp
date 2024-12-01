#pragma once

#include "scene.hpp"
#include "SDL2/SDL.h"
#include "interactable.hpp"
#include <unordered_map>

class GameManager
{
private:
    GameManager();

    SDL_Window *window;
    Scene *currentScene;

    bool isDrawing;

public:
    SDL_Event event;
    bool isRunning;
    std::unordered_map<std::string, std::shared_ptr<Interactable>> interactables;

    static SDL_Renderer *renderer;

    static GameManager &getInstance();
    ~GameManager();

    void Init(const char *title, int width, int height);
    void Run();
    void Exit();

    void ChangeCurrentScene(const char *newScene);
};
