// msw::CComDrvBoss's frame and message handlers, and CComManBoss's helpers.
// A file of its own after game/boss/CComDrvBoss.cpp's weak functions, with
// its own float constants; the file name is not known. The methods the
// actions use are in game/boss/unit_52300.cpp, ThkMain in
// game/boss/unit_52680.cpp.
#include "game/msw/CComDrvBoss.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComManBoss.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "nw4r/math/arithmetic.h"
#include <math.h>

namespace {
    // (guessed helper, as in the volley driver) The horizontal distance
    // between two points.
    inline float DistXZ(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 from(rFrom.x, 0.0f, rFrom.z);
        bcs::FVEC3 to(rTo.x, 0.0f, rTo.z);
        return bcs::FVEC3Dist(&from, &to);
    }
}

namespace msw {

int CComDrvBoss::IsActiveBallNothingThk() const
{
    return 1;
}

// Every frame: runs the action in progress, or thinks of the next one.
long CComDrvBoss::OnMain(_CHR_MSG *pMsg)
{
    CComDrv::OnMain(pMsg);
    ActInterrupt();
    long nAct = GetCurrentActType();
    if (nAct >= 0) {
        if (ActMain()) {
            ActFunc func = g_ActFuncMap[nAct];
            (this->*func)();
        }
    } else {
        ThkMain();
    }
    return 0;
}

long CComDrvBoss::OnInit(_CHR_MSG *pMsg)
{
    return CComDrv::OnInit(pMsg);
}

// The game's questions and news. (message numbers only; their names are
// not known)
long CComDrvBoss::OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = CComDrv::OnGameMsgProc(id, pMsg);
    switch (id) {
        case 0x13:
            Reset();
            break;
        case 0x14:
            Reset();
            break;
        case 0xD:
            Reset();
            break;
        case 0xE:
            Reset();
            break;
        case 0x10:
            if (GetRootActType() == ACT_GET_QPANEL) {
                Reset();
            }
            break;
        case 0x19:
            nRet = 1;
            break;
        case 0x8C:
            nRet = 1;
            break;
        case 0x8F: {
            // Whether to jump for the ball: close by and a little above.
            float fDist = DistXZ(m_BallPos, m_MyPos);
            float fHeight = m_BallPos.y - m_MyPos.y;
            float fMin = 1.0f + 0.5f * RandF();
            float fMax = 0.5f + fMin;
            if (CanbeMyTouch() && fDist < 2.0f && fHeight > fMin && fHeight < fMax && m_BallPos.y > 1.5f &&
                m_MyPos.y > 1.5f) {
                nRet = 1;
            }
            break;
        }
        case 0x86:
            Reset();
            break;
        case 0x87:
            Reset();
            break;
        case 0x8A:
            if (bcs::BCS_RandMax(8L) == 0) {
                nRet = 1;
            }
            break;
        case 0x8B:
            nRet = bcs::BCS_RandMax(2L);
            break;
        case 0x92:
            if (fabsf(m_BallPos.y - m_MyPos.y) <= 3.0f && bcs::BCS_RandMax(15L) == 0) {
                nRet = 1;
            }
            break;
        case 0x93:
            if (bcs::BCS_RandMax(120L) == 0) {
                nRet = 1;
            }
            break;
        case 0x94:
            nRet = 1;
            break;
        case 0x9C:
            Reset();
            break;
        case 0x1A:
            nRet = m_nStickAttackStep;
            break;
    }
    return nRet;
}

void CComDrvBoss::OnAlarm0(long nAlarm)
{
    ResetRelayRun();
    SetAlarm(nAlarm, 60.0f);
}

int CComManBoss::GetBallTargetPos(CBall *pBall, bcs::FVEC3 &rPos)
{
    if (pBall == 0) {
        return 0;
    }
    if (!pBall->GetTargetPos(rPos)) {
        return 0;
    }
    rPos = World2Local(rPos, -1);
    return 1;
}

int CComManBoss::CanbeTouch(CChrCore *pChr, long nBall)
{
    if (nBall < 0) {
        nBall = GetActiveBallIdx();
    }
    if (nBall < 0) {
        return 0;
    }
    return pChr->IsVolleyRightBallTouch(nBall);
}

void CComManBoss::GetTouchDownPos(CBall *pBall, bcs::FVEC3 &rPos)
{
    pBall->GetVolleyTouchDownPos(rPos, 1);
    rPos = World2Local(rPos, -1);
}

} // namespace msw
