/*
 * ui.h
 *
 * The User Interface
 *
 * Also mostly stolen from RLTK - because I need to learn this stuff!
 */

#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "common.h"
#include "console.h"


struct KeyPressed {
    KeyPressed() {}
    KeyPressed(sf::Event ev) : event(ev) {}
    sf::Event event;
};

struct Layer {
    Layer(const int X, const int Y, const int W, const int H, std::string fontName) :
        x(X), y(Y), w(W), h(H), font(fontName) {
            console = std::make_unique<Console>(W, H);
            console->setFont(fontName, 16, 16, 256, 256);  // TODO: remove hard coded values!
        }

    void render(sf::RenderWindow &window);
    int x, y, w, h;
    std::string font;
    std::unique_ptr<Console> console;
};

// TODO: add controls, rendering order

class GUI {
    public:
        GUI(const int w, const int h) : screenWidth(w), screenHeight(h) {}
        void render(sf::RenderWindow &window);
        void addLayer(const int handle, const int X, const int Y, const int W, const int H, std::string font, int order = -1);
        Layer* getLayer(const int handle);

    private:
        int screenWidth, screenHeight;
        int renderOrder = 0;
        std::unordered_map<int, Layer> layers;

        inline void checkHandleUniqueness(const int handle) {
            auto finder = layers.find(handle);
            if (finder != layers.end()) throw std::runtime_error("Adding duplicate layer handle to GUI: " + std::to_string(handle));
        }
};

