#include "game_manager.hpp"
#include "scenes.hpp"
#include "network_connector.hpp"
#include <string>

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "mainMenu";

    // For now backgrounds have unstable Z-index
    // auto bg = std::make_shared<SpriteRenderer>(0, 0, "images/background.png");
    // newScene->AddObject(bg);

    auto txt = std::make_shared<TextObject>(600, 500, "GAME");
    newScene->AddObject(txt);

    auto btn = std::make_shared<Button>(600, 400, 80, 80, "images/button.png", []()
                                        { GameManager::getInstance().ChangeCurrentScene("lobby"); });
    newScene->AddObject(btn);

    GameManager::getInstance().RegisterInteractable("startButton", btn);

    return newScene;
}

Scene *CreateLobbyScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "lobby";

    // For now backgrounds have unstable Z-index
    // auto bg = std::make_shared<SpriteRenderer>(0, 0, "images/background.png");
    // newScene->AddObject(bg);

    auto txt = std::make_shared<TextObject>(100, 30, "CREATE LOBBY");
    newScene->AddObject(txt);

    // Create lobby functionality
    auto obj = std::make_shared<Button>(100, 80, 100, 60, "images/button.png", [newScene]()                                    
    { 

        auto txt = std::make_shared<TextObject>(100, 140, "CONFIRM");
        newScene->AddObject(txt); 

        // Confirm button
        auto obj = std::make_shared<Button>(100, 160, 100, 60, "images/button.png", [newScene]()                                    
        { 
            NetworkConnector::getInstance().CreateLobby("12345678901234567890123456789012", "");
        });
        newScene->AddObject(obj);
        GameManager::getInstance().RegisterInteractable("createLobbyConfirmButton", obj);


        txt = std::make_shared<TextObject>(220, 140, "CANCEL");
        newScene->AddObject(txt); 

        // Cancel button
        obj = std::make_shared<Button>(220, 160, 100, 60, "images/button.png", [newScene]()                                    
        { 
        });
        newScene->AddObject(obj);
        GameManager::getInstance().RegisterInteractable("createLobbyCancelButton", obj);

    });
    newScene->AddObject(obj);
    GameManager::getInstance().RegisterInteractable("createLobbyButton", obj);


    txt = std::make_shared<TextObject>(100, 240, "LOBBIES");
    newScene->AddObject(txt);

    // Inicialize connection with server
    NetworkConnector::getInstance().Init(1100, "127.0.0.1");

    // Request list of lobbies
    LobbiesList list = NetworkConnector::getInstance().RequestLobbies();
    
    // Display lobbies
    for (int i = 0; i < list.size; i++) // TODO: Change lobbies to display their name
    {
        auto obj = std::make_shared<Button>(100 + 120 * (i % 4), 260 + 80 * (i / 4), 100, 60, "images/button.png", []()
                                            { GameManager::getInstance().ChangeCurrentScene("game"); });
        newScene->AddObject(obj);
        GameManager::getInstance().RegisterInteractable(list.lobbies[i].name.data(), obj);

        auto txt = std::make_shared<TextObject>(100 + 120 * (i % 4), 260 + 80 * (i / 4), list.lobbies[i].name.data());
        newScene->AddObject(txt);
    }

    return newScene;
}

Scene *CreateGameScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "game";

    // For now backgrounds have unstable Z-index
    // auto bg = std::make_shared<SpriteRenderer>(0, 0, "images/background.png");
    // newScene->AddObject(bg);

    auto cvs = std::make_shared<Canvas>();
    newScene->AddObject(cvs);

    // if the player is the one drawing, add canvas to interactables
    if (GameManager::getInstance().currentPlayer->GetGameMode() == DRAW)
    {
        GameManager::getInstance().RegisterInteractable("canvas", cvs);

        // This is a little tricky in creating color it's RRGGBBAA
        // But in ChangeColor() it's AABBGGRR (and also FF is solid, 00 is transparent for alpha)
        // MAYBE WE'LL FIX THIS LATER
        auto blackButton = std::make_shared<Button>(100, 500, 30, 30, 0x00000000, [cvs]()
                                                    { cvs.get()->ChangeColor(0xff000000); });

        newScene->AddObject(blackButton);
        GameManager::getInstance().RegisterInteractable("blackButton", blackButton);

        auto whiteButton = std::make_shared<Button>(150, 500, 30, 30, 0xf0f0f000, [cvs]()
                                                    { cvs.get()->ChangeColor(0xffffffff); });

        newScene->AddObject(whiteButton);
        GameManager::getInstance().RegisterInteractable("whiteButton", whiteButton);
    }

    auto msgWindow = std::make_shared<MessageWindow>(600, 100, 200, 300);
    newScene->AddObject(msgWindow);

    auto txtInput = std::make_shared<TextInput>(600, 10, 100, 100, msgWindow.get());
    newScene->AddObject(txtInput);

    // if the player is the one guessing, allow them that and also show button for guess
    if (GameManager::getInstance().currentPlayer->GetGameMode() == GUESS)
    {
        SDL_StartTextInput();

        GameManager::getInstance().RegisterInteractable("textInput", txtInput);

        auto enterButton = std::make_shared<Button>(720, 10, 30, 30, "images/button.png", [txtInput]()
                                                    { txtInput->SendMessage(); });
        newScene->AddObject(enterButton);
        GameManager::getInstance().RegisterInteractable("enterButton", enterButton);
    }

    return newScene;
}