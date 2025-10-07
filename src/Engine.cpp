#include "Engine.h"
#include "SDL3/SDL.h"
#include "Colours.h"
#include "Actor.h"
#include "Map.h"
#include "Point.h"

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
    Engine::instance = nullptr;
}

void Engine::Init()
{
    player = new Actor(Point::Zero, '@', WHITE);  // Temporary position
    actors.push_back(player);

    map->Init();
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
    inputHandler.CheckForEvent();
    if (inputHandler.GetKeyCode() == SDLK_ESCAPE)
    {
        std::exit(0);
    }
}

void Engine::Update()
{
    int dx{ 0 };
    int dy{ 0 };
    switch (inputHandler.GetKeyCode())
    {
    case SDLK_UP: dy = -1; break;
    case SDLK_DOWN: dy = 1; break;
    case SDLK_LEFT: dx = -1; break;
    case SDLK_RIGHT: dx = 1; break;
    default:
        break;
    }

    if (dx != 0 || dy != 0)
    {
        Point dp{ dx, dy };
        Point target{ player->GetLocation() + dp };
        if (target.x >= 0 && target.x < screenWidth && target.y >= 0 && target.y < screenHeight && !map->IsWall(target))
        {
            player->SetLocation(target);
        }
    }
}

void Engine::Render()
{
    ITCODBspCallback* thing;
    console.clear();

    map->Render();
    for (auto actor : actors)
    {
        actor->Render();
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
