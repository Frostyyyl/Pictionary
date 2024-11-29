#include "game_manager.hpp"
#include "scenes.hpp"
#include <SDL2/SDL.h>

const int FRAMES_PER_SECOND = 60;
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND;

SDL_Renderer *GameManager::renderer = nullptr;

GameManager::GameManager()
{
    inputManager = &InputManager::getInstance();
}

GameManager &GameManager::getInstance()
{
    static GameManager INSTANCE;
    return INSTANCE;
}

GameManager::~GameManager() {}

void GameManager::Init()
{

    std::cout << "skibidi" << std::endl;

    window.CreateWindow(800, 600, "Pictionary");
    renderer = window.CreateRenderer();

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    currentScene = CreateMainMenuScene();

    isRunning = true;
}

void GameManager::Run()
{
    Uint32 frameStart;
    int frameTime;

    while (isRunning)
    {
        frameStart = SDL_GetTicks();

        inputManager->HandleEvents();
        SDL_RenderClear(renderer);
        currentScene->Update();

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;

        if (FRAME_DELAY > frameTime)
        {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}

void GameManager::Exit()
{
    isRunning = false;
}
