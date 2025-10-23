#pragma once

#include "Ai.h"
#include "Actor.h"
#include "Serialise.h"


class TemporaryAiBase : public Ai
{
public:
    virtual ~TemporaryAiBase() = default;
    virtual void ApplyTo(Actor* actor) = 0;
protected:
    std::unique_ptr<Ai> oldAi{ nullptr };
};

// Templated TemporaryAi class
template<typename AlternateAiType>
class TemporaryAi : public TemporaryAiBase
{
public:
    TemporaryAi(int duration) : duration(duration) {}
    virtual ~TemporaryAi() = default;
    TemporaryAi(TemporaryAi&& other) = default;
    TemporaryAi& operator=(TemporaryAi&& other) = default;

    // Template definitions MUST be in the header
    void Update(Actor* owner, ILocationProvider& locationProvider) override
    {
        if (alternateAi)
        {
            alternateAi->Update(owner, locationProvider);
        }

        duration--;
        if (duration <= 0)
        {
            owner->ai = std::move(oldAi);
        }
    }

    void ApplyTo(Actor* actor) override
    {
        std::unique_ptr<Ai> savedOldAi = std::move(actor->ai);
        auto newTempAi = std::make_unique<TemporaryAi<AlternateAiType>>(duration);
        newTempAi->alternateAi = std::make_unique<AlternateAiType>();
        newTempAi->oldAi = std::move(savedOldAi);
        actor->ai = std::move(newTempAi);
    }

    void Save(Saver& saver)
    {
        saver.PutInt(static_cast<int>(GetType()));
        saver.PutInt(duration);
        if (alternateAi)
        {
            saver.PutInt(1);
            alternateAi->Save(saver);
        }
        else
        {
            saver.PutInt(0);
        }
        if(oldAi)
        {
            saver.PutInt(1);
            oldAi->Save(saver);
        }
        else
        {
            saver.PutInt(0);
        }
    }

    void Load(Loader& loader)
    {
        duration = loader.GetInt();
        if (loader.GetInt() == 1)
        {
            alternateAi = Ai::Create(loader);
        }
        if (loader.GetInt() == 1)
        {
            oldAi = Ai::Create(loader);
        }
    }

    AiType GetType() const override { return AiType::TEMPORARY; }
protected:
    int duration;
    std::unique_ptr<Ai> alternateAi{ nullptr };
};
