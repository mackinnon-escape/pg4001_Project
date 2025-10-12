#pragma once

class Actor;

class Attacker
{
public:
    Attacker(const int power) : power(power) {};
    virtual ~Attacker() = default;
    void Attacker::Attack(const Actor* owner, Actor* target) const;
private:
    int power;
};