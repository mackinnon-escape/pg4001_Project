#pragma once

#include <functional>
#include "libtcod.h"

class Actor;
class Input;

class Popup
{
public:
    Popup(int width, int height, Input& input) : console(width, height), inputHandler(input) {}
    virtual ~Popup() {}
    virtual void Render(tcod::Console& mainConsole);
    virtual void Update();
    virtual bool IsDone() const { return done; }

protected:
    tcod::Console console;
    Input& inputHandler;
    tcod::ColorRGB foreground{};
    bool done{ false };
};

static const int INVENTORY_WIDTH = 50;
static const int INVENTORY_HEIGHT = 30;

class InventoryPopup : public Popup
{
public:
    InventoryPopup(Actor* owner, Input& input, std::function<void(Actor*, Actor*)> action)
        : Popup(INVENTORY_WIDTH, INVENTORY_HEIGHT, input), owner(owner), action(action) {}
    ~InventoryPopup() {};

    void Render(tcod::Console& mainConsole) override;
    void Update() override;
private:
    Actor* owner;
    std::function<void(Actor*, Actor*)> action;
};

static const int LEVELUP_WIDTH{ 40 };
static const int LEVELUP_HEIGHT{ 15 };

class LevelUpPopup : public Popup
{
public:
    LevelUpPopup(Actor* player, Input& input, int newLevel)
        : Popup(LEVELUP_WIDTH, LEVELUP_HEIGHT, input), player(player), level(newLevel) {
    }
    ~LevelUpPopup() {};

    void Render(tcod::Console& console) override;
    void Update() override;

private:
    Actor* player;
    int level;
};