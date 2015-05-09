//
// Created by Cyril Leroux on 22/04/2015.
//

#pragma once

#include <string>
#include "rect.h"

/**
 * Represents a window on the screen.
 */
class Window
{
private:
    std::string title_;
    Rect<int> rect_;

public:

    Window(const std::string& title, const Rect<int>& rect) : title_(title), rect_(rect) { }

    const std::string& title() const
    {
        return title_;
    }

    void set_title(const std::string& title)
    {
        title_ = title;
    }

    const Rect<int>& rect() const
    {
        return rect_;
    }

    void set_rect(const Rect<int>& rect)
    {
        rect_ = rect;
    }
};

