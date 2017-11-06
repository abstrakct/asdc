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

extern sf::RenderWindow window;
extern sf::RenderTexture tex;

// PROBABLY RESOLVED_TODO: rendering is still too slow! Look into optimizations!

void CameraSystem::update(const double durationMS)
{
    //u32 start = SDL_GetTicks();
    if(mapConsole->dirty) {
        Position *pos = ecs::entity(playerID)->component<Position>();
        Renderable *r = ecs::entity(playerID)->component<Renderable>();
        Vision *v = ecs::entity(playerID)->component<Vision>();

        mapConsole->clear();

        i32 startx = (pos->x - v->fovRadius);
        u32 endx   = (pos->x + v->fovRadius);
        i32 starty = (pos->y - v->fovRadius);
        u32 endy   = (pos->y + v->fovRadius);

        if (startx <= 0) startx = 0;
        if (starty <= 0) starty = 0;
        if (endx >= world->currentLevel->width)  endx = world->currentLevel->width  - 1;
        if (endy >= world->currentLevel->height) endy = world->currentLevel->height - 1;

        for (i32 x = startx; x <= (i32)endx; x++) {
            for (i32 y = starty; y <= (i32)endy; y++) {
                if (world->currentLevel->cache[x][y].type != cellUnused) {
                    if (v->fovMap[x][y]) {
                        mapConsole->put(x, y, world->currentLevel->cache[x][y].glyph, world->currentLevel->cache[x][y].fgColor);
                    }
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










