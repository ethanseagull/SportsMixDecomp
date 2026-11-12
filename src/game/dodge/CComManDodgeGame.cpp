// msw::CComManDodgeGame: the computer players' manager of the dodgeball
// game. Static helpers the computer drivers ask about the outfield
// characters, the ball holder, the special shot's target markers and the
// shape of the outfield round the other team's court.
#include "game/msw/CComManDodgeGame.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCoreDodge.hpp"
#include "game/msw/CChrManDodge.hpp"
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CStageMan.hpp"

// 60 degrees in radians, set by the file's static initialiser and used by
// nothing in the file (as in game/dodge/unit_D8380.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    // The character states that add to the handicap distance, and the
    // computer parameter read for it (meanings not known).
    enum {
        STATE_HANDICAP0 = 6,
        STATE_HANDICAP1 = 7,
        STATE_HANDICAP2 = 0x3A,
        STATE_HANDICAP3 = 0x3D
    };
    enum {
        DAT_HANDICAP_DIS = 0xA9
    };

    inline float MinF(float a, float b)
    {
        if (a < b) {
            b = a;
        }
        return b;
    }

    inline float MaxF(float a, float b)
    {
        if (a > b) {
            b = a;
        }
        return b;
    }

    // v clamped to [lo, hi]; a macro, the original calls the edges again
    // for the value it returns (as in game/dodge/unit_D8380.cpp).
#define OPP_CLAMP(v, lo, hi) ((v) > (hi) ? (hi) : ((v) < (lo) ? (lo) : (v)))

    // rOut = rIn in the XZ plane scaled to length 1; rVec made fLen long in
    // the XZ plane, its height kept (as in game/dodge/unit_D8380.cpp).
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::FVEC3 xz(rIn.x, 0.0f, rIn.z);
        bcs::BCS_Vec_Normalize(rOut, xz);
    }

    inline void SetLenXZ(bcs::FVEC3 &rVec, float fLen)
    {
        float y = rVec.y;
        NormalizeXZ(rVec, rVec);
        rVec.x *= fLen;
        rVec.z *= fLen;
        rVec.y = y;
    }

    // (name guessed) The far end of the outfield of the team opposite
    // nTeam: its front for team 0, its back for team 1.
    inline float GetOppOuterFieldEnd(long nTeam)
    {
        msw::CStageMan *pStage = msw::CStageMan::GetInstance();
        if (nTeam == 0) {
            return pStage->GetTeamOuterFieldFront(1 - nTeam, 1);
        }
        return pStage->GetTeamOuterFieldBack(1 - nTeam, 1);
    }
}

namespace msw {

    long CComManDodgeGame::GetAssignOuterChrCoreIdx(long nTeam)
    {
        CChrCore *pChr = CChrManDodge::GetExclusiveOuterChrCore(nTeam);
        if (pChr != 0) {
            CComDrvDodgeGame *pDrv = static_cast<CComDrvDodgeGame *>(GetComDrv(pChr));
            if (pDrv != 0) {
                return pDrv->GetAssignOuterChrCoreIdx();
            }
        }
        return -1;
    }

    CChrCore *CComManDodgeGame::GetAssignOuterChrCore(long nTeam)
    {
        CChrCore *pChr = CChrManDodge::GetExclusiveOuterChrCore(nTeam);
        if (pChr != 0) {
            CComDrvDodgeGame *pDrv = static_cast<CComDrvDodgeGame *>(GetComDrv(pChr));
            if (pDrv != 0) {
                return pDrv->GetAssignOuterChrCore();
            }
        }
        return 0;
    }

