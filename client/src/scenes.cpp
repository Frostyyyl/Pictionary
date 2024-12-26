#include <string>

#include "game_manager.hpp"
#include "scenes.hpp"
#include "network_connector.hpp"

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();
    newScene->sceneType = SceneType::MAIN_MENU;

    // For now backgrounds have unstable Z-index
    // auto bg = std::make_shared<SpriteRenderer>(0, 0, "images/background.png");
    // newScene->AddObject(bg);

    auto obj = std::make_shared<TextButton>(600, 400, 80, 80, Padding(24),
            "Play", "images/button.png", []()
            { 
                GameManager::getInstance().ChangeCurrentScene(SceneType::LOBBY); 
            });
    newScene->AddObject(obj);
    GameManager::getInstance().RegisterInteractable("PlayButton", obj);

    return newScene;
}

Scene *CreateLobbyScene()
{
    Scene *newScene = new Scene();
    newScene->sceneType = SceneType::LOBBY;

    // For now backgrounds have unstable Z-index
    // auto bg = std::make_shared<SpriteRenderer>(0, 0, "images/background.png");
    // newScene->AddObject(bg);

    // Create lobby functionality
    auto obj = std::make_shared<TextButton>(100, 80, 100, 60, Padding(5), "Create Lobby", "images/button.png", [newScene]()
            {
                newScene->HideObject("CreateLobbyButton"); 

                // Lobby name input
                auto txt = std::make_shared<TextObject>(220, 80, "Enter Lobby Name", "LobbyNameText");
                newScene->AddObject(txt);
                auto txtInput = std::make_shared<FixedTextInput>(220, 100, 200, 20, ConnectInfo::MAX_LOBBY_NAME_SIZE, "LobbyNameInput");
                newScene->AddObject(txtInput);
                GameManager::getInstance().RegisterInteractable("LobbyNameInput", txtInput);

                // Password input
                txt = std::make_shared<TextObject>(220, 120, "Add Password", "PasswordText");
                newScene->AddObject(txt);
                txtInput = std::make_shared<FixedTextInput>(220, 140, 200, 20, ConnectInfo::MAX_LOBBY_PASSWORD_SIZE, "PasswordInput");
                newScene->AddObject(txtInput);
                GameManager::getInstance().RegisterInteractable("PasswordInput", txtInput);

                // Player name input
                txt = std::make_shared<TextObject>(220, 160, "Enter Player Name", "PlayerNameText");
                newScene->AddObject(txt);
                txtInput = std::make_shared<FixedTextInput>(220, 180, 200, 20, ConnectInfo::MAX_CLIENT_NAME_SIZE, "PlayerNameInput");
                newScene->AddObject(txtInput);
                GameManager::getInstance().RegisterInteractable("PlayerNameInput", txtInput);

                // Confirm button
                auto obj = std::make_shared<TextButton>(100, 170, 100, 60, Padding(15), "Confirm ", "images/button.png", [newScene]()
                        { 
                            if (
                                NetworkConnector::getInstance().CreateLobby(
                                std::static_pointer_cast<FixedTextInput>(newScene->GetObject("LobbyNameInput"))->GetText(),     
                                std::static_pointer_cast<FixedTextInput>(newScene->GetObject("PlayerNameInput"))->GetText(), 
                                std::static_pointer_cast<FixedTextInput>(newScene->GetObject("PasswordInput"))->GetText())
                                )
                            {
                                GameManager::getInstance().ChangeCurrentScene(SceneType::GAME);
                            } 
                            else 
                            {
                                newScene->DeleteObject("CreateErrorText");

                                auto txt = std::make_shared<TextObject>(220, 30, NetworkConnector::getInstance().GetError(), "CreateErrorText");
                                newScene->AddObject(txt);
                            } 
                        }, "ConfirmButton");
                newScene->AddObject(obj);
                GameManager::getInstance().RegisterInteractable("ConfirmButton", obj);

                // Cancel button
                obj = std::make_shared<TextButton>(100, 260, 100, 60, Padding(25), "Cancel", "images/button.png", [newScene]()
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
                        },
                        "CancelButton");

                newScene->AddObject(obj);
                GameManager::getInstance().RegisterInteractable("CancelButton", obj);

            }, "CreateLobbyButton");

    newScene->AddObject(obj);
    GameManager::getInstance().RegisterInteractable("CreateLobbyButton", obj);

    auto txt = std::make_shared<TextObject>(100, 340, "Lobbies");
    newScene->AddObject(txt);

    // Request list of lobbies
    LobbyInfoList list = NetworkConnector::getInstance().RequestLobbies();

    // Display lobbies
    for (int i = 0; i < list.GetSize(); i++)
    {
        LobbyInfo info = list.GetLobbyInfo(i);

        auto obj = std::make_shared<TextButton>(100 + 120 * (i % 4), 365 + 80 * (i / 4), 100, 60, Padding(5), info.GetLobbyName(), "images/button.png", [info, newScene]()
                { 
                    newScene->DeleteObject("PlayerNameText2");
                    newScene->DeleteObject("PlayerNameInput2");
                    newScene->DeleteObject("PasswordText2");
                    newScene->DeleteObject("PasswordInput2");


                    // Player name input
                    auto txt = std::make_shared<TextObject>(220, 320, "Enter Player Name", "PlayerNameText2");
                    newScene->AddObject(txt); 
                    auto txtInput = std::make_shared<FixedTextInput>(420, 320, 200, 20, 
                        ConnectInfo::MAX_CLIENT_NAME_SIZE, "PlayerNameInput2");
                    newScene->AddObject(txtInput);
                    GameManager::getInstance().RegisterInteractable("PlayerNameInput2", txtInput);


                    // Password input
                    if (info.hasPassword())
                    {
                        txt = std::make_shared<TextObject>(220, 340, "Enter Password", "PasswordText2");
                        newScene->AddObject(txt); 
                        txtInput = std::make_shared<FixedTextInput>(420, 340, 200, 20, 
                            ConnectInfo::MAX_LOBBY_PASSWORD_SIZE, "PasswordInput2");
                        newScene->AddObject(txtInput);
                        GameManager::getInstance().RegisterInteractable("PasswordInput2", txtInput);
                    }


                    // Confirm button
                    auto obj = std::make_shared<TextButton>(640, 340, 100, 60, Padding(15), "Confirm ", "images/button.png", [info, newScene]()
                            { 
                                if (NetworkConnector::getInstance().ConnectToLobby(info.GetLobbyName(),
                                    std::static_pointer_cast<FixedTextInput>(newScene->GetObject("PlayerNameInput2"))->GetText(),
                                    info.hasPassword() ? std::static_pointer_cast<FixedTextInput>(newScene->GetObject("PasswordInput2"))->GetText() : ""))
                                {
                                    GameManager::getInstance().ChangeCurrentScene(SceneType::GAME); 
                                } else {
                                    newScene->DeleteObject("ConnectErrorText");
                                    auto txt = std::make_shared<TextObject>(200, 240, NetworkConnector::getInstance().GetError(), "ConnectErrorText");
                                    newScene->AddObject(txt);
                                } 
                            }, 
                            "ConfirmButton2");

                    newScene->AddObject(obj);
                    GameManager::getInstance().RegisterInteractable("ConfirmButton2", obj);
                    
                }, "LobbyButton" + i);

        newScene->AddObject(obj);
        GameManager::getInstance().RegisterInteractable("LobbyButton" + i, obj);
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

    // TODO: Replace this button with a place to
    // auto obj = std::make_shared<Button>(100 + 120 * (i % 4), 260 + 80 * (i / 4), 100, 60, "images/button.png", [list, i]()
    // {

    // }

    auto cvs = std::make_shared<Canvas>("Canvas");
    newScene->AddObject(cvs);

    // if the player is the one drawing, add canvas to interactables
    if (GameManager::getInstance().GetGameMode() == GameMode::DRAW)
    {
        GameManager::getInstance().RegisterInteractable("Canvas", cvs);

        // This is a little tricky in creating color it's RRGGBBAA
        // But in ChangeColor() it's AABBGGRR (and also FF is solid, 00 is transparent for alpha)
        // MAYBE WE'LL FIX THIS LATER
        auto blackButton = std::make_shared<Button>(100, 500, 30, 30, 0x00000000, [cvs]()
                                                    { cvs.get()->ChangeColor(0xff000000); });

        newScene->AddObject(blackButton);
        GameManager::getInstance().RegisterInteractable("BlackButton", blackButton);

        auto whiteButton = std::make_shared<Button>(150, 500, 30, 30, 0xf0f0f000, [cvs]()
                                                    { cvs.get()->ChangeColor(0xffffffff); });

        newScene->AddObject(whiteButton);
        GameManager::getInstance().RegisterInteractable("WhiteButton", whiteButton);
    }

    auto msgWindow = std::make_shared<MessageWindow>(600, 100, 200, 300);
    newScene->AddObject(msgWindow);

    auto txtInput = std::make_shared<TextInput>(600, 10, 100, 100, msgWindow.get());
    newScene->AddObject(txtInput);

    // if the player is the one guessing, allow them that and also show button for guess
    if (GameManager::getInstance().GetGameMode() == GameMode::GUESS)
    {
        SDL_StartTextInput();

        GameManager::getInstance().RegisterInteractable("TextInput", txtInput);

        auto enterButton = std::make_shared<Button>(720, 10, 30, 30, "images/button.png", [txtInput]()
                                                    { txtInput->SendMessage(); });
        newScene->AddObject(enterButton);
        GameManager::getInstance().RegisterInteractable("EnterTextButton", enterButton);
    }

    return newScene;
}