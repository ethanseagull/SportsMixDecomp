// msw::CComDrvVolleyGame's frame, message and alarm handlers. A file of its
// own after game/volley/CComDrvVolleyGame.cpp, with its own float constants
// (as DodgeF's game/dodge/unit_A40EC.cpp); the file name is not known.
#include "game/msw/CComDrvVolleyGame.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CGame.hpp"

namespace {
    // (guessed helper) v brought into lo..hi.
    inline long ClampI(long v, long lo, long hi)
    {
        return v > hi ? hi : (v < lo ? lo : v);
    }
}

namespace msw {

// Every frame: nothing while the character attacks in a combination or is
// down; otherwise runs the action in progress, or thinks of the next one.
long CComDrvVolleyGame::OnMain(_CHR_MSG *pMsg)
{
    if (m_pChr->IsVolleyCombinationAttacker()) {
        Reset();
        return 0;
    }
    CComDrv::OnMain(pMsg);
    if (m_pChr->GetState() == CChrCore::STATE_DAMAGE) {
        return 0;
    }
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

long CComDrvVolleyGame::OnInit(_CHR_MSG *pMsg)
{
    long nRet = CComDrv::OnInit(pMsg);
    UpdateHomePos();
    UpdateAPosition();
    return nRet;
}

// The game's questions and news. (message numbers only; their names are
// not known)
long CComDrvVolleyGame::OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = CComDrv::OnGameMsgProc(id, pMsg);
    switch (id) {
        case 0x10:
            if (GetRootActType() == ACT_GET_QPANEL) {
                Reset();
                ActCallGoHome();
            }
            break;
        case 0x86:
            if (IsMyTurn()) {
                Reset();
            }
            break;
        case 0x87:
            if (IsMyTurn()) {
                Reset();
            }
            break;
        case 0x8C:
            if (LotRate() < 70) {
                nRet = 1;
            }
            break;
        case 0x8D:
        case 0x8E:
            nRet = 1;
            break;
        case 0x8F: {
            // Which spike to play: the one that hits.
            float fZ0 = -2.0f;
            float fZ1 = 3.5f + fZ0;
            float fY = 1.8f;
            int bHit0 = IsHitSpikeShot(0, fY, fZ0, fZ1);
            int bHit2 = IsHitSpikeShot(2, fY, fZ0, fZ1);
            if (bHit0 || bHit2) {
                LotRateSpikeBefore();
            }
            if (m_nUnk1DD == 0 || m_nUnk1DD == 1) {
                if (bHit0) {
                    nRet = 1;
                }
            } else if (m_nUnk1DD == 2) {
                if (bHit2) {
                    nRet = 2;
                }
            } else if (m_nUnk1DD == 3) {
                if (bHit2) {
                    nRet = RandRangeI(180, 200);
                }
            }
            break;
        }
        case 0x90:
            if (!IsExistPadDrv() && m_nUnk1DD == 1) {
                nRet = 1;
            }
            break;
        case 0x91: {
            // Which way to curve the spike (1 or 2; 0 for none).
            long nKind = pMsg->m_nParam0;
            int bOut = 0;
            long nCurve = m_nSpikeCurve;
            if (IsExistPadDrv() || CGame::g_GameWork.GetStage() == 5) {
                if (nKind == 1) {
                    if (IsSpikeCurveOutAtTargetMarker()) {
                        bOut = 1;
                    }
                } else if (nKind == 2) {
                    long nInner = GetSpikeCurveInnerValueAtTargetMarker();
                    if (nInner != 0) {
                        nCurve = nInner;
                    }
                }
            }
            if (nKind != 0 && !bOut) {
                if (m_nUnk98 == 0) {
                    if (nCurve > 0) {
                        nRet = 2;
                    } else {
                        nRet = 1;
                    }
                } else {
                    nRet = nCurve > 0 ? 1 : 2;
                }
                if (nKind == 1 && m_pChr->GetChrTypeID() == 6) {
                    if (nRet == 1) {
                        nRet = 2;
                    } else {
                        nRet = 1;
                    }
                }
            }
            break;
        }
        case 0x8A:
            if (RandMaxI(15) == 0) {
                nRet = 1;
            }
            break;
        case 0x8B:
            nRet = RandMaxI(2);
            break;
        case 0x17: {
            // The ball went over the net.
            long nTouch = m_pBall->GetTouchNum();
            UpdateAPosition();
            if (IsMyTurn()) {
                LotRateOffense();
                if (GetRootActType() != ACT_BLOCK_GO) {
                    Reset();
                }
            } else {
                m_bEarlyDef = 0;
                m_fEarlyDefTime = 0.0f;
                long nRate = CComMan::GetDatI(0x29E, GetComLvF());
                if (LotRate() < 20 && LotRate() < nRate) {
                    m_bEarlyDef = 1;
                    m_fEarlyDefTime = -RandRangeF(60.0f, 180.0f);
                }
                if (m_pChr->IsVolleyTargetMarkerActive()) {
                    Reset();
                    ActCallSpOffMoveMarker();
                } else {
                    LotRateDefense(nTouch);
                    ActChangeGoDef(nTouch);
                }
            }
            break;
        }
        case 0x85: {
            // The ball was touched.
            long nTouch = m_pBall->GetTouchNum();
            m_nTouchCount = 0;
            m_bSpBefore = 0;
            m_nUnk1D8 = -1;
            LotRateTouch(nTouch);
            if (nTouch == 1 || nTouch == 2) {
                UpdateAPosition();
                if (IsMyTurn()) {
                    if (nTouch == 1) {
                        LotRateTwoAttack();
                        LotRateReceiveFinished();
                    }
                    if (GetCurrentActType() != ACT_SPIKE_JUMP) {
                        Reset();
                    }
                } else {
                    if (nTouch <= 1) {
                        if (LotRate() < 25) {
                            LotDefPosHosei();
                        }
                    } else {
                        LotRateDefense(nTouch);
                    }
                    ActChangeGoDef(nTouch);
                }
            }
            break;
        }
        case 0x93: {
            // A feint: only when the spike would land well.
            long nRate = pMsg->m_nParam1 * 100 / pMsg->m_nParam0;
            if (nRate >= CComMan::GetDatI(0x2C6, GetComLvF())) {
                if (m_nUnk1F0 == 0) {
                    float fRange = CComMan::GetDatF(0x2CE, GetChrTypeID());
                    bcs::FVEC3 marker;
                    if (GetTargetMarkerPos(marker)) {
                        bcs::FVEC3 pos = Local2World(marker, -1);
                        if (IsOpenSpaceForSpike(pos, fRange)) {
                            nRet = 1;
                        }
                    }
                } else if (RandMaxI(120) == 0) {
                    nRet = 1;
                }
            }
            break;
        }
        case 0x94:
            // The draw is made, its result not used.
            RandMaxI(120);
            break;
        case 0x3B:
        case 0x3D:
        case 0x3E:
        case 0x97:
        {
            long nMin = CComMan::GetDatI(0x39, GetComLvF());
            long nMax = CComMan::GetDatI(0x41, GetComLvF());
            nMin = ClampI(nMin, 0, 0x400);
            nMax = ClampI(nMax, 0, 0x400);
            nRet = RandRangeI(nMin, nMax);
            break;
        }
        case 0x9B: {
            // A character of the other team is about to spike.
            long nChr = pMsg->m_nParam0;
            long nArg = pMsg->m_nParam1;
            int bBlock = 1;
            if (IsExistPadDrv() && nArg == 2) {
                bBlock = 0;
            }
            CChrCore *pChr = CChrMan::GetChrCore(nChr);
            if (bBlock && pChr != 0 && m_nMyTeam != pChr->GetTeamNo() && GetRootActType() == ACT_BLOCK_READY) {
                Reset();
                ActCallBlockGo();
            }
            break;
        }

        case 0x18:
            // A special shot started.
            if (GetLastSpChrCore() != 0) {
                m_bSpBefore = 1;
                LotRateSpBefore();
                m_bEarlyDef = 0;
                Reset();
                if (GetLastSpChrCore() == m_pChr) {
                    WraperActCallOffSpStart();
                } else if (m_nMyTeam == GetLastSpChrCore()->GetTeamNo()) {
                    WraperActCallOffSpSub();
                } else {
                    WraperActCallDefSpStart();
                }
            }
            break;
        case 0x88:
            if (!IsMyTurn()) {
                unsigned long nArg = pMsg->m_nParam1;
                m_nTouchCount++;
                m_bTouchGo = 0;
                if (m_nTouchCount <= 2) {
                    long nRate = CComMan::GetDatI(0xCE, GetComLvF());
                    if (nArg <= 2) {
                        nRate /= 2;
                    }
                    if (LotRate() < nRate) {
                        m_bTouchGo = 1;
                    }
                }
            }
            break;
        case 0x23:
            if (!IsExistServeMode() && !IsExistPadDrv() && !IsMyTurn() && !m_pChr->IsVolleyCombinationAttacker() &&
                m_bEarlyDef) {
                if (m_fEarlyDefTime >= 0.0f) {
                    nRet = 1;
                } else {
                    m_fEarlyDefTime += m_pChr->GetFrameStep();
                }
            }
            if (m_pChr->IsVolleyCombinationReceiver() && m_bEarlyDef) {
                nRet = 1;
            }
            break;
        case 0x89: {
            long nArg = pMsg->m_nParam1;
            if (IsMyTurn() && nArg == 4) {
                Reset();
            }
            break;
        }
        case 0x92:
            break;
    }
    return nRet;
}

void CComDrvVolleyGame::OnAlarm0(long nAlarm)
{
    UpdateHomePos();
    SetAlarm(nAlarm, 180.0f);
}

void CComDrvVolleyGame::OnAlarm1(long nAlarm)
{
    m_bGimmick = 1;
    SetAlarm(nAlarm, 3600.0f);
}

// Every two seconds: draws again what the driver escapes.
void CComDrvVolleyGame::OnAlarm2(long nAlarm)
{
    m_bEscBomb = 0;
    if (LotRate() < CComMan::GetDatI(0x8E, GetComLvF())) {
        m_bEscBomb = 1;
    }
    m_bEscItem = 0;
    if (LotRate() < CComMan::GetDatI(0x96, GetComLvF())) {
        m_bEscItem = 1;
    }
    m_bEscObstacle = 0;
    if (LotRate() < CComMan::GetDatI(0xBAD, GetComLvF())) {
        m_bEscObstacle = 1;
    }
    m_bSpObstacleSide = 0;
    if (LotRate() < CComMan::GetDatI(0xBB5, GetComLvF())) {
        m_bSpObstacleSide = 1;
    }
    SetAlarm(nAlarm, 120.0f);
}

}
