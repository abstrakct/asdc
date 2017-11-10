/*
 * fov.cpp
 *
 * Field of Vision and related things.
 */

#include <cmath>
#include "ecs.h"
#include "components.h"
#include "world.h"
#include "fov.h"

extern std::unique_ptr<World> world;

// Start of my own ray-casting FOV algorithm.
// It works ok-ish, but the results are not satisfactory.
void castRay(std::vector<std::pair<int, int>> line)
{
    std::array<std::array<bool, 256>, 256> &fovMap = ecs::entity(playerID)->component<Vision>()->fovMap;
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
}
void myRayCastingFOVAlgorithm()
{
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
        // Draw the line and cast the ray
        castRay(getLineCoordinatesBresenham(startx, starty, endx, endy));
    } // end of first x loop

    for (int endy = (starty - fov); endy <= (starty + fov); endy++) {
        int endx = startx + fov;
        castRay(getLineCoordinatesBresenham(startx, starty, endx, endy));
    } // end of first y loop

    for (int endx = (startx + fov); endx >= (startx - fov); endx--) {
        int endy = starty + fov;
        castRay(getLineCoordinatesBresenham(startx, starty, endx, endy));
    } // end of second x loop

    for (int endy = (starty + fov); endy >= (starty - fov); endy--) {
        int endx = startx - fov;
        castRay(getLineCoordinatesBresenham(startx, starty, endx, endy));
    } // end of second y loop
}
// End of my algorithm

// Draw a line from x0,y0 to x1,y1
// Return vector of int pairs, each pair contains one coordinate on the line.
// DDA algorithm, based on https://www.tutorialspoint.com/computer_graphics/line_generation_algorithm.htm
// and http://www.geeksforgeeks.org/dda-line-generation-algorithm-computer-graphics/
PointVector getLineCoordinates(int x0, int y0, int x1, int y1)
{
    PointVector line;

    float x = x0;
    float y = y0;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps;
    if(abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);

    float xinc = dx / (float) steps;
    float yinc = dy / (float) steps;

    // Add first point (needed?)
   // line.push_back(std::make_pair(static_cast<int>(x), static_cast<int>(y)));
    // Add the rest
    for (int i = 0; i <= steps; i++) {
        x += xinc;
        y += yinc;
        line.push_back(std::make_pair(static_cast<int>(x), static_cast<int>(y)));
    }

    return line;
}

// Same as above, but Bresenham's algorithm, based on http://www.roguebasin.com/index.php?title=Bresenham%27s_Line_Algorithm#C.2B.2B
PointVector getLineCoordinatesBresenham(int x0, int y0, int x1, int y1)
{
    PointVector line;

    int deltax = x1 - x0;
    int deltay = y1 - y0;
    int x = x0;
    int y = y0;

    signed char const ix((deltax > 0) - (deltax < 0));
    deltax = std::abs(deltax) << 1;

    signed char const iy((deltay > 0) - (deltay < 0));
    deltay = std::abs(deltay) << 1;

    line.push_back(std::make_pair(x, y));

    //float deltaerr = abs(deltay / deltax);
    //float error = 0.0;

    if (deltax >= deltay) {
        int error = deltay - (deltax >> 1);
        while (x0 != x1) {
            if ((error > 0) || (!error && (ix > 0))) {
                error -= deltax;
                y0 += iy;
            }

            error += deltay;
            x0 += ix;
            line.push_back(std::make_pair(x0, y0));
        }
    } else {
        int error = (deltax - (deltay >> 1));
        while (y0 != y1) {
            if ((error > 0) || (!error && (iy > 0))) {
                error -= deltay;
                x0 += ix;
            }

            error += deltax;
            y0 += iy;
            line.push_back(std::make_pair(x0, y0));
        }
    }

    return line;
}


// The shadowcasting algorithm below is based on this article:
// http://www.roguebasin.com/index.php?title=C%2B%2B_shadowcasting_implementation
// I basically copied the code in the article and modified it to fit my program.

static int multipliers[4][8] = {
    {1, 0,  0, -1, -1,  0,  0, 1},
    {0, 1, -1,  0,  0, -1,  1, 0},
    {0, 1,  1,  0,  0, -1, -1, 0},
    {1, 0,  0,  1, -1,  0,  0,-1}
};

void castLight(u32 x, u32 y, u32 radius, u32 row, float start_slope, float end_slope, u32 xx, u32 xy, u32 yx, u32 yy) {

    std::array<std::array<bool, 256>, 256> &fovMap = ecs::entity(playerID)->component<Vision>()->fovMap;

    if (start_slope < end_slope) {
        return;
    }
    float next_start_slope = start_slope;
    for (u32 i = row; i <= radius; i++) {
        bool blocked = false;
        for (int dx = -i, dy = -i; dx <= 0; dx++) {
            float l_slope = (dx - 0.5) / (dy + 0.5);
            float r_slope = (dx + 0.5) / (dy - 0.5);
            if (start_slope < r_slope) {
                continue;
            } else if (end_slope > l_slope) {
                break;
            }

            int sax = dx * xx + dy * xy;
            int say = dx * yx + dy * yy;
            if ((sax < 0 && (u32)std::abs(sax) > x) || (say < 0 && (u32)std::abs(say) > y)) {
                continue;
            }
            int ax = x + sax;
            int ay = y + say;
            if (ax >= world->currentLevel->width || ay >= world->currentLevel->height) {
                continue;
            }

            u32 radius2 = radius * radius;
            if ((u32)(dx * dx + dy * dy) < radius2) {
                fovMap[ax][ay] = true;
            }

            if (blocked) {
                if(world->currentLevel->cache[ax][ay].blocksLight) {
                    next_start_slope = r_slope;
                    continue;
                } else {
                    blocked = false;
                    start_slope = next_start_slope;
                }
            } else if(world->currentLevel->cache[ax][ay].blocksLight) {
                blocked = true;
                next_start_slope = r_slope;
                castLight(x, y, radius, i + 1, start_slope, l_slope, xx, xy, yx, yy);
            }
        }
        if (blocked) {
            break;
        }
    }
}

void doShadowCastingFOV(Position *pos, Vision *v) {
    std::array<std::array<bool, 256>, 256> &fovMap = ecs::entity(playerID)->component<Vision>()->fovMap;
    fovMap = {};
    for (u32 i = 0; i < 8; i++) {
        castLight(pos->x, pos->y, v->fovRadius, 1, 1.0, 0.0, multipliers[0][i], multipliers[1][i], multipliers[2][i], multipliers[3][i]);
    }
}






// vim: fdm=syntax
