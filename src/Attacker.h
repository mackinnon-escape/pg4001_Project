#pragma once
#include "libtcod.h"
#include "IPersistable.h"

class Actor;

class Attacker : IPersistable
{
public:
    Attacker(const int power, const TCODColor messageColour) : power(power), messageColour(messageColour) {};
    virtual ~Attacker() = default;
    void Attack(const Actor* owner, Actor* target) const;
    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
    void BoostPower() { power++; }
private:
    int power;
    TCODColor messageColour{};
};