/*
 * gui_control.cpp
 *
 * Controls for the GUI
 *
 * Partially taken from RLTK - because I need to learn this stuff!
 */


#include "gui_control.h"

void GuiStaticText::render(std::shared_ptr<Console> console)
{
    console->print(x, y, text, fgColor);
}

void GuiMessageBox::render(std::shared_ptr<Console> console)
{
    console->textBox(title, text);
}

//void GuiStaticText::handleMouseOver(GuiControl *ctrl, int termx, int termy)
//{
//}


// vim: fdm=syntax
