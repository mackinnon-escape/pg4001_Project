#include "Map.h"

#include <algorithm>
#include "Engine.h"
#include "Actor.h"
#include "Colours.h"
#include "Destructible.h"
#include "Attacker.h"
#include "Ai.h"
#include "TemporaryAi.h"
#include "CustomEvents.h"
#include "Effect.h"
#include "Serialise.h"

static constexpr int MAX_ROOM_MONSTERS = 3;
static constexpr int ROOM_MAX_SIZE{ 12 };
static constexpr int ROOM_MIN_SIZE{ 6 };
static constexpr int FOV_RADIUS{ 10 };
static constexpr int MAX_ROOM_ITEMS = 2;

Map::Map(int width, int height, Input& input, tcod::Console& console) : width(width), height(height), inputHandler(input), console(console)
{
    seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

Map::~Map()
{
    delete[] tiles;
    delete map;
    delete rng;

    for (auto actor : actors)
    {
        delete actor;
    }
    actors.clear();
}

void Map::Init(bool withActors)
{
    CreatePlayer();
    SubscribeToEvents();

    TCODBsp bsp(0, 0, width, height);

    long area{ width * height };
    tiles = new Tile[area];

    BspCallback listener(*this);
    rng = new TCODRandom(seed, TCOD_RNG_CMWC);
    map = new TCODMap(width, height);
    bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
    bsp.traverseInvertedLevelOrder(&listener, static_cast<void*>(&withActors));
}

void Map::SubscribeToEvents()
{
    EventManager::GetInstance()->Subscribe(EventType::ActorDied,
        [&, this](const Event& e)
        {
            const auto& deathEvent = static_cast<const ActorDiedEvent&>(e);
            deathEvent.actor->ChangeToCorpse(deathEvent.corpseName);
            DrawFirst(deathEvent.actor);
        });

    EventManager::GetInstance()->Subscribe(EventType::TargetingRequested,
        [this](const Event& e)
        {
            const auto& targetingEvent = static_cast<const TargetingRequestEvent&>(e);
            isPickingATile = true;
            maxPickingRange = targetingEvent.range;
        });
}

bool Map::IsWall(const Point& position) const
{
    return !map->isWalkable(position.x, position.y);
}

void Map::Update()
{
    if (isPickingATile)
    {
        HandleTileSelected();
        return;
    }

    Point oldPlayerLocation = player->GetLocation();
    player->Update(inputHandler, *this);
    if (oldPlayerLocation != player->GetLocation())
    {
        ComputeFov();
    }

    if(player->hasActedThisFrame)
    {
        for (auto actor : actors)
        {
            if (actor != player)
            {
                actor->Update(inputHandler, *this);
            }
        }
    }
}

void Map::Render() const
{
    static constexpr tcod::ColorRGB darkWall(0, 0, 100);
    static constexpr tcod::ColorRGB darkGround(50, 50, 150);
    static constexpr tcod::ColorRGB lightWall(130, 110, 50);   // Visible walls
    static constexpr tcod::ColorRGB lightGround(200, 180, 50); // Visible floors

    Point mouseLocation = inputHandler.GetMouseLocation();
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Point p{ x, y };
            if (IsInFov(p))
            {
                // Currently visible - bright colors
                console.at(x, y).bg = IsWall(p) ? lightWall : lightGround;
                if (isPickingATile && (maxPickingRange == -1 || player->GetDistanceFrom(p) <= maxPickingRange))
                {
                    if (p == mouseLocation)
                    {
                        console.at(x, y).bg = WHITE;
                    }
                    else
                    {
                        // Make tiles within range slightly brighter
                        auto& tile = console.at(x, y);
                        tile.bg.r = static_cast<uint8_t>(std::min(255, static_cast<int>(tile.bg.r * 1.2f)));
                        tile.bg.g = static_cast<uint8_t>(std::min(255, static_cast<int>(tile.bg.g * 1.2f)));
                        tile.bg.b = static_cast<uint8_t>(std::min(255, static_cast<int>(tile.bg.b * 1.2f)));
                    }
                }
            }
            else if (IsExplored(p))
            {
                // Previously seen - dim colors
                console.at(x, y).bg = IsWall(p) ? darkWall : darkGround;
            }
            // Unseen areas remain black (default background)
        }
    }

     for (auto actor : actors)
    {
        if (IsInFov(actor->GetLocation()))
        {
            actor->Render(console);
        }
    }
}

