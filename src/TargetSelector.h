#pragma once

#include <vector>
#include <functional>

class Actor;
class ILocationProvider;

class TargetSelector
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

    TargetSelector(SelectorType type, int range, ILocationProvider& locationProvider) : selectorType(type), range(range), locationProvider(locationProvider) {}
    bool SelectTargets(std::vector<Actor*>& targets, Actor* wearer, Actor* source, std::function<bool(std::vector<Actor*>&, Actor*, Actor*)> callback) const;
protected:
    SelectorType selectorType;
    int range{ -1 };
    ILocationProvider& locationProvider;
    int cachedEventHandle{ 0 };
};