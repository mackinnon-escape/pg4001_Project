#pragma once
#include "libtcod.h"
#include "Input.h"
#include "Point.h"
#include <vector>

class Actor;
class Map;

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

    tcod::Console console;

private:
    Input inputHandler{};
    int screenWidth;
    int screenHeight;
    tcod::Context context;
    Point playerLocation{ 40, 25 };
    std::vector<Actor*> actors;
    Actor* player;
    Map* map;

    void Init();
    void InitTcod();
    void HandleInput();
    void Update();
    void Render();
};