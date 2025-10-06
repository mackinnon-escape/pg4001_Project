#pragma once
#include "libtcod.h"
#include "Input.h"
#include "Point.h"

class Engine
{
    // Singleton Pattern
private:
    inline static Engine* instance{ nullptr };
    Engine(int screenWidth, int screenHeight);
    ~Engine();
public:
    Engine(Engine& other) = delete;
    void operator=(const Engine&) = delete;
    static Engine* GetInstance();

    // Game Implementation    
public:
    void Run();

private:
    Input inputHandler{};
    int screenWidth;
    int screenHeight;
    tcod::Console console;
    tcod::Context context;
    Point playerLocation{ 40, 25 };

    void InitTcod();
    void HandleInput();
    void Update();
    void Render();
};