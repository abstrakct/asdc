/*
 * systems.cpp
 */

#include <SFML/Graphics.hpp>
#include <iostream>

#include "ecs.h"
#include "systems.h"
#include "console.h"
#include "ui.h"
#include "fov.h"

extern std::unique_ptr<GUI> gui;
extern std::unique_ptr<World> world;
extern std::shared_ptr<Console> mapConsole;
extern u64 playerID;

extern sf::RenderWindow window;
extern sf::RenderTexture tex;

void MapCacheSystem::buildMapCache(RebuildMapCacheMessage &msg)
{
    std::shared_ptr<Level> level = msg.level;
    bool wizMode = msg.wizardMode;

    //TODO: this probably needs to be improved if we add invisible entities
    MapCacheCell tmp;
    for (auto it : level->cells) {
        if(!it->deleted) {
            Position *pos = it->component<Position>();
            Renderable *r = it->component<Renderable>();
            MapCell *cell = it->component<MapCell>();
            Physicality *p= it->component<Physicality>();
            if(pos && r && cell && p) {
                if(p->visible) {
                    tmp.glyph = r->glyph;
                    tmp.fgColor = r->fgColor;
                    tmp.bgColor = r->bgColor;
                    tmp.fadedColor = r->fadedColor;

                    if(wizMode) {
                        tmp.blocksLight = false;
                        tmp.seen = true;
                    } else {
                        tmp.blocksLight = p->blocksLight;
                        tmp.seen = level->cache[pos->x][pos->y].seen;      // keep the value of 'seen'
                    }

                    level->cache[pos->x][pos->y] = tmp;
                }
            }
        }
    }
}

void CameraSystem::update(const double durationMS)
{
    //u32 start = SDL_GetTicks();
    if(mapConsole->dirty) {
        Position *pos = ecs::entity(playerID)->component<Position>();
        Renderable *r = ecs::entity(playerID)->component<Renderable>();
        Vision *v = ecs::entity(playerID)->component<Vision>();

        mapConsole->clear();

        int startx = 0;
        int starty = 0;
        int endx = world->currentLevel->lastx;
        int endy = world->currentLevel->lasty;

        for (i32 x = startx; x <= (i32)endx; x++) {
            for (i32 y = starty; y <= (i32)endy; y++) {
                if (v->fovMap[x][y]) {
                    mapConsole->put(x, y, world->currentLevel->cache[x][y].glyph, world->currentLevel->cache[x][y].fgColor);
                    world->currentLevel->cache[x][y].seen = true;
                } else if (world->currentLevel->cache[x][y].seen) {
                    // TODO/IDEA: have the game choose the faded color? Just take the fgcolor and lower the alpha value.
                    mapConsole->put(x, y, world->currentLevel->cache[x][y].glyph, world->currentLevel->cache[x][y].fadedColor);
                }
            }
        }

        // put player
        mapConsole->put(pos->x, pos->y, r->glyph, r->fgColor);

        // draw everything to screen.
        window.clear(sf::Color::Black);
        // TODO: separate GUI and game loop! right now, gui only updates when player moves/presses a key
        gui->render(window);
        
        mapConsole->dirty = false;

        // Question: is it quicker to draw pixels myself instead of sprites/textures? Probably not?
    }
    //u32 end = SDL_GetTicks();
    //std::cout << "CameraSystem::update took " << (end - start) << " ms to complete." << std::endl;
}


// TODO: improve CameraSystem!
// TODO: Move to a different file!
void VisibilitySystem::configure()
{
        systemName = "Visibility System";
        subscribe<PlayerMovedMessage>([](PlayerMovedMessage &msg) {
                //myRayCastingFOVAlgorithm();
                doShadowCastingFOV(ecs::entity(playerID)->component<Position>(), ecs::entity(playerID)->component<Vision>());
                });
}

void VisibilitySystem::update(const double durationMS)
{
    if(firstRun) {
        ecs::emit(PlayerMovedMessage{});
        firstRun = false;
    }
}










