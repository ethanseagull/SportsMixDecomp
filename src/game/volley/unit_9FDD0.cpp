// msw::CComDrvVolleyGame: the actions of the volleyball driver (receive,
// toss, spike, block, serve, the runs to its places, the items and the
// gimmicks), started by ActCall* and run every frame by ActLoop*, and the
// choice of the defence action after the other team's touch. A file of its
// own before game/volley/CComDrvVolleyGame.cpp (its own float constants);
// the file name is not known.
#include "game/msw/CComDrvVolleyGame.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CSimpleSortBase.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManVolley.hpp"
#include "game/msw/CDropCoin.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CQPanel.hpp"
#include <math.h>

namespace unit_9FDD0_cpp {
// uncalled stand-in: the original file starts with the weak copies of
// bcs::BCS_ShakerSort for CSimpleSortBase<32> (and its compare functions),
// left by code the module link stripped; nothing in the file sorts.
void UnusedShakerSort(bcs::CSimpleSortBase<32>::SORT *pSort, long nNum)
{
    bcs::BCS_ShakerSort(pSort, nNum, bcs::CSimpleSortBase<32>::SortCmpParamF);
}
}

// (guessed) v kept between lo and hi; the bounds are evaluated again for
// the result, as the original calls the court edge getters twice.
#define CLAMP(v, lo, hi) ((v) > (hi) ? (hi) : ((v) < (lo) ? (lo) : (v)))

namespace {
    typedef msw::CComDrv::ACT_WORK ACT_WORK;

    // (guessed helper, as in game/volley/CComDrvVolleyGame.cpp) rIn's
    // direction in the XZ plane, length 1.
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(rIn.x, 0.0f, rIn.z));
    }

    // (names guessed) The work of the actions, after CComDrv's common
    // start. Most only keep the place they run to.
    struct POS_WORK : ACT_WORK {
        bcs::FVEC3 m_Target;        // 0x1C
    };

    // ReceiveGo, ReceiveSudden.
    struct RECEIVE_WORK : POS_WORK {
        long m_bOut;                // 0x28, the ball goes out: stand off it
        long m_bShift;              // 0x2C, wait to one side of the ball
        float m_fShift;             // 0x30, by how much (x)
        float m_fTouchFrame;        // 0x34, go under the ball from this many frames before it lands
        float m_fWait;              // 0x38, the wait before running
    };

    // SpikeReady.
    struct SPIKE_READY_WORK : POS_WORK {
        long m_bQuickLot;           // 0x28, the quick spike was drawn
    };

    // SpikeGo.
    struct SPIKE_GO_WORK : POS_WORK {
        unsigned char m_Unk28[0x34 - 0x28];
        long m_nTargetKind;         // 0x34, 0 the spike jump target, 1 the ball's, -1 none yet
        long m_bNoSpike;            // 0x38, run under the ball without jumping
    };

    // BlockGo.
    struct BLOCK_WORK : POS_WORK {
        float m_fJumpWait;          // 0x28, the frames to wait before jumping
        long m_bLate;               // 0x2C, the character's block jump comes late
        long m_bFeint;              // 0x30, jump away from the spike
    };

    // Serve.
    struct SERVE_WORK : POS_WORK {
        bcs::FVEC3 m_Marker;        // 0x28, where to move the target marker
    };

    // TargetMarkerGo.
    struct MARKER_WORK : POS_WORK {
        float m_fInterval;          // 0x28, the frames between two looks at the marker
    };

    // GetQPanel, GetDropCoin.
    struct ITEM_WORK : POS_WORK {
        unsigned long m_nItem;      // 0x28, the panel, shell or coin
    };
}

