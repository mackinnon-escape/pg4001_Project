#include "Effect.h"

#include <format>
#include "Colours.h"
#include "Actor.h"
#include "CustomEvents.h"


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

    return false;
}