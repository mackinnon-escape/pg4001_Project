#pragma once
#include "libtcod.h"
#include "Input.h"
#include "Point.h"
#include <vector>

class Map;
class Gui;
class Popup;

class Engine
{
public:
    enum GameStatus { STARTUP, IDLE, NEW_TURN, VICTORY, DEFEAT, POPUP, MENU } gameStatus{ STARTUP };

    Engine();
    ~Engine();

    void Run();


private:
    Map* map{ nullptr };
    Gui* gui{ nullptr };
    tcod::Console console;
    tcod::Context context;

    Input inputHandler{};
    int screenWidth;
    int screenHeight;
    bool computeFov{ true };
    Popup* currentPopup{ nullptr };

    void Init();
    void InitTcod();
    void HandleInput();
    void Update();  
    void Render();
};