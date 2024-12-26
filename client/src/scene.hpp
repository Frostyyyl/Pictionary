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

    void DeleteObject(const std::string& name)
    {
        for (const auto& obj : objects) {
            if (obj->GetName() == name) {
                objects.erase(obj);
                break;
            }
        }
    }

    void DeleteScene()
    {
        objects.clear();
    }
};