bool Map::CanWalk(const Point& position) const
{
    if (IsWall(position))
    {
        return false;    // this is a wall
    }

    for (auto actor : actors)
    {
        if (actor->blocks && actor->IsIn(position))
        {
            // there is a blocking actor here. cannot walk
            return false;
        }
    }
    return true;
}

bool Map::IsExplored(const Point& location) const
{
    return tiles[location.x + location.y * width].explored;
}

bool Map::IsInFov(const Point& location) const
{
    if (location.x < 0 || location.x >= width || location.y < 0 || location.y >= height) return false;

    if (map->isInFov(location.x, location.y))
    {
        tiles[location.x + location.y * width].explored = true;  // Mark as explored
        return true;
    }
    return false;
}

void Map::ComputeFov() const
{
    map->computeFov(player->GetLocation().x, player->GetLocation().y, FOV_RADIUS);
}

void Map::CreatePlayer()
{
    player = new Actor(Point::Zero, '@', "player", WHITE);
    player->destructible = new PlayerDestructible(30, 2, "your cadaver");
    player->attacker = new Attacker(5, RED);
    player->ai = std::make_unique<PlayerAi>();
    player->container = new Container(26);
    actors.push_back(player);
}

void Map::DrawFirst(Actor* actor)
{
    auto deadActor = std::find(actors.begin(), actors.end(), actor);
    if (deadActor != actors.end())
    {
        actors.erase(deadActor);
        actors.insert(actors.begin(), actor);
    }
}

std::vector<Actor*> Map::GetActorsAt(const Point& p) const
{
     std::vector<Actor*> actorsFound{};

     for (auto actor : actors)
     {
         if (actor->IsIn(p))
         {
             actorsFound.push_back(actor);
         }
     }

     return actorsFound;
}

Actor* Map::GetClosestMonster(const Point start, const float range) const
{
    Actor* closest{ nullptr };
    float bestDistance{ 1E6f };  // Start with a very large distance

    for (auto actor : actors)   // go through every monster saving the bestDistance so far.
    {
        // Check if it's a valid target (not the player and is alive)
        if (actor != player && actor->IsAlive())
        {
            float distance = actor->GetDistanceFrom(start);
            // Check if this actor is closer and within range (range -1 means unlimited)
            if (distance < bestDistance && (distance <= range || range == -1.0f))
            {
                bestDistance = distance;
                closest = actor;
            }
        }
    }
    return closest;
}

std::vector<Actor*> Map::GetActorsWithinRange(Point centre, int range, bool onlyInFov, bool skipDeadActors, bool skipPlayer) const
{
    std::vector<Actor*> result;
    for (const auto& actor : actors)
    {
        if (skipDeadActors && actor->IsDead()) continue;
        if (skipPlayer && actor == player) continue;
        if (onlyInFov && !IsInFov(actor->GetLocation())) continue;

        float distance = actor->GetDistanceFrom(centre);
        if (range == -1 || distance <= range)
        {
            result.push_back(actor);
        }
    }
    return result;
}

void Map::HandleTileSelected()
{
    if (inputHandler.IsLeftMousePressed())
    {
        Point selectedLocation = inputHandler.GetMouseLocation();
        if (!IsInFov(selectedLocation))  return;

        // Check if the selected location is within range and in FOV
        if (maxPickingRange == -1 || player->GetDistanceFrom(selectedLocation) <= maxPickingRange)
        {
            // Publish successful targeting completion
            EventManager::GetInstance()->Publish(TargetingCompletedEvent(selectedLocation, true));
        }
        else
        {
            // Invalid selection - publish failed targeting
            EventManager::GetInstance()->Publish(TargetingCompletedEvent(Point::Zero, false));
        }
        isPickingATile = false;
    }
    // Check for cancellation (right click, key press, or timeout)
    else if (inputHandler.IsRightMousePressed() || inputHandler.GetKeyCode() != 0)
    {
        // Publish cancelled targeting
        EventManager::GetInstance()->Publish(TargetingCompletedEvent(Point::Zero, false));
        isPickingATile = false;
    }
}

