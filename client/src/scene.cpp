#include <iostream>

#include "scene.hpp"
#include "game_manager.hpp"

Scene::Scene() {}
Scene::~Scene() {}

std::shared_ptr<Component> Scene::GetObject(const std::string &name)
{
    for (auto it = objects.begin(); it != objects.end();)
    {
        if ((*it)->GetName() == name)
        {
            return (*it);
        }
        else
        {
            ++it;
        }
    }

    std::cerr << "ERROR: No object: \"" << name << "\" found" << std::endl;
    throw;
}

void Scene::Update()
{
    // FIXME: For now update is split into two parts
    // Firstly update nameless objects (background objects)
    for (const auto &obj : objects)
    {
        if (obj->GetName() == "")
        {
            obj->Update();
        }
    }

    // Secondly update the rest
    for (const auto &obj : objects)
    {
        if (obj->GetName() != "")
        {
            obj->Update();
        }
    }

    GameMode mode = GameManager::getInstance().GetGameMode();

    switch (sceneType)
    {
    case SceneType::GAME:
        if (mode != GameMode::DRAW && mode != GameMode::WAIT_FOR_PLAYERS)
        {
            if (frameCount == CanvasChangeInfoList::MAX_CANVAS_CHANGES)
            {
                ReadChanges();
            }

            UpdateCanvas();
        }

        if (frameCount == FRAMES_PER_SECOND / 2)
        {
            UpdateChat();

            if (mode == GameMode::WAIT_FOR_PLAYERS)
            {
                UpdateGameMode();
            }

            DeleteObjects("Player");
            CreatePlayerNames();
            frameCount = 0;
        }

        frameCount++;
        break;

    default:
        break;
    }
}

void Scene::UpdateGameMode()
{
    GameMode prevMode = GameManager::getInstance().GetGameMode();
    GameMode mode = NetworkConnector::getInstance().RequestGameMode();
    GameManager::getInstance().SetGameMode(mode);

    if (mode != prevMode)
    {
        // Delete objects and remove from interacables
        if (prevMode == GameMode::DRAW)
        {
            GameManager::getInstance().RemoveInteractable("Canvas");
            GameManager::getInstance().StopDrawing();
            DeleteObjects("WhiteButton");
            DeleteObjects("BlackButton");
            std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                ->ChangeColor(Color::ABGR_BLACK);
        }
        else if (prevMode == GameMode::GUESS)
        {
            GameManager::getInstance().RemoveInteractable("TextInput");
            GameManager::getInstance().ResetCurrentTextInput();
            std::static_pointer_cast<TextInput>(GetObject("TextInput"))
                ->FlushText();
            GameManager::getInstance().RemoveInteractable("EnterTextButton");
        }
        else
        {
            DeleteObjects("GameModeMessage");
        }

        // Create objects
        switch (mode)
        {
        case GameMode::WAIT_FOR_PLAYERS:
            CreateForWaitMode();
            break;
        case GameMode::STANDBY:
            CreateForStandByMode();
            break;
        case GameMode::DRAW:
            GameManager::getInstance().RegisterInteractable("Canvas", std::static_pointer_cast<Interactable>(GetObject("Canvas")));
            CreateForDrawMode();
            break;
        case GameMode::GUESS:
            GameManager::getInstance().RegisterInteractable("TextInput", std::static_pointer_cast<Interactable>(GetObject("TextInput")));
            GameManager::getInstance().RegisterInteractable("EnterTextButton", std::static_pointer_cast<Interactable>(GetObject("EnterTextButton")));
            CreateForGuessMode();
            break;
        default:
            break;
        }
    }
}

void Scene::UpdateChat()
{
    ChatInfo chat = NetworkConnector::getInstance().RequestChat();

    if (chat.GetSize() != 0)
    {
        std::static_pointer_cast<MessageWindow>(GetObject("MsgWindow"))
            ->ClearMessages();
    }

    for (int i = 0; i < chat.GetSize(); i++)
    {
        std::static_pointer_cast<MessageWindow>(GetObject("MsgWindow"))
            ->AddMessage(chat.GetMessage(i).GetPlayerName() + ": " + chat.GetMessage(i).GetText());
    }
}

void Scene::UpdateCanvas()
{
    if (changes.GetSize() > 0)
    {
        CanvasChangeInfo change = changes.GetCanvasChange();
        switch (change.GetType())
        {
        case CanvasChangeInfo::Type::NONE:
            break;
        case CanvasChangeInfo::Type::CLEAR:
            std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                ->ClearCanvas();
            break;
        case CanvasChangeInfo::Type::LINE:
            std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                ->DrawLine(change.GetX1(), change.GetY1(), change.GetX2(), change.GetY2(),
                           change.GetColor() == CanvasChangeInfo::Color::ABGR_BLACK ? Color::ABGR_BLACK : Color::ABGR_WHITE);
            break;
        case CanvasChangeInfo::Type::CIRCLE:
            std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                ->DrawCircle(change.GetX1(), change.GetY1(), change.GetRadius(),
                             change.GetColor() == CanvasChangeInfo::Color::ABGR_BLACK ? Color::ABGR_BLACK : Color::ABGR_WHITE);
            break;

        default:
            break;
        }
    }
}

void Scene::ReadChanges()
{
    changes = NetworkConnector::getInstance().RequestCanvasChange();
}

void Scene::AddObject(std::shared_ptr<Component> component)
{
    objects.insert(component);
}

void Scene::DeleteObjects(const std::string &name)
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

void Scene::HideObjects(const std::string &name)
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

