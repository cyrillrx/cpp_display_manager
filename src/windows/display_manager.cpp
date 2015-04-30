//
// Created by Cyril Leroux on 21/04/2015.
//

#include "../display_manager.h"
#include <iostream>

const auto ERROR_INDEX(-1);
const auto FIRST_MON_ID(0);

std::vector<HMONITOR> DisplayManager::monitors_ = std::vector<HMONITOR>();

bool SplashRight(Window win)
{
    // TODO implement
    return false;
}

bool SplashLeft(Window win)
{
    // TODO implement
    return false;
}

int PrevScreen(Window win)
{
    const auto monitorCount = GetSystemMetrics(SM_CMONITORS);

    // If there is less than two monitors, return
    if (monitorCount < 2) {
        return false;
    }

    // Get monitor list
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);

    auto foregroundWindow = GetForegroundWindow();
    auto currentMonitor = MonitorFromWindow(foregroundWindow, MONITOR_DEFAULTTONEAREST);
    auto prevMonitor = GetPrevMonitor(currentMonitor);

    // Move the window
    return MoveWindow(foregroundWindow, currentMonitor, prevMonitor);
}

int NextScreen(Window win)
{
    const auto monitorCount = GetSystemMetrics(SM_CMONITORS);

    // If there is less than two monitors, return
    if (monitorCount < 2) {
        return false;
    }

    // Get monitor list
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);

    auto foregroundWindow = GetForegroundWindow();
    auto currentMonitor = MonitorFromWindow(foregroundWindow, MONITOR_DEFAULTTONEAREST);
    auto nextMonitor = GetNextMonitor(currentMonitor);

    // Move the window
    return MoveWindow(foregroundWindow, currentMonitor, nextMonitor);
}

int DisplayManager::MonitorCount()
{
    return GetSystemMetrics(SM_CMONITORS);
}

//
// Specific Windows methods
//

/**
 * @param monitor The monitor we want to know the index of.
 * @return The index of the current monitor or ERROR_INDEX.
 */
int DisplayManager::GetMonitorIndex(HMONITOR monitor)
{
    const int monitorsCount = monitors_.size();
    for (auto i = 0; i < monitorsCount; i++) {
        if (monitors_[i] == monitor) {
            return i;
        }
    }

    return ERROR_INDEX;
}

/**
 * @param monitor The monitor for which we want the next index.
 * @return The index of the next monitor.
 * Loops on the first monitor if the last one is reached.
 * Returns ERROR_INDEX if an error occurred.
 */
int DisplayManager::GetNextMonitorIndex(HMONITOR monitor)
{
    const auto monitorsCount = GetSystemMetrics(SM_CMONITORS);
    const auto currentIndex = GetMonitorIndex(monitor);

    // GetMonitorIndex returned an error
    if (currentIndex == ERROR_INDEX) {
        return ERROR_INDEX;
    }

    // The monitor is the last one
    if (currentIndex >= monitorsCount) {
        return FIRST_MON_ID;
    }

    // Regular case
    return currentIndex + 1;
}

/**
 * @param monitor The current monitor.
 * @return The next monitor.
 */
HMONITOR DisplayManager::GetNextMonitor(HMONITOR monitor)
{
    const auto nextMonitorIndex = GetNextMonitorIndex(monitor);

    if (nextMonitorIndex == ERROR_INDEX) {
        // TODO Throw an error
    }

    return monitors_[nextMonitorIndex];
}

/**
 * @param monitor The monitor for which we want the dimensions.
 * @return The monitor bounding rect.
 */
rect<int> DisplayManager::GetMonitorRect(HMONITOR monitor)
{
    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (GetMonitorInfo(monitor, &monitorInfo) != 0) {
        RECT src = monitorInfo.rcMonitor;
        return rect<int>(src.left, src.top, src.right, src.bottom);
    }

    // TODO Throw an error
    return rect<int>();
}

/**
 * @param window The window which we want to know the relative position.
 * @param rectMon The monitor position.
 * @return The relative window position (relative to the monitor).
 */
rect<int> DisplayManager::GetRelativePos(HWND window, rect<int> rectMon)
{
    // Get the current window position
    RECT windowPos;
    if (GetWindowRect(window, &windowPos) == 0) {
        // TODO Throw an error
        return rect<int>();
    }

    rect<int> relativePos;

    // Apply offset in px
    relativePos.left = windowPos.left - rectMon.left;
    relativePos.top = windowPos.top - rectMon.top;
    relativePos.right = windowPos.right - rectMon.left;
    relativePos.bottom = windowPos.bottom - rectMon.top;
    return relativePos;
}

/**
 * Moves the Window to the specified display.
 *
 * @param window HWND The window to move.
 * @param monitor HMONITOR The source display.
 * @param monitor HMONITOR The destination display for the given window.
 * @return True the window moved.
 */
bool DisplayManager::MoveWindow(HWND window, HMONITOR srcMonitor, HMONITOR destMonitor)
{
    if (srcMonitor == destMonitor) {
        return false;
    }

    // Get the position of the source monitor
    auto *srcMonPos = GetMonitorRect(srcMonitor);
    if (srcMonPos) {
        srcMonPos->display("srcMon");
    } else {
        std::cerr << "!!! GetMonitorRect(_srcMonitor) failed !!!" << std::endl;
    }

    // Get the position of the new monitor
    auto *destMonPos = GetMonitorRect(destMonitor);
    if (destMonPos) {
        destMonPos->display("destMon");
    } else {
        std::cerr << "!!! GetMonitorRect(_destMonitor) failed !!!" << std::endl;
    }

// TODO comment
    // Calcul de la position relative de la fen�tre par rapport au moniteur source
    auto *windowRelPos = GetRelativePos(window, *srcMonPos);
    if (windowRelPos) {
        windowRelPos->display("windowRelPos");
    } else {
        std::cerr << "!!! GetRelativePos(_window, *rectSrcMon) failed !!!" << std::endl;
    }

// TODO comment
    // Calcul du coefficient d'agrandissement pour passer d'un moniteur � l'autre
    auto coef = GetRectCoef(*srcMonPos, *destMonPos);
    coef.display("coef");

// TODO comment
    // Application du coef � la position relative de la fen�tre
    auto *newRelPos = ApplyCoef(*windowRelPos, coef);
    if (newRelPos) {
        newRelPos->display("newRelPos");
    } else {
        std::cout << "!!! ApplyCoef(*windowRelPos, coef) failed !!!" << std::endl;
    }

// TODO comment
    // Calcul de la position absolue finale de la fen�tre
    auto *newPos = ApplyOffest(*newRelPos, destMonPos->left, destMonPos->top);
    if (newPos) {
        newPos->display("newPos (final)");
    } else {
        std::cout << "!!! ApplyOffest(*newRelPos, rectDestMon->left, rectDestMon->top) failed !!!" << std::endl;
    }

    SetWindowPos(window, nullptr, newPos->left, newPos->top, newPos->get_width(), newPos->get_height(), SWP_SHOWWINDOW);

    // Delete all the pointers
    delete(srcMonPos);
    delete(destMonPos);
    delete(windowRelPos);
    delete(newRelPos);
    delete(newPos);

    return true;
}

/**
 * Windows CALLBACK to list monitors
 */
BOOL CALLBACK
DisplayManager::MonitorEnumProc(HMONITOR
monitor,
HDC hdcMonitor, LPRECT
lpMonitorRect,
LPARAM dwData
)
{
monitors_.
push_back(monitor);
return
TRUE;
}
