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

    newScene->CreateBackground(0, 0, 1000, 600, Color::YINMN_BLUE, "0");
    newScene->CreateBackground(20, 20, 960, 560, Color::LIGHT_SKY_BLUE, "1");

    // Title
    int unit = 38;
    int gap = 15;
    Position startPos = {47, 40};
    // P
    newScene->CreateBackground(startPos.x, startPos.y, unit, unit * 5, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y, unit, unit, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 2, unit, unit, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit * 2, startPos.y, unit, unit * 3, Color::CORAL_PINK, "2");
    // I
    startPos.x += unit * 3 + gap;
    newScene->CreateBackground(startPos.x, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    // C
    startPos.x += unit + gap;
    newScene->CreateBackground(startPos.x, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 2, unit, unit, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 4, unit, unit, Color::CORAL_PINK, "2");
    // T
    startPos.x += unit * 2 + gap;
    newScene->CreateBackground(startPos.x, startPos.y, unit, unit * 5, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 1, unit * 0.5, unit, Color::CORAL_PINK, "2");
    // I
    startPos.x += unit * 1.5 + gap;
    newScene->CreateBackground(startPos.x, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    // O
    startPos.x += unit + gap;
    newScene->CreateBackground(startPos.x, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 2, gap, unit, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 4, gap, unit, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit + gap, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    // N
    startPos.x += unit * 2 + gap * 2;
    newScene->CreateBackground(startPos.x, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 2.75, gap, unit, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit + gap, startPos.y + unit * 3.25, gap, unit, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit + gap * 2, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    // A
    startPos.x += unit * 2 + gap * 3;
    newScene->CreateBackground(startPos.x, startPos.y + unit * 3, unit, unit * 2, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x, startPos.y + unit * 2, unit * 2 + gap, unit * 0.5, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 3, gap, unit * 0.5, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 4.5, gap, unit * 0.5, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit + gap, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    // R
    startPos.x += unit * 2 + gap * 2;
    newScene->CreateBackground(startPos.x, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 2.5, gap, unit, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit + gap, startPos.y + unit * 2, gap, unit, Color::CORAL_PINK, "2");
    // Y
    startPos.x += unit + gap * 3;
    newScene->CreateBackground(startPos.x, startPos.y + unit * 2, unit, unit * 3, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit + gap, startPos.y + unit * 2, unit, unit * 5, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x + unit, startPos.y + unit * 4, gap, unit, Color::CORAL_PINK, "2");
    newScene->CreateBackground(startPos.x, startPos.y + unit * 6, unit * 2 + gap, unit, Color::CORAL_PINK, "2");

    // Play button
    newScene->CreateTextButton(400, 360, 200, 100, Padding(80), "PLAY", Color::BURNT_SIENNA, []()
                               { GameManager::getInstance().ChangeCurrentScene(SceneType::LOBBY); }, "PlayButton");

    return newScene;
}

Scene *CreateLobbyScene()
{
    Scene *newScene = new Scene();
    newScene->sceneType = SceneType::LOBBY;

    // Main blue background rectangle
    newScene->CreateBackground(0, 0, 1000, 600, Color::YINMN_BLUE, "0");

    // Light blue rectangle (background for lobbies)
    newScene->CreateBackground(20, 20, 600, 560, Color::LIGHT_SKY_BLUE, "1");

    // Mustard rectangle (background for creating a lobby)
    newScene->CreateBackground(640, 20, 340, 400, Color::MUSTARD, "1");

    // Orange rectangle (background for refresh)
    newScene->CreateBackground(640, 440, 340, 140, Color::ORANGE, "1");

    // Create a lobby button
    auto createLobbyButton = newScene->CreateTextButton(680, 40, 260, 60, Padding(62), "CREATE  A  LOBBY", Color::BURNT_SIENNA, [newScene]()
                                                        {
            newScene->HideObjects("CreateLobbyButton");

            // Create input fields
            newScene->CreateTextObject(680, 120, "Enter the lobby name:", "LobbyNameText", 300);
            auto lobbyNameInput = newScene->CreateFixedTextInput(680, 145, 260, 25, ConnectInfo::MAX_LOBBY_NAME_SIZE, "LobbyNameInput");

            newScene->CreateTextObject(680, 175, "Pick a nickname:", "PlayerNameText", 300);
            auto playerNameInput = newScene->CreateFixedTextInput(680, 200, 260, 25, ConnectInfo::MAX_CLIENT_NAME_SIZE, "PlayerNameInput");

            newScene->CreateTextObject(680, 230, "Add a password:", "PasswordText", 300);
            auto passwordInput = newScene->CreateFixedTextInput(680, 255, 260, 25, ConnectInfo::MAX_LOBBY_PASSWORD_SIZE, "PasswordInput");

            // Confirm button
            newScene->CreateTextButton(680, 340, 260, 60, Padding(92), "CONFIRM ", Color::BURNT_SIENNA, 
                [newScene, lobbyNameInput, playerNameInput, passwordInput]()
                {
                    if (NetworkConnector::getInstance().CreateLobby(lobbyNameInput->GetText(), playerNameInput->GetText(), passwordInput->GetText()))
                    {
                        GameManager::getInstance().SetPlayerName(playerNameInput->GetText());
                        GameManager::getInstance().SetLobbyName(lobbyNameInput->GetText());
                        GameManager::getInstance().ChangeCurrentScene(SceneType::GAME);
                    }
                    else
                    {
                        newScene->DeleteObjects("CreateErrorText");
                        newScene->CreateTextObject(680, 285, NetworkConnector::getInstance().GetError(), "CreateErrorText", 260);
                    } }, "ConfirmButton");

            // Cancel button
            newScene->CreateTextButton(680, 40, 260, 60, Padding(92), "CANCEL", Color::BURNT_SIENNA, [newScene]()
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
                    newScene->DeleteObjects("2");
                    newScene->ShowObject();
                    GameManager::getInstance().ResetCurrentTextInput();
                }, "CancelButton"); }, "CreateLobbyButton");

    newScene->CreateTextObject(40, 30, "Pick a lobby:", "LobbiesText", 160);

    newScene->CreateLobbyButtons();

    newScene->CreateTextButton(680, 480, 260, 60, Padding(62), "REFRESH  LOBBIES", Color::BURNT_SIENNA, [newScene]()
                               {
        for (size_t i = 0; i < LobbyInfoList::MAX_LOBBIES_PER_PAGE; i++)
        {
            newScene->DeleteObjects("LobbyButton" + std::to_string(i));
        }
        newScene->DeleteObjects("PlayerNameText2");
        newScene->DeleteObjects("PlayerNameInput2");
        newScene->DeleteObjects("PasswordText2");
        newScene->DeleteObjects("PasswordInput2");
        newScene->DeleteObjects("ConfirmButton2");
        newScene->DeleteObjects("ConnectErrorText");

        newScene->DeleteObjects("3"); // Input backgrounds

        newScene->CreateLobbyButtons(); }, "RefreshButton");

    return newScene;
}

Scene *CreateGameScene()
{
    Scene *newScene = new Scene();

    newScene->sceneType = SceneType::GAME;

    auto cvs = newScene->CreateCanvas();

    newScene->CreateBackground(0, 0, 1000, 600, Color::YINMN_BLUE, "0");
    newScene->CreateBackground(20, 20, 280, 405, Color::MUSTARD, "1");
    newScene->CreateBackground(20, 470, 960, 110, Color::ORANGE, "1");
    newScene->CreateBackground(20, 20, 280, 35, Color::ORANGE, "2");

    auto msgWindow = newScene->CreateMessageWindow(700, 20, 280, 405, "MsgWindow");
    auto txtInput = newScene->CreateTextInput(700, 425, 230, 25, msgWindow.get(), "TextInput");
    newScene->CreateTextButton(930, 425, 50, 25, Padding(5, -12), "SEND", Color::BURNT_SIENNA, [txtInput]()
                           { txtInput->SendMessage(); }, "EnterTextButton");

    newScene->CreateTextObject(30, 25, "Players", "PlayersListText", 200);
    newScene->CreateTextObject(240, 25, "Points", "PointsListText", 200);
    newScene->CreatePlayerNames();

    newScene->CreateBackground(65, 425, 235, 25, Color::CORAL_PINK, "2");
    newScene->CreateTextObject(70, 425, GameManager::getInstance().GetLobbyName(), "LobbyNameText", 235);
    newScene->CreateTextButton(20, 425, 45, 25, Padding(5, -12), "EXIT", Color::BURNT_SIENNA, [](){
        NetworkConnector::getInstance().UploadLobbyExit();

        GameManager::getInstance().ChangeCurrentScene(SceneType::LOBBY);
    }, "ExitButton");

    if (NetworkConnector::getInstance().hasPlayerCreatedLobby())
    {
        newScene->CreateTextButton(350, 490, 300, 70, Padding(75), "START  THE  GAME", Color::BURNT_SIENNA, [newScene]()
                                   { 
                                    NetworkConnector::getInstance().StartGame();
                                    newScene->DeleteObjects("StartButton"); }, "StartButton");
    }

    GameModeInfo info = NetworkConnector::getInstance().RequestGameMode();
    GameMode mode = info.GetGameMode();
    GameManager::getInstance().SetGameMode(mode);

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

    return newScene;
}