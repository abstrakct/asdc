/*
 * fov.h
 *
 * Field of Vision and related things.
 */

#pragma once

#include <vector>
#include <algorithm>
#include "common.h"
#include "ecs.h"
#include "components.h"

void castRay(std::vector<std::pair<int, int>> line);
void myRayCastingFOVAlgorithm();

void doShadowCastingFOV(Position *pos, Vision *v);

PointVector getLineCoordinates(int x0, int y0, int x1, int y1);
PointVector getLineCoordinatesBresenham(int x0, int y0, int x1, int y1);
