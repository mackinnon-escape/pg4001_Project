#include "EventManager.h"

// Event contructors should go here

/////////////////////////////////////////////////////////////

// Singleton pattern
EventManager* EventManager::GetInstance()
{
    static EventManager instance;
    return &instance;
}

EventManager::SubscriptionHandle EventManager::Subscribe(EventType type, EventHandler handler)
{
    SubscriptionHandle handle = nextHandle++;
    handlers[type].push_back(std::pair<SubscriptionHandle, EventHandler>(handle, handler));
    return handle;
}

void EventManager::Publish(const Event& event)
{
    if (handlers.find(event.type) != handlers.end())
    {
        for (auto& [handle, handler] : handlers[event.type])
        {
            handler(event);
        }
    }
}

void EventManager::Unsubscribe(EventType type, SubscriptionHandle handle)
{
    if (handlers.find(type) == handlers.end()) return; // No handlers for this event type

    auto& eventHandlers = handlers[type];
    auto it = std::remove_if(eventHandlers.begin(), eventHandlers.end(), [handle](const auto& pair) { return pair.first == handle; });
    eventHandlers.erase(it, eventHandlers.end());
}