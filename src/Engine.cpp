#include "Engine.h"

#include "SDL3/SDL.h"
#include "Colours.h"
#include "Map.h"
#include "Gui.h"
#include "CustomEvents.h"
#include "Popup.h"
#include "Serialise.h"

constexpr int WINDOW_WIDTH{ 80 };
constexpr int WINDOW_HEIGHT{ 50 };
static constexpr int GUI_HEIGHT{ 7 };
static constexpr int SAVEGAME_VERSION{ 0x0001 };
static const std::string SAVE_FILE{ "game.sav" };

Engine::Engine() : screenWidth(WINDOW_WIDTH), screenHeight(WINDOW_HEIGHT)
{
    InitTcod();
    gui = new Gui(Point(0, screenHeight - GUI_HEIGHT), screenWidth, GUI_HEIGHT, console);
    map = new Map(screenWidth, screenHeight - GUI_HEIGHT, inputHandler, console);
}

Engine::~Engine()
{
    Terminate();
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
}

void Engine::SubscribeToEvents()
{
    EventManager::GetInstance()->Subscribe(EventType::PopupLaunched,
        [&, this](const Event& e)
        {
            auto event = dynamic_cast<const PopupLaunchedEvent&>(e);
            currentPopup = event.popup;
        });
}

void Engine::Run()
{
    Start();

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
    if (gameStatus == STARTUP)
    {
        map->ComputeFov();
    }
    
    gameStatus = IDLE;
    if (currentPopup != nullptr)
    {
        currentPopup->Update();
        if (currentPopup->IsDone())
        {
            delete currentPopup;
            currentPopup = nullptr;
        }
        return; // Skip updating the game while a popup is active
    }

    if (inputHandler.GetKeyCode() == SDLK_ESCAPE)
    {
        Save();
        std::exit(0);
    }

    map->Update();
}

void Engine::Render()
{
    if (currentPopup != nullptr)
    {
        currentPopup->Render(console);
    }
    else
    {
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
    if (gameStatus == GameStatus::DEFEAT)   return;

    Saver saver;
    saver.InitForSave(SAVE_FILE);
    // Save version first
    saver.PutInt(SAVEGAME_VERSION);

    map->Save(saver);
}

void Engine::Start()
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
            // If loading fails, start a new game
            map = new Map(screenWidth, screenHeight - GUI_HEIGHT, inputHandler, console);
            gui = new Gui(Point(0, screenHeight - GUI_HEIGHT), screenWidth, GUI_HEIGHT, console);
            Init();
        }
    }
    else
    {
        // No save file exists, start new game
        Init();
    }
}