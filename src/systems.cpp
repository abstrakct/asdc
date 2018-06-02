/*
 * systems.cpp
 */

#include <SFML/Graphics.hpp>
#include <iostream>

#include "ecs.h"
#include "systems.h"
#include "console.h"
#include "gui.h"
#include "fov.h"

extern std::unique_ptr<GUI> gui;
extern std::unique_ptr<World> world;
extern std::shared_ptr<Console> mapConsole;
extern u64 playerID;

extern sf::RenderWindow window;
extern sf::RenderTexture tex;

void MapCacheSystem::buildMapCache(BuildMapCacheMessage &msg)
{
    std::shared_ptr<Level> level = msg.level;
    bool wizMode = msg.wizardMode;

    //TODO: this probably needs to be improved if we add invisible entities
    for (auto it : level->cells) {
        if(!it->deleted) {
            Position *pos = it->component<Position>();
            Renderable *r = it->component<Renderable>();
            MapCell *cell = it->component<MapCell>();
            Physicality *p= it->component<Physicality>();
            if(pos && r && cell && p) {
                MapCacheCell &tmp = level->cache[pos->x][pos->y];
                if(p->visible) {
                    tmp.glyph = r->glyph;
                    tmp.fgColor = r->fgColor;
                    tmp.bgColor = r->bgColor;
                    tmp.fadedColor = r->fadedColor;
                    tmp.blocksMovement = p->blocksMovement;

                    if(wizMode) {
                        tmp.blocksLight = false;
                        tmp.blocksMovement = false;
                        tmp.seen = true;
                    } else {
                        tmp.blocksLight = p->blocksLight;
                        tmp.seen = level->cache[pos->x][pos->y].seen;      // keep the value of 'seen'
                    }
                }
            }
        }
    }
}

// rebuild a part of the map cache. hopefully faster than rebuilding all of it.
void MapCacheSystem::rebuildMapCache(RebuildMapCacheMessage &msg)
{
    std::shared_ptr<Level> level = msg.level;
    bool wizMode = msg.wizardMode;

    //TODO: this probably needs to be improved if we add invisible entities
    for (auto it : level->cells) {
        if(!it->deleted) {
            Position *pos = it->component<Position>();
            if (pos && (pos->x >= msg.x1) && (pos->x <= msg.x2) && (pos->y >= msg.y1) && (pos->y <= msg.y2)) {
                MapCacheCell &tmp = level->cache[pos->x][pos->y];
                Renderable *r = it->component<Renderable>();
                MapCell *cell = it->component<MapCell>();
                Physicality *p= it->component<Physicality>();
                if(pos && r && cell && p) {
                    if(p->visible) {
                        tmp.glyph = r->glyph;
                        tmp.fgColor = r->fgColor;
                        tmp.bgColor = r->bgColor;
                        tmp.fadedColor = r->fadedColor;
                        tmp.blocksMovement = p->blocksMovement;

                        if(wizMode) {
                            tmp.blocksLight = false;
                            tmp.blocksMovement = false;
                            tmp.seen = true;
                        } else {
                            tmp.blocksLight = p->blocksLight;
                            //tmp.seen = level->cache[pos->x][pos->y].seen;      // keep the value of 'seen'
                        }
                    }
                }
            }
        }
    }
}

void RenderSystem::configure()
{
    systemName = "Render System";
    subscribe_mbox<MapRerenderMessage>();
}

void RenderSystem::update(const double durationMS)
{
    std::queue<MapRerenderMessage> *mapChanged = mbox<MapRerenderMessage>();
    while (!mapChanged->empty()) {
        dirty = true;
        mapChanged->pop();
    }

    if (dirty) {
        Position *pos = ecs::entity(playerID)->component<Position>();
        Renderable *r = ecs::entity(playerID)->component<Renderable>();
        Vision *v = ecs::entity(playerID)->component<Vision>();

        mapConsole->clear();

        int px  = mapConsole->widthInChars / 2;   // draw player in the middle of the screen.
        int py  = mapConsole->heightInChars / 2;
        int vx1 = ((i32)(pos->x - px) < 0) ? 0 : (pos->x - px);                    // viewport x1 (coordinate on map)
        int vy1 = ((i32)(pos->y - py) < 0) ? 0 : (pos->y - py);                    // viewport y1 (coordinate on map)
        int sx1 = 0;                             // screen x1 (coordinate on screen)
        int sy1 = 0;
        int sx2 = mapConsole->widthInChars;
        int sy2 = mapConsole->heightInChars;

        // mx, my = map x,y
        for (int y = sy1, my = vy1; y <= sy2; y++, my++) {
            for (int x = sx1, mx = vx1; x <= sx2; x++, mx++) {
                if (v->fovMap[mx][my]) {
                    mapConsole->put(x, y, world->currentLevel->cache[mx][my].glyph, world->currentLevel->cache[mx][my].fgColor);
                    world->currentLevel->cache[mx][my].seen = true;
                } else if (world->currentLevel->cache[mx][my].seen) {
                    // TODO/IDEA: have the game choose the faded color? Just take the fgcolor and lower the alpha value.
                    mapConsole->put(x, y, world->currentLevel->cache[mx][my].glyph, world->currentLevel->cache[mx][my].fadedColor);
                }
                if (mx == pos->x && my == pos->y)
                    mapConsole->put(x, y, r->glyph, r->fgColor);
            }
        }

        // put player

        // draw everything to screen.
        window.clear(sf::Color::Black);
        // TODO: separate GUI and game loop! right now, gui only updates when player moves/presses a key
        gui->render(window);
        
        dirty = false;

        // Question: is it quicker to draw pixels myself instead of sprites/textures? Probably not?
    }
}


// TODO: improve RenderSystem!
// TODO: Move to a different file!
void FOVSystem::configure()
{
        systemName = "Visibility System";
        subscribe<PlayerMovedMessage>([](PlayerMovedMessage &msg) {
                //myRayCastingFOVAlgorithm();
                doShadowCastingFOV(ecs::entity(playerID)->component<Position>(), ecs::entity(playerID)->component<Vision>());
                });
}

void FOVSystem::update(const double durationMS)
{
    if(firstRun) {
        ecs::emit(PlayerMovedMessage{});
        firstRun = false;
    }
}










// vim: fdm=syntax
