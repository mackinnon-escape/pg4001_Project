#include "Engine.h"

#include "SDL3/SDL.h"

constexpr int WINDOW_WIDTH{ 80 };
constexpr int WINDOW_HEIGHT{ 50 };

Engine::Engine() : screenWidth(WINDOW_WIDTH), screenHeight(WINDOW_HEIGHT)
{
    initTcod();
}

void Engine::Run()
{
    // This is our traditional game loop
    while (true) 
    {
        Update();
        Render();
        context.present(console);
    }
}

void Engine::initTcod()
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

void Engine::Render() 
{
    console.clear();

    console.at(40, 25).ch = '@';
    console.at(40, 25).fg = tcod::ColorRGB{255, 255, 255};

}

void Engine::Update()
{
    // we don't have anything here yet. 
    // This method will be run every frame
}