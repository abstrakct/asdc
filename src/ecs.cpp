/*
 * ecs.cpp
 */

#include <vector>
#include "ecs.h"

// Static private members must be initialized like this here.
// This way, we can have several entity managers, and every ID will still be unique.
u64 ecs::EntityManager::entityCounter = 0;

ecs::EntityManager::EntityManager()
{
}

void ecs::EntityManager::create()
{
    entity.emplace_back(new Entity(++entityCounter));
}
