// Entry points of the SystemF module.

#include "game/msw/CMainStepEx.hpp"

typedef void (*VoidFunc)();

extern "C" {

extern VoidFunc _ctors[];
extern VoidFunc _dtors[];

void _prolog();
void _epilog();
void _unresolved();
void SystemModuleMain();

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

// Hands control to the main step machine.
void SystemModuleMain()
{
    msw::CMainStepEx::Initialize();
}

}
