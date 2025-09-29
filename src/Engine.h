#pragma once

#include "libtcod.h"

class Engine 
{
public:
    Engine();
    ~Engine() = default;

public:
    void Run();
    
private:
    tcod::Console console;
    tcod::Context context;
    int screenWidth;
    int screenHeight;


    void initTcod();
    void Update();
    void Render();
};