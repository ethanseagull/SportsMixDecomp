// msw::CChrManDodge: the character manager of the dodgeball game, and
// msw::CChrManDodgeDodgeOrDie, the one of its party game.
// Started from src/game/boss/CChrManBoss.cpp; the rest is Dodge's own.
#include "game/msw/CChrManDodge.hpp"

#include <math.h>

#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCoreDodge.hpp"
#include "game/msw/CChrDrvPadDodge.hpp"
#include "game/msw/CChrMarkerDodge.hpp"
#include "game/msw/CChrTargetMarkerDodge.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/triangular.h"

// The file's static initialiser sets this float, which nothing in the file
// uses: about 60 files of the game modules start their .bss with it, so it
// most likely comes from a shared header (see game/dodge/CChrMarkerDodge.cpp,
// which has the same guess).
namespace {
    inline float DegToRad(float fDeg)
    {
        return fDeg * (3.1415927f / 180.0f);
    }

    float s_fAngle60 = DegToRad(60.0f);

    // (name guessed) The length of v in the XZ plane.
    inline float GetLengthXZ(const bcs::FVEC3 &v)
    {
        bcs::FVEC3 xz(v.x, 0.0f, v.z);
        return bcs::FVEC3Len(&xz);
    }

    // rOut = r1 - r2 and rOut = r1 + r2. Stand-ins (a guess, as in
    // game/hockey/CChrManHockey.cpp) for vector operators of the original:
    // called one inline level deeper, the paired-single routines get their
    // float registers numbered as in the original.
    inline void VecSub(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Sub(&rOut, &r1, &r2);
    }
    inline void VecAdd(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Add(&rOut, &r1, &r2);
    }

    // The difference of two yaws, folded into [0, pi].
    inline float GetRotYDiff(float fRot, float fBase)
    {
        float fDiff = fRot - fBase;

        while (fDiff > 6.2831855f) {
            fDiff -= 6.2831855f;
        }
        while (fDiff < 0.0f) {
            fDiff += 6.2831855f;
        }
        if (fDiff > 3.1415927f) {
            fDiff -= 6.2831855f;
        }
        return fabsf(fDiff);
    }
}

namespace msw {

CChrManDodge::~CChrManDodge()
{
    if (m_ppChrCore != 0) {
        for (long i = 0; i < m_nChrCoreNum; i++) {
            if (m_ppChrCore[i] != 0) {
                delete m_ppChrCore[i];
            }
            m_ppChrCore[i] = 0;
        }
    }
}

CChrMan *CChrManDodge::CreateInstance(bcs::CTask *pParent)
{
    SetInstance_(new (0) CChrManDodge(pParent));
    return GetInstance_();
}

void CChrManDodge::Create_()
{
    CreateObj_();
    CreateDriver_();
    CreateMarker_();

    for (long i = 0; i < 2; i++) {
        SetTeamLeader(i, i, 0);
    }
}

void CChrManDodge::CreateObj_()
{
    m_nChrCoreNum = 8;

    for (long i = 0; i < m_nChrCoreNum; i++) {
        long nType = CGame::GetCharType(i);
        long nColor = CGame::GetCharColor(i);
        CChrCore *pChr;

        switch (nType) {
            case 0:  pChr = new (0) CChrCoreDodgeMario; break;
            case 1:  pChr = new (0) CChrCoreDodgeLuigi; break;
            case 2:  pChr = new (0) CChrCoreDodgePeach; break;
            case 3:  pChr = new (0) CChrCoreDodgeDaisy; break;
            case 4:  pChr = new (0) CChrCoreDodgeYoshi; break;
            case 5:  pChr = new (0) CChrCoreDodgeWario; break;
            case 6:  pChr = new (0) CChrCoreDodgeWaluigi; break;
            case 7:  pChr = new (0) CChrCoreDodgeDonkey; break;
            case 8:  pChr = new (0) CChrCoreDodgeDiddy; break;
            case 9:  pChr = new (0) CChrCoreDodgeKinopio; break;
            case 10: pChr = new (0) CChrCoreDodgeKoopa; break;
            case 11: pChr = new (0) CChrCoreDodgeKoopaJr; break;
            case 12: pChr = new (0) CChrCoreDodgeMoogle; break;
            case 13: pChr = new (0) CChrCoreDodgeSaboten; break;
            case 14: pChr = new (0) CChrCoreDodgeNinja; break;
            case 15: pChr = new (0) CChrCoreDodgeSiroma; break;
            case 16: pChr = new (0) CChrCoreDodgeSlime; break;
            case 17: pChr = new (0) CChrCoreDodgeKuroma; break;
            case 19:
            case 21:
            case 23:
                pChr = new (0) CChrCoreDodgeMii00;
                break;
            case 20:
            case 22:
            case 24:
                pChr = new (0) CChrCoreDodgeMii01;
                break;
            default: pChr = new (0) CChrCoreDodge; break;
        }
        m_ppChrCore[i] = pChr;

        // A negative type is an empty slot: the character stays out of
        // the game.
        if (nType < 0) {
            pChr->SetDisable();
        } else {
            pChr->Create(m_pChrTask, i, nType, nColor);
        }
    }
}

void CChrManDodge::CreateDriver_()
{
    long padId[8];
    long camId[8];
    long drvId[8];

    SetPadIdAssign_(8, padId);
    SetCamIdAssign_(8, camId);
    SetDrvIdAssign_(8, drvId);

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }

