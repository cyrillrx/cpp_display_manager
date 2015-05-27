//
// Created by Cyril Leroux on 21/04/2015.
//

#include "../display_manager.h"

#if defined(WINDOWS_PLATFORM)

#include <iostream>
#include "core/displayer.h"

const auto ERROR_INDEX(-1);
const auto FIRST_MON_ID(0);

void displayWindows(const Window& window)
{
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Window title: " << window.title() << std::endl;
    Display(window.rect());
}

std::vector<Window> DisplayManager::windows_ = std::vector<Window>();

std::vector<HMONITOR> DisplayManager::monitors_ = std::vector<HMONITOR>();

bool DisplayManager::GetWindowList()
{
    EnumWindows(
            [](HWND hwnd, LPARAM lparam) -> BOOL {
                if (!IsWindowVisible(hwnd)) {
                    return TRUE;
                }
                char class_name[256];
                char title[256];
                if (GetWindowTextLength(hwnd) < 1) {
                    return TRUE;
                }
                GetWindowText(hwnd, title, sizeof(title));
//                           GetClassName(hwnd, class_name, sizeof(class_name));
//                           std::cout << "Class name: " << class_name << std::endl;

                WINDOWINFO windowInfo;
                GetWindowInfo(hwnd, &windowInfo);
                auto window = Window(title,
                                     Rect<int>(windowInfo.rcWindow.left,
                                               windowInfo.rcWindow.top,
                                               windowInfo.rcWindow.right,
                                               windowInfo.rcWindow.bottom));
                windows_.push_back(window);

                return TRUE;
            }, NULL);

    std::cout << "---------------------------------" << std::endl;
    std::cout << windows_.size() << " windows found !" << std::endl;
    std::cout << "---------------------------------" << std::endl;
    for (const auto& window : windows_) {
        displayWindows(window);
    }
    return true;
}

/////////////////////////////////////////////////////

int DisplayManager::MonitorCount()
{
    return GetSystemMetrics(SM_CMONITORS);
}

bool DisplayManager::SplashLeft(Window win)
{
    // TODO implement
    return false;
}

bool DisplayManager::SplashRight(Window win)
{
    // TODO implement
    return false;
}

bool DisplayManager::PrevScreen(Window win)
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

bool DisplayManager::NextScreen(Window win)
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
 * @param monitor The monitor for which we want the previous index.
 * @return The index of the previous monitor.
 * Loops on the last monitor if the first one is reached.
 * Returns ERROR_INDEX if an error occurred.
 */
int DisplayManager::GetPrevMonitorIndex(HMONITOR monitor)
{
    const auto monitorsCount = GetSystemMetrics(SM_CMONITORS);
    const auto currentIndex = GetMonitorIndex(monitor);

    // GetMonitorIndex returned an error
    if (currentIndex == ERROR_INDEX) {
        return ERROR_INDEX;
    }

    // The monitor is the fist one
    if (currentIndex <= 0) {
        return monitorsCount - 1;
    }

    // Regular case
    return currentIndex - 1;
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
 * @return The previous monitor.
 */
HMONITOR DisplayManager::GetPrevMonitor(HMONITOR monitor)
{
    const auto prevMonitorIndex = GetPrevMonitorIndex(monitor);

    if (prevMonitorIndex == ERROR_INDEX) {
        // TODO Throw proper exception
        throw std::exception();
    }

    return monitors_[prevMonitorIndex];
}

/**
 * @param monitor The current monitor.
 * @return The next monitor.
 */
HMONITOR DisplayManager::GetNextMonitor(HMONITOR monitor)
{
    const auto nextMonitorIndex = GetNextMonitorIndex(monitor);

    if (nextMonitorIndex == ERROR_INDEX) {
        // TODO Throw proper exception
        throw std::exception();
    }

    return monitors_[nextMonitorIndex];
}

/**
 * @param monitor The monitor for which we want the dimensions.
 * @return The monitor bounding rect.
 * @throw TODO
 */
Rect<int> DisplayManager::GetMonitorRect(HMONITOR monitor)
{
    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (GetMonitorInfo(monitor, &monitorInfo) != 0) {
        RECT src = monitorInfo.rcMonitor;
        return Rect<int>(src.left, src.top, src.right, src.bottom);
    }

    // TODO Throw proper exception
    std::cerr << "!!! GetMonitorRect(HMONITOR) failed !!!" << std::endl;
    throw std::exception();
}

/**
 * @param window The window which we want to know the relative position.
 * @param rectMon The monitor position.
 * @return The relative window position (relative to the monitor).
 * @throw TODO
 */
Rect<int> DisplayManager::GetRelativePos(HWND window, Rect<int> rectMon)
{
    // Get the current window position
    RECT windowPos;
    if (GetWindowRect(window, &windowPos) == 0) {
        // TODO Throw proper exception
        std::cerr << "!!! GetRelativePos(HWND, Rect<int>) failed !!!" << std::endl;
        throw std::exception();
    }

    // Apply offset in px
    return Rect<int>(
            windowPos.left - rectMon.left,
            windowPos.top - rectMon.top,
            windowPos.right - rectMon.left,
            windowPos.bottom - rectMon.top
    );
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
    auto srcMonPos = GetMonitorRect(srcMonitor);
    Display(srcMonPos, "srcMon");

    // Get the position of the new monitor
    auto destMonPos = GetMonitorRect(destMonitor);
    Display(destMonPos, "destMon");

    // Get the window's relative position (relative to the source monitor)
    auto windowRelPos = GetRelativePos(window, srcMonPos);
    Display(windowRelPos, "windowRelPos");

    // Compute scale coef from a screen to another
    auto coef = GetRectCoef(srcMonPos, destMonPos);
    Display(coef, "coef");

    // Apply coef
    auto newRelPos = ApplyCoef(windowRelPos, coef);
    Display(newRelPos, "newRelPos");

    // Compute window's absolute destination position
    auto newPos = ApplyOffest(newRelPos, destMonPos.left, destMonPos.top);
    Display(newPos, "newPos (final)");

    SetWindowPos(window, nullptr, newPos.left, newPos.top, newPos.width(), newPos.height(), SWP_SHOWWINDOW);

    return true;
}

/**
 * Windows CALLBACK to list monitors
 */
BOOL DisplayManager::MonitorEnumProc(HMONITOR monitor, HDC, LPRECT lpMonitorRect, LPARAM dwData)
{
    monitors_.push_back(monitor);
    return TRUE;
}

# endif
