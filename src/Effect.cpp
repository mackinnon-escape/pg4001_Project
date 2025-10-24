#include "Effect.h"

#include <format>
#include "Colours.h"
#include "Actor.h"
#include "CustomEvents.h"
#include "TemporaryAi.h"
#include "Serialise.h"

std::unique_ptr<Effect> Effect::Create(Loader& loader)
{
    EFFECT_TYPE type = static_cast<EFFECT_TYPE>(loader.GetInt());
    std::unique_ptr<Effect> effect;

    switch (type)
    {
    case EFFECT_TYPE::HEALTH:
        effect = std::make_unique<HealthEffect>(0, "");
        break;
    case EFFECT_TYPE::AI_CHANGE:
        effect = std::make_unique<AiChangeEffect>(nullptr, "");
        break;
    }

    if (effect)
    {
        effect->Load(loader);
    }

    return effect;
}

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

void HealthEffect::Save(Saver& saver) const
{
    saver.PutInt(static_cast<int>(GetType()));
    saver.PutInt(amount);
    saver.PutString(message);
}

void HealthEffect::Load(Loader& loader)
{
    amount = loader.GetInt();
    message = loader.GetString();
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



void AiChangeEffect::Save(Saver& saver) const
{
    saver.PutInt(static_cast<int>(GetType()));
    newAi->Save(saver);
    saver.PutString(message);
}

void AiChangeEffect::Load(Loader& loader)
{
    loader.GetInt();    // we need to remove the int that specifies that our Ai class is a TemporaryAi...
    newAi = new TemporaryAi<ConfusedMonsterAi>(1);
    newAi->Load(loader);
    message = loader.GetString();
}