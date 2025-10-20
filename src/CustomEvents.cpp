#include "CustomEvents.h"

HealthChangedEvent::HealthChangedEvent(int current, int max) : currentHp(current), maxHp(max)
{
    type = EventType::HealthChanged;
}

ActorDiedEvent::ActorDiedEvent(Actor* deadActor, std::string& corpse) : actor(deadActor), corpseName(corpse)
{
    type = EventType::ActorDied;
}

MessageEvent::MessageEvent(const std::string& msg, const TCODColor& color)
    : message(msg), col(color)
{
    type = EventType::MessageLogged;
}
