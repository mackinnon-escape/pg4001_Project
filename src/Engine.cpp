#include "Engine.h"

#include <filesystem>
#include <cassert>

#include "SDL3/SDL.h"

#include "Colours.h"
#include "Map.h"
#include "Gui.h"
#include "CustomEvents.h"
#include "Popup.h"
#include "Serialise.h"
#include "Menu.h"

constexpr int WINDOW_WIDTH{ 80 };
constexpr int WINDOW_HEIGHT{ 50 };
static constexpr int GUI_HEIGHT{ 7 };
static constexpr int SAVEGAME_VERSION{ 0x0001 };
static const std::string SAVE_FILE{ "game.sav" };

Engine::Engine() : screenWidth(WINDOW_WIDTH), screenHeight(WINDOW_HEIGHT)
{
    InitTcod();
}

Engine::~Engine()
{
    Terminate();
    delete currentMenu;
}

void Engine::Terminate()
{
    EventManager::GetInstance()->Reset();
    delete map;
    map = nullptr;
    delete gui;
    gui = nullptr;
}

void Engine::Init()
{
    map->Init(true);
    SubscribeToEvents();
    EventManager::GetInstance()->Publish(MessageEvent("Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.", RED));
    gameStatus = STARTUP;
}

void Engine::SubscribeToEvents()
{
    EventManager::GetInstance()->Subscribe(EventType::PopupLaunched,
        [&, this](const Event& e)
        {
            const auto& event = dynamic_cast<const PopupLaunchedEvent&>(e);
            currentPopup = event.popup;
            gameStatus = POPUP;
        });

    EventManager::GetInstance()->Subscribe(EventType::GameOver,
        [&, this](const Event&)
        {
            GameOver();
        });
}

void Engine::Run()
{
    ShowMenu();

    while (true)
    {
        HandleInput();
        Update();
        Render();
        SDL_PumpEvents();   // Only needed if running on Mac
        context.present(console);
    }
}

void Engine::HandleInput()
{
    inputHandler.ClearKey();
    inputHandler.CheckForEvent(context);
}

void Engine::Update()
{
    switch (gameStatus)
    {
    case MENU:
        GameMenuUpdate();
        break;
    case POPUP:
        currentPopup->Update();
        if (currentPopup->IsDone())
        {
            delete currentPopup;
            currentPopup = nullptr;
            gameStatus = IDLE;
        }
        break;
    case STARTUP:
        map->ComputeFov();
        gameStatus = IDLE;
        break;
    case DEFEAT:
    case IDLE:
        if (inputHandler.GetKeyCode() == SDLK_ESCAPE)
        {
            Save();
            ShowMenu();
            return;
        }
        map->Update();
        break;
    default:
        break;
    }
}

void Engine::GameMenuUpdate()
{
    Menu::MenuItemCode selection = currentMenu->GetMenuSelection(inputHandler);

    switch (selection)
    {
    case Menu::NEW_GAME:
        delete currentMenu;
        currentMenu = nullptr;
        Terminate();
        // Recreate map for new game
        map = new Map(screenWidth, screenHeight - GUI_HEIGHT, inputHandler, console);
        gui = new Gui(Point(0, screenHeight - GUI_HEIGHT), screenWidth, GUI_HEIGHT, console);
        Init();
        break;
    case Menu::CONTINUE:
        delete currentMenu;
        currentMenu = nullptr;
        Load();
        break;
    case Menu::EXIT:
        std::exit(0);
        break;
    default:
        // Don't cleanup if no selection was made
        break;
    }
}

void Engine::Render()
{
    switch (gameStatus)
    {
    case MENU:
        currentMenu->Render(console);
        break;
    case POPUP:
        currentPopup->Render(console);
        break;
    case IDLE:
    case DEFEAT:
        console.clear();

        map->Render();
        gui->Render(inputHandler, *map);
    }
}

// InitTcod() remains unchanged from previous lab
void Engine::InitTcod()
{
    console = tcod::Console{ screenWidth, screenHeight };  // Main console.
    auto tileset = tcod::load_tilesheet("dejavu32x8.png", { 32, 8 }, tcod::CHARMAP_TCOD);

    TCOD_ContextParams params{};
    params.console = console.get(); 
    params.tcod_version = TCOD_COMPILEDVERSION;
    params.columns = screenWidth;
    params.rows = screenHeight;
    params.tileset = tileset.get();
    params.window_title = "Roguelike";
    params.vsync = 1;
    params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
    context = tcod::Context(params);
}

void Engine::Save() const
{
    // Don't save if game has been lost
    if (gameStatus == GameStatus::DEFEAT)
    {
        if (std::filesystem::exists(SAVE_FILE))
        {
            std::filesystem::remove(SAVE_FILE);
        }
        return;
    }

    Saver saver;
    saver.InitForSave(SAVE_FILE);
    // Save version first
    saver.PutInt(SAVEGAME_VERSION);

    map->Save(saver);
}

void Engine::Load()
{
    if (std::filesystem::exists(SAVE_FILE))
    {
        try
        {
            Loader loader;
            loader.LoadFromFile(SAVE_FILE);
            int version = loader.GetInt();
            if (version != SAVEGAME_VERSION)
            {
                throw std::runtime_error("Incompatible save version");
            }

            Terminate();
            SubscribeToEvents();
            map = new Map(screenWidth, screenHeight - GUI_HEIGHT, inputHandler, console);
            gui = new Gui(Point(0, screenHeight - GUI_HEIGHT), screenWidth, GUI_HEIGHT, console);

            map->Load(loader);
            map->ComputeFov();
        }
        catch (const std::exception&)
        {
            assert(false);

            // If loading fails, start a new game
            map = new Map(screenWidth, screenHeight - GUI_HEIGHT, inputHandler, console);
            gui = new Gui(Point(0, screenHeight - GUI_HEIGHT), screenWidth, GUI_HEIGHT, console);
            Init();
        }
        gameStatus = IDLE;
    }
}

void Engine::ShowMenu()
{
    if (currentMenu == nullptr)
    {
        currentMenu = new GameMenu();
    }
    currentMenu->Initialise(IsSaveGameValid());

    gameStatus = MENU;
}

bool Engine::IsSaveGameValid()
{
    if (std::filesystem::exists(SAVE_FILE))
    {
        try
        {
            Loader loader;
            loader.LoadFromFile(SAVE_FILE);
            int version = loader.GetInt();
            if (version == SAVEGAME_VERSION)
            {
                return true;
            }
        }
        catch (const std::exception&)
        {
            // Invalid save file, don't add continue option
        }
    }

    return false;
}

void Engine::GameOver()
{
    gameStatus = DEFEAT;
 }