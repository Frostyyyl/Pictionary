#include "game_manager.hpp"
#include "scenes.hpp"

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

    Button *btn = new Button(300, 300, 80, 80, []()
                             { GameManager::getInstance().ChangeCurrentScene("game"); });
    newScene->AddObject(btn);

    auto obj = std::shared_ptr<Interactable>(btn);
    GameManager::getInstance().RegisterInteractable("startButton", obj);

    return newScene;
}

Scene *CreateGameScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "game";
    Canvas *cvs = new Canvas();
    newScene->AddObject(cvs);
    auto obj = std::shared_ptr<Interactable>(cvs);
    GameManager::getInstance().RegisterInteractable("canvas", obj);

    // add here box for messages
    MessageWindow *msgWindow = new MessageWindow(600, 100, 200, 300);
    newScene->AddObject(msgWindow);

    TextInput *txtInput = new TextInput(600, 10, 100, 100, msgWindow);
    newScene->AddObject(txtInput);
    auto objInput = std::shared_ptr<Interactable>(txtInput);
    GameManager::getInstance().RegisterInteractable("textInput", objInput);

    Button *enterButton = new Button(720, 10, 30, 30, [txtInput]()
                                     { txtInput->SendMessage(); });
    newScene->AddObject(enterButton);
    auto btnObj = std::shared_ptr<Interactable>(enterButton);
    GameManager::getInstance().RegisterInteractable("startButton", btnObj);

    return newScene;
}