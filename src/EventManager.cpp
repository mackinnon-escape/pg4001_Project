#include "EventManager.h"

// Event contructors should go here

/////////////////////////////////////////////////////////////

// Singleton pattern
EventManager* EventManager::GetInstance()
{
    static EventManager instance;
    return &instance;
}

void EventManager::Subscribe(EventType type, EventHandler handler)
{
    handlers[type].push_back(handler);
}

void EventManager::Publish(const Event& event)
{
    if (handlers.find(event.type) != handlers.end())
    {
        for (auto& handler : handlers[event.type])
        {
            handler(event);
        }
    }
}