namespace msw {

void CComDrvVolleyGame::ActCallReceiveGo()
{
    RECEIVE_WORK *pWork = static_cast<RECEIVE_WORK *>(EntryAct(ACT_RECEIVE_GO));
    pWork->m_bOut = 0;

    long nRate = CComMan::GetDatI(0x196, GetComLvF());
    if (LotRate() < nRate) {
        // Wait beside the ball, on the side away from the team mates.
        pWork->m_bShift = 1;
        pWork->m_fShift = RandRange(2.0f, 3.5f);
        if (LotRate() < 50) {
            pWork->m_fShift = -pWork->m_fShift;
        }
        bcs::FVEC3 target;
        if (GetBallTargetPos(target)) {
            target.x += pWork->m_fShift;
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo()) {
                    bcs::FVEC3 pos = CComMan::GetChrPos(pChr);
                    if (CComMan::GetDisXZ(pos, target) < 2.0f) {
                        pWork->m_fShift = -pWork->m_fShift;
                        break;
                    }
                }
            }
        }
    } else {
        float fMin = CComMan::GetDatF(0x19E, GetComLvF());
        float fMax = CComMan::GetDatF(0x1A6, GetComLvF());
        pWork->m_bShift = 0;
        pWork->m_fTouchFrame = RandRange(fMin, fMax);
    }

    // Against a special shot, wait a little before going.
    pWork->m_fWait = 0.0f;
    CBall *pBall = GetActiveBall();
    if (pBall == 0) {
        return;
    }
    if (pBall->GetShootType() == 2) {
        long nWaitRate = CComMan::GetDatI(0x1C6, GetComLvF());
        if (LotRate() < nWaitRate) {
            float fMin = CComMan::GetDatF(0x1CE, GetComLvF());
            pWork->m_fWait = RandRange(fMin, CComMan::GetDatF(0x1D6, GetComLvF()));
        }
    } else if (pBall->IsArtsShootMode()) {
        CChrCore *pSp = GetLastSpChrCore();
        if (pSp != 0 && !IsExistPadDrv()) {
            switch (pSp->GetChrTypeID()) {
                case 3:
                    if (GetComLvF() >= 0.0f) {
                        pWork->m_fWait = RandRange(34.0f, 38.0f);
                    }
                    break;
                case 6:
                    if (GetComLvF() >= 4.0f) {
                        pWork->m_fWait = RandRange(4.0f, 6.0f);
                    }
                    break;
                case 10:
                    if (GetComLvF() >= 4.0f) {
                        pWork->m_fWait = RandRange(16.0f, 18.0f);
                    }
                    break;
                case 14:
                    if (m_nMateNum == 2 && GetComLvF() >= 4.0f) {
                        pWork->m_fWait = RandRange(18.0f, 20.0f);
                    }
                    break;
            }
        }
    }
}

// Runs to where the ball comes down and receives it; gives the ball up to
// a human team mate who stands there.
void CComDrvVolleyGame::ActLoopReceiveGo()
{
    RECEIVE_WORK *pWork = static_cast<RECEIVE_WORK *>(GetCurrentActWork());
    if (!CanbeMyTouch()) {
        ReturnAct(0);
        return;
    }
    if (!GetBallTargetPos(pWork->m_Target)) {
        ReturnAct(0);
        return;
    }

    float fFrame = m_pBall->GetTouchDownFrame(0.3f);
    bcs::FVEC3 touchDown;
    CComManVolley::GetTouchDownPos(m_pBall, touchDown);
    if (m_pBall->IsVolleyCurveSpike()) {
        if (pWork->m_bShift) {
            pWork->m_Target.x += pWork->m_fShift;
        } else if (fFrame <= pWork->m_fTouchFrame) {
            pWork->m_Target = touchDown;
        }
    }
    float fDist = GetDisXZ(m_MyPos, touchDown);

    if (IsOutBallJudge()) {
        // Stand off the ball on the driver's side.
        pWork->m_bOut = 1;
        bcs::FVEC3 diff;
        diff.x = m_MyPos.x - pWork->m_Target.x;
        diff.z = m_MyPos.z - pWork->m_Target.z;
        bcs::FVEC3 dir;
        NormalizeXZ(dir, diff);
        dir.x *= 2.5f;
        dir.z *= 2.5f;
        pWork->m_Target.x = dir.x + pWork->m_Target.x;
        pWork->m_Target.z = dir.z + pWork->m_Target.z;
    } else {
        pWork->m_bOut = 0;
    }

    if (!m_pBall->IsVolleyCurveSpike() &&
        IsExistChrTargetWithCondition(pWork->m_Target, 2.0f, GetMyTeamPadDrvCanTouchEnableBit())) {
        Reset();
        EntryAct(ACT_GO_AWAY);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            if (pWork->m_fWait > 0.0f) {
                ActCallWait(pWork->m_fWait);
            }
            pWork->m_nStep++;
            break;
        case 3:
            if (!pWork->m_bOut && fFrame <= 15.0f && CanbeReceive()) {
                if (MethdReceive()) {
                    ReturnAct(1);
                }
            } else {
                MethdRunEx(pWork->m_Target, 0.4f, 1.0f, 600.0f, 6, 10.0f);
            }
            break;
    }
}

