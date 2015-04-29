//
// Created by Cyril Leroux on 21/04/2015.
//

#include "../display_manager.h"

#include <iostream>

const auto ERROR_INDEX(-1);
const auto FIRST_MON_ID(0);

std::vector <HMONITOR> DisplayManager::monitors_ = std::vector<HMONITOR>();

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

    // Si GetMonitorIndex a renvoyé une erreur
    if (currentIndex == ERROR_INDEX) {
        return ERROR_INDEX;

        // Si Le moniteur n'est pas le dernier moniteur de la liste
    } else if (currentIndex < monitorsCount - 1) {
        return currentIndex + 1;
    }

    // Le moniteur est le dernier, on boucle sur le premier
    return FIRST_MON_ID;
}

/**
 * @param monitor Le moniteur précédent celui que l'on veut récupérer.
 * @return Le moniteur suivant celui passé en paramètre.
 */
HMONITOR DisplayManager::GetNextMonitor(HMONITOR monitor)
{
    const auto nextMonitorIndex = GetNextMonitorIndex(monitor);

    if (nextMonitorIndex == ERROR_INDEX) {
        return nullptr;
    }

    return monitors_[nextMonitorIndex];
}

/**
 * @param monitor Le moniteur dont on veut connaitre les dimensions.
 * @return Le rectangle délimitant la position et les dimensions du moniteur.
 * Retourne nullptr en cas d'erreur.
 */
rect* DisplayManager::GetMonitorRect(HMONITOR monitor)
{
    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (GetMonitorInfo(monitor, &monitorInfo) != 0) {
        return new rect(monitorInfo.rcMonitor);
    }

    return nullptr;
}

/**
 * @param window La fenêtre dont on veut connaitre la position relative.
 * @param rectMon La position du moniteur.
 * @return La position relative de la fenêtre par rappot au moniteur.
 * Retourne nullptr en cas d'erreur.
 */
rect* DisplayManager::GetRelativePos(HWND window, rect rectMon)
{
    // Récuperation de la position de la fenêtre
    RECT windowPos;
    if (GetWindowRect(window, &windowPos) != 0) {
        auto* relativePos = new rect();
        // Calcul de l'offset en pixels
        relativePos->left = windowPos.left - rectMon.left;
        relativePos->top = windowPos.top - rectMon.top;
        relativePos->right = windowPos.right - rectMon.left;
        relativePos->bottom = windowPos.bottom - rectMon.top;
        return relativePos;
    }

    return nullptr;
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

    // Récupèration de la position du moniteur source
    auto* srcMonPos = GetMonitorRect(srcMonitor);
    if (srcMonPos) {
        srcMonPos->display("srcMon");
    } else {
        std::cerr << "!!! GetMonitorRect(_srcMonitor) failed !!!" << std::endl;
    }

    // Récupèration de la position du nouveau moniteur
    auto* destMonPos = GetMonitorRect(destMonitor);
    if (destMonPos) {
        destMonPos->display("destMon");
    } else {
        std::cerr << "!!! GetMonitorRect(_destMonitor) failed !!!" << std::endl;
    }

    // Calcul de la position relative de la fenêtre par rapport au moniteur source
    auto* windowRelPos = GetRelativePos(window, *srcMonPos);
    if (windowRelPos) {
        windowRelPos->display("windowRelPos");
    } else {
        std::cerr << "!!! GetRelativePos(_window, *rectSrcMon) failed !!!" << std::endl;
    }

    // Calcul du coefficient d'agrandissement pour passer d'un moniteur à l'autre
    auto coef = GetRectCoef(*srcMonPos, *destMonPos);
    coef.display("coef");

    // Application du coef à la position relative de la fenêtre
    auto* newRelPos = ApplyCoef(*windowRelPos, coef);
    if (newRelPos) {
        newRelPos->display("newRelPos");
    } else {
        std::cout << "!!! ApplyCoef(*windowRelPos, coef) failed !!!" << std::endl;
    }

    // Calcul de la position absolue finale de la fenêtre
    auto* newPos = ApplyOffest(*newRelPos, destMonPos->left, destMonPos->top);
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
BOOL CALLBACK DisplayManager::MonitorEnumProc(HMONITOR monitor, HDC hdcMonitor, LPRECT lpMonitorRect, LPARAM dwData )
{
monitors_.push_back(monitor);
return TRUE;
}
