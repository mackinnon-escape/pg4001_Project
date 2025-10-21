#pragma once

#include <string>

class Actor;

enum class EFFECT_TYPE
{
    NONE = 0,
    HEALTH = 1
};

class Effect
{
public:
    virtual bool ApplyTo(Actor* actor) = 0;
    virtual ~Effect() = default;
protected:
};

class HealthEffect : public Effect
{
public:
    int amount;
    std::string message;

    HealthEffect(int amount, const std::string& message) : amount(amount), message(message) {}
    bool ApplyTo(Actor* actor);
};