void Scene::CreatePlayerNames()
{
    PlayerInfoList list = NetworkConnector::getInstance().RequestPlayers();
    for (int i = 0; i < list.GetSize(); i++)
    {
        auto txt = std::make_shared<TextObject>(440, 20 + (20 * i), list.GetPlayer(i).GetPlayerName(), "Player", 200);
        AddObject(txt);
    }
}

void Scene::CreateForDrawMode()
{
    // This is a little tricky in creating color it's RRGGBBAA
    // But in ChangeColor() it's AABBGGRR (and also FF is solid, 00 is transparent for alpha)
    // MAYBE WE'LL FIX THIS LATER

    CreateButton(100, 500, 30, 30, Color::WHITE, [this]()
                 { std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                       ->ChangeColor(Color::ABGR_WHITE); }, "WhiteButton");
    CreateButton(150, 500, 30, 30, Color::BLACK, [this]()
                 { std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                       ->ChangeColor(Color::ABGR_BLACK); }, "BlackButton");
}

void Scene::CreateForGuessMode()
{
}

void Scene::CreateForWaitMode()
{
    CreateTextObject(300, 0, "Wait for others to play", "GameModeMessage", 400);
}

void Scene::CreateForStandByMode()
{
    CreateTextObject(250, 0, "Wait for the round to end to play", "GameModeMessage", 475);
}

// Helper function to create a TextButton
std::shared_ptr<TextButton> Scene::CreateTextButton(int x, int y, int w, int h, const Padding &padding, const std::string &text,
                                                    const std::string &filename, std::function<void()> onClick, const std::string &name)
{
    auto txtButton = std::make_shared<TextButton>(x, y, w, h, padding, text, filename, onClick, name);
    AddObject(txtButton);
    GameManager::getInstance().RegisterInteractable(name, txtButton);
    return txtButton;
}

// Helper function to create a TextButton
std::shared_ptr<TextButton> Scene::CreateTextButton(int x, int y, int w, int h, const Padding &padding, const std::string &text,
                                                    Uint32 color, std::function<void()> onClick, const std::string &name)
{
    auto txtButton = std::make_shared<TextButton>(x, y, w, h, padding, text, color, onClick, name);
    AddObject(txtButton);
    GameManager::getInstance().RegisterInteractable(name, txtButton);
    return txtButton;
}

// Helper function to create a Button
std::shared_ptr<Button> Scene::CreateButton(int x, int y, int w, int h, const std::string &filename,
                                            std::function<void()> onClick, const std::string &name)
{
    auto button = std::make_shared<Button>(x, y, w, h, filename, onClick, name);
    AddObject(button);
    GameManager::getInstance().RegisterInteractable(name, button);
    return button;
}

// Helper function to create a Button
std::shared_ptr<Button> Scene::CreateButton(int x, int y, int w, int h, Uint32 color,
                                            std::function<void()> onClick, const std::string &name)
{
    auto button = std::make_shared<Button>(x, y, w, h, color, onClick, name);
    AddObject(button);
    GameManager::getInstance().RegisterInteractable(name, button);
    return button;
}

// Helper function to create FixedTextInput
std::shared_ptr<FixedTextInput> Scene::CreateFixedTextInput(int x, int y, int w, int h, int maxSize, const std::string &name)
{
    // FIXME: For now fixed text input object's background is achieved using a color button
    CreateButton(x, y, w, h, Color::LIGHT_PINK, []() {}, "");
    auto fixedTxtInput = std::make_shared<FixedTextInput>(x + 5, y, w, h, maxSize, name);
    AddObject(fixedTxtInput);
    GameManager::getInstance().RegisterInteractable(name, fixedTxtInput);
    return fixedTxtInput;
}

// Helper function to create TextObject
std::shared_ptr<TextObject> Scene::CreateTextObject(int x, int y, const std::string &text, const std::string &name, int wrapLength)
{
    auto txt = std::make_shared<TextObject>(x, y, text, name, wrapLength);
    AddObject(txt);
    return txt;
}

// Helper function to create MessageWindow
std::shared_ptr<MessageWindow> Scene::CreateMessageWindow(int x, int y, int w, int h, const std::string &name)
{
    // FIXME: For now fixed text input object's background is achieved using a color button
    CreateButton(x, y, w, h, Color::PUMPKIN, []() {}, "");
    auto msgWindow = std::make_shared<MessageWindow>(x, y, w, h, name);
    AddObject(msgWindow);
    return msgWindow;
}

// Helper function to create a TextInput
std::shared_ptr<TextInput> Scene::CreateTextInput(int x, int y, int w, int h, MessageWindow *msgWindow, const std::string &name)
{
    // FIXME: For now fixed text input object's background is achieved using a color button
    CreateButton(x, y, w, h, Color::DARK_PINK, []() {}, "");
    auto txtInput = std::make_shared<TextInput>(x, y, w, h, msgWindow, name);
    AddObject(txtInput);

    if (GameManager::getInstance().GetGameMode() == GameMode::GUESS)
    {
        GameManager::getInstance().RegisterInteractable(name, txtInput);
    }

    return txtInput;
}

// Helper function to create Canvas
std::shared_ptr<Canvas> Scene::CreateCanvas(const std::string &name)
{
    auto cvs = std::make_shared<Canvas>(name);
    AddObject(cvs);

    if (GameManager::getInstance().GetGameMode() == GameMode::DRAW)
    {
        GameManager::getInstance().RegisterInteractable("Canvas", cvs);
    }

    return cvs;
}