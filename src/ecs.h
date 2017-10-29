/*
 * entity.h
 */

#pragma once
#include <vector>
#include "common.h"

namespace ecs {

const  u64 invalidEntity = 0;

class Entity {
    public:
        Entity(u64 i) : id(i) {};

        u64 id;
};

class EntityManager {
    private:
        static u64 entityCounter;
        std::vector<Entity*> entity;
    public:
        EntityManager();
        void create();
};

}