/*
* dig out the room: change Tile::canWalk to true
* Can also dig out a hallway, will do so if a coordinate is the same in both corners
*/
void Map::Dig(const Point& corner1, const Point& corner2) const
{
    Point UpperRight{ corner1 };
    Point LowerLeft{ corner2 };
    if (corner2.x < corner1.x)
    {
        UpperRight.x = corner2.x;
        LowerLeft.x = corner1.x;
    }
    if (corner2.y < corner1.y)
    {
        UpperRight.y = corner2.y;
        LowerLeft.y = corner1.y;
    }
    for (int tileX = UpperRight.x; tileX <= LowerLeft.x; tileX++)
    {
        for (int tileY = UpperRight.y; tileY <= LowerLeft.y; tileY++)
        {
            map->setProperties(tileX, tileY, true, true);  // transparent=true, walkable=true
        }
    }
}

void Map::CreateRoom(const bool first, const Point& corner1, const Point& corner2, bool withActors)
{
    if (!withActors) { return; }	// skip out from making actors if withActors is false

    Dig(corner1, corner2);
    Point middleOfRoom{ (corner1 + corner2) / 2 };

    // put the player in the first room
    if (first)
    {
        player->SetLocation(middleOfRoom);
    }
    else
    {
        // add items first so that monsters show on top (although only in that room)
        int nbItems = rng->getInt(0, MAX_ROOM_ITEMS);
        while (nbItems > 0)
        {
            Point p{ rng->getInt(corner1.x, corner2.x), rng->getInt(corner1.y, corner2.y) };
            if (CanWalk(p))
            {
                AddItem(p);
            }
            nbItems--;
        }

        int MonstersToAdd = rng->getInt(0, MAX_ROOM_MONSTERS);
        while (MonstersToAdd > 0)
        {
            Point p{ rng->getInt(corner1.x, corner2.x), rng->getInt(corner1.y, corner2.y) };
            if (CanWalk(p))
            {
                AddMonster(p);
            }
            MonstersToAdd--;
        }
    }
}

void Map::AddMonster(const Point& location)
{
    if (rng->getInt(0, 100) < 80)
    {
        // create an orc
        Actor* orc = new Actor(location, 'o', "orc", DESATURATED_GREEN);
        orc->destructible = new MonsterDestructible(10, 0, "dead orc");
        orc->attacker = new Attacker(3, LIGHT_GREY);
        orc->ai = std::make_unique<MonsterAi>();
        actors.push_back(orc);
    }
    else
    {
        // create a troll
        Actor* troll = new Actor(location, 'T', "troll", DARKER_GREEN);
        troll->destructible = new MonsterDestructible(16, 1, "troll carcass");
        troll->attacker = new Attacker(4, LIGHT_GREY);
        troll->ai = std::make_unique<MonsterAi>();
        actors.push_back(troll);
    }
}

void Map::AddItem(const Point& location)
{
    int dice = rng->getInt(0, 100);
    if (dice < 70)                  // 70% chance: health potion
    {
        AddItem("health potion", '!', location, VIOLET, false,
            TargetSelector::SelectorType::NONE, 0,
            EFFECT_TYPE::HEALTH, "You are healed!", 4);
    }
    else if (dice < 80)            // 10% chance: lightning bolt
    {
        AddItem("scroll of lightning bolt", '#', location, LIGHT_YELLOW, false,
            TargetSelector::SelectorType::CLOSEST_MONSTER, 5,
            EFFECT_TYPE::HEALTH,
            "A lighting bolt strikes the {} with a loud thunder!\nThe damage is {} hit points.",
            -20);
    }
    else if (dice < 90)            // 10% chance: fireball
    {
        AddItem("scroll of fireball", '#', location, LIGHT_YELLOW, false,
            TargetSelector::SelectorType::SELECTED_RANGE, 3,
            EFFECT_TYPE::HEALTH,
            "The {} gets burned for {} hit points.", -12);
    }
    else                           // 10% chance: confusion
    {
        AddItem("scroll of confusion", '#', location, LIGHT_YELLOW, false,
            TargetSelector::SelectorType::SELECTED_MONSTER, 5,
            EFFECT_TYPE::AI_CHANGE,
            "The eyes of the {} look vacant,\nas he starts to stumble around!",
            0, 10, 1);  // duration=10, aiChangeType=1 for confused
    }
}

