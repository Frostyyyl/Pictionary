#pragma once

#include "scene.hpp"
#include "components.hpp"

Scene *CreateMainMenuScene()
{
    Scene *newScene = new Scene();

    auto startButton = std::make_shared<Entity>();
    startButton->AddComponent<SpriteRenderer>();
    newScene->AddObject(startButton);
    return newScene;
}