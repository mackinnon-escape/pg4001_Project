#include "Engine.h"

//#include <iostream>
//#include <cstdlib>

#include "SDL3/SDL.h"
#include "Colours.h"
#include "Actor.h"
#include "Map.h"
#include "Point.h"
#include "Destructible.h"
#include "Attacker.h"
#include "Ai.h"
#include "Gui.h"

constexpr int WINDOW_WIDTH{ 80 };
constexpr int WINDOW_HEIGHT{ 50 };
static constexpr int GUI_HEIGHT{ 7 };

// Singleton Implementation
Engine* Engine::GetInstance()
{
    if (Engine::instance == nullptr)
    {
        Engine::instance = new Engine(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    return Engine::instance;
}

Engine::Engine(int width, int height) : player(nullptr), map(nullptr), screenWidth(width), screenHeight(height)
{
    InitTcod();
    gui = new Gui(Point(0, screenHeight - GUI_HEIGHT), screenWidth, GUI_HEIGHT, console);
    map = new Map(screenWidth, screenHeight - GUI_HEIGHT);
}

Engine::~Engine()
{
    for (auto actor : actors)
    {
        delete actor;
    }
    actors.clear();

    delete map;
    delete gui;
    Engine::instance = nullptr;
}

void Engine::Init()
{
    player = new Actor(Point::Zero, '@', "player", WHITE);
    player->destructible = new PlayerDestructible(30, 2, "your cadaver");
    player->attacker = new Attacker(5);
    player->ai = std::make_unique<PlayerAi>();
    actors.push_back(player);

    map->Init(true);
    if (gameStatus == STARTUP)
    {
        map->ComputeFov();
    }
    gameStatus = IDLE;
}

void Engine::Run()
{
    Init();

    while (true)
    {
        HandleInput();
        Update();
        Render();
        context.present(console);
    }
}

void Engine::HandleInput()
{
    inputHandler.ClearKey();
    inputHandler.CheckForEvent(context);
    if (inputHandler.GetKeyCode() == SDLK_ESCAPE)
    {
        std::exit(0);
    }
}

void Engine::Update()
{
    if (gameStatus == STARTUP)
    {
        map->ComputeFov();
    }
    gameStatus = IDLE;

    player->Update();
    if (gameStatus == NEW_TURN)
    {
        for (auto actor : actors)
        {
            if (actor != player)
            {
                actor->Update();
            }
        }
    }
}

void Engine::Render()
{
    console.clear();

    map->Render();
    for (auto actor : actors)
    {
        if (map->IsInFov(actor->GetLocation()))
        {
            actor->Render();
        }
    }

    gui->Render();
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

void Engine::DrawFirst(Actor* actor)
{
    auto deadActor = std::find(actors.begin(), actors.end(), actor);
    if (deadActor != actors.end())
    {
        actors.erase(deadActor);
        actors.insert(actors.begin(), actor);
    }
}