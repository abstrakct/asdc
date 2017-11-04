/*
 * systems.cpp
 */

#include <SFML/Graphics.hpp>
#include <iostream>

#include "ecs.h"
#include "systems.h"
#include "console.h"
#include "ui.h"

#define con gui->getLayer(0)->console
extern std::unique_ptr<GUI> gui;
extern std::shared_ptr<World> world;

extern sf::RenderWindow window;
extern sf::RenderTexture tex;

// TODO: rendering is still too slow! Look into optimizations!

void CameraSystem::update(const double durationMS)
{
    //u32 start = SDL_GetTicks();
    if(con->dirty) {
        Position *pos = ecs::entity(playerID)->component<Position>();
        Renderable *r = ecs::entity(playerID)->component<Renderable>();

        con->clear();

        i32 startx = (pos->x - FOV);
        u32 endx   = (pos->x + FOV);
        i32 starty = (pos->y - FOV);
        u32 endy   = (pos->y + FOV);

        if (startx <= 0) startx = 0;
        if (starty <= 0) starty = 0;
        if (endx >= con->widthInChars) endx = con->widthInChars-1;
        if (endy >= con->heightInChars) endy = con->heightInChars-1;

        for (i32 x = startx; x <= (i32)endx; x++) {
            for (i32 y = starty; y <= (i32)endy; y++) {
                if(world->level->cache[x][y].type != cellUnused)
                    con->put(x, y, world->level->cache[x][y].glyph, world->level->cache[x][y].fgColor);
            }
        }

        // put player
        con->put(pos->x, pos->y, r->glyph, r->fgColor);

        // draw everything to screen.
        window.clear(sf::Color::Black);
        gui->render(window);
        con->dirty = false;

        // Question: is it quicker to draw pixels myself instead of sprites/textures? Probably not?
        //sf::Uint8 *pixels = console->getPixels();
        //tex.update(pixels);
        //window.draw(sprite); 
        // tex.loadfrommemory?
        //tex.update(console->getPixels());
    }
    //u32 end = SDL_GetTicks();
    //std::cout << "CameraSystem::update took " << (end - start) << " ms to complete." << std::endl;
}
