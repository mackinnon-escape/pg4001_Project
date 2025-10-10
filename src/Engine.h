#pragma once
#include "libtcod.h"
#include "Input.h"
#include "Point.h"
#include <vector>

class Actor;
class Map;

class Engine
{
public:
    enum GameStatus { STARTUP, IDLE, NEW_TURN, VICTORY, DEFEAT } gameStatus{ STARTUP };
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
    Actor* player;
    int fovRadius{ 10 };	// how far can the player see?
    std::vector<Actor*> actors;
    Map* map;

private:
    Input inputHandler{};
    int screenWidth;
    int screenHeight;
    tcod::Context context;
    bool computeFov{ true };

    void Init();
    void InitTcod();
    void HandleInput();
    void Update();
    void Render();
};