#include <string>
#include <functional>

#include "game_manager.hpp"
#include "scenes.hpp"
#include "network_connector.hpp"

// Helper function to create a TextButton
std::shared_ptr<TextButton> CreateTextButton(Scene *scene, int x, int y, int w, int h, const Padding &padding, const std::string &text,
                                             const std::string &filename, std::function<void()> onClick, const std::string &name)
{
    auto txtButton = std::make_shared<TextButton>(x, y, w, h, padding, text, filename, onClick, name);
    scene->AddObject(txtButton);
    GameManager::getInstance().RegisterInteractable(name, txtButton);
    return txtButton;
}

// Helper function to create a TextButton
std::shared_ptr<TextButton> CreateTextButton(Scene *scene, int x, int y, int w, int h, const Padding &padding, const std::string &text,
                                             Uint32 color, std::function<void()> onClick, const std::string &name)
{
    auto txtButton = std::make_shared<TextButton>(x, y, w, h, padding, text, color, onClick, name);
    scene->AddObject(txtButton);
    GameManager::getInstance().RegisterInteractable(name, txtButton);
    return txtButton;
}

// Helper function to create a Button
std::shared_ptr<Button> CreateButton(Scene *scene, int x, int y, int w, int h, const std::string &filename,
                                     std::function<void()> onClick, const std::string &name)
{
    auto button = std::make_shared<Button>(x, y, w, h, filename, onClick, name);
    scene->AddObject(button);
    GameManager::getInstance().RegisterInteractable(name, button);
    return button;
}

// Helper function to create a Button
std::shared_ptr<Button> CreateButton(Scene *scene, int x, int y, int w, int h, Uint32 color,
                                     std::function<void()> onClick, const std::string &name)
{
    auto button = std::make_shared<Button>(x, y, w, h, color, onClick, name);
    scene->AddObject(button);
    GameManager::getInstance().RegisterInteractable(name, button);
    return button;
}

// Helper function to create FixedTextInput
std::shared_ptr<FixedTextInput> CreateFixedTextInput(Scene *scene, int x, int y, int w, int h, int maxSize, const std::string &name)
{
    auto fixedTxtInput = std::make_shared<FixedTextInput>(x, y, w, h, maxSize, name);
    scene->AddObject(fixedTxtInput);
    GameManager::getInstance().RegisterInteractable(name, fixedTxtInput);
    return fixedTxtInput;
}

// Helper function to create TextObject
std::shared_ptr<TextObject> CreateTextObject(Scene *scene, int x, int y, const std::string &text, const std::string &name)
{
    auto txt = std::make_shared<TextObject>(x, y, text, name);
    scene->AddObject(txt);
    return txt;
}

// Helper function to create MessageWindow
std::shared_ptr<MessageWindow> CreateMessageWindow(Scene *scene, GameMode mode, int x, int y, int w, int h, const std::string &name)
{
    auto msgWindow = std::make_shared<MessageWindow>(600, 100, 200, 300);
    scene->AddObject(msgWindow);
    return msgWindow;
}

// Helper function to create a TextInput
std::shared_ptr<TextInput> CreateTextInput(Scene *scene, GameMode gameMode, int x, int y, int w, int h, MessageWindow *msgWindow, const std::string &name)
{
    auto txtInput = std::make_shared<TextInput>(x, y, w, h, msgWindow);
    scene->AddObject(txtInput);

    if (gameMode == GameMode::GUESS)
    {
        GameManager::getInstance().RegisterInteractable(name, txtInput);
    }

    return txtInput;
}

// Helper function to create Canvas
std::shared_ptr<Canvas> CreateCanvas(Scene *scene, GameMode gameMode, const std::string &name)
{
    auto cvs = std::make_shared<Canvas>(name);
    scene->AddObject(cvs);

    if (gameMode == GameMode::DRAW)
    {
        GameManager::getInstance().RegisterInteractable("Canvas", cvs);
    }

    return cvs;
}

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();
    newScene->sceneType = SceneType::MAIN_MENU;

    CreateTextButton(newScene, 600, 400, 80, 80, Padding(24), "Play", "images/button.png", []()
                     { GameManager::getInstance().ChangeCurrentScene(SceneType::LOBBY); }, "PlayButton");

    return newScene;
}

