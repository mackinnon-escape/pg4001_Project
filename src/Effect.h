#pragma once

#include <string>

class Actor;
template<typename T> class TemporaryAi;
class ConfusedMonsterAi;

enum class EFFECT_TYPE
{
    NONE = 0,
    HEALTH = 1,
    AI_CHANGE
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

class AiChangeEffect : public Effect
{
public:
    AiChangeEffect(TemporaryAi<ConfusedMonsterAi>* newAi, const std::string& message) : newAi(newAi), message(message) {}
    virtual ~AiChangeEffect() = default;  // No manual delete needed anymore
    bool ApplyTo(Actor* actor);

private:
    TemporaryAi<ConfusedMonsterAi>* newAi;
    std::string message;
};