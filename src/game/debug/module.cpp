// Entry points of the DebugF module.

#include "game/msw/CSystemTest.hpp"

typedef void (*VoidFunc)();

extern "C" {

extern VoidFunc _ctors[];
extern VoidFunc _dtors[];

void _prolog();
void _epilog();
void _unresolved();
void DebugModuleMain();

void _prolog()
{
    VoidFunc* pFunc;

    for (pFunc = _ctors; *pFunc != 0; pFunc++) {
        (*pFunc)();
    }
}

void _epilog()
{
    VoidFunc* pFunc;

    for (pFunc = _dtors; *pFunc != 0; pFunc++) {
        (*pFunc)();
    }
}

void _unresolved()
{
}

// Starts the system test task; the task registers itself with its parent
// through the bcs::CTask constructor, so the pointer is not kept.
void DebugModuleMain()
{
    new msw::CSystemTest;
}

}