        pChr->DriverSetEntry(1, new (0) CComDrvDodgeGame);
        pChr->DriverSetEntry(0, new (0) CChrDrvPadDodge);
        pChr->SetTeamNo(i & 1);
        pChr->SetPadID(padId[i]);
        pChr->SetCamNo(camId[i]);
        // Driver kind 1 is a player: the pad driver (slot 0).
        pChr->DriverConnect(drvId[i] != 1, 0, 0, 0, 0);
        if (i < 2) {
            pChr->OnStatusFlag(0x100);
        }
    }
}

void CChrManDodge::ReconnectDefaultDriver()
{
    CChrManDodge *pMan = static_cast<CChrManDodge *>(GetInstance_());
    if (pMan != 0) {
        pMan->ReconnectDefaultDriver_();
    }
}

void CChrManDodge::ReconnectDefaultDriver_()
{
    long drvId[8];

    SetDrvIdAssign_(8, drvId);

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }

        pChr->DriverConnect(drvId[i] != 1, 0, 0, 0, 0);
        if (i < 2) {
            pChr->OnStatusFlag(0x100);
        }
    }
}

void CChrManDodge::CreateMarker_()
{
    long i;

    for (i = 0; i < m_nChrCoreNum; i++) {
        if (!m_ppChrCore[i]->IsDisable()) {
            CreateMarkerOne_(i);
        }
    }
    for (i = 0; i < m_nChrCoreNum; i++) {
        if (!m_ppChrCore[i]->IsDisable()) {
            new (1) CChrTargetMarkerDodge(this, i, 0);
        }
    }
    for (i = 0; i < m_nChrCoreNum; i++) {
        if (!m_ppChrCore[i]->IsDisable()) {
            new (0x100) CChrArtsTargetMarkerDodge(this, i, 1);
        }
    }
}

void CChrManDodge::CreateMarkerOne_(long nChr)
{
    new (0) CChrMarkerDodge(this, nChr, 2, -1);
    new (0) CChrMarkerDodgeCloseRange(this, nChr, -1);
    new (0x100) CChrMarkerDodgeArrow(this, nChr, -1);

    CChrCore *pChr = GetChrCore(nChr);
    if (pChr != 0 && IsNeedSubComChrMarker_(pChr->GetTeamNo())) {
        new (0) CChrMarkerDodge(this, nChr, 3, -1);
        new (0) CChrMarkerDodgeCloseRange(this, nChr, -1);
    }
}

