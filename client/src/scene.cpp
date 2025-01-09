#include <iostream>

#include "scene.hpp"
#include "game_manager.hpp"

#include <chrono>

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
    // NOTE: For now update is split into parts
    // Firstly update objects with numeric names (background objects)
    for (const auto &obj : objects)
    {
        if (obj->GetName() == "0")
        {
            obj->Update();
        }
    }

    for (const auto &obj : objects)
    {
        if (obj->GetName() == "1")
        {
            obj->Update();
        }
    }

    for (const auto &obj : objects)
    {
        if (obj->GetName() == "2")
        {
            obj->Update();
        }
    }

    // Secondly update the rest
    for (const auto &obj : objects)
    {
        const std::string &name = obj->GetName();
        if (!std::all_of(name.begin(), name.end(), ::isdigit)) // Check if all characters are digits
        {
            obj->Update();
        }
    }

    // Lastly update the rest of numeric objects
    for (const auto &obj : objects)
    {
        const std::string &name = obj->GetName();
        if (std::all_of(name.begin(), name.end(), ::isdigit)) // Check if all characters are digits
        {
            if (name != "0" && name != "1" && name != "2")
            {
                obj->Update();
            }
        }
    }

    if (sceneType == SceneType::GAME)
    {
        GameMode mode = GameManager::getInstance().GetGameMode();

        // When a second passes reset frame count
        if (frameCount == FRAMES_PER_SECOND)
        {
            frameCount = 0;
        }

        // NOTE: Split updates dependant on exchanging data with the server
        // to different frames to prevent lag

        // NOTE: Make sure updates happen before frame count is reset

        if (mode == GameMode::STANDBY || mode == GameMode::GUESS)
        {
            if ((frameCount % (CanvasChangeInfoList::MAX_CANVAS_CHANGES)) - 25 == 0) // Every MAX_CANVAS_CHANGES frames after 25 frames
            {
                ReadChanges();
            }
            UpdateCanvas(); // Every frame
        }

        if (mode != GameMode::WAIT_FOR_PLAYERS)
        {
            if ((frameCount % (FRAMES_PER_SECOND / 2)) - 50 == 0) // Every half a second after 50 frames
            {
                if (UpdateTime())
                {
                    UpdateGameMode(GameMode::BETWEEN_ROUNDS);
                }
            }
        }

        if ((frameCount % (FRAMES_PER_SECOND)) - 100 == 0) // Every second after 100 frames
        {
            UpdateChat();
        }

        if (frameCount % (FRAMES_PER_SECOND) == 0) // Every second
        {
            UpdatePlayers();

            if (mode == GameMode::WAIT_FOR_PLAYERS || mode == GameMode::BETWEEN_ROUNDS)
            {
                UpdateGameMode();
            }
        }

        frameCount++;
    }
}

void Scene::UpdatePlayers()
{
    DeleteObjects("Player");
    CreatePlayerNames();
}

void Scene::UpdateGameMode(GameMode mode)
{
    GameMode prevMode = GameManager::getInstance().GetGameMode();
    GameManager::getInstance().SetGameMode(mode);

    if (mode != prevMode)
    {
        // Delete objects and remove from interacables
        switch (prevMode)
        {
        case GameMode::DRAW:
            GameManager::getInstance().RemoveInteractable("Canvas");
            GameManager::getInstance().StopDrawing();
            DeleteObjects("WhiteButton");
            DeleteObjects("BlackButton");
            DeleteObjects("PromptButton0");
            DeleteObjects("PromptButton1");
            DeleteObjects("PromptButton2");
            std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                ->ChangeColor(Color::ABGR_BLACK);
            NetworkConnector::getInstance().UploadCanvasChange(CanvasChangeInfo(CanvasChangeInfo::Type::CLEAR));
            break;
        case GameMode::GUESS:
            GameManager::getInstance().RemoveInteractable("TextInput");
            GameManager::getInstance().ResetCurrentTextInput();
            std::static_pointer_cast<TextInput>(GetObject("TextInput"))
                ->FlushText();
            GameManager::getInstance().RemoveInteractable("EnterTextButton");
            break;
        case GameMode::STANDBY:
            DeleteObjects("GameModeMessage");
            break;
        case GameMode::WAIT_FOR_PLAYERS:
            DeleteObjects("GameModeMessage");
            break;
        default:
            break;
        }

        // Create objects
        switch (mode)
        {
        case GameMode::WAIT_FOR_PLAYERS:
            CreateForWaitMode();
            break;
        case GameMode::STANDBY:
            changes = CanvasChangeInfoList();
            CreateForStandByMode();
            break;
        case GameMode::DRAW:
            CreateForDrawMode();
            break;
        case GameMode::GUESS:
            changes = CanvasChangeInfoList();
            GameManager::getInstance().RegisterInteractable("TextInput", std::static_pointer_cast<Interactable>(GetObject("TextInput")));
            GameManager::getInstance().RegisterInteractable("EnterTextButton", std::static_pointer_cast<Interactable>(GetObject("EnterTextButton")));
            CreateForGuessMode();
            break;
        case GameMode::BETWEEN_ROUNDS:
            std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                ->ClearCanvas();
            DeleteObjects("Prompt");
            break;
        default:
            break;
        }
    }
}

