#include "dataHandling\win32_SetWorkingDirectory.hpp"
#include "dataHandling\win32_GameDLL.hpp"
#include "Windows.h"
#include "windowsx.h"

LRESULT CALLBACK WindowProcedure(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
    {
        LOG("Close\n");
        DestroyWindow(hWindow);
        return 0;
    }

    case WM_DESTROY:
    {
        LOG("Destroy\n");
        PostQuitMessage(0);
        return 0;
    }

    default:
        LOG("SOME MSG\n");
        break;
    }

    return DefWindowProc(hWindow, message, wParam, lParam);
}

HWND WindowCreate(HINSTANCE hInstance, int clientWidth, int clientHeight)
{
    WNDCLASSA windowClass;
    windowClass.style = CS_HREDRAW | CS_VREDRAW; // TODO: is it required?
    windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = NULL;
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = "a";
    ATOM windowClassAtom = RegisterClassA(&windowClass);
    if (windowClassAtom == 0)
    {
        return NULL;
    }

    constexpr DWORD style = WS_OVERLAPPEDWINDOW;
    constexpr DWORD exStyle = 0;
    RECT rect = {0, 0, clientWidth, clientHeight};
    if (!AdjustWindowRectEx(&rect, style, FALSE, exStyle))
    {
        LOG_LAST_ERROR("Unable to adjust window rect.\n");
    }

    const int windowWidth = rect.right - rect.left;
    const int windowHeight = rect.bottom - rect.top;

    return CreateWindowExA(
        exStyle,
        (LPCSTR)windowClassAtom,
        "Game",
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL);
}

int main(int argc, const char *argv[])
{
    HINSTANCE hInstance = GetModuleHandleA(NULL);

    SetWorkingDirectory();
    GameDLL gameDLL = {0};
    gameDLL.Load();

    LARGE_INTEGER qpcFrequency;
    QueryPerformanceFrequency(&qpcFrequency);

    LARGE_INTEGER qpcNow;
    constexpr LONGLONG targetFramesPerSecond = 60;
    constexpr LONGLONG targetTicksPerSecond = 10;
    const LONGLONG renderInterval = qpcFrequency.QuadPart / targetFramesPerSecond;
    const LONGLONG tickInterval = qpcFrequency.QuadPart / targetTicksPerSecond;
    LONGLONG nextTickQPC = 0;
    LONGLONG nextRenderQPC = 0;

    HWND hWindow = WindowCreate(hInstance, 800, 600);
    if (!hWindow)
    {
        LOG_LAST_ERROR("Unable to create window\n");
        return 0;
    }
    ShowWindow(hWindow, SW_SHOW);

    MSG msg;
    while (true)
    {
        if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                LOG("QUIT\n");
                goto release_and_return;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        QueryPerformanceCounter(&qpcNow);
        if (qpcNow.QuadPart >= nextTickQPC)
        {
            gameDLL.DoSomething();
            nextTickQPC = qpcNow.QuadPart + tickInterval;
        }
        if (qpcNow.QuadPart >= nextRenderQPC)
        {
            if (qpcNow.QuadPart >= nextRenderQPC + renderInterval / 2)
            {
                LOG("LOST %lld\n", qpcNow.QuadPart - nextRenderQPC);
            }
            // LOG("Render\n");
            nextRenderQPC = qpcNow.QuadPart + renderInterval;
        }

        gameDLL.LoadIfNeeded();
    }

release_and_return:
    // Redundant frees that only waste time since system would clear everything.

    return 0;
}