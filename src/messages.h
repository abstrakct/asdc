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

struct BuildMapCacheMessage : ecs::BaseMessage {
    BuildMapCacheMessage() {};
    BuildMapCacheMessage(std::shared_ptr<Level> l, bool wizMode) : level(l), wizardMode(wizMode) {}

    std::shared_ptr<Level> level;
    bool wizardMode;
};

struct RebuildMapCacheMessage : ecs::BaseMessage {
    RebuildMapCacheMessage() {};
    RebuildMapCacheMessage(std::shared_ptr<Level> l, int _x1, int _y1, int _x2, int _y2, bool wizMode) : level(l), x1(_x1), y1(_y1), x2(_x2), y2(_y2), wizardMode(wizMode) {}

    std::shared_ptr<Level> level;
    int x1, y1, x2, y2;
    bool wizardMode;
};

struct MapRerenderMessage : ecs::BaseMessage {
};
