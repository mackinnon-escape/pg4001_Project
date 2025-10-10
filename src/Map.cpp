#include "Map.h"

#include "Engine.h"
#include "Actor.h"
#include "Colours.h"

static constexpr int MAX_ROOM_MONSTERS = 3;
static constexpr int ROOM_MAX_SIZE{ 12 };
static constexpr int ROOM_MIN_SIZE{ 6 };

Map::Map(int width, int height) : width(width), height(height)
{
    seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

Map::~Map()
{
    delete[] tiles;
    delete map;
}

void Map::Init(bool withActors)
{
    TCODBsp bsp(0, 0, width, height);

    long area{ width * height };
    tiles = new Tile[area];

    BspCallback listener(*this);
    rng = new TCODRandom(seed, TCOD_RNG_CMWC);
    map = new TCODMap(width, height);
    bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
    bsp.traverseInvertedLevelOrder(&listener, static_cast<void*>(&withActors));
}

bool Map::IsWall(const Point& position) const
{
    return !map->isWalkable(position.x, position.y);
}

void Map::Render() const
{
    static constexpr tcod::ColorRGB darkWall(0, 0, 100);
    static constexpr tcod::ColorRGB darkGround(50, 50, 150);
    static constexpr tcod::ColorRGB lightWall(130, 110, 50);   // Visible walls
    static constexpr tcod::ColorRGB lightGround(200, 180, 50); // Visible floors

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Point p{ x, y };
            if (IsInFov(p))
            {
                // Currently visible - bright colors
                Engine::GetInstance()->console.at(x, y).bg = IsWall(p) ? lightWall : lightGround;
            }
            else if (IsExplored(p))
            {
                // Previously seen - dim colors
                Engine::GetInstance()->console.at(x, y).bg = IsWall(p) ? darkWall : darkGround;
            }
            // Unseen areas remain black (default background)
        }
    }
}

bool Map::CanWalk(const Point& position) const
{
    if (IsWall(position))
    {
        return false;    // this is a wall
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
    map->computeFov(Engine::GetInstance()->player->GetLocation().x,
        Engine::GetInstance()->player->GetLocation().y,
        Engine::GetInstance()->fovRadius);
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

void Map::CreateRoom(const bool first, const Point& corner1, const Point& corner2, bool withActors) const
{
    if (!withActors) { return; }	// skip out from making actors if withActors is false

    Dig(corner1, corner2);
    Point middleOfRoom{ (corner1 + corner2) / 2 };

    // put the player in the first room
    if (first)
    {
        Engine::GetInstance()->player->SetLocation(middleOfRoom);
    }
    else
    {
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

void Map::AddMonster(const Point& location) const
{
    if (rng->getInt(0, 100) < 80)
    {
        // create an orc
        Actor* orc = new Actor(location, 'o', "orc", DESATURATED_GREEN);
        Engine::GetInstance()->actors.push_back(orc);
    }
    else
    {
        // create a troll
        Actor* troll = new Actor(location, 'T', "troll", DARKER_GREEN);
        Engine::GetInstance()->actors.push_back(troll);
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