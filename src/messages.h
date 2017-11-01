/*
 * messages.h
 */

#pragma once
#include "ecs.h"

struct ActorMovedMessage : ecs::BaseMessage {
    ActorMovedMessage() {}
    ActorMovedMessage(ecs::Entity* a, i32 x, i32 y) : actor(a), dx(x), dy(y) {}
    
    ecs::Entity *actor;
    i32 dx, dy;
};
