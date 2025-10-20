#include "Attacker.h"
#include <string>
#include "Actor.h"
#include "CustomEvents.h"

void Attacker::Attack(const Actor* owner, Actor* target) const
{
    if (!target->IsDead())
    {
        if (power - target->GetDefense() > 0)
        {
            std::string attackMessage = owner->name + " attacks " + target->name + " for " + std::to_string(static_cast<int>(power - target->GetDefense())) + " hit points.";
            EventManager::GetInstance()->Publish(MessageEvent(attackMessage, messageColour));
        }
        else
        {
            EventManager::GetInstance()->Publish(MessageEvent(owner->name + " attacks " + target->name + " but it has no effect!", messageColour));
        }
        target->TakeDamage(power);
    }
    else
    {
        EventManager::GetInstance()->Publish(MessageEvent(owner->name + " attacks " + target->name + " in vain.", messageColour));
    }
}