void CChrManDodge::AllReset_()
{
}

void CChrManDodge::InitLeader_(int bConnect)
{
    if (m_ppChrCore == 0) {
        return;
    }

    for (long i = 0; i < 2; i++) {
        SetTeamLeader(i, i, 0);
    }

    if (!bConnect) {
        return;
    }

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable()) {
            pChr->DriverConnect(pChr->GetDriverIdx(), 0, 0, 0, 0);
        }
    }
}

void CChrManDodge::DispFunc()
{
}

CChrCore *CChrManDodge::Util_SearchChgLeader_(long nBall, long nTeam, CChrCore *pExclude)
{
    CBall *pBall = CBallMan::GetBallPtr(nBall);
    if (pBall == 0 || !pBall->IsActive()) {
        return 0;
    }

    if ((CGame::g_GameWork.m_nFlag18 & 1) || pBall->IsShootMode()) {
        return Util_SearchNearBall_(nBall, nTeam, 1, pExclude);
    }

    // While a pass flies, the character nearest to its receiver.
    if (pBall->IsPassMode()) {
        CChrCore *pPasser = GetChrCore_(pBall->GetShotChrIdx());
        if (pPasser->GetPassCheckChr() != 0) {
            return Util_SearchNearChar_(pPasser->GetPassCheckChr()->GetPlayerNo(), nTeam, 0, pExclude);
        }
        return Util_SearchNearBall_(nBall, nTeam, 0, pExclude);
    }

    if (nTeam >= 0 && CGame::IsPlayTypeCanChangeLeader(nTeam)) {
        for (long i = 0; i < m_nChrCoreNum; i++) {
            CChrCore *pChr = m_ppChrCore[i];
            if (!pChr->IsDisable() && pChr != pExclude && nTeam == pChr->GetTeamNo() &&
                (pChr->GetStatusFlag() & 1) && !(pChr->GetStatusFlag() & 0x100)) {
                return pChr;
            }
        }
    }

    // Otherwise the character nearest to the ball among those between the
    // ball and the team's own goal, within 65 degrees of the line to it.
    CChrCore *pBest = 0;
    float fBestDist;
    const bcs::FVEC3 &ballPos = pBall->GetPos();
    const bcs::FVEC3 &goalPos = CGame::GetGoalPos(CGame::GetGuardGoalIdx(nTeam));
    bcs::FVEC3 toGoal;
    toGoal.x = goalPos.x - ballPos.x;
    toGoal.z = goalPos.z - ballPos.z;
    float fGoalRotY = nw4r::math::Atan2Rad(toGoal.x, toGoal.z);
    float fGoalDist = GetLengthXZ(toGoal);

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable() || pChr == pExclude) {
            continue;
        }
        if (nTeam >= 0 && nTeam != pChr->GetTeamNo()) {
            continue;
        }
        if (!pChr->IsInnerXZ(goalPos, fGoalDist)) {
            continue;
        }

        float fDiff = pChr->GetRotY_FromPosXZ(ballPos) - fGoalRotY;
        while (fDiff > 6.2831855f) {
            fDiff -= 6.2831855f;
        }
        while (fDiff < 0.0f) {
            fDiff += 6.2831855f;
        }
        if (fDiff >= 3.1415927f) {
            fDiff -= 6.2831855f;
        }
        if (fabsf(fDiff) >= 1.134464f) {
            continue;
        }

        if (pBest == 0) {
            pBest = pChr;
            fBestDist = pChr->GetDist_ToPosXZ(ballPos);
        } else if (pChr->IsInnerXZ(ballPos, fBestDist)) {
            pBest = pChr;
            fBestDist = pChr->GetDist_ToPosXZ(ballPos);
        }
    }

    return pBest != 0 ? pBest : Util_SearchNearOwnGoal_(nTeam, 1, pExclude);
}

