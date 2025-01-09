#include "dataHandling\SetWorkingDirectory.hpp"
#include "dataHandling\GameDLL.hpp"
#include "Windows.h"

int main(int argc, const char *argv[])
{
    SetWorkingDirectory();

    GameDLL gameDLL = {0};

    gameDLL.Load();

    while (true)
    {
        gameDLL.DoSomething();
        Sleep(1000);
        gameDLL.LoadIfNeeded();
    }
    return 0;
}