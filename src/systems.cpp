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
//extern sf::RenderTexture renderTexture;
extern sf::RenderWindow window;
extern sf::Texture tex;
extern sf::Sprite sprite;

void CameraSystem::update(const double durationMS)
{
    //u32 start = SDL_GetTicks();
    if(console->dirty) {
        Position *pos = ecs::entity(playerID)->component<Position>();
        Renderable *r = ecs::entity(playerID)->component<Renderable>();

        window.clear(sf::Color::Black);
        console->clear();

        // TODO: more boundary checks!!!!!
        i32 startx = (pos->x - FOV);
        if(startx <= 0) startx = 0;
        i32 starty = (pos->y - FOV);
        if(starty <= 0) starty = 0;
        for (u32 x = startx; x <= (pos->x + FOV); x++) {
            for (u32 y = starty; y <= (pos->y + FOV); y++) {
                if(world->level->cache[x][y].type != cellUnused)
                    console->put(x, y, world->level->cache[x][y].glyph, world->level->cache[x][y].fgColor);
            }
        }

        // Render player
        console->put(pos->x, pos->y, r->glyph, r->fgColor);

        // draw everything to screen.
        window.clear(sf::Color::Black);
        console->render();

        //sf::Uint8 *pixels = console->getPixels();
        //tex.update(pixels);
        //window.draw(sprite); 
        // tex.loadfrommemory?
        //tex.update(console->getPixels());
        
        window.display();
        
        /*SDL_LockTexture(screen, NULL, &pix, &pitch);
        SDL_memcpy(pix, console->getPixels(), SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(u32));
        SDL_UnlockTexture(screen);

        SDL_RenderCopy(renderer, screen, NULL, NULL);
        SDL_RenderPresent(renderer);*/
        console->dirty = false;
    }
    //u32 end = SDL_GetTicks();
    //std::cout << "CameraSystem::update took " << (end - start) << " ms to complete." << std::endl;
}
