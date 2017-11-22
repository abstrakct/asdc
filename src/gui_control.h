/*
 * gui_control.h
 *
 * Controls for the GUI
 *
 * Also mostly stolen from RLTK - because I need to learn this stuff!
 */

#pragma once

#include <memory>
#include <functional>

#include "console.h"

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
    //void handleMouseOver(GuiControl *ctrl, int termx, int termy);

    int x, y;
    std::string text = "";
    u32 fgColor;
};

struct GuiMessageBox : public GuiControl {
    GuiMessageBox(const std::string _title, std::string _text) : title(_title), text(_text) {};

    virtual void render(std::shared_ptr<Console> console) override;
    //virtual bool mouseInsideControl(const int tx, const int ty) { return false; }; // TODO: fix/implement
    
    std::string title = "";
    std::string text = "";
};

// vim: fdm=syntax