void CComDrvVolleyGame::ActCallReceiveSudden()
{
    RECEIVE_WORK *pWork = static_cast<RECEIVE_WORK *>(EntryAct(ACT_RECEIVE_SUDDEN));
    pWork->m_bOut = 0;
}

// Runs straight to where the ball comes down and receives it.
void CComDrvVolleyGame::ActLoopReceiveSudden()
{
    RECEIVE_WORK *pWork = static_cast<RECEIVE_WORK *>(GetCurrentActWork());
    if (!CanbeMyTouch()) {
        ReturnAct(0);
        return;
    }

    float fFrame = m_pBall->GetTouchDownFrame(0.3f);
    bcs::FVEC3 touchDown;
    CComManVolley::GetTouchDownPos(m_pBall, touchDown);
    float fDist = GetDisXZ(m_MyPos, touchDown);
    pWork->m_Target = touchDown;

    if (IsOutBallJudge()) {
        pWork->m_bOut = 1;
        bcs::FVEC3 diff;
        diff.x = m_MyPos.x - pWork->m_Target.x;
        diff.z = m_MyPos.z - pWork->m_Target.z;
        bcs::FVEC3 dir;
        NormalizeXZ(dir, diff);
        dir.x *= 1.5f;
        dir.z *= 1.5f;
        pWork->m_Target.x = dir.x + pWork->m_Target.x;
        pWork->m_Target.z = dir.z + pWork->m_Target.z;
    } else {
        pWork->m_bOut = 0;
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            if (!pWork->m_bOut && fFrame <= 15.0f && CanbeReceive()) {
                if (MethdReceive()) {
                    ReturnAct(1);
                }
            } else {
                MethdRunEx(pWork->m_Target, 0.4f, 1.0f, 600.0f, 6, 10.0f);
            }
            break;
    }
}

void CComDrvVolleyGame::ActCallDiveReceive()
{
    EntryAct(ACT_DIVE_RECEIVE);
}

// Waits a moment (shorter for a curve spike, and on the higher levels for
// a feint), then dives for the ball.
void CComDrvVolleyGame::ActLoopDiveReceive()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            float fMin;
            float fMax;
            if (m_pBall->IsVolleyCurveSpike()) {
                fMin = CComMan::GetDatF(0x116, GetComLvF());
                fMax = CComMan::GetDatF(0x11E, GetComLvF());
            } else {
                fMin = CComMan::GetDatF(0x106, GetComLvF());
                fMax = CComMan::GetDatF(0x10E, GetComLvF());
                if (m_pBall->IsVolleyFeintSpike() && GetComLvF() >= 4.5f) {
                    fMin *= 0.5f;
                    fMax *= 0.5f;
                }
            }
            ActCallWait(RandRange(fMin, fMax));
            pWork->m_nStep++;
            break;
        }
        case 1:
            if (MethdDiveReceive()) {
                pWork->m_nStep++;
            } else if (MethdReceive()) {
                pWork->m_nStep++;
            }
            break;
        case 2:
            ActCallWait(60.0f);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallTossReady()
{
    EntryAct(ACT_TOSS_READY);
}

// Runs to the place for the second touch and waits there.
void CComDrvVolleyGame::ActLoopTossReady()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetTossReadyPos(pWork->m_Target);
            GetSafePosFromGimmick(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Target, 0.4f, 1.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 4:
            ActCallLookBall();
            pWork->m_nStep++;
            break;
        case 5:
            ActCallWait(120.0f);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallTossGo()
{
    EntryAct(ACT_TOSS_GO);
}

// Runs under the ball and tosses it.
void CComDrvVolleyGame::ActLoopTossGo()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    if (!CanbeMyTouch()) {
        ReturnAct(0);
        return;
    }
    if (!GetBallTargetPos(pWork->m_Target)) {
        ReturnAct(0);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            if (m_pBall->GetTouchDownFrame(0.3f) <= 15.0f && CanbeReceive()) {
                if (MethdReceive()) {
                    ReturnAct(1);
                }
            } else {
                MethdRunEx(pWork->m_Target, 0.4f, 1.0f, 600.0f, 6, 10.0f);
            }
            break;
    }
}

void CComDrvVolleyGame::ActCallSpikeReady()
{
    SPIKE_READY_WORK *pWork = static_cast<SPIKE_READY_WORK *>(EntryAct(ACT_SPIKE_READY));
    pWork->m_bQuickLot = 0;
}

