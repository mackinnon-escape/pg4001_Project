#pragma once
#include <vector>
#include <unordered_map>
#include <functional>

enum class EventType
{
    HealthChanged,
    ActorDied,
    MessageLogged,
    GameOver,
    PopupLaunched
};

struct Event
{
    virtual ~Event() = default;
    EventType type;
};


// Put new event structs here

class EventManager
{
public:
    using EventHandler = std::function<void(const Event&)>;

    // Singleton pattern
    static EventManager* GetInstance();

    void Subscribe(EventType type, EventHandler handler);
    void Publish(const Event& event);

private:
    std::unordered_map<EventType, std::vector<EventHandler>> handlers;
};