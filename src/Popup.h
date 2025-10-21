#pragma once

#include <functional>
#include "libtcod.h"

class Actor;
class Input;

class Popup
{
public:
    Popup() {}
    Popup(int width, int height) : console(width, height) {}
    virtual ~Popup() {}
    virtual void Render(tcod::Console& mainConsole) = 0;
    virtual void Update() = 0;
    virtual bool IsDone() const = 0;

protected:
    tcod::Console console;
};

static const int INVENTORY_WIDTH = 50;
static const int INVENTORY_HEIGHT = 30;

class InventoryPopup : public Popup
{
public:
    InventoryPopup(Actor* owner, std::function<void(Actor*, Actor*)> action, Input& input)
        : Popup(INVENTORY_WIDTH, INVENTORY_HEIGHT), owner(owner), action(action), inputHandler(input) {
    }
    ~InventoryPopup() {};

    void Render(tcod::Console& mainConsole) override;
    void Update() override;
    bool IsDone() const override { return done; }
private:
    Actor* owner;
    bool done = false;
    std::function<void(Actor*, Actor*)> action;
    Input& inputHandler;
};