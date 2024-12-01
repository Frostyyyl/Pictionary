#pragma once

#include "component.hpp"
#include <memory>
#include <set>

class Scene
{
private:
    std::set<std::shared_ptr<Component>> objects;

public:
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
};