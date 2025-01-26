#include "dataHandling\win32_SetWorkingDirectory.hpp"
#include "dataHandling\win32_GameDLL.hpp"
#include "win32_rendering.hpp"
#include "win32_gamePlatform.hpp"

#include "Windows.h"
#include "windowsx.h"

constexpr int renderWidth = 1920;
constexpr int renderHeight = 1080;
constexpr int clientWidth = 1920;
constexpr int clientHeight = 1080;
constexpr LONGLONG targetFramesPerSecond = 60;
constexpr LONGLONG targetTicksPerSecond = 10;


// void DoSomethingWithFont(GameMemory memory)
// {
//     constexpr wchar_t *path = L"assets\\fonts\\Marck_Script\\MarckScript-Regular.ttf";
//     constexpr int width = 1024;
//     constexpr int height = 1024;
//     if (AddFontResourceExW(path, FR_PRIVATE, nullptr) == 0)
//     {
//         LOG("Unable to read font file!\n");
//         return;
//     }

//     HDC hdc = CreateCompatibleDC(GetDC(nullptr));
//     HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
//     SelectObject(hdc, hBitmap);

//     SetBkColor(hdc, RGB(255, 255, 255));
//     SetTextColor(hdc, RGB(0, 0, 0));

//     HFONT hFont = CreateFontW(
//         48,
//         0,
//         0,
//         0,
//         FW_NORMAL,
//         false,
//         false,
//         false,
//         DEFAULT_CHARSET,
//         OUT_TT_PRECIS,
//         CLIP_DEFAULT_PRECIS,
//         ANTIALIASED_QUALITY,
//         DEFAULT_PITCH | FF_DONTCARE,
//         L"MarckScript-Regular"
//     );
//     if (!hFont) {
//         LOG_LAST_ERROR("Unable to create font\n");
//         RemoveFontResourceExW(path, FR_PRIVATE, nullptr);
//         return;
//     }

//     HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
// }


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
    constexpr DWORD exStyle = WS_EX_TOPMOST;
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
#if SLOW
    LARGE_INTEGER qpcStart;
    QueryPerformanceCounter(&qpcStart);
#endif

    SetProcessDPIAware();

    SetWorkingDirectory();
    GameDLL gameDLL = {0};
    gameDLL.Load();

    PlatformLayer platformLayer;
    Win32GamePlatformInit(&platformLayer);
    gameDLL.GameInitialize(&platformLayer);

    Renderer renderer;
    renderer.Initialize(renderWidth, renderHeight);

    LARGE_INTEGER qpcFrequency;
    QueryPerformanceFrequency(&qpcFrequency);

    LARGE_INTEGER qpcNow;

    const LONGLONG renderInterval = qpcFrequency.QuadPart / targetFramesPerSecond;
    const LONGLONG tickInterval = qpcFrequency.QuadPart / targetTicksPerSecond;
    LONGLONG nextTickQPC = 0;
    LONGLONG nextRenderQPC = 0;

    HWND hWindow = WindowCreate(hInstance, clientWidth, clientHeight);
    if (!hWindow)
    {
        LOG_LAST_ERROR("Unable to create window\n");
        return 0;
    }

    HDC hdc = GetDC(hWindow);
    if (!hdc)
    {
        LOG_LAST_ERROR("Unable to create device context.\n");
    }

#if SLOW
    QueryPerformanceCounter(&qpcNow);
    double setupTime = (double)(qpcNow.QuadPart - qpcStart.QuadPart) / (double)(qpcFrequency.QuadPart);
    LOG("Setup took %lf seconds.\n", setupTime);
#endif
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
            LONGLONG ticksElapsed = (qpcNow.QuadPart - nextTickQPC) / tickInterval + 1;
#if SLOW
            if (ticksElapsed > 1)
            {
                LOG("TICKS LOST %lld\n", qpcNow.QuadPart - nextTickQPC);
            }
#endif
            nextTickQPC = qpcNow.QuadPart + tickInterval;
            gameDLL.GameProcessTick(&platformLayer, (int)ticksElapsed);
        }
        if (qpcNow.QuadPart >= nextRenderQPC)
        {
#if SLOW
            if (qpcNow.QuadPart >= nextRenderQPC + renderInterval)
            {
                LOG("RENDERING LOST %lld frames\n", (qpcNow.QuadPart - nextRenderQPC) / renderInterval);
            }
#endif
            nextRenderQPC = qpcNow.QuadPart + renderInterval;
            gameDLL.GamePrepareForRenderer(&platformLayer, renderer.gameInfo);

            RECT rect;
            GetClientRect(hWindow, &rect);
            renderer.Present(hdc, rect.right - rect.left, rect.bottom - rect.top);
        }

        gameDLL.LoadIfNeeded();
    }

release_and_return:
    // Redundant frees that only waste time since system would clear everything.
#if SLOW
    renderer.Deinitialize();
    GameMemoryRelease(&platformLayer);
#endif
    return 0;
}