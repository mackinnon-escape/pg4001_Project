#include "Effect.h"

#include <format>
#include "Colours.h"
#include "Actor.h"
#include "CustomEvents.h"
#include "TemporaryAi.h"


bool HealthEffect::ApplyTo(Actor* actor)
{
    if (!actor->destructible) return false;

    if (amount > 0)
    {
        int pointsHealed = actor->Heal(amount);
        if (pointsHealed > 0)
        {
            if (!message.empty())
            {
                // vformat is C++20: it takes a format string and a list of arguments
                auto completeMessage = std::vformat(message, std::make_format_args(actor->name, pointsHealed));
                EventManager::GetInstance()->Publish(MessageEvent(completeMessage, LIGHT_GREY));
            }
            return true;
        }
    }
    else  // Add this else block for damage effects
    {
        // Damage effect (amount is negative)
        int damageDealt = -amount - actor->destructible->defense;
        if (damageDealt > 0)
        {
            if (!message.empty())
            {
                auto completeMessage = std::vformat(message, std::make_format_args(actor->name, damageDealt));
                EventManager::GetInstance()->Publish(MessageEvent(completeMessage, LIGHT_GREY));
            }
        }
        if (actor->TakeDamage(-amount) > 0)
        {
            return true;
        }
    }

    return false;
}

bool AiChangeEffect::ApplyTo(Actor* actor)
{
    if (newAi != nullptr && actor != nullptr && actor->ai != nullptr)
    {
        newAi->ApplyTo(actor);
        if (!message.empty())
        {
            auto completeMessage = std::vformat(message, std::make_format_args(actor->name));
            EventManager::GetInstance()->Publish(MessageEvent(completeMessage, LIGHT_GREY));
        }
        return true;
    }
    return false;
}