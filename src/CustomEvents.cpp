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

GameOverEvent::GameOverEvent(const std::string& reason) : reason(reason)
{
    type = EventType::GameOver;
}

XPGainedEvent::XPGainedEvent(int xpAmount) : xpAmount(xpAmount)
{
    type = EventType::XPGained;
}

LevelChangingEvent::LevelChangingEvent(int newLevel) : newLevel(newLevel)
{
    type = EventType::LevelChanging;
}

UpdateLevelAndXPEvent::UpdateLevelAndXPEvent(int level, int currentXp, int xpForNextLevel)
    : level(level), currentXp(currentXp), xpForNextLevel(xpForNextLevel)
{
    type = EventType::UpdateLevelAndXP;
}