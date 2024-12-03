#include "game_manager.hpp"
#include "scenes.hpp"

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "mainMenu";
    Button *btn = new Button(600, 400, 80, 80, []()
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

    TextObject *txt = new TextObject();
    newScene->AddObject(txt);

    return newScene;
}