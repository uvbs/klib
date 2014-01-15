
#include <windows.h>
#include "window.h"


namespace klib {
namespace ui {




bool window::has_fullscrenn_wnd()
{
    POINT point;
    HWND hwndTopLeft;
    HWND hwndTopRight;
    HWND hwndBottomLeft;
    HWND hwndBottomRight;
    RECT rcScreen;
    int widthScreen;
    int heightScreen;

    GetWindowRect(::GetDesktopWindow(), &rcScreen);
    widthScreen = rcScreen.right - rcScreen.left;
    heightScreen = rcScreen.bottom - rcScreen.top;

    const int xoffset = 3;
    const int  yoffset = 3;

    bool Result = false;

    point.x = xoffset;
    point.y = yoffset;
    hwndTopLeft = WindowFromPoint(point);

    point.x = widthScreen - xoffset;
    point.y = yoffset;
    hwndTopRight = WindowFromPoint(point);

    point.x = xoffset;
    point.y = heightScreen - yoffset;
    hwndBottomLeft = WindowFromPoint(point);

    point.x = widthScreen - xoffset;
    point.y = heightScreen - yoffset;
    hwndBottomRight = WindowFromPoint(point);

    if ((hwndTopLeft == hwndTopRight) &&
        (hwndTopRight == hwndBottomLeft) &&
        (hwndBottomLeft == hwndBottomRight) )
    {
        Result = true;
    }

    return Result;
}




}}