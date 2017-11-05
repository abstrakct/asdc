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
#include <functional>

#include "common.h"
#include "console.h"

#define rootLayer 0
#define mapLayer  1
#define testLayer 2

struct KeyPressed {
    KeyPressed() {}
    KeyPressed(sf::Event ev) : event(ev) {}
    sf::Event event;
};

struct GuiControl {
    virtual void render(std::shared_ptr<Console> console) = 0;
    virtual bool mouseInsideControl(const int tx, const int ty) { return false; };
    
    // Callbacks
    std::function<void(GuiControl*)> onRenderStart;
    std::function<void(GuiControl*, int, int)> onMouseOver;
    std::function<void(GuiControl*, int, int)> onMouseDown;
    std::function<void(GuiControl*, int, int)> onMouseUp;
};

struct GuiStaticText : public GuiControl {
    // TODO: use sf::Color ?
    // TODO: strings with color tags? to add different colors to different parts of the string.
    GuiStaticText(const int X, const int Y, const std::string txt, u32 fC) : x(X), y(Y), text(txt), fgColor(fC) {
        onMouseOver = [this] (GuiControl *ctrl, int tx, int ty) {
            fgColor = 0x0000fffff;
        };
    };
    virtual void render(std::shared_ptr<Console> console) override;
    virtual bool mouseInsideControl(const int tx, const int ty) override {
        return (tx >= x && tx <= x + (static_cast<int>(text.size())) && ty == y);
    }
    
    // Callbacks for handling mouse events
    void handleMouseOver(GuiControl *ctrl, int termx, int termy);

    int x, y;
    std::string text = "";
    u32 fgColor;
};

struct Layer {
    Layer(const int X, const int Y, const int W, const int H, std::string fontName, const int fW, const int fH) :
        x(X), y(Y), w(W), h(H), font(fontName), fontWidth(fW), fontHeight(fH) {
            console = std::make_shared<Console>(X, Y, W, H);
            console->setFont(fontName, fW, fH, fW*16, fH*16);
        }

    template<class T> T* control(const int handle) {
        auto finder = controls.find(handle);
        if (finder == controls.end()) throw std::runtime_error("Unknown GUI control handle: " + std::to_string(handle));
        return static_cast<T*>(finder->second.get());
    }

    inline void checkHandleUniqueness(const int handle) {
        auto finder = controls.find(handle);
        if (finder != controls.end()) throw std::runtime_error("Adding duplicate GUI control handle: " + std::to_string(handle));
    }

    inline void addStaticText(const int handle, const int x, const int y, const std::string text, u32 fgColor) {
        checkHandleUniqueness(handle);
        controls.emplace(handle, std::make_unique<GuiStaticText>(x, y, text, fgColor));
    }

    void render(sf::RenderWindow &window);
    int x, y, w, h;
    std::string font;
    int fontWidth, fontHeight;
    std::shared_ptr<Console> console;
    std::unordered_map<int, std::unique_ptr<GuiControl>> controls;
};

class GUI {
    public:
        GUI(const int w, const int h) : screenWidth(w), screenHeight(h) {}
        void render(sf::RenderWindow &window);
        void addLayer(const int handle, const int X, const int Y, const int W, const int H, std::string font, const int fontW, const int fontH, int order = -1);
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

extern std::unique_ptr<GUI> gui;
inline Layer* layer(const int &handle) { return gui->getLayer(handle); };
