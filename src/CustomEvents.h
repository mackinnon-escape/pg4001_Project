#pragma once

#include <string>
#include "libtcod.h"
#include "EventManager.h"
#include "Point.h"

class Actor;
class Popup;

struct HealthChangedEvent : Event
{
    HealthChangedEvent(int current, int max);

    int currentHp;
    int maxHp;
};

struct ActorDiedEvent : Event
{
    ActorDiedEvent(Actor* deadActor, std::string& corpse);
    Actor* actor;
    std::string corpseName;
};

struct MessageEvent : Event
{
    MessageEvent(const std::string& msg, const TCODColor& color);
    std::string message;
    TCODColor col;
};

struct PopupLaunchedEvent : Event
{
    PopupLaunchedEvent(Popup* popup);
    Popup* popup;
};

struct TargetingRequestEvent : Event
{
    TargetingRequestEvent(Actor* caster, int range = -1);
    Actor* caster;
    int range;
};

struct TargetingCompletedEvent : Event
{
    TargetingCompletedEvent(Point selectedLocation, bool success);
    Point selectedLocation;
    bool success;
};

struct GameOverEvent : Event
{
    GameOverEvent(const std::string& reason);
    std::string reason;
};

struct XPGainedEvent : Event
{
    XPGainedEvent(int amount);
    int xpAmount;
};

struct LevelChangingEvent : Event
{
    LevelChangingEvent(int newLevel);
    int newLevel;
};

struct UpdateLevelAndXPEvent : Event
{
    UpdateLevelAndXPEvent(int level, int currentXp, int xpForNext);
    int level;
    int currentXp;
    int xpForNextLevel;
};