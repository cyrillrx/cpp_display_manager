//
// Created by Cyril Leroux on 21/04/2015.
//

#pragma once

#include "platform_config.h"

# if defined(WINDOWS_PLATFORM)
#   include <windows.h>
# endif

#include <vector>
#include "rect.h"
#include "point.h"
#include "model/window.h"

typedef point<float> pointf;

/**
 * Screens and windows manager class
 */
class DisplayManager
{
public:

    static bool SplashRight(Window win);

    static bool SplashLeft(Window win);

    /**
     * Move the focused window to the previous monitor.
     * @return True if the windows has been moved.
     */
    static bool PrevScreen(Window win);

    /**
     * Move the focused window to the next monitor.
     * @return True if the windows has been moved.
     */
    static bool NextScreen(Window win);

private:
    /** @return The active monitor count. */
    static int MonitorCount();

    /** @return The scale coefficient between two rectangles. */
    static pointf GetRectCoef(rect<int> rect1, rect<int> rect2);

    /**
     * Applies a scale coefficient to a rectangle.
     * @param rectIn The rectangle to resize.
     * @param coef The scale coefficient to apply.
     * @return The rescaled rectangle.
     */
    static rect<int> ApplyCoef(rect<int> rectIn, pointf coef);

    /**
     * Applies an offset to a rectangle.
     * @param relativePosition The relative position of the window inside the display.
     * @param offsetX Abscissa offset.
     * @param offsetY Ordinate offset.
     */
    static rect<int> ApplyOffest(rect<int> relativePos, const int& offsetX, const int& offsetY);

# if defined(WINDOWS_PLATFORM)
    #include <windows.h>
	static std::vector<HMONITOR> monitors_;

	static BOOL CALLBACK MonitorEnumProc(HMONITOR monitor, HDC hdcMonitor, LPRECT lpMonitorRect, LPARAM dwData);
	static int GetMonitorIndex(HMONITOR monitor);
	static int GetNextMonitorIndex(HMONITOR monitor);
	static HMONITOR GetPrevMonitor(HMONITOR monitor);
	static HMONITOR GetNextMonitor(HMONITOR monitor);
	static rect<int>* GetMonitorRect(HMONITOR monitor);
	static rect<int>* GetRelativePos(HWND window, rect<int> rectMon);
	static bool MoveWindow(HWND window, HMONITOR monitorSrc, HMONITOR monitorDest);
# endif
};
