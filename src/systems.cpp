/*
 * systems.cpp
 */

#include <SFML/Graphics.hpp>
#include <iostream>

#include "ecs.h"
#include "systems.h"
#include "console.h"

extern std::shared_ptr<Console> console;
extern std::shared_ptr<World> world;
extern sf::RenderWindow window;

void CameraSystem::update(const double durationMS)
{
    //u32 start = SDL_GetTicks();
    if(console->dirty) {
        Position *pos = ecs::entity(playerID)->component<Position>();
        Renderable *r = ecs::entity(playerID)->component<Renderable>();

        console->clear();

        i32 startx = (pos->x - FOV);
        u32 endx   = (pos->x + FOV);
        i32 starty = (pos->y - FOV);
        u32 endy   = (pos->y + FOV);

        if (startx <= 0) startx = 0;
        if (starty <= 0) starty = 0;
        if (endx >= console->cols) endx = console->cols-1;
        if (endy >= console->cols) endy = console->rows-1;

        for (i32 x = startx; x <= (i32)endx; x++) {
            for (i32 y = starty; y <= (i32)endy; y++) {
                if(world->level->cache[x][y].type != cellUnused)
                    console->put(x, y, world->level->cache[x][y].glyph, world->level->cache[x][y].fgColor);
            }
        }

        // Render player
        console->put(pos->x, pos->y, r->glyph, r->fgColor);

        // draw everything to screen.
        window.clear(sf::Color::Black);
        console->render();
        window.display();
        console->dirty = false;

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
