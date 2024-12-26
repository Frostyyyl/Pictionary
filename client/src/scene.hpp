#pragma once

#include <memory>
#include <set>
#include <vector>
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
    std::vector<std::shared_ptr<Component>> hiddenObjects;

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

    void DeleteObject(const std::string &name)
    {
        for (const auto &obj : objects)
        {
            if (obj->GetName() == name)
            {
                objects.erase(obj);
                break;
            }
        }
    }

    void HideObject(const std::string &name)
    {
        for (const auto &obj : objects)
        {
            if (obj->GetName() == name)
            {
                hiddenObjects.push_back(obj);
                objects.erase(obj);
                break;
            }
        }
    }

    void ShowObject()
    {
        objects.insert(hiddenObjects.back());
        hiddenObjects.pop_back();
    }

    std::shared_ptr<Component> GetObject(const std::string &name)
    {
        for (const auto &obj : objects)
        {
            if (obj->GetName() == name)
            {
                return obj;
            }
        }
        std::cerr << "ERROR: Not found object by name: \"" << name << "\"" << std::endl;
        throw;
    }

    void DeleteScene()
    {
        objects.clear();
    }
};