CChrCore *CChrManDodge::Util_SearchNearOwnGoal_(long nTeam, int nArg, CChrCore *pExclude)
{
    return Util_SearchNearPos_(CGame::GetGoalPos(1 - CGame::GetGoalIdx(nTeam)), nTeam, nArg, pExclude);
}

CChrCore *CChrManDodge::Util_SearchPassTo_(long nChr, float fRotY)
{
    float fMin = 12.566371f;
    CChrCore *pRet = 0;
    CChrCore *pMe = GetChrCore_(nChr);
    long nTeam = pMe->GetTeamNo();

    // The team mate nearest to the direction fRotY.
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && nTeam == pChr->GetTeamNo() &&
            !(pChr->GetStatusFlag() & 0x8000) && !(pChr->GetStatusFlag() & 0x10000)) {
            float fDiff = GetRotYDiff(pMe->GetRotY_ToChr(pChr), fRotY);
            if (fDiff < fMin) {
                fMin = fDiff;
                pRet = pChr;
            }
        }
    }
    return pRet;
}

CChrCore *CChrManDodge::Util_SearchPassmeetTo_(long nChr, float fRotY, bcs::FVEC3 &rPos)
{
    // How far off fRotY the receiver may stand: within fRangeMid the pass
    // leads it, within fRangeFar it goes ahead of it all the same without
    // the checks of the first case. fRangeNear (15 degrees) is not used; its
    // constant is in the original's .rodata all the same, ahead of the
    // other two.
    float fRangeNear = 0.2617994f;
    float fRangeMid = 0.7853982f;
    float fRangeFar = 1.5707964f;

    CChrCore *pRet = 0;
    float fMin = 12.566371f;
    CChrCore *pMe = GetChrCore_(nChr);
    long nTeam = pMe->GetTeamNo();
    bcs::FVEC3 myPos;
    pMe->GetCompPos(myPos);

    // The team mate nearest to the direction fRotY; from the infield, not
    // another infield player.
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && nTeam == pChr->GetTeamNo() &&
            !(pChr->GetStatusFlag() & 0x8000) && !(pChr->GetStatusFlag() & 0x10000) &&
            !(pMe->IsDodgeInfield() && pChr->IsDodgeInfield())) {
            float fDiff = GetRotYDiff(pMe->GetRotY_ToChr(pChr), fRotY);
            if (fDiff < fMin) {
                fMin = fDiff;
                pRet = pChr;
            }
        }
    }

    if (pRet != 0) {
        // Aim the pass at the point in direction fRotY as far away as the
        // receiver, stopped at the walls.
        bcs::FVEC3 pos;
        bcs::FVEC3 vec;
        float fTime;
        if (fMin <= fRangeMid) {
            pRet->GetCompPos(pos);
            VecSub(vec, pos, myPos);
            bcs::FVEC3 vecXZ(vec.x, 0.0f, vec.z);
            bcs::BCS_VecZ_RotY(rPos, bcs::FVEC3Len(&vecXZ), fRotY);
            VecAdd(rPos, rPos, myPos);
            // The original works out how far the receiver is from the aim
            // and how long the pass flies, and uses neither.
            VecSub(vec, rPos, pos);
            bcs::FVEC3 missXZ(vec.x, 0.0f, vec.z);
            bcs::FVEC3Len(&missXZ);
            CBallMan::GetPassFlyFrame(myPos, rPos, 1);
            if (CCollMan::DetectMapHit(pos, vec, fTime, myPos, rPos, 0.0f, 0, 0)) {
                rPos.x = pos.x;
                rPos.y = pos.y;
                rPos.z = pos.z;
            }
        } else if (fMin <= fRangeFar) {
            pRet->GetCompPos(vec);
            VecSub(vec, vec, myPos);
            bcs::FVEC3 vecXZ(vec.x, 0.0f, vec.z);
            bcs::BCS_VecZ_RotY(rPos, bcs::FVEC3Len(&vecXZ), fRotY);
            VecAdd(rPos, rPos, myPos);
            if (CCollMan::DetectMapHit(pos, vec, fTime, myPos, rPos, 0.0f, 0, 0)) {
                rPos.x = pos.x;
                rPos.y = pos.y;
                rPos.z = pos.z;
            }
        } else {
            pRet = 0;
        }
    }
    return pRet;
}

