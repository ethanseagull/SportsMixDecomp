// msw::CCockpitMenu: the menu's cockpit task. Same shape as
// msw::CCockpitTournament (TourF, src/game/tour/CCockpitTournament.cpp),
// with one empty state and only the common display objects.
#include "game/msw/CCockpitMenu.hpp"

// The constructor and InitState are ordinary out-of-line functions here,
// and both are inlined into Create. The original module has no copy of
// either: its link dropped them as unreferenced (see notes/toolchain.md,
// "Module links dead-strip"). Written this way, the unit reproduces the
// three copies of the state function pointers in .data (one per body that
// holds InitState: InitState itself, the constructor, Create).
msw::CCockpitMenu::CCockpitMenu(bcs::CTask *pParent)
    : CCockpit(pParent)
{
    m_Unk21C = 0;
    InitState();
    m_Unk70 = 0x10000;
    m_Result = 0x10000;
}

// (guess) The original stores both vtable pointers before the inlined
// bcs::CFsm destructor runs, which the compiler only does when the body
// calls something: here an inline function that is empty in this build
// (see notes/decisions.md). What it was is not known.
static inline void Nothing() {}

msw::CCockpitMenu::~CCockpitMenu()
{
    Nothing();
}

long msw::CCockpitMenu::ChangeState(long nState)
{
    return bcs::CFsm<CCockpitMenu>::ChangeState(nState);
}

void msw::CCockpitMenu::InitState()
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_WAIT, &CCockpitMenu::Wait_In, &CCockpitMenu::Wait_Loop, &CCockpitMenu::Wait_Out);
    ChangeState(STATE_WAIT);
}

void msw::CCockpitMenu::Main()
{
    MainLoopCommon();
    ExecState();
    ModListMainLoop();
}

void msw::CCockpitMenu::Disp()
{
    ModListDispLoop();
}

void msw::CCockpitMenu::Create(bcs::CTask *pParent)
{
    if (s_TaskHandle == 0) {
        new CCockpitMenu(pParent);
    }
}

long msw::CCockpitMenu::CreateObjImpl(long nObjId, long nParam)
{
    return AddObj(CreateObjCommonImpl(nObjId, nParam));
}

void msw::CCockpitMenu::Wait_In()
{
}

void msw::CCockpitMenu::Wait_Loop()
{
}

void msw::CCockpitMenu::Wait_Out()
{
}
