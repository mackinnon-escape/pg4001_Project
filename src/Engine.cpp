#include "Engine.h"

#include "SDL3/SDL.h"
#include "Colours.h"
#include "Map.h"
#include "Gui.h"
#include "CustomEvents.h"
#include "Popup.h"

constexpr int WINDOW_WIDTH{ 80 };
constexpr int WINDOW_HEIGHT{ 50 };
static constexpr int GUI_HEIGHT{ 7 };

Engine::Engine() : screenWidth(WINDOW_WIDTH), screenHeight(WINDOW_HEIGHT)
{
    InitTcod();
    gui = new Gui(Point(0, screenHeight - GUI_HEIGHT), screenWidth, GUI_HEIGHT, console);
    map = new Map(screenWidth, screenHeight - GUI_HEIGHT, inputHandler, console);
}

Engine::~Engine()
{
    delete map;
    delete gui;
}

void Engine::Init()
{
     map->Init(true);
     EventManager::GetInstance()->Publish(MessageEvent("Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.", RED));

     EventManager::GetInstance()->Subscribe(EventType::PopupLaunched,
         [&, this](const Event& e)
         {
             auto& event = dynamic_cast<const PopupLaunchedEvent&>(e);
             currentPopup = event.popup;
         });
}

void Engine::Run()
{
    Init();

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
