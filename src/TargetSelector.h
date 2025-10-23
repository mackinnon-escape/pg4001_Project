#pragma once

#include <vector>
#include <functional>
#include "IPersistable.h"

class Actor;
class ILocationProvider;

class TargetSelector : IPersistable
{
public:
    enum class SelectorType
    {
        NONE,              // No targeting needed (self-target)
        CLOSEST_MONSTER,   // Automatically target closest enemy
        SELECTED_MONSTER,  // Player selects a single target
        WEARER_RANGE,      // All enemies within range of caster
        SELECTED_RANGE     // All enemies within range of selected point
    };

    TargetSelector(SelectorType type, int range) : selectorType(type), range(range) {}
    bool SelectTargets(std::vector<Actor*>& targets, Actor* wearer, Actor* source, ILocationProvider& locationProvider, std::function<bool(std::vector<Actor*>&, Actor*, Actor*)> callback) const;
    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
protected:
    SelectorType selectorType;
    int range{ -1 };
    int cachedEventHandle{ 0 };
};