#pragma once

#include <memory>
#include <vector>

#include "Effect.h"
#include "TargetSelector.h"
#include "IPersistable.h"

class Actor;

class Pickable : IPersistable
{
public:
    Pickable(std::unique_ptr<TargetSelector> selector, std::unique_ptr<Effect> effect)
        : effect(std::move(effect)), selector(std::move(selector)) {}
    Pickable(std::unique_ptr<Effect> effect) : effect(std::move(effect)), selector(nullptr) {}
    Pickable(const Pickable& other) = delete;
    virtual ~Pickable() = default;
    Pickable& operator=(const Pickable& other) = delete;

    static bool Pick(Actor* pickedItem, Actor* picker, std::vector<Actor*>& actors);
    bool Use(Actor* usedItem, Actor* user, ILocationProvider& locationProvider) const;
    bool Use(std::vector<Actor*>& targets, Actor* usedItem, Actor* user) const;
    static void Drop(Actor* droppedItem, Actor* dropper, std::vector<Actor*>& actors);

    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
protected:
    std::unique_ptr<Effect> effect;
    std::unique_ptr<TargetSelector> selector;
};