#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "game_manager.hpp"
#include "scenes.hpp"
#include "text_manager.hpp"
#include "network_connector.hpp"

const int FRAMES_PER_SECOND = 60;
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND;

SDL_Renderer *GameManager::renderer = nullptr;

GameManager::GameManager()
{
    currentPlayer = nullptr;
    inputManager = nullptr;
    wasSceneChanged = false;
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

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, 0);

    TextManager::getInstance().init();

    inputManager = new InputManager(this);

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

        SDL_RenderClear(renderer);

        currentScene->Update();

        SDL_RenderPresent(renderer);

        inputManager->HandleEvent();

        if (currentScene->sceneType == SceneType::GAME)
        {

        }

        if (wasSceneChanged)
        {
            wasSceneChanged = false;
        }
        // Stop running if quitted
        if (!isRunning)
            return;

        frameTime = SDL_GetTicks() - frameStart;

        if (FRAME_DELAY > frameTime)
        {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}

void GameManager::ChangeCurrentScene(SceneType newScene)
{

    if (newScene == SceneType::GAME)
    {
        // For now this causes segmentation fault in inputManager, maybe it's not important to have
        inputManager->ClearInteractables();

        // BE CAREFUL WITH THIS FUNCTION FOR NOW
        currentScene->DeleteScene();

        currentPlayer = new Player(1, "player");
        currentPlayer->ChangeGameMode(DRAW);

        currentScene = CreateGameScene();
    }
    if (newScene == SceneType::LOBBY)
    {
        inputManager->ClearInteractables();
        currentScene->DeleteScene();

        // Inicialize connection with server
        if (!NetworkConnector::getInstance().isInitialized()){
            NetworkConnector::getInstance().Init(1100, "127.0.0.1");
        }
        
        currentScene = CreateLobbyScene();
    }
    wasSceneChanged = true;
}

void GameManager::RegisterInteractable(std::string name, std::shared_ptr<Interactable> interactable)
{
    inputManager->AddInteractable(name, interactable);
}

void GameManager::Exit()
{
    isRunning = false;
    SDL_StopTextInput();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}