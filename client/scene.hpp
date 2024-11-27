#include "entity.hpp"

class Scene
{
private:
    std::set<std::shared_ptr<Entity>> SceneObjects;

public:
    Scene() {}
    ~Scene() {}

    void AddObject(Entity newEntity)
    {
        std::shared_ptr<Entity> ptr(&newEntity);
        SceneObjects.insert(ptr);
    }

    void DeleteObject(Entity entity)
    {
        for (const auto &obj : SceneObjects)
        {
            obj.get()->Delete();
        }
    }
};