#pragma once

#include <string>
#include <memory>
#include "IPersistable.h"

class Actor;
template<typename T> class TemporaryAi;
class ConfusedMonsterAi;

enum class EFFECT_TYPE
{
    NONE = 0,
    HEALTH = 1,
    AI_CHANGE
};

class Effect : IPersistable
{
public:
    virtual bool ApplyTo(Actor* actor) = 0;
    virtual ~Effect() = default;
    virtual EFFECT_TYPE GetType() const = 0;
    void Load(Loader& loader) override = 0;
    void Save(Saver& saver) const override = 0;

    static std::unique_ptr<Effect> Create(Loader& loader);
protected:
};

class HealthEffect : public Effect
{
public:
    int amount;
    std::string message;

    HealthEffect(int amount, const std::string& message) : amount(amount), message(message) {}
    bool ApplyTo(Actor* actor);
    EFFECT_TYPE GetType() const override { return EFFECT_TYPE::HEALTH; }
    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
};

class AiChangeEffect : public Effect
{
public:
    AiChangeEffect(TemporaryAi<ConfusedMonsterAi>* newAi, const std::string& message) : newAi(newAi), message(message) {}
    virtual ~AiChangeEffect() = default;  // No manual delete needed anymore
    bool ApplyTo(Actor* actor);
    EFFECT_TYPE GetType() const override { return EFFECT_TYPE::AI_CHANGE; }
    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
private:
    TemporaryAi<ConfusedMonsterAi>* newAi;
    std::string message;
};