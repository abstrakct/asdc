/*
 * ui.cpp
 *
 * The User Interface
 */

#include "ui.h"

std::vector<std::pair<int, Layer*>> RenderOrderDetail;

void GUI::render(sf::RenderWindow &window)
{
    for (auto l : RenderOrderDetail) {
        l.second->render(window);
    }
}

Layer* GUI::getLayer(const int handle)
{
    auto finder = layers.find(handle);
    if (finder == layers.end()) throw std::runtime_error("Unknown layer handle: " + std::to_string(handle));
    return &(finder->second);
}

void GUI::addLayer(const int handle, const int X, const int Y, const int W, const int H, std::string font, int order)
{
    checkHandleUniqueness(handle);
    layers.emplace(std::make_pair(handle, Layer(X, Y, W, H, font)));
    if(order == -1) {
        order = renderOrder;
        ++renderOrder;
    }

    RenderOrderDetail.push_back(std::make_pair(order, getLayer(handle)));

    // Sort layers by render order using a lambda function.
    std::sort(RenderOrderDetail.begin(), RenderOrderDetail.end(),
            [] (std::pair<int, Layer*> a, std::pair<int, Layer *> b)
            { return a.first < b.first; }
            );
}

void Layer::render(sf::RenderWindow &window)
{
    console->render(window);
}
