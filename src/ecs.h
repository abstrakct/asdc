/*
 * entity.h
 *
 * A lot of this (almost everything) has been adopted from RLTK - github.com/thebracket
 */

#pragma once

#include <bitset>
#include <memory>
#include <unordered_map>
#include <queue>
#include <functional>
#include <mutex>
#include <algorithm>

#include "common.h"

#define MAX_COMPONENTS 128

namespace ecs {

// forward declarations
class Entity;
inline void unsetComponentMask(const u64 id, const u64 familyID, bool deleteIfEmpty = false);
struct SubscriptionMailbox;
struct SubscriptionBase;
struct BaseSystem;
template<typename S, typename ...Args> inline void addSystem(Args && ... args);
void configureAllSystems();
void tick(const double durationMS);
void run(std::function<void(double)> on_tick);

// externs, consts
extern std::unordered_map<u64, Entity> entityStore;
extern std::vector<std::unique_ptr<SubscriptionBase>> pubsubHolder;
extern std::vector<std::unique_ptr<BaseSystem>> systemStore;
const u64 invalidEntity = 0;

/*
 * Base Component class. Components need to be derived from this.
 */
struct BaseComponent {
    static u64 typeCounter;
    u64 entityID;
    bool deleted = false;
};

/*
 * Component class. For handling components.
 */
template<class C> struct Component : public BaseComponent {
    Component() {
        data = C{};
        family();
    }
    Component(C comp) : data(comp) {
        family();
    }

    u64 familyID;
    C data;

    inline void family() {
        static u64 familyIDTemp = ++BaseComponent::typeCounter;
        familyID = familyIDTemp;
    }
};

/*
 * Base Component Store. Actual component stores derive from this class.
 */
struct BaseComponentStore {
    virtual void eraseByEntityID(const u64 &id) = 0;
    virtual void reallyDelete() = 0;
    virtual u64 size() = 0;
};

/*
 * Component store.
 */
template<class C> struct ComponentStore : public BaseComponentStore {
    std::vector<C> components;

    virtual void eraseByEntityID(const u64 &id) override final {
        for(auto &item : components) {
            item.deleted = true;
            unsetComponentMask(id, item.familyID);
        }
    }
    virtual void reallyDelete() override final {
        components.erase(std::remove_if(components.begin(), components.end(),
                                        [] (auto x) { return x.deleted; }),
                                        components.end());
    }
    virtual u64 size() override final { return components.size(); }
};

extern std::vector<std::unique_ptr<BaseComponentStore>> componentStore;

/*
 * Entity class definition
 */
class Entity {
    static u64 entityCounter;
public:
    Entity() {
        id = ++entityCounter;
    };
    Entity(u64 newID) {
        id = newID;
        entityCounter = newID+1;
    }
    // TODO: constructor for specific ID
    // Operator overloading
    bool operator == (const Entity& other) const { return id == other.id; }
    bool operator != (const Entity& other) const { return id != other.id; }

    u64 id;
    bool deleted = false;
    std::bitset<MAX_COMPONENTS> componentMask;

    /* Assign a component to this entity */
    template<class C>
    inline Entity* assign(C &&component) {
        if(deleted) throw std::runtime_error("Cannot assign component to a deleted entity!");
        Component<C> temp(component);
        temp.entityID = id;
        if(componentStore.size() < temp.familyID+1)
            componentStore.resize(temp.familyID+1);
        if(!componentStore[temp.familyID])
            componentStore[temp.familyID] = std::move(std::make_unique<ComponentStore<Component<C>>>());

        static_cast<ComponentStore<Component<C>> *>(componentStore[temp.familyID].get())->components.push_back(temp);
        componentMask.set(temp.familyID);
        return this;
    }