Scene *CreateLobbyScene()
{
    Scene *newScene = new Scene();
    newScene->sceneType = SceneType::LOBBY;

    auto createLobbyButton = CreateTextButton(newScene, 100, 80, 100, 60, Padding(5), "Create Lobby", "images/button.png", [newScene]()
                                              {
            newScene->HideObject("CreateLobbyButton");

            CreateTextObject(newScene, 220, 80, "Enter Lobby Name", "LobbyNameText");
            auto lobbyNameInput = CreateFixedTextInput(newScene, 220, 100, 200, 20, ConnectInfo::MAX_LOBBY_NAME_SIZE, "LobbyNameInput");

            CreateTextObject(newScene, 220, 120, "Add Password", "PasswordText");
            auto passwordInput = CreateFixedTextInput(newScene, 220, 140, 200, 20, ConnectInfo::MAX_LOBBY_PASSWORD_SIZE, "PasswordInput");

            CreateTextObject(newScene, 220, 160, "Enter Player Name", "PlayerNameText");
            auto playerNameInput = CreateFixedTextInput(newScene, 220, 180, 200, 20, ConnectInfo::MAX_CLIENT_NAME_SIZE, "PlayerNameInput");

            CreateTextButton(newScene, 100, 170, 100, 60, Padding(15), "Confirm ", "images/button.png", 
                [newScene, lobbyNameInput, playerNameInput, passwordInput]()
                {
                    if (NetworkConnector::getInstance().CreateLobby(lobbyNameInput->GetText(), playerNameInput->GetText(), passwordInput->GetText()))
                    {
                        GameManager::getInstance().ChangeCurrentScene(SceneType::GAME);
                    }
                    else
                    {
                        newScene->DeleteObject("CreateErrorText");
                        CreateTextObject(newScene, 220, 30, NetworkConnector::getInstance().GetError(), "CreateErrorText");
                    } }, "ConfirmButton");

            CreateTextButton(newScene, 100, 260, 100, 60, Padding(25), "Cancel", "images/button.png", [newScene]()
                {
                    newScene->DeleteObject("ConfirmButton");
                    newScene->DeleteObject("CancelButton");
                    newScene->DeleteObject("CreateErrorText");
                    newScene->DeleteObject("LobbyNameText");
                    newScene->DeleteObject("LobbyNameInput");
                    newScene->DeleteObject("PasswordText");
                    newScene->DeleteObject("PasswordInput");
                    newScene->DeleteObject("PlayerNameText");
                    newScene->DeleteObject("PlayerNameInput");
                    newScene->ShowObject();
                    GameManager::getInstance().ResetCurrentTextInput();
                }, "CancelButton"); }, "CreateLobbyButton");

    CreateTextObject(newScene, 100, 340, "Lobbies", "LobbiesText");

    LobbyInfoList list = NetworkConnector::getInstance().RequestLobbies();

    for (int i = 0; i < list.GetSize(); i++)
    {
        LobbyInfo info = list.GetLobbyInfo(i);

        CreateTextButton(newScene, 100 + 120 * (i % 4), 365 + 80 * (i / 4), 100, 60, Padding(5), info.GetLobbyName(), "images/button.png", [info, newScene]()
                         {
                newScene->DeleteObject("PlayerNameText2");
                newScene->DeleteObject("PlayerNameInput2");
                newScene->DeleteObject("PasswordText2");
                newScene->DeleteObject("PasswordInput2");

                CreateTextObject(newScene, 220, 320, "Enter Player Name", "PlayerNameText2");
                auto playerNameInput = CreateFixedTextInput(newScene, 420, 320, 200, 20, ConnectInfo::MAX_CLIENT_NAME_SIZE, "PlayerNameInput2");

                if (info.hasPassword())
                {
                    CreateTextObject(newScene, 220, 340, "Enter Password", "PasswordText2");
                    auto passwordInput = CreateFixedTextInput(newScene, 420, 340, 200, 20, ConnectInfo::MAX_LOBBY_PASSWORD_SIZE, "PasswordInput2");

                    CreateTextButton(newScene, 640, 340, 100, 60, Padding(15), "Confirm ", "images/button.png", [info, newScene, playerNameInput, passwordInput]()
                        {
                            if (NetworkConnector::getInstance().ConnectToLobby(info.GetLobbyName(), playerNameInput->GetText(), passwordInput->GetText()))
                            {
                                GameManager::getInstance().ChangeCurrentScene(SceneType::GAME);
                            }
                            else
                            {
                                newScene->DeleteObject("ConnectErrorText");
                                CreateTextObject(newScene, 200, 240, NetworkConnector::getInstance().GetError(), "ConnectErrorText");
                            } }, "ConfirmButton2");
                }
                else
                {
                    CreateTextButton(newScene, 640, 340, 100, 60, Padding(15), "Confirm ", "images/button.png", [info, newScene, playerNameInput]()
                        {
                            if (NetworkConnector::getInstance().ConnectToLobby(info.GetLobbyName(), playerNameInput->GetText(), "")) // No password
                            {
                                GameManager::getInstance().ChangeCurrentScene(SceneType::GAME);
                            }
                            else
                            {
                                newScene->DeleteObject("ConnectErrorText");
                                CreateTextObject(newScene, 200, 240, NetworkConnector::getInstance().GetError(), "ConnectErrorText");
                            } }, "ConfirmButton2");
                } }, "LobbyButton");
    }

    return newScene;
}

Scene *CreateGameScene()
{
    Scene *newScene = new Scene();

    newScene->sceneType = SceneType::GAME;

    // For now backgrounds have unstable Z-index
    // auto bg = std::make_shared<SpriteRenderer>(0, 0, "images/background.png");
    // newScene->AddObject(bg);

    GameMode gameMode = GameManager::getInstance().GetGameMode();

    auto cvs = CreateCanvas(newScene, gameMode, "Canvas");

    // If the player is the one drawing, add canvas to interactables
    if (gameMode == GameMode::DRAW)
    {

        GameManager::getInstance().RegisterInteractable("Canvas", cvs);

        // This is a little tricky in creating color it's RRGGBBAA
        // But in ChangeColor() it's AABBGGRR (and also FF is solid, 00 is transparent for alpha)
        // MAYBE WE'LL FIX THIS LATER

        CreateButton(newScene, 100, 500, 30, 30, (Uint32)0x00000000, [cvs]()
                     { cvs.get()->ChangeColor(0xff000000); }, "BlackButton");
        CreateButton(newScene, 150, 500, 30, 30, 0xf0f0f000, [cvs]()
                     { cvs.get()->ChangeColor(0xffffffff); }, "WhiteButton");
    }

    auto msgWindow = CreateMessageWindow(newScene, gameMode, 600, 100, 200, 300, "MsgWindow");
    auto txtInput = CreateTextInput(newScene, gameMode, 600, 10, 100, 100, msgWindow.get(), "TextInput");

    // If the player is the one guessing, allow them that and also show button for guessing
    if (GameManager::getInstance().GetGameMode() == GameMode::GUESS)
    {
        SDL_StartTextInput();
        CreateButton(newScene, 720, 10, 30, 30, "images/button.png", [txtInput]()
                     { txtInput->SendMessage(); }, "EnterTextButton");
    }

    return newScene;
}