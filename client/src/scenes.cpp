#include <string>
#include <functional>

#include "game_manager.hpp"
#include "scenes.hpp"
#include "network_connector.hpp"
#include "../../global/src/game_mode.hpp"

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();
    newScene->sceneType = SceneType::MAIN_MENU;

    newScene->CreateTextButton(400, 350, 200, 100, Padding(80), "Play", "images/button.png", []()
                               { GameManager::getInstance().ChangeCurrentScene(SceneType::LOBBY); }, "PlayButton");

    return newScene;
}

Scene *CreateLobbyScene()
{
    Scene *newScene = new Scene();
    newScene->sceneType = SceneType::LOBBY;

    // Main blue background rectangle
    newScene->CreateBackground(0, 40, 1000, 540, Color::YINMN_BLUE, "0");
    // newScene->CreateBackground(0, 20, 1000, 570, Color::YINMN_BLUE, "0");

    // Light blue rectangle (background for lobbies)
    newScene->CreateBackground(20, 60, 600, 500, Color::LIGHT_SKY_BLUE, "1");
    
    // Orange rectangle (background for refresh)
    newScene->CreateBackground(640, 440, 340, 120, Color::ORANGE, "1");


    auto createLobbyButton = newScene->CreateTextButton(100, 80, 100, 60, Padding(5), "Create Lobby", "images/button.png", [newScene]()
                                                        {
            newScene->HideObjects("CreateLobbyButton");

            newScene->CreateTextObject(220, 80, "Enter Lobby Name", "LobbyNameText", 300);
            auto lobbyNameInput = newScene->CreateFixedTextInput(220, 100, 255, 22, ConnectInfo::MAX_LOBBY_NAME_SIZE, "LobbyNameInput");

            newScene->CreateTextObject(220, 120, "Add Password", "PasswordText", 300);
            auto passwordInput = newScene->CreateFixedTextInput(220, 140, 255, 22, ConnectInfo::MAX_LOBBY_PASSWORD_SIZE, "PasswordInput");

            newScene->CreateTextObject(220, 160, "Enter Player Name", "PlayerNameText", 300);
            auto playerNameInput = newScene->CreateFixedTextInput(220, 180, 255, 22, ConnectInfo::MAX_CLIENT_NAME_SIZE, "PlayerNameInput");

            newScene->CreateTextButton(100, 170, 100, 60, Padding(15), "Confirm ", "images/button.png", 
                [newScene, lobbyNameInput, playerNameInput, passwordInput]()
                {
                    if (NetworkConnector::getInstance().CreateLobby(lobbyNameInput->GetText(), playerNameInput->GetText(), passwordInput->GetText()))
                    {
                        GameManager::getInstance().SetPlayerName(playerNameInput->GetText());
                        GameManager::getInstance().ChangeCurrentScene(SceneType::GAME);
                    }
                    else
                    {
                        newScene->DeleteObjects("CreateErrorText");
                        newScene->CreateTextObject(220, 30, NetworkConnector::getInstance().GetError(), "CreateErrorText", 500);
                    } }, "ConfirmButton");

            newScene->CreateTextButton(100, 260, 100, 60, Padding(25), "Cancel", "images/button.png", [newScene]()
                {
                    newScene->DeleteObjects("ConfirmButton");
                    newScene->DeleteObjects("CancelButton");
                    newScene->DeleteObjects("CreateErrorText");
                    newScene->DeleteObjects("LobbyNameText");
                    newScene->DeleteObjects("LobbyNameInput");
                    newScene->DeleteObjects("PasswordText");
                    newScene->DeleteObjects("PasswordInput");
                    newScene->DeleteObjects("PlayerNameText");
                    newScene->DeleteObjects("PlayerNameInput");
                    newScene->ShowObject();
                    GameManager::getInstance().ResetCurrentTextInput();
                }, "CancelButton"); }, "CreateLobbyButton");

    newScene->CreateTextObject(100, 340, "Lobbies", "LobbiesText", 100);

    LobbyInfoList list = NetworkConnector::getInstance().RequestLobbies();

    for (int i = 0; i < list.GetSize(); i++)
    {
        LobbyInfo info = list.GetLobbyInfo(i);

        newScene->CreateTextButton(100 + 120 * (i % 4), 365 + 80 * (i / 4), 100, 60, Padding(5), info.GetLobbyName(), "images/button.png", [info, newScene]()
                                   {
                newScene->DeleteObjects("PlayerNameText2");
                newScene->DeleteObjects("PlayerNameInput2");
                newScene->DeleteObjects("PasswordText2");
                newScene->DeleteObjects("PasswordInput2");

                newScene->CreateTextObject(220, 320, "Enter Player Name", "PlayerNameText2", 300);
                auto playerNameInput = newScene->CreateFixedTextInput(420, 320, 255, 22, ConnectInfo::MAX_CLIENT_NAME_SIZE, "PlayerNameInput2");

                if (info.GetPlayerCount() >= info.MAX_PLAYERS_PER_LOBBY)
                {
                    newScene->DeleteObjects("ConnectErrorText");
                    newScene->CreateTextObject(200, 240, "This lobby is full", "ConnectErrorText", 500);
                }
                else if (info.hasPassword())
                {
                    newScene->CreateTextObject(220, 340, "Enter Password", "PasswordText2", 300);
                    auto passwordInput = newScene->CreateFixedTextInput(420, 340, 255, 22, ConnectInfo::MAX_LOBBY_PASSWORD_SIZE, "PasswordInput2");

                    newScene->CreateTextButton(640, 340, 100, 60, Padding(15), "Confirm ", "images/button.png", [newScene, info, playerNameInput, passwordInput]()
                        {
                            if (NetworkConnector::getInstance().ConnectToLobby(info.GetLobbyName(), playerNameInput->GetText(), passwordInput->GetText()))
                            {
                                GameManager::getInstance().SetPlayerName(playerNameInput->GetText());
                                GameManager::getInstance().ChangeCurrentScene(SceneType::GAME);
                            }
                            else
                            {
                                newScene->DeleteObjects("ConnectErrorText");
                                newScene->CreateTextObject(200, 240, NetworkConnector::getInstance().GetError(), "ConnectErrorText", 500);
                            } }, "ConfirmButton2");
                }
                else
                {
                    newScene->CreateTextButton(640, 340, 100, 60, Padding(15), "Confirm ", "images/button.png", [newScene, info, playerNameInput]()
                        {
                            if (NetworkConnector::getInstance().ConnectToLobby(info.GetLobbyName(), playerNameInput->GetText(), "")) // No password
                            {
                                GameManager::getInstance().SetPlayerName(playerNameInput->GetText());
                                GameManager::getInstance().ChangeCurrentScene(SceneType::GAME);
                            }
                            else
                            {
                                newScene->DeleteObjects("ConnectErrorText");
                                newScene->CreateTextObject(200, 240, NetworkConnector::getInstance().GetError(), "ConnectErrorText", 500);
                            } }, "ConfirmButton2");
                } }, "LobbyButton" + std::to_string(i));
    }

    return newScene;
}

