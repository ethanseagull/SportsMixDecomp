// msw::CComDrvVolleyGame: the run check, the spike and special shot
// methods, and the interrupt and think functions that pick the next
// action. A file of its own after game/volley/CComDrvVolleyGame.cpp; the
// file name is not known.
#include "game/msw/CComDrvVolleyGame.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/BCS_Vec.hpp"
#include "nw4r/math/triangular.h"

namespace {
    // (guessed helper, as in the volley cameras) The horizontal distance
    // between two points.
    inline float DistXZ(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 from(rFrom.x, 0.0f, rFrom.z);
        bcs::FVEC3 to(rTo.x, 0.0f, rTo.z);
        return bcs::FVEC3Dist(&from, &to);
    }
}

// Whether a character other than the driver's team mates stands in the way
// to rPos.
int msw::CComDrvVolleyGame::IsExistChrToTargetWorldForRunEx(const bcs::FVEC3 &rPos)
{
    unsigned long nIgnore = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo()) {
            CComMan::SetEnableBit(nIgnore, i);
        }
    }
    return CComMan::IsExistChrToTargetWorld(m_pChr, rPos, 0.2f, 0.5f + m_pChr->GetCollSize_BodyPush(), nIgnore, 0);
}

int msw::CComDrvVolleyGame::CanbeSpike() const
{
    return m_pChr->CanbeSpike(0);
}

// Spikes, or plays the special shot instead when the level's rate says so
// (never on the lowest levels while the team is not behind).
int msw::CComDrvVolleyGame::MethdSpike()
{
    if (m_pChr->IsMoveAssist()) {
        return ReturnAutoRunMode();
    }

    long nRate = CComMan::GetDatI(0x28E, GetComLvF());
    if (!CanbeSp()) {
        nRate = 0;
    }
    if (GetComBaseLvF() <= 3.0f && CGame::GetTeamScore(m_nMyTeam) >= CGame::GetTeamScore(m_nEmTeam)) {
        nRate = 0;
    }
    if (LotRate() < nRate) {
        return MethdSp();
    }

    int ret = m_pChr->MethdSpike(0);
    if (ret) {
        m_nUnk1DD = -1;
    }
    return ret;
}

int msw::CComDrvVolleyGame::CanbeSp() const
{
    if (IsExistPadDrv()) {
        return 0;
    }
    return m_pChr->CanbeAirTrick();
}

int msw::CComDrvVolleyGame::MethdSp()
{
    return m_pChr->MethdAirTrick();
}

int msw::CComDrvVolleyGame::CanbeMoveTargetMarkerPos() const
{
    return m_pChr->CanbeMoveVolleyTargetMarkerPos();
}

int msw::CComDrvVolleyGame::MethdMoveTargetMarkerPos(const bcs::FVEC3 &rPos, float fRange) const
{
    if (m_pChr->IsMoveAssist()) {
        return ReturnAutoRunMode();
    }

    bcs::FVEC3 marker;
    if (!m_pChr->GetTargetMarkerPos(marker)) {
        return 1;
    }

    bcs::FVEC3 pos = Local2World(rPos, -1);
    float fDist = DistXZ(pos, marker);
    if (fDist < fRange) {
        return 1;
    }

    // The direction from the marker to rPos, from 0 to 2 pi.
    bcs::FVEC3 diff;
    diff.x = pos.x - marker.x;
    diff.z = pos.z - marker.z;
    float fRot = nw4r::math::Atan2Rad(diff.x, diff.z);
    while (fRot > 6.2831855f) {
        fRot -= 6.2831855f;
    }
    while (fRot < 0.0f) {
        fRot += 6.2831855f;
    }

    if (fDist > 0.2f && !m_pChr->MoveTargetMarkerPos(fRot)) {
        return 1;
    }
    return 0;
}

// Breaks off the running action for something more urgent.
void msw::CComDrvVolleyGame::ActInterrupt()
{
    if (ShoudSpDefensive() && MethdSp()) {
        Reset();
        return;
    }
    if (ShoudReceiveSuddenGo()) {
        Reset();
        ActCallReceiveSudden();
        return;
    }
    if (ShoudDiveReceiveGo()) {
        Reset();
        ActCallDiveReceive();
        return;
    }
    if (ShoudEscGimmick()) {
        Reset();
        ActCallEscGimmick();
        return;
    }
    if (IsShoudTargetMarkerGoSp() && ShoudTargetMarkerGo()) {
        Reset();
        ActCallTargetMarkerGo();
        return;
    }
    if (ShoudInterThrowItem()) {
        Reset();
        ActCallInterThrowItem();
        return;
    }

    // The driver's own special shot (state 0x27): go after its marker.
    CChrCore *pChr = m_pChr;
    if (GetLastSpChrCore() == pChr && pChr->GetState() == 0x27 && IsSpMarkerMode() && GetRootActType() != 0x20 &&
        GetRootActType() != 0x22) {
        Reset();
        WraperActCallOffSpStart();
    }
}

// Picks the next action when none is running.
void msw::CComDrvVolleyGame::ThkMain()
{
    if (IsExistServeMode()) {
        if (m_pChr->IsVolleyServeMode()) {
            ActCallServe();
        } else {
            ActCallServeWait();
        }
        return;
    }

    if (IsSpMarkerMode()) {
        if (GetLastSpChrCore() == 0) {
            return;
        }
        if (GetLastSpChrCore() == m_pChr) {
            WraperActCallOffSpStart();
        } else if (m_nMyTeam == GetLastSpChrCore()->GetTeamNo()) {
            WraperActCallOffSpSub();
        } else {
            WraperActCallDefSpStart();
        }
        return;
    }

    if (!IsMyTurn()) {
        return;
    }

    // What to do depends on how many touches the team has made.
    long nTouch = m_pBall->GetTouchNum();
    if (nTouch == 0) {
        if (ShoudReceiveGo()) {
            ActCallReceiveGo();
        } else {
            ActCallTossReady();
        }
    } else if (nTouch == 1) {
        if (ShoudTossGo()) {
            if (!IsExistPadDrv() && m_bTwoAttack && m_nMyChrIdx == GetShoudTwoAttackChrIdx()) {
                ActCallSpikeGo();
            } else {
                ActCallTossGo();
            }
        } else {
            ActCallSpikeReady();
        }
    } else if (ShoudSpikeGo()) {
        ActCallSpikeGo();
    } else if (GetMyStockItemType() >= 0 && LotRateItemThrowTimingGood()) {
        ActCallItemShoot();
    } else if (IsShoudGetQPanel() && LotRateGetQPanelGood()) {
        ActCallGetQPanel();
    } else if (IsShoudGetFallGShell() && LotRateGetQPanelGood()) {
        ActCallGetQPanel();
    } else if (IsShoudGetDropCoin() && LotRateGetDropCoinGood()) {
        ActCallGetDropCoin();
    } else {
        ActCallGoHome();
    }
}
