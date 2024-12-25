#include "game_manager.hpp"
#include "scenes.hpp"
#include "network_connector.hpp"
#include <string>

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();
    newScene->sceneType = SceneType::MAIN_MENU;

    // For now backgrounds have unstable Z-index
    // auto bg = std::make_shared<SpriteRenderer>(0, 0, "images/background.png");
    // newScene->AddObject(bg);

    auto txt = std::make_shared<TextObject>(600, 500, "Play");
    newScene->AddObject(txt);

    auto btn = std::make_shared<Button>(600, 400, 80, 80, "images/button.png", []()
                                        { GameManager::getInstance().ChangeCurrentScene(SceneType::LOBBY); });
    newScene->AddObject(btn);

    GameManager::getInstance().RegisterInteractable("PlayButton", btn);

    return newScene;
}

Scene *CreateLobbyScene()
{
    Scene *newScene = new Scene();
    newScene->sceneType = SceneType::LOBBY;

    // For now backgrounds have unstable Z-index
    // auto bg = std::make_shared<SpriteRenderer>(0, 0, "images/background.png");
    // newScene->AddObject(bg);

    auto txt = std::make_shared<TextObject>(100, 30, "Create Lobby", "CreateLobbyText");
    newScene->AddObject(txt);

    // Create lobby functionality
    auto obj = std::make_shared<Button>(100, 80, 100, 60, "images/button.png", [newScene]()                                    
    { 

        auto txt = std::make_shared<TextObject>(100, 140, "Confirm", "ConfirmText");
        newScene->AddObject(txt); 

        // Confirm button
        auto obj = std::make_shared<Button>(100, 160, 100, 60, "images/button.png", [newScene]()                                    
        { 
            if (NetworkConnector::getInstance().CreateLobby("LobbyName", "bob", ""))
            {
                GameManager::getInstance().ChangeCurrentScene(SceneType::GAME);
            }
        }, "ConfirmButton");
        newScene->AddObject(obj);
        GameManager::getInstance().RegisterInteractable("ConfirmLobbyCreationButton", obj);


        txt = std::make_shared<TextObject>(220, 140, "Cancel", "CancelText");
        newScene->AddObject(txt); 

        // Cancel button
        obj = std::make_shared<Button>(220, 160, 100, 60, "images/button.png", [newScene]()                                    
        { 
            // TODO: Hide the creation lobby window
        }, "CancelButton");
        newScene->AddObject(obj);
        GameManager::getInstance().RegisterInteractable("CancelLobbyCreationButton", obj);

    }, "CreateLobbyButton");
    newScene->AddObject(obj);
    GameManager::getInstance().RegisterInteractable("CreateLobbyButton", obj);


    txt = std::make_shared<TextObject>(100, 240, "Lobbies");
    newScene->AddObject(txt);


    // Request list of lobbies
    LobbyInfoList list = NetworkConnector::getInstance().RequestLobbies();
    
    // Display lobbies
    for (int i = 0; i < list.getSize(); i++) // TODO: Change lobbies to display their name
    {
        std::string lobby = list.getLobbyInfo(i).getLobbyName();

        auto obj = std::make_shared<Button>(100 + 120 * (i % 4), 260 + 80 * (i / 4), 100, 60, "images/button.png", [lobby]()
        { 
            // TODO: Change to request password only if lobby has one
            if (NetworkConnector::getInstance().ConnectToLobby(lobby, "alex", ""))
            {
                GameManager::getInstance().ChangeCurrentScene(SceneType::GAME); 
            }
        }, "LobbyButton" + i);
        newScene->AddObject(obj);
        GameManager::getInstance().RegisterInteractable("Lobby" + i, obj);

        auto txt = std::make_shared<TextObject>(100 + 120 * (i % 4), 260 + 80 * (i / 4), lobby, "LobbyText" + i);
        newScene->AddObject(txt);
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
    if (GameManager::getInstance().currentPlayer->GetGameMode() == DRAW)
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
    if (GameManager::getInstance().currentPlayer->GetGameMode() == GUESS)
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