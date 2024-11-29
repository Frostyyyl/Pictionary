#pragma once

#include "entity.hpp"
#include <iostream>

class Scene
{
private:
    std::set<std::shared_ptr<Entity>> SceneObjects;

public:
    Scene() {}
    ~Scene() {}

    void AddObject(std::shared_ptr<Entity> newEntity)
    {
        SceneObjects.insert(newEntity);
        std::cout << "SceneObjects size: " << SceneObjects.size() << std::endl; // Debug
    }

    void DeleteObject(Entity entity)
    {
        for (const auto &obj : SceneObjects)
        {
            obj.get()->Delete();
        }
    }

    void HandleEvent()
    {
        for (const auto &obj : SceneObjects)
        {
            if (obj.get()->enabled == true)
                obj.get()->HandleEvent();
        }
    }

    void Update()
    {
        for (const auto &obj : SceneObjects)
        {
            if (obj.get()->enabled == true)
                obj.get()->Update();
        }
    }
};