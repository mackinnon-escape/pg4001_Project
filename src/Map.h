#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include "libtcod.h"
#include "Point.h"
#include "ILocationProvider.h"
#include "Actor.h"
#include "IPersistable.h"

class Input;
enum class EFFECT_TYPE;

struct Tile
{
    Tile() {}
    bool explored{ false }; // has the player already seen this tile ?
};

class Map : public ILocationProvider, IPersistable
{
public:
    Map(int width, int height, Input& input, tcod::Console& console);
    virtual ~Map();

    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;

    void Init(bool withActors);
    void SubscribeToEvents();
    void Render() const;
    void Update();
    bool IsExplored(const Point& location) const;
    void ComputeFov() const;
    void DrawFirst(Actor* actor);
    Actor* GetClosestMonster(const Point start, const float range) const;
    std::vector<Actor*> GetActorsWithinRange(Point centre, int range, bool onlyInFov, bool skipDeadActors, bool skipPlayer) const;

    // ILocationProvider
    bool IsWall(const Point& position) const override;
    bool CanWalk(const Point& position) const override;
    bool IsInFov(const Point& location) const override;
    Actor* GetPlayer() const override { return player; }
    Point GetPlayerLocation() const  override { return player->GetLocation(); }
    std::vector<Actor*> GetActorsAt(const Point& p) const override;
    std::vector<Actor*>& GetActors() { return actors; }

private:
    Tile* tiles{ nullptr };
    int width;
    int height;
    long seed;
    TCODRandom* rng{ nullptr };
    TCODMap* map{ nullptr };
    Input& inputHandler;
    Actor* player{ nullptr };
    std::vector<Actor*> actors{};
    tcod::Console& console;
    bool isPickingATile{ false };
    int maxPickingRange{ -1 };
    friend class BspCallback;

    void Dig(const Point& corner1, const Point& corner2) const;
    void CreateRoom(const bool first, const Point& corner1, const Point& corner2, bool withActors);
    void AddMonster(const Point& location);
    void CreatePlayer();
    void AddItem(const Point& location);
    void AddItem(const std::string& name, const char symbol, const Point& location, const tcod::ColorRGB& colour, const bool isBlocking,
        const TargetSelector::SelectorType selectorType, const int range, const EFFECT_TYPE effectType, const std::string& description,
        const int amount, const int duration = 0, const int aiChangeType = 0);
    void HandleTileSelected();
};

class BspCallback : public ITCODBspCallback
{
public:
    BspCallback(Map& map) : map(map), roomNumber(0) {}
    bool visitNode(TCODBsp* node, void* userdata) override;

private:
    Map& map;                    // Reference to map to modify
    int roomNumber{ 0 };         // Room counter for first room detection
    Point previousLocation{};   // Center of the last room created
};
#endif // MAP_H