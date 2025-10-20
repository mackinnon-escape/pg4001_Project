#pragma once
#include "libtcod.h"

class Actor;

class Attacker
{
public:
    Attacker(const int power, const TCODColor messageColour) : power(power), messageColour(messageColour) {};
    virtual ~Attacker() = default;
    void Attacker::Attack(const Actor* owner, Actor* target) const;
private:
    int power;
    TCODColor messageColour{};
};