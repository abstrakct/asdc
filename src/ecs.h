/*
 * entity.h
 *
 * A lot of this (almost everything) has been adopted from RLTK - github.com/thebracket
 */

#pragma once

#include <bitset>
#include <memory>
#include "common.h"

#define MAX_COMPONENTS 128

namespace ecs {

const u64 invalidEntity = 0;

/*
 * Base Component class. Components need to be derived from this.
 */
struct BaseComponent {
    static u64 typeCounter;
    u64 entityId;
    bool deleted = false;
};

/*
 * Component class. For handling components.
 */
template<class C>
struct Component : public BaseComponent {
    Component() {
        data = C{};
        family();
    }
    Component(C comp) : data(comp) {
        family();
    }

    u64 familyId;
    C data;

    inline void family() {
        static u64 familyIdTemp = BaseComponent::typeCounter++;
        familyId = familyIdTemp;
    }
};

/*
 * Base Component Store. Actual component stores derive from this class.
 */
struct BaseComponentStore {
    virtual void eraseByEntityId(const u64 &id) = 0;
    virtual void reallyDelete() = 0;
    virtual u64 size() = 0;
};

/*
 * Component store.
 */
template<class C>
struct ComponentStore : public BaseComponentStore {
    std::vector<C> components;

    virtual void eraseByEntityId(const u64 &id) override final {
    }
    virtual void reallyDelete() override final {
    }
    virtual u64 size() override final { return components.size(); }
};

extern std::vector<std::unique_ptr<BaseComponentStore>> componentStore;

/*
 * Entity class
 */
class Entity {
    static u64 entityCounter;
public:
    Entity() {
        id = ++entityCounter;
    };
    // TODO: constructor for specific ID
    // Operator overloading
    bool operator == (const Entity& other) const { return id == other.id; }
    bool operator != (const Entity& other) const { return id != other.id; }

    u64 id; // make private? probably not
    bool deleted = false;
    std::bitset<MAX_COMPONENTS> componentMask;

    /* Assign a component to this entity */
    template<class C>
    inline Entity* assign(C &&component) {
        if(deleted) throw std::runtime_error("Cannot assign component to a deleted entity!");
        Component<C> temp(component);
        temp.entityId = id;
        if(componentStore.size() < temp.familyId+1)
            componentStore.resize(temp.familyId+1);
        if(!componentStore[temp.familyId])
            componentStore[temp.familyId] = std::move(std::make_unique<ComponentStore<Component<C>>>());

        static_cast<ComponentStore<Component<C>> *>(componentStore[temp.familyId].get())->components.push_back(temp);
        componentMask.set(temp.familyId);
        return this;
    }

    /* Get the component of type C. Returns pointer to component. */
    template<class C>
    inline C * component() noexcept {
        C * result = nullptr;
        if(deleted) return result;

        C emptyComponent;
        Component<C> temp(emptyComponent);
        if(!componentMask.test(temp.familyId)) return result;
        for(Component<C> &component : static_cast<ComponentStore<Component<C>> *>(componentStore[temp.familyId].get())->components) {
            if(component.entityId == id) {
                result = &component.data;
                return result;
            }
        }
        return result;
    }

    /*template<class C>
    inline C * component() {
        return component<C>(*this);
    }*/
};

// Borrowed from RLTK:
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


}
