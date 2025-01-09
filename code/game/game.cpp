#include "platform/platform.hpp"

extern "C" DLL_EXPORT GAME_DO_SOMETHING(DoSomething)
{
    LOG("do something new\n");
}
