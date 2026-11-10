// msw::CCockpitTournament: the tournament's cockpit task, and the
// destructor of msw::CCockpitTournamentLayoutBase.
#include "game/msw/CCockpitTournament.hpp"

#include "game/msw/CCockpitTournamentTelopLogo.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CTournament.hpp"

namespace {
    // Events the cockpit sends to its display objects (the same numbers
    // CCockpitTournamentTelopLogo::SendEvent handles).
    enum {
        EVENT_SET_DISP = 10001,      // pParam: nonzero to show
        EVENT_PLAY_END_ANIM = 10002, // play the out animation
        EVENT_ANIM_END_CLOSE = 10003 // pParam: nonzero to go out after the in animation
    };

    // Display object ids this cockpit creates itself; the rest go to
    // CCockpit::CreateObjCommonImpl.
    enum {
        OBJ_TELOP_LAST = 3,   // 0 to 3: colour-switching telops
        OBJ_MODEL_TELOP_0 = 4, // effect telops; the suffix is the last
        OBJ_MODEL_TELOP_2 = 5, // constructor argument
        OBJ_LOGO = 6,
        OBJ_CAP_LOGO = 7,
        OBJ_TELOP_EX = 0x1025
    };

    // Number of display objects SetDisp walks.
    const long DISP_OBJ_NUM = 8;
}

// The constructor and InitState are ordinary out-of-line functions here,
// and both are inlined into Create. The original module has no copy of
// either: its link dropped them as unreferenced (see notes/toolchain.md,
// "Module links dead-strip"). Written this way, the unit reproduces the
// original's code and data: the order of the weak functions around them,
// the three copies of the state function pointers in .data (one per body
// that holds InitState: InitState itself, the constructor, Create) and the
// stack layout of Create.
msw::CCockpitTournament::CCockpitTournament(bcs::CTask *pParent)
    : CCockpit(pParent)
{
    m_Unk44 = 0;
    InitState();
}

// (guess) The original stores both vtable pointers before the inlined
// bcs::CFsm destructor runs, which the compiler only does when the body
// calls something: here an inline function that is empty in this build
// (see notes/decisions.md). What it was is not known.
static inline void Nothing() {}

msw::CCockpitTournament::~CCockpitTournament()
{
    Nothing();
}

long msw::CCockpitTournament::ChangeState(long nState)
{
    return bcs::CFsm<CCockpitTournament>::ChangeState(nState);
}

void msw::CCockpitTournament::InitState()
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_INITIALIZE, &CCockpitTournament::Initialize_In, &CCockpitTournament::Initialize_Loop,
             &CCockpitTournament::Initialize_Out);
    SetState(STATE_GAMEPLAY, &CCockpitTournament::GamePlay_In, &CCockpitTournament::GamePlay_Loop,
             &CCockpitTournament::GamePlay_Out);
    ChangeState(STATE_INITIALIZE);
}

void msw::CCockpitTournament::Main()
{
    MainLoopCommon();
    ExecState();
    ModListMainLoop();
}

void msw::CCockpitTournament::Disp()
{
    ModListDispLoop();
}

void msw::CCockpitTournament::Create(bcs::CTask *pParent)
{
    if (s_TaskHandle == 0) {
        new CCockpitTournament(pParent);
    }
}

void msw::CCockpitTournament::SetDisp(int bDisp)
{
    for (long i = 0; i < DISP_OBJ_NUM; i++) {
        SendObjEventTrigger(i, EVENT_SET_DISP, (void *)bDisp);
    }
}

void msw::CCockpitTournament::SetAnimEndClose(long nObjId, int bClose)
{
    SendObjEventTrigger(nObjId, EVENT_ANIM_END_CLOSE, (void *)bClose);
}

void msw::CCockpitTournament::PlayEndAnim(long nObjId)
{
    SendObjEventTrigger(nObjId, EVENT_PLAY_END_ANIM, 0);
}

long msw::CCockpitTournament::CreateObj(long nObjId, long nParam)
{
    CCockpit *pCockpit = GetTask();

    if (pCockpit == 0) {
        return -2;
    }
    return pCockpit->CreateObjImpl(nObjId, nParam);
}

long msw::CCockpitTournament::CreateObjImpl(long nObjId, long nParam)
{
    long nType;
    CMenuLinkBase *pObj = 0;

    switch (nObjId) {
        case 0:
        case 1:
        case 2:
        case OBJ_TELOP_LAST: {
            int bMix = CTournament::IsMixTournamentGame();
            nType = 0x10;
            if (bMix) {
                nType = 0x11;
            }
            pObj = new CCockpitTelopSwitchColor(nObjId, nType, 0.0f);
            break;
        }
        case OBJ_MODEL_TELOP_0: {
            CCockpitTournamentModelTelopHold *pTelop = new CCockpitTournamentModelTelopHold(nObjId, 0, 0.0f, 0);
            if (pTelop != 0) {
                pTelop->Set();
                pObj = pTelop;
            }
            break;
        }
        case OBJ_MODEL_TELOP_2: {
            CCockpitTournamentModelTelopHold *pTelop = new CCockpitTournamentModelTelopHold(nObjId, 0, 0.0f, 2);
            if (pTelop != 0) {
                pTelop->Set();
                pObj = pTelop;
            }
            break;
        }
        case OBJ_LOGO: {
            CCockpitTournamentTelopLogo *pLogo = new CCockpitTournamentTelopLogo(nObjId, 0, 0.0f);
            if (pLogo != 0) {
                pObj = pLogo;
            }
            break;
        }
        case OBJ_CAP_LOGO: {
            CCockpitTournamentTelopLogo *pLogo = new CCockpitTournamentTelopLogo(nObjId, 1, 0.0f);
            short nValue;
            switch (CGame::g_GameWork.GetUnk07()) {
                default:
                    nValue = 0;
                    break;
                case 2:
                    nValue = 1;
                    break;
                case 3:
                    nValue = 2;
                    break;
            }
            pLogo->SetAppendTableParam(0, nValue);
            pLogo->Set();
            pObj = pLogo;
            break;
        }
        case OBJ_TELOP_EX: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0xE, 0.0f);
            pTelop->SetAppendTableParam(0, nParam);
            pObj = pTelop;
            break;
        }
        default:
            pObj = CreateObjCommonImpl(nObjId, nParam);
            break;
    }
    return AddObj(pObj);
}

void msw::CCockpitTournament::Initialize_In()
{
}

void msw::CCockpitTournament::Initialize_Loop()
{
    ChangeState(STATE_GAMEPLAY);
}

void msw::CCockpitTournament::Initialize_Out()
{
}

void msw::CCockpitTournament::GamePlay_In()
{
}

void msw::CCockpitTournament::GamePlay_Loop()
{
}

void msw::CCockpitTournament::GamePlay_Out()
{
}

// Included here rather than at the top: classes declared later get their
// vtables emitted earlier in .data, and the original has this class's
// vtable first, ahead of bcs::CStateTemplate<CCockpitTournament>'s, which
// the functions above instantiate.
#include "game/msw/CCockpitTournamentLayoutBase.hpp"

// Same shape as CCockpitBossLayoutBase's destructor (BossF), with the same
// stand-in as ~CCockpitTournament above.
msw::CCockpitTournamentLayoutBase::~CCockpitTournamentLayoutBase()
{
    Nothing();
}
