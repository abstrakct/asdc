/*
 * ecs.cpp
 *
 * Pretty much all of the ECS was taken from RLTK (github.com/thebracket) - Huge thanks!
 */

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "ecs.h"

namespace ecs {

std::unordered_map<u64, Entity> entityStore;
std::vector<std::unique_ptr<BaseComponentStore>> componentStore;
std::vector<std::unique_ptr<SubscriptionBase>> pubsubHolder;
std::vector<std::unique_ptr<BaseSystem>> systemStore;

u64 Entity::entityCounter{0};
u64 BaseComponent::typeCounter = 0;
u64 BaseMessage::typeCounter = 0;

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

Entity* createEntity(u64 newID)
{
    Entity newEntity(newID);
    if (entityStore.find(newEntity.id) != entityStore.end()) {
        throw std::runtime_error("WARNING! Duplicate Entity IDs! This will not end well!");
    }
    entityStore.emplace(newEntity.id, newEntity);
    return entity(newEntity.id);
}

void configureAllSystems()
{
    for (std::unique_ptr<BaseSystem> &sys : systemStore)
        sys->configure();
}

// Actually delete entities and components marked as deleted. Should be called periodically.
void collectGarbage()
{
    std::unordered_set<u64> entitiesToDelete;

    for (auto it = entityStore.begin(); it != entityStore.end(); ++it) {
        if (it->second.deleted) {
            for (std::unique_ptr<BaseComponentStore> &store : componentStore) {
                if (store)
                    store->eraseByEntityID(it->second.id);
            }
            entitiesToDelete.insert(it->second.id);
        }
    }

    for (const u64 &id : entitiesToDelete)
        entityStore.erase(id);

    for (std::unique_ptr<BaseComponentStore> &store : componentStore) {
        if (store)
            store->reallyDelete();
    }
}


void tick(const double durationMS)
{
    for (std::unique_ptr<BaseSystem> &sys : systemStore) {
        sys->update(durationMS);
    }
    //collectGarbage();
}


} // namespace ecs
