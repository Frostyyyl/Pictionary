#include "game_manager.hpp"
#include "scenes.hpp"

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "mainMenu";
    Button *btn = new Button(600, 400, 80, 80, []()
                             { GameManager::getInstance().ChangeCurrentScene("game"); });
    newScene->AddObject(btn);
    GameManager::getInstance().interactables.insert(
        std::make_pair("startButton", std::shared_ptr<Interactable>(btn)));

    return newScene;
}

Scene *CreateGameScene()
{
    Scene *newScene = new Scene();
    newScene->sceneName = "game";
    Canvas *cvs = new Canvas();
    newScene->AddObject(cvs);
    GameManager::getInstance().interactables.insert(
        std::make_pair("canvas", std::shared_ptr<Interactable>(cvs)));

    TextObject *txt = new TextObject();
    newScene->AddObject(txt);

    return newScene;
}