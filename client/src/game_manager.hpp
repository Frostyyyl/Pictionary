#pragma once

#include "scene.hpp"
#include <SDL2/SDL.h>
#include "interactable.hpp"
#include "player.hpp"
#include "input_manager.hpp"

class GameManager
{
private:
    GameManager();

    SDL_Window *window;
    Scene *currentScene;
    InputManager *inputManager;

public:
    bool wasSceneChanged;
    Player *currentPlayer;
    bool isRunning;

    static SDL_Renderer *renderer;

    static GameManager &getInstance();
    ~GameManager();

    void Init(const char *title, int width, int height);
    void Run();
    void Exit();

    void ChangeCurrentScene(const char *newScene);

    void RegisterInteractable(std::string name, std::shared_ptr<Interactable> interactable);
};
