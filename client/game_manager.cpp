#include "game_manager.hpp"
#include "scenes.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "text_manager.hpp"

const int FRAMES_PER_SECOND = 60;
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND;

SDL_Renderer *GameManager::renderer = nullptr;

GameManager::GameManager()
{
    player = nullptr;
}

GameManager &GameManager::getInstance()
{
    static GameManager INSTANCE;
    return INSTANCE;
}

GameManager::~GameManager() {}

void GameManager::Init(const char *title, int width, int height)
{
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        GameManager::getInstance().isRunning = false;
        std::cerr << "ERROR: Game init error" << std::endl;
        exit(1);
    }
    if (IMG_Init(IMG_INIT_PNG) < 0)
    {
        std::cerr << "ERROR: IMG_Init error: " << IMG_GetError() << std::endl;
        isRunning = false;
        exit(1);
    }
    if (TTF_Init() < 0)
    {
        std::cerr << "ERROR: TTF_Init error: " << TTF_GetError() << std::endl;
        isRunning = false;
        exit(1);
    }

    SDL_StartTextInput();

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

    renderer = renderer = SDL_CreateRenderer(window, -1, 0);

    TextManager::getInstance().init();

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    currentScene = CreateMainMenuScene();

    isRunning = true;
    isDrawing = false;
    isTyping = false;
}

void GameManager::Run()
{
    Uint32 frameStart;
    int frameTime;

    while (isRunning)
    {
        frameStart = SDL_GetTicks();

        SDL_RenderClear(renderer);

        HandleInput();

        currentScene->Update();

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;

        if (FRAME_DELAY > frameTime)
        {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}

void GameManager::HandleInput()
{
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT)
    {
        Exit();
        return;
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        // Canvas
        if (currentScene->sceneName == "game")
        {
            isDrawing = true;
            interactables["canvas"].get()->HandleEvent(SDL_MOUSEBUTTONDOWN);
        }
        else
        {
            // Button
            std::shared_ptr<Button> btnShared = std::dynamic_pointer_cast<Button>(GameManager::interactables["startButton"]);

            if (btnShared)
            {
                Button *btn = btnShared.get();
                btn->HandleEvent(SDL_MOUSEBUTTONDOWN);
            }
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP && currentScene->sceneName == "game")
    {
        isDrawing = false;
        interactables["canvas"].get()->HandleEvent(SDL_MOUSEBUTTONUP);
    }
    else if (event.type == SDL_MOUSEMOTION && currentScene->sceneName == "game")
    {
        if (isDrawing)
        {
            interactables["canvas"].get()->HandleEvent(SDL_MOUSEMOTION);
        }
    }
    else if (event.type == SDL_TEXTINPUT)
    {
        std::cout << event.text.text << std::endl;
    }
}

void GameManager::ChangeCurrentScene(const char *newScene)
{
    if (strcmp(newScene, "game") == 0)
    {
        currentScene = CreateGameScene();
        player = new Player(1);
        player->ChangeGameMode(DRAW);
    }
}

void GameManager::Exit()
{
    isRunning = false;
    SDL_StopTextInput();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}