Scene *CreateGameScene()
{
    Scene *newScene = new Scene();

    newScene->sceneType = SceneType::GAME;
    newScene->CreatePlayerNames();

    GameModeInfo info = NetworkConnector::getInstance().RequestGameMode();
    GameMode mode = info.GetGameMode();
    GameManager::getInstance().SetGameMode(mode);

    // For now backgrounds have unstable Z-index
    // auto bg = std::make_shared<SpriteRenderer>(0, 0, "images/background.png");
    // newScene->AddObject(bg);

    auto cvs = newScene->CreateCanvas("Canvas");
    auto msgWindow = newScene->CreateMessageWindow(600, 100, 200, 308, "MsgWindow");
    auto txtInput = newScene->CreateTextInput(600, 100 + 308, 200 - 22, 22, msgWindow.get(), "TextInput");
    newScene->CreateButton(600 + 200 - 22, 100 + 308, 22, 22, "images/button.png", [txtInput]()
                           { txtInput->SendMessage(); }, "EnterTextButton");

    if (NetworkConnector::getInstance().hasPlayerCreatedLobby())
    {
        newScene->CreateTextButton(400, 400, 200, 80, Padding(20), "Start the game!", "images/button.png", [newScene]()
                                   { 
                                    NetworkConnector::getInstance().StartGame();
                                    newScene->DeleteObjects("StartButton"); }, "StartButton");
    }

    // Create objects based on game mode
    switch (mode)
    {
    case GameMode::WAIT_FOR_PLAYERS:
        newScene->CreateForWaitMode();
        break;
    case GameMode::STANDBY:
        newScene->CreateForStandByMode();
        break;
    case GameMode::DRAW:
        newScene->CreateForDrawMode();
        break;
    case GameMode::GUESS:
        newScene->CreateForGuessMode();
        break;
    default:
        break;
    }

    SDL_StartTextInput();

    return newScene;
}