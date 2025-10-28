#include "Actor.h"

#include "Engine.h"
#include "Map.h"
#include "Serialise.h"

Actor::~Actor()
{
    delete attacker;
    delete destructible;
    delete pickable;
    delete container;
}

void Actor::Update(Input& input, ILocationProvider& locationProvider)
{
    hasActedThisFrame = false;
    if (ai) 
    { 
        ai->Update(this, locationProvider, input);
    }
}

void Actor::Render(tcod::Console& console) const
{
    console.at(position.x, position.y).ch = ch;
    console.at(position.x, position.y).fg = TCOD_ColorRGBA{ colour.r, colour.g, colour.b, 255 };
}

/*
* transform the actor into a corpse!
*/
void Actor::ChangeToCorpse(std::string corpseName)
{
    ch = '%';
    colour = DARK_RED;
    name = corpseName;
    blocks = false;
}

int Actor::TakeDamage(int damage)
{
    if (destructible)
    {
        return destructible->TakeDamage(this, damage);
    }
    return 0;
}

bool Actor::RemoveFromContainer(Actor* item)
{
    if (container)
    {
        container->Remove(item);
        return true;
    }
    return false;
}

void Actor::Save(Saver& saver) const
{
    position.Save(saver);
    saver.PutInt(ch);
    saver.PutColor(&colour);
    saver.PutString(name);
    saver.PutInt(blocks ? 1 : 0);
    saver.PutInt(fovOnly ? 1 : 0);

    saver.PutInt(attacker != nullptr);
    saver.PutInt(destructible != nullptr);
    saver.PutInt(ai != nullptr ? 1 : 0);
    saver.PutInt(pickable != nullptr);
    saver.PutInt(container != nullptr);

    if (attacker != nullptr) attacker->Save(saver);
    if (destructible != nullptr) destructible->Save(saver);
    if (ai != nullptr) ai->Save(saver);
    if (pickable != nullptr) pickable->Save(saver);
    if (container != nullptr) container->Save(saver);
}

void Actor::Load(Loader& loader)
{
    position.Load(loader);
    ch = loader.GetInt();
    colour = loader.GetColor();
    name = loader.GetString();
    blocks = loader.GetInt();
    fovOnly = loader.GetInt();

    bool hasAttacker = loader.GetInt();
    bool hasDestructible = loader.GetInt();
    bool hasAi = loader.GetInt();
    bool hasPickable = loader.GetInt();
    bool hasContainer = loader.GetInt();

    attacker = nullptr;
    destructible = nullptr;
    ai.reset(nullptr);
    pickable = nullptr;
    container = nullptr;

    if (hasAttacker)
    {
        attacker = new Attacker(0, WHITE);
        attacker->Load(loader);
    }

    if (hasDestructible)
    {
        destructible = Destructible::Create(loader);
    }

    if (hasAi)
    {
        ai = Ai::Create(loader);
    }

    if (hasPickable)
    {
        pickable = new Pickable(nullptr, nullptr);
        pickable->Load(loader);
    }

    if (hasContainer)
    {
        container = new Container(0);
        container->Load(loader);
    }
}