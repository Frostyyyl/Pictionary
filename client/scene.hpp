#pragma once

#include "entity.hpp"

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
            obj.get()->HandleEvent();
        }
    }

    void Update()
    {
        for (const auto &obj : SceneObjects)
        {
            obj.get()->Update();
        }
    }
};