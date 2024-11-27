#pragma once
#include <SDL2/SDL.h>
#include "scene.hpp"

class GameManager
{
private:
    GameManager() {}
    Scene currentScene;

public:
    static GameManager &getInstance()
    {
        static GameManager INSTANCE;
        return INSTANCE;
    }

    static SDL_Renderer *renderer;
    static SDL_Event event;

    ~GameManager() {}

    void ChangeScene(Scene newScene)
    {
        currentScene = newScene;
    }
};

SDL_Renderer *GameManager::renderer = nullptr;