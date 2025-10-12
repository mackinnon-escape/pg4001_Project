#include "Attacker.h"
#include "Actor.h"
#include "Engine.h"
#include "Gui.h"

void Attacker::Attack(const Actor* owner, Actor* target) const
{
    if (!target->IsDead())
    {
        if (power - target->GetDefense() > 0)
        {
            Engine::GetInstance()->gui->SendMessage(owner == Engine::GetInstance()->player ? RED : LIGHT_GREY, "%s attacks %s for %d hit points.", owner->name.c_str(), target->name.c_str(),
                power - target->GetDefense());
        }
        else
        {
            Engine::GetInstance()->gui->SendMessage(LIGHT_GREY, "%s attacks %s but it has no effect!", owner->name.c_str(), target->name.c_str());
        }
        target->TakeDamage(power);
    }
    else
    {
        Engine::GetInstance()->gui->SendMessage(LIGHT_GREY, "%s attacks %s in vain.", owner->name.c_str(), target->name.c_str());
    }
}