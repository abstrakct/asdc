/*
 * ecs.cpp
 *
 * some inspiration taken from RLTK (github.com/thebracket) - see notes here and there!
 */

#include <vector>
#include <unordered_map>
#include <memory>

#include "ecs.h"

namespace ecs {

std::unordered_map<u64, Entity> entityStore;
std::vector<std::unique_ptr<BaseComponentStore>> componentStore;

u64 Entity::entityCounter{1};
u64 BaseComponent::typeCounter = 1;

// Borrowed from RLTK because I really like it :)
// My understanding: it looks up the entity with a specific ID in entityStore and returns a reference to it.
// Returns false if no entity with ID is found, or entity is deleted.
// segfaults if you try to just do entity(invalidid)->...
Entity* entity(const u64 id) noexcept {
    Entity *result = nullptr;
	auto finder = entityStore.find(id);

	if (finder == entityStore.end())
	    return result;
	if (finder->second.deleted)
	    return result;

	result = &finder->second;
	return result;
}

Entity* createEntity()
{
    Entity newEntity;
    entityStore.emplace(newEntity.id, newEntity);
    return entity(newEntity.id);
}


} // namespace ecs
