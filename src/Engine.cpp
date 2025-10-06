#include "Engine.h"
#include "SDL3/SDL.h"
#include "Colours.h"

constexpr int WINDOW_WIDTH{ 80 };
constexpr int WINDOW_HEIGHT{ 50 };

// Singleton Implementation
Engine* Engine::GetInstance()
{
    if (Engine::instance == nullptr)
    {
        Engine::instance = new Engine(WINDOW_WIDTH, WINDOW_HEIGHT);
    }
    return Engine::instance;
}

Engine::Engine(int width, int height) : screenWidth(width), screenHeight(height)
{
    InitTcod();
}

Engine::~Engine()
{
    Engine::instance = nullptr;
}

void Engine::Run()
{
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
        Point newLocation = playerLocation + Point{ dx, dy };
        if (newLocation.x >= 0 && newLocation.x < screenWidth &&
            newLocation.y >= 0 && newLocation.y < screenHeight)
        {
            playerLocation = newLocation;
        }
    }
}

void Engine::Render()
{
    console.clear();
    console.at(playerLocation.x, playerLocation.y).ch = '@';
    console.at(playerLocation.x, playerLocation.y).fg = WHITE;
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
