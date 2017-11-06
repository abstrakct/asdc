/*
 * systems.cpp
 */

#include <SFML/Graphics.hpp>
#include <iostream>

#include "ecs.h"
#include "systems.h"
#include "console.h"
#include "ui.h"
#include "utils.h"

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

// TODO: I don't think the FOV update triggers correctly! It seems to calculate FOV from player's previous position!
// TODO: improve CameraSystem!
void VisibilitySystem::configure()
{
        systemName = "Visibility System";
        subscribe<PlayerMovedMessage>([](PlayerMovedMessage &msg) {
                const int fov = ecs::entity(playerID)->component<Vision>()->fovRadius; 
                const Position *pos = ecs::entity(playerID)->component<Position>();
                std::array<std::array<bool, 256>, 256> &fovMap = ecs::entity(playerID)->component<Vision>()->fovMap;

                // Algorithm for FOV:
                // Clear FOVmap
                // Draw lines to each point on the edge of FOV
                // Walk each line, mark each point as visible if it doesn't block light.
                // If we come across something that blocks light, mark it as visible, and the rest of the line as invisible.

                // clear FOV
                fovMap = {};

                const int startx = pos->x;
                const int starty = pos->y;
                for (int endx = (startx - fov); endx <= (startx + fov); endx++) {
                    // Find endpoint
                    int endy = starty - fov;
                    // Draw the line
                    std::vector<std::pair<int, int>> line = getLineCoordinates(startx, starty, endx, endy);

                    // TODO: move "line walking" to separate function? inline?
                    
                    bool vis  = true;
                    // Iterate through all points in line.
                    for (auto it : line) {
                        int tx, ty;
                        std::tie(tx, ty) = it;
                        if(tx < 0) break;
                        if(ty < 0) break;
                        if (!world->currentLevel->cache[tx][ty].blocksLight) {
                            fovMap[tx][ty] = vis;
                        } else {
                            // If a cell blocks light, set that cell to visible
                            fovMap[tx][ty] = vis;
                            // But further cells on that line will be set to invisible, regardless of whether they block light or not.
                            vis = false;
                        }
                    }
                } // end of first x loop

                for (int endy = (starty - fov); endy <= (starty + fov); endy++) {
                    int endx = startx + fov;
                    std::vector<std::pair<int, int>> line = getLineCoordinates(startx, starty, endx, endy);
                    bool vis  = true;
                    // Iterate through all points in line.
                    for (auto it : line) {
                        int tx, ty;
                        std::tie(tx, ty) = it;
                        if(tx < 0) break;
                        if(ty < 0) break;
                        if (!world->currentLevel->cache[tx][ty].blocksLight) {
                            fovMap[tx][ty] = vis;
                        } else {
                            // If a cell blocks light, set that cell to visible
                            fovMap[tx][ty] = vis;
                            // But further cells on that line will be set to invisible, regardless of whether they block light or not.
                            vis = false;
                        }
                    }
                } // end of first y loop

                for (int endx = (startx + fov); endx >= (startx - fov); endx--) {
                    int endy = starty + fov;
                    std::vector<std::pair<int, int>> line = getLineCoordinates(startx, starty, endx, endy);
                    bool vis  = true;
                    // Iterate through all points in line.
                    for (auto it : line) {
                        int tx, ty;
                        std::tie(tx, ty) = it;
                        if(tx < 0) break;
                        if(ty < 0) break;
                        if (!world->currentLevel->cache[tx][ty].blocksLight) {
                            fovMap[tx][ty] = vis;
                        } else {
                            // If a cell blocks light, set that cell to visible
                            fovMap[tx][ty] = vis;
                            // But further cells on that line will be set to invisible, regardless of whether they block light or not.
                            vis = false;
                        }
                    }
                } // end of second x loop

                for (int endy = (starty + fov); endy >= (starty - fov); endy--) {
                    // Find endpoint
                    int endx = startx - fov;
                    // Draw the line
                    std::vector<std::pair<int, int>> line = getLineCoordinates(startx, starty, endx, endy);
                    bool vis  = true;
                    // Iterate through all points in line.
                    for (auto it : line) {
                        int tx, ty;
                        std::tie(tx, ty) = it;
                        if(tx < 0) break;
                        if(ty < 0) break;
                        if (!world->currentLevel->cache[tx][ty].blocksLight) {
                            fovMap[tx][ty] = vis;
                        } else {
                            // If a cell blocks light, set that cell to visible
                            fovMap[tx][ty] = vis;
                            // But further cells on that line will be set to invisible, regardless of whether they block light or not.
                            vis = false;
                        }
                    }
                } // end of second y loop

                });
}

void VisibilitySystem::update(const double durationMS)
{
    if(firstRun) {
        //ecs::emit(PlayerMovedMessage{});
        firstRun = false;
    }
}










