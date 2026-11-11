// msw::CComDrvDodgeDD, the computer driver of the party game "dodge or die":
// constructor, destructor, action table and the small overrides. The
// script functions are in the next file.
#include "game/msw/CComDrvDodge.hpp"

// The file's static initialiser sets this float, which nothing here uses
// (as in src/game/dodge/CComDrvDodge.cpp); name and header are not known.
namespace {
    inline float DegToRad(float fDeg)
    {
        return fDeg * (3.1415927f / 180.0f);
    }

    float s_fAngle60 = DegToRad(60.0f);
}

namespace msw {

CComDrvDodgeDD::ActFunc CComDrvDodgeDD::g_ActFuncMap[12] = {
    &CComDrv::ActLoopNop,
    &CComDrv::ActLoopWait,
    &CComDrv::ActLoopWaitNeutral,
    &CComDrv::ActLoopLookTarget,
    &CComDrv::ActLoopLookFront,
    &CComDrv::ActLoopLookBall,
    &CComDrv::ActLoopRun,
    &CComDrv::ActLoopRunTurn,
    &CComDrv::ActLoopJump,
    &CComDrv::ActLoopJumpBlock,
    &CComDrvDodgeDD::ActLoopGoHome,
    &CComDrvDodgeDD::ActLoopScript,
};

// Going home does nothing in this game; only the work is looked up.
void CComDrvDodgeDD::ActLoopGoHome()
{
    GetCurrentActWork();
}

// The action work is at least 0x28 bytes, this driver's own need.
static inline unsigned long CalcWorkMaxSize(const CComDrv *pDrv)
{
    unsigned long nBase = pDrv->CalcWorkBaseMaxSize();
    unsigned long nSize = 0x28;
    if (nBase > nSize) {
        nSize = nBase;
    }
    return nSize;
}

CComDrvDodgeDD::CComDrvDodgeDD() : CComDrvDodge(CalcWorkMaxSize(this))
{
    m_pScript = 0;
    m_nUnk170 = 0;
    ClearAll();
}

CComDrvDodgeDD::~CComDrvDodgeDD()
{
    DeleteScript();
}

void CComDrvDodgeDD::Reset()
{
    CComDrv::Reset();
    ResetScript();
}

void CComDrvDodgeDD::ClearAll()
{
    CComDrv::ClearAll();
    DeleteScript();
}

void CComDrvDodgeDD::UpdateInfo()
{
    CComDrv::UpdateInfo();
}

void CComDrvDodgeDD::EntryActCallBack(long)
{
}

void CComDrvDodgeDD::NormalizeCanMoveWorldPos(bcs::FVEC3 &pos) const
{
    CComDrv::NormalizeCanMoveWorldPos(pos);
}

int CComDrvDodgeDD::IsActiveBallNothingThk() const
{
    return 1;
}

} // namespace msw
