/*
 * systems.h
 */

#pragma once

#include "ecs.h"
#include "messages.h"
#include "world.h"
#include "ui.h"
#include "console.h"

extern std::unique_ptr<GUI> gui;

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

struct PlayerSystem : public ecs::BaseSystem {
    virtual void configure() override {
        systemName = "Player System";
        subscribe<ActorMovedMessage>([](ActorMovedMessage &msg) {
                gui->getLayer(0)->console->dirty = true;
                emit(PlayerMovedMessage{});
                });
        subscribe_mbox<KeyPressed>();
    }

    virtual void update(const double durationMS) override {
        // This system is subscribed to KeyPressed messages.
        // Now we get that message queue and loop through it.
        std::queue<KeyPressed> *messages = mbox<KeyPressed>();
        while(!messages->empty()) {
            KeyPressed e = messages->front();
            messages->pop();

            if (e.event.key.code == sf::Keyboard::H) ecs::emit(ActorMovedMessage { ecs::entity(playerID), -1,  0 });
            if (e.event.key.code == sf::Keyboard::J) ecs::emit(ActorMovedMessage { ecs::entity(playerID),  0,  1 });
            if (e.event.key.code == sf::Keyboard::K) ecs::emit(ActorMovedMessage { ecs::entity(playerID),  0, -1 });
            if (e.event.key.code == sf::Keyboard::L) ecs::emit(ActorMovedMessage { ecs::entity(playerID),  1,  0 });
            if (e.event.key.code == sf::Keyboard::Y) ecs::emit(ActorMovedMessage { ecs::entity(playerID), -1, -1 });
            if (e.event.key.code == sf::Keyboard::U) ecs::emit(ActorMovedMessage { ecs::entity(playerID),  1, -1 });
            if (e.event.key.code == sf::Keyboard::B) ecs::emit(ActorMovedMessage { ecs::entity(playerID), -1,  1 });
            if (e.event.key.code == sf::Keyboard::N) ecs::emit(ActorMovedMessage { ecs::entity(playerID),  1,  1 });
        }
    }
};

struct CameraSystem : public ecs::BaseSystem {
    virtual void configure() override {
        systemName = "Camera System";
    }

    virtual void update(const double durationMS) override;
};
