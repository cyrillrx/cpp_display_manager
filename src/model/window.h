//
// Created by Cyril Leroux on 22/04/2015.
//

#pragma once

#include <iosfwd>

/**
 * Represents a window on the screen.
 */
class Window
{
private:
    std::string name_;
    bool visible_;
    int screen_id;
};

