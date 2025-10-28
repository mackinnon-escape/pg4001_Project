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
    PopupLaunched,
    TargetingRequested,
    TargetingCompleted,
    XPGained,
    LevelChanging,
    UpdateLevelAndXP,
    DungeonLevelChanged
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
    using SubscriptionHandle = size_t;

    // Singleton pattern
    static EventManager* GetInstance();

    SubscriptionHandle Subscribe(EventType type, EventHandler handler);
    void Unsubscribe(EventType type, SubscriptionHandle handle);
    void Publish(const Event& event);
    int GetNextHandle() { return static_cast<int>(nextHandle); }
    void Reset();
private:
    std::unordered_map<EventType, std::vector<std::pair<SubscriptionHandle, EventHandler>>> handlers;
    SubscriptionHandle nextHandle{ 1 };
};