#pragma once

#include <string>
#include "libtcod.h"
#include "EventManager.h"

class Actor;

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