/*
 * fov.h
 *
 * Field of Vision and related things.
 */

#pragma once

#include <vector>
#include <algorithm>

void castRay(std::vector<std::pair<int, int>> line);
void myRayCastingFOVAlgorithm();

std::vector<std::pair<int, int>> getLineCoordinates(int x0, int y0, int x1, int y1);
std::vector<std::pair<int, int>> getLineCoordinatesBresenham(int x0, int y0, int x1, int y1);
