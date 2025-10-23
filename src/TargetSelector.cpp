#include "TargetSelector.h"

#include "Colours.h"
#include "Actor.h"
#include "EventManager.h"
#include "CustomEvents.h"
#include "ILocationProvider.h"
#include "Serialise.h"

// return true if we have targets, false if none
bool TargetSelector::SelectTargets(std::vector<Actor*>&actorsInRange, Actor * wearer, Actor * source, ILocationProvider& locationProvider, std::function<bool(std::vector<Actor*>&, Actor*, Actor*)> callback) const
{
    switch (selectorType)
    {
    case SelectorType::CLOSEST_MONSTER:     // Find the closest enemy within range - immediate execution
    {
        if (Actor* closestMonster = locationProvider.GetClosestMonster(wearer->GetLocation(), static_cast<float>(range)))
        {
            actorsInRange.push_back(closestMonster);
        }
        break;
    }
    case SelectorType::WEARER_RANGE:        // All enemies around the caster - immediate execution

    {
        auto actors = locationProvider.GetActorsWithinRange(wearer->GetLocation(), range, false, true, false);
        for (auto actor : actors)
        {
            if (actor != wearer)
            {
                actorsInRange.push_back(actor);
            }
        }
        break;
    }
    case SelectorType::SELECTED_MONSTER:
    {
        auto handle = EventManager::GetInstance()->GetNextHandle();
        auto eventCallback = [&, this, handle, callback, source, wearer](const Event& e)
            {
                const auto& completedEvent = static_cast<const TargetingCompletedEvent&>(e);
                if (completedEvent.success)
                {
                    auto actors = locationProvider.GetActorsAt(completedEvent.selectedLocation);
                    if (!actors.empty())
                    {
                        auto foundActor = std::vector<Actor*>{ actors.front() };    // function takes a vector of actors, so make one with just the first actor
                        callback(foundActor, source, wearer);
                    }
                    else
                    {
                        EventManager::GetInstance()->Publish(MessageEvent("No enemy selected", LIGHT_GREY));
                    }
                }

                EventManager::GetInstance()->Unsubscribe(EventType::TargetingCompleted, handle);
            };

        // subscribe to target completed event and send out targeting request event
        EventManager::GetInstance()->Subscribe(EventType::TargetingCompleted, eventCallback);
        EventManager::GetInstance()->Publish(TargetingRequestEvent(wearer, range));
        EventManager::GetInstance()->Publish(MessageEvent("Left-click to select a tile,\nor right-click to cancel.", CYAN));
        return false;
    }
    case SelectorType::SELECTED_RANGE:
    {
        auto handle = EventManager::GetInstance()->GetNextHandle();
        //auto handlePtr = std::make_shared<EventManager::SubscriptionHandle>();      // what happens if we use a normal handle here? it will go out of scope before the lambda is called
        auto eventCallback = [&, this, handle, callback, source, wearer](const Event& e)
            {
                const auto& completedEvent = static_cast<const TargetingCompletedEvent&>(e);
                if (completedEvent.success)
                {
                    auto actors = locationProvider.GetActorsWithinRange(completedEvent.selectedLocation, range, false, true, false);
                    if (!actors.empty())
                    {
                        callback(actors, source, wearer);
                    }
                    else
                    {
                        EventManager::GetInstance()->Publish(MessageEvent("No enemy is close enough", LIGHT_GREY));
                    }
                }

                EventManager::GetInstance()->Unsubscribe(EventType::TargetingCompleted, handle);
            };
        EventManager::GetInstance()->Subscribe(EventType::TargetingCompleted, eventCallback);
        EventManager::GetInstance()->Publish(TargetingRequestEvent(wearer));
        EventManager::GetInstance()->Publish(MessageEvent("Left-click to select a tile,\nor right-click to cancel.", CYAN));

        return false;
    }
    case SelectorType::NONE:
        // No targeting needed
        break;
    }

    // Only show "no enemy close enough" message for immediate targeting
    if (actorsInRange.empty() && (selectorType == SelectorType::CLOSEST_MONSTER || selectorType == SelectorType::WEARER_RANGE))
    {
        EventManager::GetInstance()->Publish(MessageEvent("No enemy is close enough", LIGHT_GREY));
    }

    return !actorsInRange.empty();
}

void TargetSelector::Save(Saver& saver) const
{
    saver.PutInt(static_cast<int>(selectorType));
    saver.PutInt(range);
}

void TargetSelector::Load(Loader& loader)
{
    selectorType = static_cast<SelectorType>(loader.GetInt());
    range = loader.GetInt();
}