CChrCore *CChrManDodge::Util_SearchArtsPassTo_(long nChr)
{
    // How near the goal a receiver of the alley-oop may be.
    static const float CC_CAN_ALLEYOOP_B_RANGE = 5.0f;

    CChrCore *pRet = 0;
    float fMin = 99999.9f;
    CChrCore *pMe = GetChrCore_(nChr);
    long nTeam = pMe->GetTeamNo();
    bcs::FVEC3 goalPos(pMe->GetTargetGoalPos());

    // The team mate in state 0, 1 or 4 nearest to the goal the team
    // attacks, outside the range.
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && nTeam == pChr->GetTeamNo() &&
            !(pChr->GetStatusFlag() & 0x8000) && !(pChr->GetStatusFlag() & 0x10000)) {
            switch (pChr->GetState()) {
                case 0:
                case 1:
                case 4: {
                    float fDist = pChr->GetDist_ToPosXZ(goalPos) - CC_CAN_ALLEYOOP_B_RANGE;
                    if (fDist < 0.0f) {
                        break;
                    }
                    if (pRet == 0 || fDist < fMin) {
                        fMin = fDist;
                        pRet = pChr;
                    }
                    break;
                }
            }
        }
    }

    // Else the farthest one.
    if (pRet == 0) {
        fMin = -1.0f;
        for (long i = 0; i < m_nChrCoreNum; i++) {
            CChrCore *pChr = m_ppChrCore[i];
            if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && nTeam == pChr->GetTeamNo() &&
                !(pChr->GetStatusFlag() & 0x8000) && !(pChr->GetStatusFlag() & 0x10000)) {
                float fDist = pChr->GetDist_ToPosXZ(goalPos);
                if (pRet == 0 || fDist > fMin) {
                    fMin = fDist;
                    pRet = pChr;
                }
            }
        }
    }
    return pRet;
}

int CChrManDodge::Util_GetSubChrTargetPos_(long, bcs::FVEC3 &rPos)
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    return 1;
}

CChrCore *CChrManDodge::Util_GetSubChrMarkChrCore_(CChrCore *)
{
    return 0;
}

void CChrManDodge::Util_SetChgLeaderOrder_(long)
{
}

long CChrManDodge::Util_GetNextChgLeader_(long)
{
    return 0;
}

CChrCore *CChrManDodge::GetExclusiveOuterChrCore(long nTeam)
{
    long nNum = GetChrCoreNum();
    for (long i = 0; i < nNum; i++) {
        CChrCore *pChr = GetChrCore(i);
        if (pChr != 0 && pChr->IsEnable() && nTeam == pChr->GetTeamNo() &&
            CGame::IsCharEx_DodgeOutfielder(pChr->GetPlayerNo())) {
            return pChr;
        }
    }
    return 0;
}

int CChrManDodge::CanbeTurnOver_()
{
    int bResult = CChrMan::CanbeTurnOver_();

    if (bResult) {
        int bBlock = 0;
        long nNum = GetChrCoreNum();
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = GetChrCore(i);
            if (pChr != 0 && pChr->IsEnable()) {
                // (states 0x3B and 0x27 are not named yet)
                switch (pChr->GetState()) {
                    case 0x3B:
                        return 0;
                    case 0x27:
                        if (pChr->GetStatusFlag() & 1) {
                            bBlock = 1;
                        }
                        break;
                }
            }
        }
        if (bBlock) {
            return 0;
        }
    }
    return bResult;
}

int CChrManDodge::CanbeExitLoop()
{
    CChrManDodge *pMan = static_cast<CChrManDodge *>(GetInstance_());
    if (pMan != 0) {
        return pMan->CanbeExitLoop_();
    }
    return 0;
}

