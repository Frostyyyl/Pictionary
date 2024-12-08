#include "game_manager.hpp"
#include "scenes.hpp"
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

    auto txt = std::make_shared<TextObject>(300, 50, "LOBBY");
    newScene->AddObject(txt);

    std::string names[4] = {"lobby_1", "lobby_2", "lobby_3", "lobby_4"};
    for (int i = 0; i < 4; i++)
    {
        auto obj = std::make_shared<Button>(100 + 500 * (i % 2), 80 + 300 * (int)(i / 2), 240, 180, "images/button.png", []()
                                            { GameManager::getInstance().ChangeCurrentScene("game"); });
        newScene->AddObject(obj);
        GameManager::getInstance().RegisterInteractable("lobby_" + std::to_string(i + 1), obj);
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