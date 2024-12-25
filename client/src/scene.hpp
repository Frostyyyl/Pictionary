#pragma once

#include <memory>
#include <set>
#include <string>
#include <iostream>

#include "component.hpp"

enum class SceneType
{
    MAIN_MENU,
    LOBBY,
    GAME,
};

class Scene
{
private:
    std::set<std::shared_ptr<Component>> objects;

public:
    SceneType sceneType;

    Scene() {}
    ~Scene() {}
    void Update()
    {
        for (const auto &obj : objects)
        {
            obj.get()->Update();
        }
    }

    void AddObject(std::shared_ptr<Component> component)
    {
        objects.insert(component);
    }

    void DeleteScene()
    {
        objects.clear();
    }
};