int CChrManDodge::CanbeExitLoop_() const
{
    int bBlock = 0;
    long nNum = GetChrCoreNum();

    for (long i = 0; i < nNum; i++) {
        CChrCore *pChr = GetChrCore(i);
        if (pChr != 0 && pChr->IsEnable()) {
            // Waluigi (6) and the ninja (14) only. (state 0x27 is not named
            // yet)
            long nId = pChr->GetChrTypeID();
            if (nId == 6 || nId == 14) {
                if (pChr->GetState() == 0x27 && (pChr->GetStatusFlag() & 1)) {
                    bBlock = 1;
                }
                // The original also reads the life count here and drops it.
                if (pChr->IsDodgeInfield()) {
                    static_cast<CChrCoreDodge *>(pChr)->GetLife();
                }
            }
        }
    }
    return bBlock == 0;
}

CChrMan *CChrManDodgeDodgeOrDie::CreateInstance(bcs::CTask *pParent)
{
    SetInstance_(new (0) CChrManDodgeDodgeOrDie(pParent));
    return GetInstance_();
}

void CChrManDodgeDodgeOrDie::CreateObj_()
{
    m_nChrCoreNum = 8;

    for (long i = 0; i < m_nChrCoreNum; i++) {
        long nType = CGame::GetCharType(i);
        long nColor = CGame::GetCharColor(i);
        CChrCore *pChr = new (0) CChrCoreDodgeDodgeOrDie;
        m_ppChrCore[i] = pChr;

        if (nType < 0) {
            pChr->SetDisable();
        } else {
            pChr->Create(m_pChrTask, i, nType, nColor);
        }
    }
}

void CChrManDodgeDodgeOrDie::CreateDriver_()
{
    // The number of human players, from the game setting at 0x06 (its
    // values are not named yet).
    long nPlayerNum = 0;
    switch (CGame::g_GameWork.m_nUnk06) {
        case 1:
        case 3:
        case 5:
        case 6:
        case 18:
            nPlayerNum = 2;
            break;
        case 7:
        case 9:
        case 15:
        case 16:
        case 17:
        case 20:
        case 21:
            nPlayerNum = 3;
            break;
        case 8:
        case 10:
        case 14:
        case 19:
        case 22:
        case 23:
        case 24:
        case 25:
            nPlayerNum = 4;
            break;
        case 11:
        case 12:
        case 13:
            break;
        default:
            nPlayerNum = 1;
            break;
    }

    long nNo = 0;
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }

        CChrDrv *pDrv;
        if (nNo < nPlayerNum) {
            pDrv = new (0) CChrDrvPadDodge;
        } else {
            pDrv = new (0) CComDrvDodgeDD;
        }
        pChr->DriverSetEntry(0, pDrv);
        // Every character is a team of its own.
        pChr->SetTeamNo(nNo & 3);
        pChr->SetPadID(nNo & 3);
        pChr->SetCamNo(0);
        pChr->OnStatusFlag(0x100);
        pChr->DriverConnect(0, 0, 0, 0, 0);
        nNo++;
    }
}

void CChrManDodgeDodgeOrDie::ReconnectDefaultDriver_()
{
}

void CChrManDodgeDodgeOrDie::InitLeader_(int bConnect)
{
    if (m_ppChrCore != 0 && bConnect) {
        for (long i = 0; i < m_nChrCoreNum; i++) {
            CChrCore *pChr = m_ppChrCore[i];
            if (!pChr->IsDisable()) {
                pChr->DriverConnect(0, 0, 0, 0, 0);
            }
        }
    }
}

void CChrManDodgeDodgeOrDie::CreateMarkerOne_(long nChr)
{
    if (m_ppChrCore[nChr]->DriverGetType() == 1) {
        new (0) CChrMarkerDodgeDodgeOrDie(this, nChr, 2, -1);
    } else {
        new (0) CChrMarkerDodgeDodgeOrDie(this, nChr, 3, -1);
    }
}

} // namespace msw