    CChrCore *CComManDodgeGame::GetInnerChrCoreRand(long nTeam, int bPadOnly, int bAll)
    {
        long aChr[8];
        long *pChrIdx = aChr;
        long nNum = 0;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }
            if (nTeam != pChr->GetTeamNo()) {
                continue;
            }
            if (bPadOnly && !pChr->IsPadCtrl()) {
                continue;
            }
            if (!bAll && pChr->GetUnk17B() == 1) {
                continue;
            }
            if (!pChr->IsDodgeInfield()) {
                continue;
            }
            *pChrIdx++ = i;
            nNum++;
        }
        if (nNum <= 0) {
            return 0;
        }
        return CChrMan::GetChrCore(aChr[bcs::BCS_RandMax(nNum - 1)]);
    }

    int CComManDodgeGame::IsBallOffensive(CChrCore *pChr, long nBall)
    {
        CBall *pBall = CBallMan::GetBallPtr(nBall);
        if (pBall == 0) {
            return 0;
        }
        if (!pBall->IsActive()) {
            return 0;
        }
        if (!pBall->IsOffensive()) {
            return 0;
        }
        return pBall->GetShotTeamIdx() != pChr->GetTeamNo();
    }

    int CComManDodgeGame::IsInnerTeamHoldBall(long nTeam, long nBall)
    {
        CChrCore *pChr = CChrMan::Util_SearchHoldBallChr(nBall);
        if (pChr == 0) {
            return 0;
        }
        if (nTeam != pChr->GetTeamNo()) {
            return 0;
        }
        return pChr->IsDodgeInfield() != 0;
    }

    int CComManDodgeGame::IsOuterTeamHoldBall(long nTeam, long nBall)
    {
        CChrCore *pChr = CChrMan::Util_SearchHoldBallChr(nBall);
        if (pChr == 0) {
            return 0;
        }
        if (nTeam != pChr->GetTeamNo()) {
            return 0;
        }
        return !pChr->IsDodgeInfield();
    }

    int CComManDodgeGame::GetOuterCrossHoseiPos(bcs::FVEC3 &rPos, long nTeam)
    {
        float fLeft = GetStageInOuterLeft(nTeam);
        float fRight = GetStageInOuterRight(nTeam);
        float fFront = GetStageInOuterFront(nTeam);
        float fBack = GetStageInOuterBack(nTeam);
        float fOuterFront = GetStageOuterFront(nTeam);
        float fOuterBack = GetStageOuterBack(nTeam);
        if (nTeam == 1) {
            if (fOuterBack > fBack) {
                fBack = fOuterBack;
            }
        } else {
            if (fOuterFront < fFront) {
                fFront = fOuterFront;
            }
        }

        // How far into the outfield the position is moved.
        float fMargin = 0.001f;
        float fLeftIn = fLeft - fMargin;
        float fRightIn = fRight + fMargin;
        long nSide0 = bcs::BCS_Util_IsPointSide(rPos.x, rPos.z, fLeft, fFront, fRight, fBack);
        long nSide1 = bcs::BCS_Util_IsPointSide(rPos.x, rPos.z, fRight, fFront, fLeft, fBack);
        if (nTeam == 1) {
            float fFrontIn = fFront - fMargin;
            float fBackIn = fOuterBack - fMargin;
            if (nSide0 >= 0 && nSide1 >= 0) {
                rPos.x = MinF(fLeftIn, rPos.x);
                rPos.z = MinF(fBackIn, rPos.z);
            } else if (nSide0 <= 0 && nSide1 <= 0) {
                rPos.x = MaxF(fRightIn, rPos.x);
                rPos.z = MinF(fBackIn, rPos.z);
            } else if (nSide0 <= 0 && nSide1 >= 0) {
                rPos.z = MinF(fFrontIn, rPos.z);
            } else {
                if (rPos.x <= 0.0f) {
                    rPos.x = MinF(fLeftIn, rPos.x);
                } else {
                    rPos.x = MaxF(fRightIn, rPos.x);
                }
                rPos.z = MinF(fBackIn, rPos.z);
            }
        } else {
            float fFrontIn = fOuterFront + fMargin;
            float fBackIn = fBack + fMargin;
            if (nSide0 >= 0 && nSide1 >= 0) {
                rPos.x = MinF(fLeftIn, rPos.x);
                rPos.z = MaxF(fFrontIn, rPos.z);
            } else if (nSide0 <= 0 && nSide1 <= 0) {
                rPos.x = MaxF(fRightIn, rPos.x);
                rPos.z = MaxF(fFrontIn, rPos.z);
            } else if (nSide0 >= 0 && nSide1 <= 0) {
                rPos.z = MaxF(fBackIn, rPos.z);
            } else {
                if (rPos.x <= 0.0f) {
                    rPos.x = MinF(fLeftIn, rPos.x);
                } else {
                    rPos.x = MaxF(fRightIn, rPos.x);
                }
                rPos.z = MaxF(fFrontIn, rPos.z);
            }
        }
        return 1;
    }

    int CComManDodgeGame::GetBallOppositePos(bcs::FVEC3 &rPos, int bTarget, long nBall)
    {
        rPos.Set(0.0f, 0.0f, 0.0f);
        CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(nBall);
        if (pHold == 0) {
            return 0;
        }
        if (!pHold->IsDodgeInfield()) {
            return 0;
        }
        long nTarget = pHold->GetDodgeLockTargetChrIdx();
        if (nTarget < 0) {
            return 0;
        }
        CChrCore *pTarget = CChrMan::GetChrCore(nTarget);
        if (pTarget == 0) {
            return 0;
        }

        bcs::FVEC3 ball = GetBallWorldPos(nBall);
        bcs::FVEC3 chr = GetChrWorldPos(pTarget);
        long nCourt = GetTerritoryCourtIdx(pTarget);
        ball.y = 0.0f;
        chr.y = 0.0f;
        bcs::FVEC3 target = chr;
        if (bTarget && !static_cast<CChrCoreDodge *>(pHold)->GetOuterTargetPos(target)) {
            return 0;
        }

        // The target is kept inside its court, the ball inside the other.
        target.x = OPP_CLAMP(target.x, GetStageCourtWorldLeft(nCourt), GetStageCourtWorldRight(nCourt));
        target.z = OPP_CLAMP(target.z, GetStageCourtWorldFront(nCourt), GetStageCourtWorldBack(nCourt));
        long nOther = 1 - nCourt;
        ball.x = OPP_CLAMP(ball.x, GetStageCourtWorldLeft(nOther), GetStageCourtWorldRight(nOther));
        ball.z = OPP_CLAMP(ball.z, GetStageCourtWorldFront(nOther), GetStageCourtWorldBack(nOther));

        bcs::FVEC3 dir(target.x - ball.x, 0.0f, target.z - ball.z);
        return GetBallOppositePos2(rPos, ball, dir, nCourt);
    }

    int CComManDodgeGame::GetBallOppositePos2(bcs::FVEC3 &rPos, const bcs::FVEC3 &rBall, const bcs::FVEC3 &rDir,
                                              long nTeam)
    {
        bcs::FVEC3 far;
        bcs::FVEC3 cross;
        bcs::FVEC3 pos = rBall;
        bcs::FVEC3 dir = rDir;
        pos.y = 0.0f;
        dir.y = 0.0f;
        int bFound = 0;
        rPos = World2Local(pos, -1);
        far.Set(0.0f, 0.0f, 0.0f);
        cross.Set(0.0f, 0.0f, 0.0f);

        // A ball still in the outfield beyond the other court: nothing to do.
        float fBorderZ = GetCourtBorderWorldZ(pos);
        int bOwnSide = 0;
        if (nTeam == 1 && pos.z >= fBorderZ) {
            bOwnSide = 1;
        }
        if (nTeam == 0 && pos.z <= fBorderZ) {
            bOwnSide = 1;
        }
        if (!bOwnSide) {
            if (pos.x <= GetStageInOuterLeft(nTeam)) {
                return 1;
            }
            if (pos.x >= GetStageInOuterRight(nTeam)) {
                return 1;
            }
            if (nTeam == 1 && pos.z <= GetOppOuterFieldEnd(nTeam)) {
                return 1;
            }
            if (nTeam == 0 && pos.z >= GetOppOuterFieldEnd(nTeam)) {
                return 1;
            }
        }

        pos.x = OPP_CLAMP(pos.x, GetStageWorldLeft(), GetStageWorldRight());
        pos.z = OPP_CLAMP(pos.z, GetStageWorldFront(), GetStageWorldBack());

        // The three inner edges of the outfield round the other court.
        float fMargin = 0.1f;
        float fLeft = GetStageInOuterLeft(nTeam) - fMargin;
        float fRight = fMargin + GetStageInOuterRight(nTeam);
        float fFront = GetStageInOuterFront(nTeam) - fMargin;
        float fBack = fMargin + GetStageInOuterBack(nTeam);
        bcs::FVEC3 aEdge[4] = {
            bcs::FVEC3(0.0f, 0.0f, 0.0f),
            bcs::FVEC3(0.0f, 0.0f, 0.0f),
            bcs::FVEC3(0.0f, 0.0f, 0.0f),
            bcs::FVEC3(0.0f, 0.0f, 0.0f),
        };
        if (nTeam == 1) {
            aEdge[0].x = fLeft;
            aEdge[0].z = fBack;
            aEdge[1].x = fLeft;
            aEdge[1].z = fFront;
            aEdge[2].x = fRight;
            aEdge[2].z = fFront;
            aEdge[3].x = fRight;
            aEdge[3].z = fBack;
        } else {
            aEdge[0].x = fLeft;
            aEdge[0].z = fFront;
            aEdge[1].x = fLeft;
            aEdge[1].z = fBack;
            aEdge[2].x = fRight;
            aEdge[2].z = fBack;
            aEdge[3].x = fRight;
            aEdge[3].z = fFront;
        }

        SetLenXZ(dir, 50.0f);
        far.x = pos.x + dir.x;
        far.z = pos.z + dir.z;
        SetLenXZ(dir, 1.0f);

        for (long i = 0; i < 3; i++) {
            if (bcs::BCS_GetSegLineToSegLineCross(&cross.x, &cross.z, pos.x, pos.z, far.x, far.z, aEdge[i].x,
                                                  aEdge[i].z, aEdge[i + 1].x, aEdge[i + 1].z)) {
                cross.x += dir.x;
                cross.z += dir.z;
                rPos = World2Local(cross, nTeam);
                GetOuterCrossHoseiPos(rPos, nTeam);
                bFound = 1;
                break;
            }
        }
        return bFound;
    }

    int CComManDodgeGame::IsBallChargeMax(const CChrCore *pChr)
    {
        const CChrCoreDodge *pDodge = static_cast<const CChrCoreDodge *>(pChr);
        return pDodge->GetStamina() >= pDodge->GetMaxStamina();
    }

    float CComManDodgeGame::GetConditionHandicapDis(const CChrCore *pChr)
    {
        float fDis = 0.0f;
        if (pChr == 0) {
            return fDis;
        }

        long nState = pChr->GetState();
        if (nState == STATE_HANDICAP0 || nState == STATE_HANDICAP3) {
            fDis += 8.0f;
        } else if (nState == STATE_HANDICAP1) {
            fDis += 8.0f;
        } else if (nState == STATE_HANDICAP2) {
            fDis += 1.5f;
        }
        fDis += GetDatF(DAT_HANDICAP_DIS, pChr->GetComLevelWork());
        return fDis;
    }

    float CComManDodgeGame::GetStageOuterDepth(long nTeam)
    {
        CStageMan *pStage = CStageMan::GetInstance();
        float fLeft;
        float fRight;
        float fFront;
        float fBack;
        if (nTeam == 0) {
            pStage->GetTeamOuterFieldRect(1, &fLeft, &fRight, &fFront, &fBack, 0);
            return __fabsf(fFront);
        }
        pStage->GetTeamOuterFieldRect(0, &fLeft, &fRight, &fFront, &fBack, 0);
        return __fabsf(fBack);
    }

    float CComManDodgeGame::GetStageInOuterLeft(long nTeam)
    {
        return CStageMan::GetInstance()->GetTeamOuterFieldRight(1 - nTeam, 3);
    }

    float CComManDodgeGame::GetStageInOuterRight(long nTeam)
    {
        return CStageMan::GetInstance()->GetTeamOuterFieldLeft(1 - nTeam, 2);
    }

    float CComManDodgeGame::GetStageInOuterFront(long nTeam)
    {
        if (nTeam == 0) {
            return GetStageCourtWorldFront(nTeam);
        }
        return GetOppOuterFieldEnd(nTeam);
    }

    float CComManDodgeGame::GetStageInOuterBack(long nTeam)
    {
        if (nTeam == 0) {
            return GetOppOuterFieldEnd(nTeam);
        }
        return GetStageCourtWorldBack(nTeam);
    }

    int CComManDodgeGame::GetSpTargetMarkerPos(CChrCore *pSp, bcs::FVEC3 &rPos, long nMarker)
    {
        if (!pSp->GetDodgeArtsTargetMarkerPos(rPos, nMarker)) {
            return 0;
        }
        rPos = World2Local(rPos, -1);
        return 1;
    }

    int CComManDodgeGame::GetSpTargetMarkerNearPos(bcs::FVEC3 &rPos, CChrCore *pSp, const CChrCore *pChr)
    {
        bcs::FVEC3 chr = GetChrPos(pChr);
        long nNum = pSp->GetDodgeArtsTargetMarkerNum();
        float fMinDis = -1.0f;
        for (long i = 0; i < nNum; i++) {
            bcs::FVEC3 marker;
            if (!GetSpTargetMarkerPos(pSp, marker, i)) {
                continue;
            }
            float fDis = GetWorldDisXZ(chr, marker);
            if (fMinDis < 0.0f || fMinDis > fDis) {
                fMinDis = fDis;
                rPos = marker;
            }
        }
        return fMinDis >= 0.0f;
    }

    // The characters 5, 7, 10, 15 and 17 (which ones is not known) cannot
    // roll past an item ball; the same characters reflect a shot with a roll.
    int CComManDodgeGame::CanAvoidDodgeRoll_ItemBall(const CChrCore *pChr)
    {
        switch (pChr->GetChrTypeID()) {
            case 5:
            case 7:
            case 10:
            case 15:
            case 17:
                return 0;
        }
        return 1;
    }

    int CComManDodgeGame::IsReflectDodgeRoll(const CChrCore *pChr)
    {
        switch (pChr->GetChrTypeID()) {
            case 5:
            case 7:
            case 10:
            case 15:
            case 17:
                return 1;
        }
        return 0;
    }
}
