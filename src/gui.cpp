/*
 * ui.cpp
 *
 * The User Interface
 */

#include "gui.h"
#include "input.h"

// TODO: Add layer blending?!

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

void GUI::addLayer(const int handle, const int X, const int Y, const int W, const int H, std::string font, const int fontW, const int fontH, int order)
{
    checkHandleUniqueness(handle);
    layers.emplace(std::make_pair(handle, Layer(X, Y, W, H, font, fontW, fontH)));
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
    if (console) {
        // Render GUI Controls
        if (!controls.empty()) {
            // Render start events
            for (auto it = controls.begin(); it != controls.end(); ++it) {
                if (it->second->onRenderStart) {
                    auto callfunc = it->second->onRenderStart;
                    callfunc(it->second.get());
                }

                // Add mouse stuff here

                const auto [mx, my] = getMousePosition();

                if (mx >= x && mx <= (x+w) && my >= y && my <= (y+h)) {
                    const int termx = (mx - x) / fontWidth;
                    const int termy = (my - y) / fontHeight;
                    // Mouse over is possible
                    if (it->second->mouseInsideControl(termx, termy)) {
                        // If inside control, and has onMouseOver callback - call that
                        if(it->second->onMouseOver) {
                            auto callfunc = it->second->onMouseOver;
                            callfunc(it->second.get(), termx, termy);
                        }
                        // TODO: Add mouse button handling
                    }
                }

                // Render the control
                it->second->render(console);
            }
        }

        // Render the console
        console->render(window);
    }
}



// vim: fdm=syntax
