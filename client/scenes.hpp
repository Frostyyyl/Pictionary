#pragma once

#include "scene.hpp"
#include "components.hpp"

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();
    // Button *btn = new Button(400, 400, 80, 80);
    // newScene->AddObject(btn);
    // GameManager::getInstance().interactables.insert(
    //     std::make_pair("startButton", std::shared_ptr<Interactable>(btn)));

    Canvas *cvs = new Canvas();
    newScene->AddObject(cvs);
    GameManager::getInstance().interactables.insert(
        std::make_pair("canvas", std::shared_ptr<Interactable>(cvs)));
    return newScene;
}