// Runs to the place for the spike and waits for the toss; goes for a
// quick spike when the rate says so and the ball comes down close.
void CComDrvVolleyGame::ActLoopSpikeReady()
{
    SPIKE_READY_WORK *pWork = static_cast<SPIKE_READY_WORK *>(GetCurrentActWork());
    bcs::FVEC3 target;
    if (!pWork->m_bQuickLot && CanbeSpike() && m_nMyChrIdx == GetShoudQuickSpikeChrIdx() &&
        GetBallTargetPos(target) && GetDisXZ(target, m_BallPos) < 1.85f) {
        pWork->m_bQuickLot = 1;
        LotRateQuickSpike();
        if (m_nUnk1CF) {
            EntryAct(ACT_SPIKE_JUMP);
            pWork->m_nStep = 5;
            return;
        }
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetSpikeReadyPos(pWork->m_Target);
            GetSafePosFromGimmick(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Target, 0.4f, 1.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 4:
            ActCallLookBall();
            pWork->m_nStep++;
            break;
        case 5:
            pWork->m_nStep++;
            break;
        case 6:
            ActCallWait(120.0f);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallSpikeGo()
{
    SPIKE_GO_WORK *pWork = static_cast<SPIKE_GO_WORK *>(EntryAct(ACT_SPIKE_GO));
    pWork->m_nTargetKind = -1;
    pWork->m_bNoSpike = 0;
    if (m_pBall != 0 && m_pBall->GetTouchNum() >= 2) {
        long nRate = CComMan::GetDatI(0x286, GetComLvF());
        if (LotRate() < nRate) {
            pWork->m_bNoSpike = 1;
        }
    }
    if (IsBanSpike()) {
        pWork->m_bNoSpike = 1;
    }
}

// Runs to where the spike jump is to start and jumps when close enough.
void CComDrvVolleyGame::ActLoopSpikeGo()
{
    SPIKE_GO_WORK *pWork = static_cast<SPIKE_GO_WORK *>(GetCurrentActWork());
    int bJumpTarget = 0;
    if (!CanbeMyTouch()) {
        ReturnAct(0);
        return;
    }

    if (pWork->m_nStep >= 2) {
        long nOldKind = pWork->m_nTargetKind;
        int bFound = 1;
        if (!GetBallSpikeJumpTargetPos(pWork->m_Target)) {
            if (!GetBallTargetPos(pWork->m_Target)) {
                bFound = 0;
            } else {
                pWork->m_nTargetKind = 1;
            }
        } else {
            pWork->m_nTargetKind = 0;
            bJumpTarget = 1;
        }
        if (nOldKind != pWork->m_nTargetKind) {
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
        }
        if (pWork->m_nStep == 2) {
            if (bFound) {
                pWork->m_nStep = 3;
            }
        } else if (pWork->m_nStep >= 3 && !bFound) {
            ReturnAct(0);
            return;
        }
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(0.3f);
            pWork->m_nStep++;
            break;
        case 2:
            break;
        case 3: {
            float fDist;
            if (bJumpTarget) {
                fDist = GetWorldDisXZ(m_MyPos, pWork->m_Target);
            } else {
                fDist = GetDisXZ(m_MyWorldPos, m_BallWorldPos);
            }
            if (m_pChr->CanbeSpike(0) && fDist < 1.0f && !pWork->m_bNoSpike) {
                EntryAct(ACT_SPIKE_JUMP);
                pWork->m_nStep++;
                break;
            }
            MethdRunEx(pWork->m_Target, 0.5f, 1.0f, 600.0f, 6, 10.0f);
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

// Spikes, then moves the target marker where the spike is to go.
void CComDrvVolleyGame::ActLoopSpikeJump()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            if (MethdSpike()) {
                pWork->m_nStep++;
            }
            break;
        case 1:
            if (CanbeMoveTargetMarkerPos()) {
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (GetSpikeTargetMarkerPos(pWork->m_Target)) {
                pWork->m_nStep++;
            }
            break;
        case 3:
            if (MethdMoveTargetMarkerPos(pWork->m_Target, 0.2f)) {
                pWork->m_nStep++;
            }
            break;
        case 4:
            // Done: the action stays until the next one replaces it.
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Runs to the net to block and waits there.
void CComDrvVolleyGame::ActLoopBlockReady()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            if (!GetBlockReadyPos(pWork->m_Target)) {
                ReturnAct(0);
                break;
            }
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1: {
            int nRet = MethdRunEx(pWork->m_Target, 0.4f, 1.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 2:
            ActCallLookBall();
            pWork->m_nStep++;
            break;
        case 3:
            ActCallWait(120.0f);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallBlockGo()
{
    BLOCK_WORK *pWork = static_cast<BLOCK_WORK *>(EntryAct(ACT_BLOCK_GO));
    if (m_pChr->GetVolleyBlockJumpDelayFrame() >= 4.0f) {
        pWork->m_bLate = 1;
    } else {
        pWork->m_bLate = 0;
    }
    pWork->m_bFeint = 0;
    long nRate = CComMan::GetDatI(0xC6, GetComLvF());
    if (LotRate() < nRate) {
        pWork->m_bFeint = 1;
    }
}

// Jumps to block when the ball is about to come over close by (or, for a
// feint, when it does not), then follows the ball.
void CComDrvVolleyGame::ActLoopBlockGo()
{
    BLOCK_WORK *pWork = static_cast<BLOCK_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            if (!pWork->m_bFeint) {
                if (pWork->m_bLate) {
                    pWork->m_fJumpWait = 0.0f;
                } else {
                    pWork->m_fJumpWait = bcs::BCS_RandRange(0.0f, 3.0f);
                }
            } else {
                pWork->m_fJumpWait = bcs::BCS_RandRange(4.0f, 7.0f);
            }
            pWork->m_fTime = 0.0f;
            pWork->m_nStep++;
            break;
        case 1: {
            m_pBall->GetPosAfterFrame(pWork->m_Target, 6.0f);
            float fDistX = fabsf(pWork->m_Target.x - m_MyPos.x);
            float fDist = GetDisXZ(pWork->m_Target, m_MyPos);
            float fRangeX;
            float fRange;
            float fFeintRangeX;
            float fFeintRange;
            if (pWork->m_bLate) {
                fRangeX = 2.5f;
                fFeintRangeX = 3.0f;
                fRange = 6.0f;
                fFeintRange = 7.0f;
            } else {
                fRangeX = 1.8f;
                fFeintRangeX = 3.0f;
                fRange = fFeintRangeX;
                fFeintRange = 4.0f;
            }
            if (!pWork->m_bFeint) {
                if (fDistX < fRangeX && fDist < fRange && pWork->m_fTime >= pWork->m_fJumpWait && MethdJump()) {
                    pWork->m_nStep++;
                }
            } else {
                if ((fDistX > fFeintRangeX || fDist > fFeintRange) && pWork->m_fTime >= pWork->m_fJumpWait && MethdJump()) {
                    pWork->m_nStep++;
                }
            }
            break;
        }
        case 2:
            if (!pWork->m_bFeint) {
                MethdSetMoveRevise(m_BallPos);
            }
            break;
    }
}

void CComDrvVolleyGame::ActCallServe()
{
    EntryAct(ACT_SERVE);
}

// Goes to the serve position, serves and moves the target marker.
void CComDrvVolleyGame::ActLoopServe()
{
    SERVE_WORK *pWork = static_cast<SERVE_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            GetServePos(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 1: {
            int nRet = MethdRunEx(pWork->m_Target, 0.4f, 1.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 2:
            ActCallWait(20.0f);
            pWork->m_nStep++;
            break;
        case 3:
            if (MethdServe()) {
                pWork->m_nStep++;
            }
            break;
        case 4:
            if (CanbeMoveTargetMarkerPos()) {
                pWork->m_nStep++;
            }
            break;
        case 5:
            if (GetServeTargetMarkerPos(pWork->m_Marker)) {
                pWork->m_nStep++;
            }
            break;
        case 6:
            ActCallWait(10.0f);
            pWork->m_nStep++;
            break;
        case 7:
            if (MethdMoveTargetMarkerPos(pWork->m_Marker, 0.2f)) {
                pWork->m_nStep++;
            }
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallServeWait()
{
    EntryAct(ACT_SERVE_WAIT);
}

// Waits at its place while the other team serves.
void CComDrvVolleyGame::ActLoopServeWait()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetHomePos(pWork->m_Target, 1);
            GetSafePosFromGimmick(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Target, 0.3f, 2.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 4:
            ActCallLookFront();
            pWork->m_nStep++;
            break;
        case 5:
            ActCallWait(RandRangeF(150.0f, 220.0f));
            LotDefPosKind(0);
            pWork->m_nStep = 0;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallGoHome()
{
    EntryAct(ACT_GO_HOME);
}

// Runs to the home position and watches the ball.
void CComDrvVolleyGame::ActLoopGoHome()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetHomePos(pWork->m_Target, 1);
            GetSafePosFromGimmick(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Target, 0.4f, 2.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 4:
            ActCallLookBall();
            pWork->m_nStep++;
            break;
        case 5:
            ActCallWait(RandRangeF(30.0f, 40.0f));
            pWork->m_nStep = 0;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallDefHome()
{
    EntryAct(ACT_DEF_HOME);
}

// Runs to the defence position and watches the ball.
void CComDrvVolleyGame::ActLoopDefHome()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetDefHomePos(pWork->m_Target);
            GetSafePosFromGimmick(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Target, 0.4f, 2.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 4:
            ActCallLookBall();
            pWork->m_nStep++;
            break;
        case 5:
            ActCallWait(RandRangeF(30.0f, 40.0f));
            pWork->m_nStep = 0;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Runs to the place opposite the team mate and watches the ball.
void CComDrvVolleyGame::ActLoopSymmetry()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetSymmetryPos(pWork->m_Target);
            GetSafePosFromGimmick(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Target, 0.4f, 2.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 4:
            ActCallLookBall();
            pWork->m_nStep++;
            break;
        case 5:
            ActCallWait(RandRangeF(30.0f, 40.0f));
            pWork->m_nStep = 0;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Leaves the ball to a team mate: back to the defence position at once.
void CComDrvVolleyGame::ActLoopGoAway()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            GetDefHomePos(pWork->m_Target);
            GetSafePosFromGimmick(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 1: {
            int nRet = MethdRunEx(pWork->m_Target, 0.4f, 2.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 2:
            ActCallLookBall();
            pWork->m_nStep++;
            break;
        case 3:
            ActCallWait(RandRangeF(30.0f, 40.0f));
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallTargetMarkerGo()
{
    MARKER_WORK *pWork = static_cast<MARKER_WORK *>(EntryAct(ACT_TARGET_MARKER_GO));
    float fMin = CComMan::GetDatF(0xE6, GetComLvF());
    pWork->m_fInterval = RandRangeF(fMin, CComMan::GetDatF(0xEE, GetComLvF()));
    pWork->m_fTime = 1000.0f;
}

// Runs to where the other team's target marker points, looking at it again
// every m_fInterval frames.
void CComDrvVolleyGame::ActLoopTargetMarkerGo()
{
    MARKER_WORK *pWork = static_cast<MARKER_WORK *>(GetCurrentActWork());
    // The original compares the result and never uses it.
    CComMan::IsExistComRootActChr_WithoutONE(m_nMyTeam, ACT_TARGET_MARKER_GO, m_nMyChrIdx);

    if (pWork->m_fTime >= pWork->m_fInterval) {
        if (!GetEmTargetMarkerPos(pWork->m_Target)) {
            ReturnAct(0);
            return;
        }
        if (!IsSpMarkerMode() && !(m_pChr->GetStatusFlag() & 0x10000)) {
            // Keep inside the court, closer to the edges on higher levels.
            float fMarginX = 1.0f;
            float fMarginZ = fMarginX;
            if (GetComLvF() >= 5.0f) {
                fMarginX = 0.65f;
                fMarginZ = 0.4f;
            } else if (GetComLvF() >= 4.0f) {
                fMarginX = 0.7f;
                fMarginZ = 0.5f;
            }
            pWork->m_Target.x = CLAMP(pWork->m_Target.x, GetMyCourtParamLeft(fMarginX), GetMyCourtParamRight(fMarginX));
            pWork->m_Target.z = CLAMP(pWork->m_Target.z, GetMyCourtParamFront(fMarginZ), GetMyCourtParamBack(fMarginZ));
        }
        pWork->m_fTime = 0.0f;
    }

    if (IsExistChrTargetWithCondition(pWork->m_Target, 2.0f, GetMyTeamPadDrvCanTouchEnableBit())) {
        Reset();
        EntryAct(ACT_GO_AWAY);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1: {
            float fMin = CComMan::GetDatF(0xD6, GetComLvF());
            float fMax = CComMan::GetDatF(0xDE, GetComLvF());
            if (IsExistServeMode()) {
                if (fMin > 30.0f) {
                    fMin = 30.0f;
                }
                if (fMax > 40.0f) {
                    fMax = 40.0f;
                }
            }
            if (fMin > fMax) {
                fMin = fMax;
            }
            float fWait = RandRangeF(fMin, fMax);
            if (IsExistServeMode() && LotRate() < 80) {
                fWait *= 0.5f;
            }
            if (IsExistPadDrv()) {
                fWait *= 0.5f;
            }
            ActCallWait(fWait);
            pWork->m_nStep++;
            break;
        }
        case 2:
            MethdRunEx(pWork->m_Target, 0.4f, 1.0f, 600.0f, 6, 10.0f);
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Goes for the nearest "?" panel in the court, or on stage 2 the nearest
// falling green shell.
void CComDrvVolleyGame::ActCallGetQPanel()
{
    ITEM_WORK *pWork = static_cast<ITEM_WORK *>(EntryAct(ACT_GET_QPANEL));
    if (CGame::g_GameWork.GetStage() != 2) {
        pWork->m_nItem = CComMan::QPanel_GetNearInCourt(m_MyPos, m_nUnk98, 7);
    } else {
        pWork->m_nItem = CComMan::FallGShell_GetNearInCourt(m_MyPos, m_nUnk98);
    }
}

void CComDrvVolleyGame::ActLoopGetQPanel()
{
    ITEM_WORK *pWork = static_cast<ITEM_WORK *>(GetCurrentActWork());
    float fRange;
    if (CGame::g_GameWork.GetStage() != 2) {
        CQPanel *pPanel = CComMan::QPanel_GetPtr(pWork->m_nItem);
        if (pPanel == 0) {
            ReturnAct(0);
            return;
        }
        if (!pPanel->CanAccess()) {
            ReturnAct(0);
            return;
        }
        pWork->m_Target = CComMan::QPanel_GetPos(pWork->m_nItem);
        fRange = 0.4f;
    } else {
        if (IsHaveMyStockItem()) {
            ReturnAct(1);
            return;
        }
        if (CComMan::FallGShell_GetPtr(pWork->m_nItem) == 0) {
            ReturnAct(0);
            return;
        }
        pWork->m_Target = CComMan::FallGShell_GetPos(pWork->m_nItem);
        fRange = 0.2f;
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            MethdRunEx(pWork->m_Target, fRange, 1.0f, 600.0f, 2, 10.0f);
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallGetDropCoin()
{
    EntryAct(ACT_GET_DROP_COIN);
}

// Runs to the coin GetShoudGetDropCoin chose while it is there.
void CComDrvVolleyGame::ActLoopGetDropCoin()
{
    ITEM_WORK *pWork = static_cast<ITEM_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(0.5f);
            pWork->m_nStep++;
            break;
        case 2:
            pWork->m_nItem = GetShoudGetDropCoin();
            if (pWork->m_nItem == 0) {
                ReturnAct(0);
                break;
            }
            pWork->m_nStep++;
            break;
        case 3: {
            CDropCoin *pCoin = CComMan::DropCoin_GetPtr(pWork->m_nItem);
            if (pCoin == 0) {
                ReturnAct(1);
                break;
            }
            if (!pCoin->CanbePickup()) {
                ReturnAct(1);
                break;
            }
            pWork->m_Target = CComMan::DropCoin_GetPos(pCoin);
            if (MethdRunEx(pWork->m_Target, 0.3f, 1.0f, 600.0f, 7, 10.0f)) {
                pWork->m_nStep = 0;
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallItemShoot()
{
    EntryAct(ACT_ITEM_SHOOT);
}

// Runs to the place to throw the held item from and throws it.
void CComDrvVolleyGame::ActLoopItemShoot()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            ActCallBaseWait(1.0f);
            pWork->m_nStep++;
            break;
        case 2:
            GetItemShootPos(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 3: {
            int nRet = MethdRunEx(pWork->m_Target, 0.8f, 1.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                bcs::FVEC3 target;
                GetItemShootTarget(target);
                if (!IsExistChrToThrowItemTarget(target)) {
                    MethdItemShoot(target);
                }
                pWork->m_nStep++;
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallInterThrowItem()
{
    EntryAct(ACT_INTER_THROW_ITEM);
}

// Throws the held item at once.
void CComDrvVolleyGame::ActLoopInterThrowItem()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            bcs::FVEC3 target;
            GetItemShootTarget(target);
            if (!IsExistChrToThrowItemTarget(target)) {
                MethdItemShoot(target);
            }
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyGame::ActCallEscGimmick()
{
    EntryAct(ACT_ESC_GIMMICK);
}

// Runs away from a stage gimmick and waits a moment.
void CComDrvVolleyGame::ActLoopEscGimmick()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            if (GetEscPosFromGimmick(pWork->m_Target)) {
                m_bGimmick = 0;
                SetAlarm(1, 60.0f);
                m_nFlag &= ~4;
                m_fUnk54 = 0.0f;
                pWork->m_nStep++;
            } else {
                ReturnAct(0);
            }
            break;
        case 1: {
            int nRet = MethdRunEx(pWork->m_Target, 0.3f, 1.0f, 600.0f, 7, 10.0f);
            if (nRet == 2 || nRet == 3) {
                pWork->m_nStep++;
            }
            break;
        }
        case 2:
            ActCallWait(RandRange(15.0f, 30.0f));
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Waits a random time from the level's reaction table, scaled by fRate
// (longer while the team has two characters and no human).
void CComDrvVolleyGame::ActCallBaseWait(float fRate)
{
    float fMin = CComMan::GetDatF(9, GetComLvF());
    float fMax = CComMan::GetDatF(0x11, GetComLvF());
    if (!IsExistPadDrv() && m_nMateNum == 2) {
        fMax += 10.0f;
    }
    long nMin = fMin * fRate;
    long nMax = fMax * fRate;
    if (nMin > nMax) {
        nMax = nMin;
    }
    ActCallWait(RandRangeI(nMin, nMax));
}

// Picks the defence action after the other team's nTouch-th touch: block,
// throw an item, go for a panel, shell or coin, or go to the defence place.
void CComDrvVolleyGame::ActChangeGoDef(long nTouch)
{
    // Not while running from a gimmick or going for an item.
    if (GetRootActType() == ACT_ESC_GIMMICK) {
        return;
    }
    if (GetRootActType() == ACT_GET_QPANEL) {
        return;
    }
    if (GetRootActType() == ACT_ITEM_SHOOT) {
        return;
    }
    if (GetRootActType() == ACT_GET_DROP_COIN) {
        return;
    }

    if (ShoudBlockReady() && LotRateBlock(nTouch)) {
        Reset();
        EntryAct(ACT_BLOCK_READY);
        return;
    }

    if (nTouch >= 2) {
        // Late in the rally: only what is worth a detour.
        if (GetMyStockItemType() >= 0 && LotRateItemThrowTimingBad()) {
            Reset();
            ActCallItemShoot();
            return;
        }
        if (IsShoudGetQPanel() && GetNearQPanelDis() > 3.0f && LotRateGetQPanelBad()) {
            Reset();
            ActCallGetQPanel();
            return;
        }
        if (IsShoudGetFallGShell() && GetNearFallGShellDis() > 3.0f && LotRateGetQPanelBad()) {
            Reset();
            ActCallGetQPanel();
            return;
        }
        if (IsShoudGetDropCoin() && GetNearDropCoinDis() > 3.0f && LotRateGetDropCoinBad()) {
            Reset();
            ActCallGetDropCoin();
            return;
        }
        if ((unsigned char)m_nDefPosKind <= 1) {
            Reset();
            ActCallDefHome();
            return;
        }
        Reset();
        EntryAct(ACT_SYMMETRY);
        return;
    }

    if (GetMyStockItemType() >= 0 && LotRateItemThrowTimingGood()) {
        Reset();
        ActCallItemShoot();
        return;
    }
    if (IsShoudGetQPanel()) {
        float fDist = GetNearQPanelDis();
        if (fDist >= 0.0f && fDist < 3.0f && LotRateGetQPanelGood()) {
            Reset();
            ActCallGetQPanel();
            return;
        }
    }
    if (IsShoudGetFallGShell()) {
        float fDist = GetNearFallGShellDis();
        if (fDist >= 0.0f && fDist < 3.0f && LotRateGetQPanelGood()) {
            Reset();
            ActCallGetQPanel();
            return;
        }
    }
    if (IsShoudGetDropCoin()) {
        float fDist = GetNearDropCoinDis();
        if (fDist >= 0.0f && fDist < 3.0f && LotRateGetDropCoinGood()) {
            Reset();
            ActCallGetDropCoin();
            return;
        }
    }
    Reset();
    ActCallDefHome();
}
}
