/*
 * components.h
 */

#pragma once

#include "ecs.h"

struct HealthComponent {
    HealthComponent() {}
    HealthComponent(u32 h, u32 m) : hitPoints(h), maxHitPoints(m) {}
    u32 hitPoints = 50;
    u32 maxHitPoints = 100;
};