    /* Get the component of type C. Returns pointer to component. */
    template<class C>
    inline C * component() noexcept {
        C * result = nullptr;
        if(deleted) return result;

        C emptyComponent;
        Component<C> temp(emptyComponent);
        if(!componentMask.test(temp.familyID)) return result;
        for(Component<C> &component : static_cast<ComponentStore<Component<C>> *>(componentStore[temp.familyID].get())->components) {
            if(component.entityID == id) {
                result = &component.data;
                return result;
            }
        }
        return result;
    }
};

// More stuff borrowed/adapted/adopted from RLTK:
/*
 * entity(ID) is used to reference an entity. So you can, for example, do:
 * entity(12)->component<position_component>()->x = 3;
 */
Entity* entity(const u64 id) noexcept;

/*
 * Creates an entity with a new ID #. Returns a pointer to the entity, to enable
 * call chaining. For example create_entity()->assign(foo)->assign(bar)
 */
Entity* createEntity();
Entity* createEntity(u64 newID);

/*
 * Delete an entity.
 */
inline void deleteEntity(const u64 id)
{
    auto e = entity(id);
    if(!e) return;
    e->deleted = true; // todo: implement garbage collection?!

    for(auto &store : componentStore) {
        if(store)
            store->eraseByEntityID(id);
    }
}

inline void deleteEntity(Entity &e) noexcept
{
    deleteEntity(e.id);
}

inline void unsetComponentMask(const u64 id, const u64 familyID, bool deleteIfEmpty)
{
    auto finder = entityStore.find(id);
    if(finder != entityStore.end()) {
        finder->second.componentMask.reset(familyID);
        if(deleteIfEmpty && finder->second.componentMask.none())
            finder->second.deleted = true;
    }
}

template<class C>
inline void deleteComponent(const u64 entityID, bool deleteEntityIfEmpty=false) noexcept {
    auto eptr = entity(entityID);
    if (!eptr) return;
    Entity e = *entity(entityID);
    C emptyComponent;
    Component<C> temp(emptyComponent);
    if(!e.componentMask.test(temp.familyID)) return;
    for(Component<C> &component : static_cast<ComponentStore<Component<C>> *>(componentStore[temp.familyID].get())->components) {
        if (component.entityID == entityID) {
            component.deleted = true;
            unsetComponentMask(entityID, temp.familyID, deleteEntityIfEmpty);
        }
    }
}

/* 
 * I wrote this one myself, and it works! I consider it proof that I now understand this architecture pretty well. 
 * This returns a vector with all Entities that have a component of type C
 * TODO: it is slow, maybe. check that, and update function to match the one in RLTK if so.
 */
template<class C>
inline std::vector<Entity *> findAllEntitiesWithComponent()
{
    C empty;
    Component<C> handle(empty);
    u64 family = handle.familyID;
    std::vector<Entity*> result;

    for(auto &it : entityStore) {
        if(it.second.componentMask[family]) {
            result.push_back(&it.second);
        }
    }

    return result;
}

/*
 * Mailboxes and message delivery system - again from RLTK
 */

struct BaseMessage {
    static u64 typeCounter;
};

template <class C> struct Message : public BaseMessage {
    Message() {
        C empty;
        data = empty;
        family();
    }
    Message(C comp) : data(comp) {
        family();
    }

    u64 familyID;
    C data;

    inline void family() {
        static u64 familyIDTemp = BaseMessage::typeCounter++;
        familyID = familyIDTemp;
    }
};

struct SubscriptionBase {
    virtual void deliverMessages() = 0;
};

struct SubscriptionMailbox {
};

template <class C> struct Mailbox : SubscriptionMailbox {
    std::queue<C> messages;
};

template <class C> struct SubscriptionHolder : SubscriptionBase {
    std::queue<C> deliveryQueue;
    std::mutex deliveryMutex;
    std::vector<std::tuple<bool, std::function<void(C& message)>, BaseSystem *>> subscriptions;

    virtual void deliverMessages() override {
        std::lock_guard<std::mutex> guard(deliveryMutex);
        while(!deliveryQueue.empty()) {
            C message = deliveryQueue.front();
            deliveryQueue.pop();
            Message<C> handle(message);

            for(auto &func : subscriptions) {
                if(std::get<0>(func) && std::get<1>(func)) {
                    std::get<1>(func)(message);
                } else {
                    // It is destined for the system's mailbox queue.
                    auto finder = std::get<2>(func)->mailboxes.find(handle.familyID);
                    if (finder != std::get<2>(func)->mailboxes.end()) {
                        static_cast<Mailbox<C> *>(finder->second.get())->messages.push(message);
                    }
                }
            }
        }
    }
};

/*
 * Systems
 */
/*
 * Base System definition. Systems must inherit from this class.
 */
struct BaseSystem {
    virtual void configure() {}
    virtual void update(const double durationMS) = 0;
    std::string systemName = "Unnamed system";
    std::unordered_map<u64, std::unique_ptr<SubscriptionMailbox>> mailboxes;

    template<class MSG> void subscribe(std::function<void(MSG &message)> destination) {
        MSG emptyMessage;
        Message<MSG> handle(emptyMessage);
        if(pubsubHolder.size() < handle.familyID + 1)
            pubsubHolder.resize(handle.familyID + 1);
        if(!pubsubHolder[handle.familyID])
            pubsubHolder[handle.familyID] = std::move(std::make_unique<SubscriptionHolder<MSG>>());

        static_cast<SubscriptionHolder<MSG> *>(pubsubHolder[handle.familyID].get())->subscriptions.push_back(std::make_tuple(true, destination, nullptr));
    }

    template<class MSG> void subscribe_mbox() {
        MSG emptyMessage{};
        Message<MSG> handle(emptyMessage);
        if (pubsubHolder.size() < handle.familyID + 1)
            pubsubHolder.resize(handle.familyID + 1);
        if (!pubsubHolder[handle.familyID])
            pubsubHolder[handle.familyID] = std::move(std::make_unique<SubscriptionHolder<MSG>>());
        std::function<void(MSG &message)> destination;
        static_cast<SubscriptionHolder<MSG> *>(pubsubHolder[handle.familyID].get())->subscriptions.push_back(std::make_tuple(false, destination, this));
        mailboxes[handle.familyID] = std::make_unique<Mailbox<MSG>>();
    }

    template<class MSG> std::queue<MSG> *mbox() {
        Message<MSG> handle(MSG{});
        auto finder = mailboxes.find(handle.familyID);
        if (finder != mailboxes.end()) {
            return &static_cast<Mailbox<MSG> *>(finder->second.get())->messages;
        } else {
            return nullptr;
        }
    }
};

template<typename S, typename ...Args>
inline void addSystem(Args && ... args) {
    systemStore.push_back(std::make_unique<S>(std::forward<Args>(args) ... ));
}

/*
 * Submit a message for delivery. It will be delivered to every system that has issued a subscribe or subscribe_mbox call.
 */
template <class MSG> inline void emit(MSG message)
{
    Message<MSG> handle(message);
    if(pubsubHolder.size() > handle.familyID) {
        for(auto &func : static_cast<SubscriptionHolder<MSG> *>(pubsubHolder[handle.familyID].get())->subscriptions) {
            if (std::get<0>(func) && std::get<1>(func)) {
                std::get<1>(func)(message);
            } else {
                auto finder = std::get<2>(func)->mailboxes.find(handle.familyID);
                if (finder != std::get<2>(func)->mailboxes.end()) {
                    static_cast<Mailbox<MSG> *>(finder->second.get())->messages.push(message);
                }
            }
        }
    }
}

void collectGarbage();

} // namespace ecs
// vim: fdm=syntax
