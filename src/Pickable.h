#pragma once

#include <memory>
#include <vector>

#include "Effect.h"

class Actor;

class Pickable
{
public:
    Pickable(std::unique_ptr<Effect> effect) : effect(std::move(effect)) {}
    Pickable(const Pickable& other) = delete;
    Pickable& operator=(const Pickable& other) = delete;
    virtual ~Pickable() = default;

    static bool Pick(Actor* pickedItem, Actor* picker, std::vector<Actor*>& actors);
    bool Use(Actor* usedItem, Actor* user) const;

protected:
    std::unique_ptr<Effect> effect;
};