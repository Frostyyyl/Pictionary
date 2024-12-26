#include "scene.hpp"
#include <iostream>

Scene::Scene() {}
Scene::~Scene() {}

void Scene::Update()
{
    for (const auto &obj : objects)
    {
        obj->Update();
    }

    if (sceneType == SceneType::GAME)
    {
        if (frameCount == 60)
        {
            for (auto it = objects.begin(); it != objects.end();)
            {
                if ((*it)->GetName() == "Player")
                {
                    it = objects.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            PlayerInfoList list = NetworkConnector::getInstance().RequestPlayers();
            for (int i = 0; i < list.GetSize(); i++)
            {
                auto txt = std::make_shared<TextObject>(600, 60 + (20 * i), list.GetPlayer(i).GetPlayerName(), "Player", 200);
                AddObject(txt);
            }

            frameCount = 0;
        }

        frameCount++;
    }
}

void Scene::AddObject(std::shared_ptr<Component> component)
{
    objects.insert(component);
}

void Scene::DeleteObject(const std::string &name)
{
    for (auto it = objects.begin(); it != objects.end();)
    {
        if ((*it)->GetName() == name)
        {
            it = objects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Scene::DeleteObject(const std::shared_ptr<Component>& obj)
{
    objects.erase(obj);
}

void Scene::HideObject(const std::string &name)
{
    for (auto it = objects.begin(); it != objects.end();)
    {
        if ((*it)->GetName() == name)
        {
            hiddenObjects.push_back(*it);
            it = objects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Scene::ShowObject()
{
    if (!hiddenObjects.empty())
    {
        objects.insert(hiddenObjects.back());
        hiddenObjects.pop_back();
    }
}

void Scene::DeleteScene()
{
    objects.clear();
}