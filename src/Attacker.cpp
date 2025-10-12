#include "Attacker.h"
#include "Actor.h"

void Attacker::Attack(Actor* target) const
{
    if (!target->IsDead())
    {
        target->TakeDamage(power);
    }
}