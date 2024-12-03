#pragma once

#include "component.hpp"
#include <memory>
#include <set>
#include <string>
#include <iostream>

class Scene
{
private:
    std::set<std::shared_ptr<Component>> objects;

public:
    std::string sceneName;

    Scene() {}
    ~Scene() {}
    void Update()
    {
        for (const auto &obj : objects)
        {
            obj.get()->Update();
        }
    }

    void AddObject(Component *component)
    {
        objects.insert(std::shared_ptr<Component>(component));
    }

    void DeleteScene()
    {
        objects.clear();
    }
};