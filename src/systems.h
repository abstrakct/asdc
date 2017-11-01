/*
 * systems.h
 */

#pragma once

#include "ecs.h"
#include "messages.h"
#include "world.h"

struct ActorMovementSystem : public ecs::BaseSystem {
    virtual void configure() override {
        systemName = "Actor Movement System";
        subscribe<ActorMovedMessage>([](ActorMovedMessage &msg) {
                u32 newx = msg.actor->component<Position>()->x + msg.dx;
                u32 newy = msg.actor->component<Position>()->y + msg.dy;
                if (!positionBlocksMovement(newx, newy)) {
                    msg.actor->component<Position>()->x = newx;
                    msg.actor->component<Position>()->y = newy;
                }
                });
    }
    virtual void update(const double durationMS) override {
    }
};

struct CameraSystem : public ecs::BaseSystem {
    virtual void configure() override {
        systemName = "Camera System";
    }

    virtual void update(const double durationMS) override;
};
