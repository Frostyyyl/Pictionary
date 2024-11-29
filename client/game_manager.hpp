#pragma once

#include "scene.hpp"
#include "input_manager.hpp"
#include "window.hpp"

class GameManager
{
private:
    GameManager();

    Window window;
    Scene *currentScene;

    InputManager *inputManager;

public:
    bool isRunning;
    static GameManager &getInstance();

    static SDL_Renderer *renderer;

    ~GameManager();

    void Init();
    void Run();
    void Exit();
};
