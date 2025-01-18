#pragma once

#include <SDL2/SDL.h>

#include "scene.hpp"
#include "interactable.hpp"
#include "input_manager.hpp"
#include "../../global/src/game_mode.hpp"

const int FRAMES_PER_SECOND = 400;

class GameManager
{
private:
    GameManager();

    SDL_Window *window;
    Scene *currentScene;
    InputManager *inputManager;
    GameMode mode = GameMode::STANDBY;
    std::string currentTextInput = "";
    std::string playerName = "";
    std::string lobbyName = "";
    
    std::string address = "127.0.0.1";
    int port = 1100;

public:
    CanvasChangeInfoList changes = {};
    bool wasSceneChanged;
    bool isRunning;

    static SDL_Renderer *renderer;

    static GameManager &getInstance();
    ~GameManager();

    void Init(const char *title, int width, int height, const std::string &address, int port);
    void Run();
    void Exit();

    void ChangeCurrentScene(SceneType newScene);

    void RegisterInteractable(const std::string& name, std::shared_ptr<Interactable> interactable);
    void RemoveInteractable(const std::string& name);

    void SetGameMode(GameMode mode) { this->mode = mode; }
    GameMode GetGameMode() { return mode; }

    void StopDrawing() { inputManager->StopDrawing(); }

    void SetCurrentTextInput(const std::string &input) { currentTextInput = input; }
    std::string GetCurrentTextInput() { return currentTextInput; }
    void ResetCurrentTextInput() { currentTextInput = ""; }

    void SetPlayerName(const std::string& name) { playerName = name; }
    std::string GetPlayerName() { return playerName; }
    void SetLobbyName(const std::string& name) { lobbyName = name; }
    std::string GetLobbyName() { return lobbyName; }
};
