/*
 * messages.h
 */

#pragma once
#include "ecs.h"
#include "world.h"

struct ActorMovedMessage : ecs::BaseMessage {
    ActorMovedMessage() {}
    ActorMovedMessage(ecs::Entity* a, i32 x, i32 y) : actor(a), dx(x), dy(y) {}
    
    ecs::Entity *actor;
    i32 dx, dy;
};

struct PlayerMovedMessage : ecs::BaseMessage {
};

// TODO: rename to BuildMapCacheMessage
struct RebuildMapCacheMessage : ecs::BaseMessage {
    RebuildMapCacheMessage() {};
    RebuildMapCacheMessage(std::shared_ptr<Level> l, bool wizMode) : level(l), wizardMode(wizMode) {}

    std::shared_ptr<Level> level;
    bool wizardMode;
};

struct MapRerenderMessage : ecs::BaseMessage {
};
