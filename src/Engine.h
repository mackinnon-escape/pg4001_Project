#pragma once
#include "libtcod.h"
#include "Input.h"
#include "Point.h"
#include <vector>

class Map;
class Gui;
class Popup;
class GameMenu;

class Engine
{
public:
    enum GameStatus { STARTUP, IDLE, VICTORY, DEFEAT, POPUP, MENU } gameStatus{ STARTUP };
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
    GameMenu* currentMenu{ nullptr };

    void Load();
    void Terminate();
    void Init();
    void InitTcod();
    void HandleInput();
    void Update();  
    void GameMenuUpdate();
    void Render();
    void SubscribeToEvents();
    void Save() const;
    void ShowMenu();
    bool IsSaveGameValid();
    void GameOver();
};