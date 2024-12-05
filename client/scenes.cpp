#include "game_manager.hpp"
#include "scenes.hpp"
#include <string>

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "mainMenu";

    TextObject *txt = new TextObject(600, 500, "GAME");
    newScene->AddObject(txt);

    Button *btn = new Button(600, 400, 80, 80, []()
                             { GameManager::getInstance().ChangeCurrentScene("lobby"); });
    newScene->AddObject(btn);

    auto obj = std::shared_ptr<Interactable>(btn);
    GameManager::getInstance().RegisterInteractable("startButton", obj);

    return newScene;
}

Scene *CreateLobbyScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "mainMenu";

    TextObject *txt = new TextObject(300, 50, "LOBBY");
    newScene->AddObject(txt);

    std::string names[4] = {"lobby_1", "lobby_2", "lobby_3", "lobby_4"};
    for (int i = 0; i < 4; i++)
    {
        Button *btn = new Button(100 + 500 * (i % 2), 80 + 300 * (int)(i / 2), 240, 180, []()
                                 { GameManager::getInstance().ChangeCurrentScene("game"); });
        newScene->AddObject(btn);

        auto obj = std::shared_ptr<Interactable>(btn);
        GameManager::getInstance().RegisterInteractable("lobby_" + std::to_string(i + 1), obj);
    }

    return newScene;
}

Scene *CreateGameScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "game";
    Canvas *cvs = new Canvas();
    newScene->AddObject(cvs);

    // if the player is the one drawing, add canvas to interactables
    if (GameManager::getInstance().currentPlayer->GetGameMode() == DRAW)
    {
        auto obj = std::shared_ptr<Interactable>(cvs);
        GameManager::getInstance().RegisterInteractable("canvas", obj);
    }

    MessageWindow *msgWindow = new MessageWindow(600, 100, 200, 300);
    newScene->AddObject(msgWindow);

    TextInput *txtInput = new TextInput(600, 10, 100, 100, msgWindow);
    newScene->AddObject(txtInput);

    // if the player is the one guessing, allow them that and also show button for guess
    if (GameManager::getInstance().currentPlayer->GetGameMode() == GUESS)
    {
        auto objInput = std::shared_ptr<Interactable>(txtInput);
        GameManager::getInstance().RegisterInteractable("textInput", objInput);

        Button *enterButton = new Button(720, 10, 30, 30, [txtInput]()
                                         { txtInput->SendMessage(); });
        newScene->AddObject(enterButton);
        auto btnObj = std::shared_ptr<Interactable>(enterButton);
        GameManager::getInstance().RegisterInteractable("startButton", btnObj);
    }

    return newScene;
}