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

PopupLaunchedEvent::PopupLaunchedEvent(Popup* popup) : popup(popup)
{
    type = EventType::PopupLaunched;
}

TargetingRequestEvent::TargetingRequestEvent(Actor* caster, int range)
    : caster(caster), range(range)
{
    type = EventType::TargetingRequested;
}

TargetingCompletedEvent::TargetingCompletedEvent(Point selectedLocation, bool success)
    : selectedLocation(selectedLocation), success(success)
{
    type = EventType::TargetingCompleted;
}