void Scene::UpdateGameMode()
{
    GameModeInfo info = NetworkConnector::getInstance().RequestGameMode();
    UpdateGameMode(info.GetGameMode());
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

bool Scene::UpdateTime()
{
    bool value = false;
    TimeInfo time = NetworkConnector::getInstance().RequestTime();
    timeCount = time.GetTime();

    if (timeCount < 0)
    {
        timeCount = 0;
        value = true;
    }

    std::string seconds = std::to_string(timeCount % 60);
    if (seconds.size() < 2)
    {
        seconds = "0" + seconds;
    }

    DeleteObjects("Time");
    CreateTextObject(800, 0, std::to_string(timeCount / 60) + ":" + seconds, "Time", 200);

    return value;
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
        auto player = list.GetPlayer(i);
        CreateTextObject(440, 20 + (20 * i), player.GetPlayerName(), "Player", 200);
        CreateTextObject(640, 20 + (20 * i), std::to_string(player.GetPoints()), "Player", 200);
    }
}

void Scene::CreateForDrawMode()
{
    // This is a little tricky in creating color it's RRGGBBAA
    // But in ChangeColor() it's AABBGGRR (and also FF is solid, 00 is transparent for alpha)
    // MAYBE WE'LL FIX THIS LATER

    PromptsInfoList prompts = NetworkConnector::getInstance().RequestPrompts();
    for (int i = 0; i < PromptsInfoList::MAX_PROMPTS; i++)
    {
        std::string prompt = prompts.GetPrompt(i);
        CreateTextButton(200 + (i * 100), 500, 90, 30, Padding(5), prompt, Color::LIGHT_SKY_BLUE, [this, prompt]()
                         { 
                            NetworkConnector::getInstance().UploadPrompt(prompt);
                            DeleteObjects("PromptButton0");
                            DeleteObjects("PromptButton1");
                            DeleteObjects("PromptButton2");
                            CreateTextObject(0, 0, prompt, "Prompt", 400);
                            CreateButton(100, 500, 30, 30, Color::WHITE, [this]()
                                        { std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                                            ->ChangeColor(Color::ABGR_WHITE); }, "WhiteButton");
                            CreateButton(150, 500, 30, 30, Color::BLACK, [this]()
                                        { std::static_pointer_cast<Canvas>(GetObject("Canvas"))
                                            ->ChangeColor(Color::ABGR_BLACK); }, "BlackButton");
                            GameManager::getInstance().RegisterInteractable("Canvas", std::static_pointer_cast<Interactable>(GetObject("Canvas"))); }, "PromptButton" + std::to_string(i));
    }
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

std::shared_ptr<TextButton> Scene::CreateTextButton(int x, int y, int w, int h, const Padding &padding, const std::string &text,
                                                    const std::string &filename, std::function<void()> onClick, const std::string &name)
{
    auto txtButton = std::make_shared<TextButton>(x, y, w, h, padding, text, filename, onClick, name);
    AddObject(txtButton);
    GameManager::getInstance().RegisterInteractable(name, txtButton);
    return txtButton;
}

std::shared_ptr<TextButton> Scene::CreateTextButton(int x, int y, int w, int h, const Padding &padding, const std::string &text,
                                                    Uint32 color, std::function<void()> onClick, const std::string &name)
{
    auto txtButton = std::make_shared<TextButton>(x, y, w, h, padding, text, color, onClick, name);
    AddObject(txtButton);
    GameManager::getInstance().RegisterInteractable(name, txtButton);
    return txtButton;
}

std::shared_ptr<Button> Scene::CreateButton(int x, int y, int w, int h, const std::string &filename,
                                            std::function<void()> onClick, const std::string &name)
{
    auto button = std::make_shared<Button>(x, y, w, h, filename, onClick, name);
    AddObject(button);
    GameManager::getInstance().RegisterInteractable(name, button);
    return button;
}

std::shared_ptr<Button> Scene::CreateButton(int x, int y, int w, int h, Uint32 color,
                                            std::function<void()> onClick, const std::string &name)
{
    auto button = std::make_shared<Button>(x, y, w, h, color, onClick, name);
    AddObject(button);
    GameManager::getInstance().RegisterInteractable(name, button);
    return button;
}

std::shared_ptr<FixedTextInput> Scene::CreateFixedTextInput(int x, int y, int w, int h, int maxSize, const std::string &name)
{
    CreateBackground(x, y, w, h, Color::CORAL_PINK, "2");
    auto fixedTxtInput = std::make_shared<FixedTextInput>(x + 5, y, w, h, maxSize, name);
    AddObject(fixedTxtInput);
    GameManager::getInstance().RegisterInteractable(name, fixedTxtInput);
    return fixedTxtInput;
}

std::shared_ptr<TextObject> Scene::CreateTextObject(int x, int y, const std::string &text, const std::string &name, int wrapLength)
{
    auto txt = std::make_shared<TextObject>(x, y, text, name, wrapLength);
    AddObject(txt);
    return txt;
}

std::shared_ptr<MessageWindow> Scene::CreateMessageWindow(int x, int y, int w, int h, const std::string &name)
{
    CreateBackground(x, y, w, h, Color::MUSTARD, "");
    auto msgWindow = std::make_shared<MessageWindow>(x, y, w, h, name);
    AddObject(msgWindow);
    return msgWindow;
}

std::shared_ptr<TextInput> Scene::CreateTextInput(int x, int y, int w, int h, MessageWindow *msgWindow, const std::string &name)
{
    CreateBackground(x, y, w, h, Color::LIGHT_SKY_BLUE, "");
    auto txtInput = std::make_shared<TextInput>(x, y, w, h, msgWindow, name);
    AddObject(txtInput);

    if (GameManager::getInstance().GetGameMode() == GameMode::GUESS)
    {
        GameManager::getInstance().RegisterInteractable(name, txtInput);
    }

    return txtInput;
}

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

std::shared_ptr<Background> Scene::CreateBackground(int x, int y, int w, int h, Uint32 color, const std::string &name)
{
    auto obj = std::make_shared<Background>(x, y, w, h, color, name);
    AddObject(obj);

    return obj;
}