void Map::AddItem(const std::string& name, const char symbol, const Point& location, const tcod::ColorRGB& colour, const bool isBlocking,
    const TargetSelector::SelectorType selectorType, const int range, const EFFECT_TYPE effectType, const std::string& description,
    const int amount, const int duration, const int type)
{
    Actor* item = new Actor(location, symbol, name, colour);
    item->blocks = isBlocking;

    // Create the selector if needed
    std::unique_ptr<TargetSelector> selector{ nullptr };
    if (selectorType != TargetSelector::SelectorType::NONE)
    {
        selector = std::make_unique<TargetSelector>(selectorType, range);
    }

    // Create the appropriate effect
    std::unique_ptr<Effect> effect{ nullptr };
    switch (effectType)
    {
    case EFFECT_TYPE::NONE:
        break;
    case EFFECT_TYPE::HEALTH:
        effect = std::make_unique<HealthEffect>(amount, description);
        break;
    case EFFECT_TYPE::AI_CHANGE:
        TemporaryAi<ConfusedMonsterAi>* ai{ nullptr };
        if (type == 1)
        {
            ai = new TemporaryAi<ConfusedMonsterAi>(duration);
        }
        effect = std::make_unique<AiChangeEffect>(ai, description);
        break;
    }

    item->pickable = new Pickable(std::move(selector), std::move(effect));
    actors.push_back(item);
}

void Map::Save(Saver& saver) const
{
    saver.PutInt(seed);
    saver.PutInt(width);
    saver.PutInt(height);

    // Save the complete map state
    for (int i = 0; i < width * height; i++)
    {
        saver.PutInt(tiles[i].explored);
        saver.PutInt(map->isWalkable(i % width, i / width) ? 1 : 0);
    }

    // Save player separately first
     player->Save(saver);
     int otherActorCount = static_cast<int>(actors.size()) - 1;    // Save all other actors (excluding player)
     saver.PutInt(otherActorCount);
    //int actorCount = static_cast<int>(actors.size());
    //saver.PutInt(actorCount);
    for (auto actor : actors)
    {
        //actor->Save(saver);
         if (actor != player)
         {
             actor->Save(saver);
         }
    }
}

void Map::Load(Loader& loader)
{
    seed = loader.GetInt();
    int serializedWidth = loader.GetInt();
    int serializedHeight = loader.GetInt();

    // Ensure dimensions match
    if (serializedWidth != width || serializedHeight != height)
    {
        throw std::runtime_error("Map dimensions mismatch during load");
    }

    // Initialize basic structures without generating rooms
    long area = width * height;
    tiles = new Tile[area];
    map = new TCODMap(width, height);
    rng = new TCODRandom(seed, TCOD_RNG_CMWC);

    // Load the saved map state
    for (int i = 0; i < area; i++)
    {
        tiles[i].explored = loader.GetInt();
        bool walkable = loader.GetInt();
        int x = i % width;
        int y = i / width;
        map->setProperties(x, y, walkable, walkable);
    }

    SubscribeToEvents();

     // Load player
     player = new Actor{};
     player->Load(loader);
     actors.push_back(player);

    // Load other actors
    int otherActorCount = loader.GetInt();
    for (int i = 0; i < otherActorCount; i++)
    {
        Actor* actor = new Actor{};
        actor->Load(loader);
        actors.push_back(actor);
 /*       if (actor->name == "player")
        {
            player = actor;
        }*/
    }
}


bool BspCallback::visitNode(TCODBsp* node, void* userData)
{
    if (node->isLeaf())
    {
        // dig a room
        int w{ map.rng->getInt(ROOM_MIN_SIZE, node->w - 2) };
        int h{ map.rng->getInt(ROOM_MIN_SIZE, node->h - 2) };
        int x{ map.rng->getInt(node->x + 1, node->x + node->w - w - 1) };
        int y{ map.rng->getInt(node->y + 1, node->y + node->h - h - 1) };
        Point upperLeft{ x,y };
        Point lowerRight{ x + w - 1, y + h - 1 };
        bool withActors = static_cast<bool>(userData);
        map.CreateRoom(roomNumber == 0, upperLeft, lowerRight, withActors);


        Point roomMiddle{ x + w / 2, y + h / 2 };
        if (roomNumber != 0)
        {
            // dig a corridor from last room
            // hall goes from centre of previous room to middleHallPoint
            // and then on to the middle of the current room
            Point hallMiddle{ x + w / 2, previousLocation.y };
            map.Dig(previousLocation, hallMiddle);
            map.Dig(hallMiddle, roomMiddle);
        }

        previousLocation = roomMiddle;
        roomNumber++;
    }
    return true;
}
