// msw::CChrManHockey: the character manager of the hockey game, with the
// fight over the puck (msw::CFighting) and the manager of the party and
// mission games (msw::CHR_MAN_HOCKEY::CChrManHockeyCS).
#include "game/msw/CChrManHockey.hpp"
#include "game/msw/CFighting.hpp"
#include "game/msw/CChrManHockeyCS.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCoreHockey.hpp"
#include "game/msw/CChrDrvPadHockey.hpp"
#include "game/msw/CChrMarkerHockey.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitHockey.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CComDrvHockeyGame.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameGuideMan.hpp"

namespace {

    // The camera of each character for one screen (SetCamIdAssign_ without
    // the flag at g_GameWork + 0x151): by kind of game (0: the rest, 1:
    // g_GameWork + 6 from 6 to 10 or 14 to 15, 2: 16 or 17), then by screen
    // split (0 to 3), then by character.
    const signed char s_CamIdTable[3][4][8] = {
        {
            { 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 1, 0, 1, 0, 1, 0, 1 },
            { 0, 1, 0, 1, 0, 1, 0, 1 },
            { 0, 1, 2, 3, 0, 1, 0, 1 },
        },
        {
            { 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 1, 1, 0, 0, 1, 1 },
            { 0, 0, 1, 1, 0, 0, 1, 1 },
            { 0, 3, 1, 3, 2, 3, 0, 1 },
        },
        {
            { 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 1, 1, 0, 0, 1, 1 },
            { 0, 0, 1, 1, 0, 0, 1, 1 },
            { 0, 1, 2, 3, 0, 1, 0, 1 },
        },
    };

    // (name guessed) The distance between two points in the XZ plane.
    inline float GetDistXZ(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 pos1(r1.x, 0.0f, r1.z);
        bcs::FVEC3 pos2(r2.x, 0.0f, r2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    // rOut = r1 - r2 and rOut = r1 + r2. Stand-ins (a guess) for vector
    // operators of the original: with the paired-single routines called one
    // inline level deeper than from the functions themselves, the float
    // registers come out numbered as in the original (see
    // notes/toolchain.md on asm register numbering by inline depth).
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
        return __fabsf(fDiff);
    }
}

namespace msw {

CChrManHockey::~CChrManHockey()
{
    DeleteChrCore_();
}

CChrMan *CChrManHockey::CreateInstance(bcs::CTask *pParent)
{
    CChrMan *pMan;

    switch (CGame::g_GameWork.m_nGameType) {
        case 2:
        case 3:
            pMan = new CHR_MAN_HOCKEY::CChrManHockeyCS(pParent);
            break;
        default:
            pMan = new CChrManHockey(pParent);
            break;
    }
    SetInstance_(pMan);
    return GetInstance_();
}

void CChrManHockey::Create_()
{
    CreateObj_();
    CreateDriver_();
    CreateMarker_();

    for (long i = 0; i < 2; i++) {
        SetTeamLeader(i, i, 0);
    }
}

void CChrManHockey::CreateObj_()
{
    for (long i = 0; i < m_nChrCoreNum; i++) {
        long nType = CGame::GetCharType(i);
        long nColor = CGame::GetCharColor(i);
        CChrCore *pChr;

        switch (nType) {
            case 0:  pChr = new (0) CChrCoreHockeyMario; break;
            case 1:  pChr = new (0) CChrCoreHockeyLuigi; break;
            case 2:  pChr = new (0) CChrCoreHockeyPeach; break;
            case 3:  pChr = new (0) CChrCoreHockeyDaisy; break;
            case 4:  pChr = new (0) CChrCoreHockeyYoshi; break;
            case 5:  pChr = new (0) CChrCoreHockeyWario; break;
            case 6:  pChr = new (0) CChrCoreHockeyWaluigi; break;
            case 7:  pChr = new (0) CChrCoreHockeyDonkey; break;
            case 8:  pChr = new (0) CChrCoreHockeyDiddy; break;
            case 9:  pChr = new (0) CChrCoreHockeyKinopio; break;
            case 10: pChr = new (0) CChrCoreHockeyKoopa; break;
            case 11: pChr = new (0) CChrCoreHockeyKoopaJr; break;
            case 12: pChr = new (0) CChrCoreHockeyMoogle; break;
            case 13: pChr = new (0) CChrCoreHockeySaboten; break;
            case 14: pChr = new (0) CChrCoreHockeyNinja; break;
            case 17: pChr = new (0) CChrCoreHockeyKuroma; break;
            case 15: pChr = new (0) CChrCoreHockeySiroma; break;
            case 16: pChr = new (0) CChrCoreHockeySlime; break;
            case 19: pChr = new (0) CChrCoreHockeyMii00; break;
            case 20: pChr = new (0) CChrCoreHockeyMii01; break;
            default: pChr = new (0) CChrCoreHockey; break;
        }
        m_ppChrCore[i] = pChr;

        // A negative type is an empty slot: the character stays out of
        // the game.
        if (nType < 0) {
            pChr->SetDisable();
            continue;
        }

        // Type 18 is the keeper. It plays only where the stage has one (not
        // in the party game, not on stages 3 and 4).
        if (nType == 18) {
            int bKeeper;
            if (CGame::g_GameWork.GetGameType() == 2) {
                bKeeper = 0;
            } else if (CGame::g_GameWork.GetStage() == 3) {
                bKeeper = 0;
            } else {
                bKeeper = CGame::g_GameWork.GetStage() != 4;
            }

            if (bKeeper) {
                pChr->SetKeeper(1);
            } else {
                pChr->SetDisable();
                continue;
            }
        }

        pChr->Create(m_pChrTask, i, nType, nColor);
    }
}

void CChrManHockey::CreateDriver_()
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

        pChr->DriverSetEntry(0, new (0) CChrDrvPadHockey);
        CChrDrv *pCom;
        if (pChr->IsKeeper()) {
            pCom = new (0) CComDrvHockeyKeeper;
        } else {
            pCom = new (0) CComDrvHockeyGame;
        }
        pChr->DriverSetEntry(1, pCom);
        pChr->SetTeamNo(i & 1);
        pChr->SetPadID(padId[i]);
        pChr->SetCamNo(camId[i]);
        if (drvId[i] == 1) {
            pChr->DriverConnect(0, 0, 0, 0, 0);
        } else {
            pChr->DriverConnect(1, 0, 0, 0, 0);
        }
        if (i < 2) {
            pChr->OnStatusFlag(0x100);
        }
    }
}

void CChrManHockey::SetCamIdAssign_(long nNum, long *pId)
{
    if (CGame::g_GameWork.m_Unk151 != 0) {
        switch (bcs::CWorldEnv::m_vstViewSplitType) {
            case 1:
            case 2:
                for (long i = 0; i < nNum; i++) {
                    pId[i] = (i >> 1) & 1;
                }
                switch (CGame::g_GameWork.GetUnk06()) {
                    case 15:
                        if (nNum >= 4) {
                            pId[3] = 0;
                        }
                        break;
                    case 16:
                        if (nNum >= 3) {
                            pId[2] = 0;
                        }
                        break;
                }
                break;
            case 3:
                for (long i = 0; i < nNum; i++) {
                    pId[i] = (i >> 1) & 3;
                }
                break;
            default:
                for (long i = 0; i < nNum; i++) {
                    pId[i] = 0;
                }
                break;
        }
        return;
    }

    long nKind = 0;
    switch (CGame::g_GameWork.GetUnk06()) {
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 14:
        case 15:
            nKind = 1;
            break;
        case 16:
        case 17:
            nKind = 2;
            break;
    }

    long nSplit = bcs::CWorldEnv::m_vstViewSplitType;
    nSplit = nSplit > 3 ? 3 : (nSplit < 0 ? 0 : nSplit);
    for (long i = 0; i < nNum; i++) {
        pId[i] = s_CamIdTable[nKind][nSplit][i];
    }
}

void CChrManHockey::CreateMarker_()
{
    for (long i = 0; i < m_nChrCoreNum; i++) {
        if (m_ppChrCore[i]->IsDisable()) {
            continue;
        }

        if (m_ppChrCore[i]->DriverGetType() == 1) {
            if (IsNeedSubComChrMarker_(m_ppChrCore[i]->GetTeamNo())) {
                new (0) CChrMarkerHockey(this, i, 1, -1);
            }
            new (0) CChrMarkerHockey(this, i, 0, -1);
            new (0) CChrMarkerHockey(this, i, 6, -1);
            new (0) CChrMarkerHockey(this, i, 7, -1);
        } else {
            new (0) CChrMarkerHockey(this, i, 0, -1);
            if (IsNeedSubComChrMarker_(m_ppChrCore[i]->GetTeamNo()) || m_ppChrCore[i]->IsKeeper()) {
                new (0) CChrMarkerHockey(this, i, 1, -1);
            }
            new (0) CChrMarkerHockey(this, i, 6, -1);
            new (0) CChrMarkerHockey(this, i, 7, -1);
        }
        new (0) CChrMarkerHockeyArrow(this, i, -1);
        new (0) CChr1on1MarkerHockey(this, i);
    }

    for (long i = 0; i < CGame::g_GameWork.m_nTeamNum; i++) {
        m_pShootMarker[i] = new (0) CShootMarker(this, i);
    }

    m_pFighting = new (0) CFighting(this);
}

void CChrManHockey::AllReset_()
{
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }
        if (pChr->IsPadCtrl()) {
            pChr->DriverConnect(0, 0, 0, 0, 0);
        } else {
            pChr->DriverConnect(1, 0, 0, 0, 0);
        }
    }

    CCockpit::InitFaceIcon();
    GetFighting()->Reset();
}

void CChrManHockey::InitLeader_(int bConnect)
{
    if (m_ppChrCore == 0) {
        return;
    }

    long padId[8];
    long camId[8];
    long drvId[8];

    SetPadIdAssign_(8, padId);
    SetCamIdAssign_(8, camId);
    SetDrvIdAssign_(8, drvId);

    for (long i = 0; i < 2; i++) {
        SetTeamLeader(i, i, 0);
    }

    if (!bConnect) {
        return;
    }

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr->IsDisable()) {
            continue;
        }

        pChr->SetTeamNo(i & 1);
        pChr->SetPadID(padId[i]);
        pChr->SetCamNo(camId[i]);
        if (drvId[i] == 1) {
            pChr->DriverConnect(0, 0, 0, 0, 0);
        } else {
            pChr->DriverConnect(1, 0, 0, 0, 0);
        }
        if (i < 2) {
            pChr->OnStatusFlag(0x100);
        }
    }
}

void CChrManHockey::DispFunc()
{
}

void CChrManHockey::MainFunc_()
{
    float fZero = 0.0f;

    for (long i = 0; i < 4; i++) {
        if (m_ChgLeader[i].m_fWait > fZero) {
            m_ChgLeader[i].m_fWait -= GetFrameRateF();
            if (m_ChgLeader[i].m_fWait < fZero) {
                m_ChgLeader[i].m_fWait = fZero;
            }
        }
        if (m_ChgLeader[i].m_fKeep > fZero) {
            m_ChgLeader[i].m_fKeep -= GetFrameRateF();
            if (m_ChgLeader[i].m_fKeep < fZero) {
                m_ChgLeader[i].m_fKeep = fZero;
            }
        }
    }

    GameGuideDispCheck_();
}

CChrCore *CChrManHockey::Util_SearchChgLeader_(long nBall, long nTeam, CChrCore *pNow)
{
    if (m_ChgLeader[nTeam].m_fWait > 0.0f) {
        return 0;
    }

    // The computer's field players of the team other than pNow.
    long nNum = 0;
    CChrCore *apChr[2];
    apChr[0] = 0;
    apChr[1] = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && nTeam == pChr->GetTeamNo() && !pChr->IsKeeper() && pChr != pNow &&
            !pChr->IsPadCtrl()) {
            apChr[nNum] = pChr;
            nNum++;
        }
    }

    CChrCore *pRet = 0;
    if (nNum == 1) {
        pRet = apChr[0];
    } else if (nNum == 2) {
        // The one that was leader last stays out for a while.
        if (m_ChgLeader[nTeam].m_nLast <= -1) {
            m_ChgLeader[nTeam].m_nLast = pNow->GetPlayerNo();
        }
        if (m_ChgLeader[nTeam].m_fKeep > 0.0f) {
            CChrCore *pLast = CChrMan::GetChrCore(m_ChgLeader[nTeam].m_nLast);
            for (long i = 0; i < 2; i++) {
                if (apChr[i] == pLast) {
                    apChr[i] = 0;
                }
            }
        }
        m_ChgLeader[nTeam].m_fKeep = 40.0f;
        m_ChgLeader[nTeam].m_nLast = pNow->GetPlayerNo();
    }

    CBall *pBall = CBallMan::GetBallPtr(nBall);
    if (pBall == 0) {
        return 0;
    }

    // The nearer one to the puck.
    float fMin = -1.0f;
    for (long i = 0; i < 2; i++) {
        if (apChr[i] != 0) {
            float fDist = GetDistXZ(pBall->GetPos(), apChr[i]->GetPos());
            if (fMin < 0.0f || fMin > fDist) {
                fMin = fDist;
                pRet = apChr[i];
            }
        }
    }

    m_ChgLeader[nTeam].m_fWait = 20.0f;
    return pRet;
}

CChrCore *CChrManHockey::Util_SearchNearOwnGoal_(long nTeam, int bArg, CChrCore *pChr)
{
    return Util_SearchNearPos_(CGame::GetGoalPos(1 - CGame::GetGoalIdx(nTeam)), nTeam, bArg, pChr);
}

CChrCore *CChrManHockey::Util_SearchPassTo_(long nChr, float fRotY)
{
    CChrCore *pRet = 0;
    float fMin = 12.566371f;
    CChrCore *pMe = GetChrCore_(nChr);
    long nTeam = pMe->GetTeamNo();

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && !pChr->IsKeeper() && nTeam == pChr->GetTeamNo() &&
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

CChrCore *CChrManHockey::Util_SearchPassmeetTo_(long nChr, float fRotY, bcs::FVEC3 &rPos)
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

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && nTeam == pChr->GetTeamNo() && !pChr->IsKeeper() &&
            !(pChr->GetStatusFlag() & 0x8000) && !(pChr->GetStatusFlag() & 0x10000)) {
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

CChrCore *CChrManHockey::Util_SearchArtsPassTo_(long nChr)
{
    // How near the goal a receiver of the alley-oop may be.
    static const float CC_CAN_ALLEYOOP_B_RANGE = 5.0f;

    CChrCore *pRet = 0;
    float fMin = 99999.9f;
    CChrCore *pMe = GetChrCore_(nChr);
    long nTeam = pMe->GetTeamNo();
    bcs::FVEC3 goalPos(pMe->GetTargetGoalPos());

    // The nearest team mate to the goal outside the minimum range that is
    // standing or skating.
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && !pChr->IsKeeper() && nTeam == pChr->GetTeamNo() &&
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
            if (!pChr->IsDisable() && nChr != pChr->GetPlayerNo() && nTeam == pChr->GetTeamNo() && !pChr->IsKeeper() &&
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

int CChrManHockey::Util_GetSubChrTargetPos_(long, bcs::FVEC3 &rPos)
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    return 1;
}

CChrCore *CChrManHockey::Util_GetSubChrMarkChrCore_(CChrCore *)
{
    return 0;
}

void CChrManHockey::Util_SetChgLeaderOrder_(long)
{
}

long CChrManHockey::Util_GetNextChgLeader_(long)
{
    return 0;
}

void CChrManHockey::FightingStart_(long nChr1, long nChr2)
{
    CFighting *pFighting = m_pFighting;

    pFighting->Reset();
    pFighting->m_nChr1 = nChr1;
    pFighting->m_nChr2 = nChr2;
    pFighting->m_bActive = 1;
    pFighting->m_fTime = 300.0f;
    pFighting->m_bCanJoin = 1;
    pFighting->m_fWinPoint = bcs::BCS_RandRange(70.0f, 120.0f);
    pFighting->m_fJoinWait = 10.0f;
}

void CChrManHockey::GetFightingWork_(FIGHTING_WORK &rWork)
{
    rWork = *m_pFighting;
}

void CChrManHockey::FightingUpdateChrData_(long nChr, float fPower, int bFirst, int bJoin)
{
    CFighting *pFighting = m_pFighting;

    pFighting->m_bJoin[nChr] = bJoin;
    pFighting->m_fPower[nChr] += fPower;
    if (bFirst && pFighting->m_nFirst < 0) {
        pFighting->m_nFirst = nChr;
    }
}

void CChrManHockey::ArtsStart_()
{
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (!pChr->IsDisable() && pChr->IsKeeper()) {
            pChr->GetMdl()->SetAlphaRateF(0.0f);
        }
    }
}

void CChrManHockey::GameGuideDispCheck_()
{
    if (!CGameGuideMan::IsDispEnable()) {
        return;
    }
    if (CGame::g_GameWork.GetGameType() == 2) {
        return;
    }

    // The first human player that has a telop to show.
    long nType = -1;
    long nChr = -1;
    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[i];
        if (pChr != 0 && !pChr->IsDisable() && pChr->IsPadCtrl()) {
            long nTelop = pChr->CheckGuideTelopDisp();
            if (nTelop >= 0 && nChr < 0) {
                nChr = i;
                nType = nTelop;
            }
        }
    }

    if (nType >= 0 && nChr >= 0) {
        float fTime = 10.0f;
        bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
        float f0 = -1.0f;
        float f1 = -1.0f;
        float f2 = -1.0f;
        switch (nType) {
            case 0x18:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                f0 = 1.0f;
                f1 = 0.0f;
                f2 = 0.0f;
                break;
            case 0x19:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                f0 = 0.0f;
                fTime = 60.0f;
                break;
            case 0x1A:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                f0 = 0.0f;
                fTime = 60.0f;
                break;
            case 0x1B:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                f0 = 0.0f;
                fTime = 60.0f;
                break;
            case 0x1C:
                ofs.Set(-0.01f, -0.01f, 0.0f);
                f0 = 0.0f;
                fTime = 60.0f;
                break;
        }
        CGameGuideMan::StartGuideTelop(nType, m_ppChrCore[nChr], fTime, &ofs, f0, f1, f2);
    } else {
        CGameGuideMan::EndGuideTelop(-1);
    }
}

CFighting::~CFighting()
{
}

void CFighting::Reset()
{
    m_fTime = 0.0f;
    m_Unk04 = 0;
    m_bActive = 0;
    m_bCanJoin = 0;
    m_fWinPoint = 0.0f;
    m_nState = 0;
    m_nFirst = -1;
    m_nChr1 = -1;
    m_nChr2 = -1;
    m_nWinner = -1;
    m_Unk6C = 0;
    m_nRound = 0;
    m_fJoinWait = 10.0f;
    m_fCallWait = 0.0f;
    m_fEndWait = 10.0f;
    m_bShakeIcon = 0;
    for (long i = 0; i < 8; i++) {
        m_bJoin[i] = 0;
        m_fPower[i] = 0.0f;
    }

    CCockpit::RemoveObj(0x1072);
    CCockpit::RemoveObj(0x1E);
    CCockpitHockey::ClearFightGaugeIcon(0, 0);
    CCockpitHockey::ClearFightGaugeIcon(1, 0);
}

float CFighting::GetNowWinMotFrm(long nChr)
{
    float fFrm = 0.0f;

    if (nChr >= 0) {
        float fNow = CChrMan::GetChrCore(nChr)->AnimGetFrame(3);
        if (fNow < 20.0f) {
            fFrm = 20.0f;
        } else if (fNow < 50.0f) {
            fFrm = 50.0f;
        } else if (fNow < 80.0f) {
            fFrm = 80.0f;
        } else if (fNow < 110.0f) {
            fFrm = 110.0f;
        } else {
            fFrm = 20.0f;
        }
    }
    return fFrm;
}

void CFighting::MainFunc(float fRate)
{
    // Nothing while the game is held up (flags 0x4306) or not in its main
    // loop.
    if ((CGame::g_GameWork.m_nFlag18 & 0x4306) != 0 || !CGame::IsMainLoopState(0)) {
        return;
    }

    CheckFightingStart();
    if (m_bActive == 0) {
        return;
    }

    switch (m_nState) {
        // The fight: each team's power fills its gauge.
        case 0: {
            CheckbShakeIconDisp();

            bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
            if (CGame::g_GameWork.m_nUnk180 == 0) {
                CChrCore *pChr = CChrMan::GetChrCore(m_nChr1);
                ofs.y = -0.2f;
                VecAdd(ofs, ofs, pChr->GetPos());
                CCockpitHockey::SetFightGaugeIcon(pChr->GetTeamNo(), &ofs);

                pChr = CChrMan::GetChrCore(m_nChr2);
                ofs.Set(0.0f, 0.0f, 0.0f);
                ofs.y = -0.2f;
                VecAdd(ofs, ofs, pChr->GetPos());
                CCockpitHockey::SetFightGaugeIcon(pChr->GetTeamNo(), &ofs);
            }

            m_fJoinWait -= fRate;
            ofs.y = 0.2f;
            if (m_fJoinWait < 0.0f) {
                m_fJoinWait = 0.0f;
            }
            m_fTime -= fRate;

            long nWinner = -1;
            float afPower[2];
            afPower[0] = 0.0f;
            afPower[1] = 0.0f;
            for (long i = 0; i < 8; i++) {
                if (m_bJoin[i]) {
                    afPower[CChrMan::GetChrCore(i)->GetTeamNo()] += m_fPower[i];
                }
            }

            for (long nTeam = 0; nTeam < 2; nTeam++) {
                float fGauge = afPower[0];
                if (nTeam != CGame::g_GameWork.m_nUnk00) {
                    fGauge = afPower[1];
                }
                CCockpitHockey::SetFightGaugeRatio(nTeam, fGauge / m_fWinPoint);

                if (afPower[nTeam] > m_fWinPoint) {
                    long nOther = 0;
                    if (nTeam == 0) {
                        nOther = 1;
                    }
                    if (afPower[nOther] >= 0.9f * m_fWinPoint) {
                        // Too close: nobody wins.
                        m_fTime = 0.0f;
                        nWinner = -1;
                    } else {
                        // The team's strongest shaker wins.
                        float fMax = -1.0f;
                        for (long i = 0; i < 8; i++) {
                            if (m_bJoin[i] && nTeam == CChrMan::GetChrCore(i)->GetTeamNo() && fMax < m_fPower[i]) {
                                fMax = m_fPower[i];
                                nWinner = i;
                            }
                        }
                    }
                }
            }

            if (m_fTime <= 0.0f || nWinner >= 0) {
                // The fight is over (won, or out of time).
                m_bCanJoin = 0;
                m_nWinner = nWinner;
                m_nState++;
                m_bShakeIcon = 0;
                m_fWinMotFrm = GetNowWinMotFrm(m_nChr1);
                CCockpit::RemoveObj(0x1072);
                CCockpitHockey::ClearFightGaugeIcon(0, 0);
                CCockpitHockey::ClearFightGaugeIcon(1, 0);

                if (m_nWinner >= 0) {
                    m_Unk6C = 0;
                    break;
                }

                if (afPower[0] > 5.0f || afPower[1] > 5.0f) {
                    // Time is up: the stronger team wins unless it is close.
                    long nMax = afPower[0];
                    long nLess = 1;
                    if (afPower[0] < afPower[1]) {
                        nMax = afPower[1];
                        nLess = 0;
                    }
                    if (afPower[nLess] >= 0.9f * nMax) {
                        m_Unk6C = 0;
                    } else {
                        CChrCore *pChr = CChrMan::GetChrCore(m_nChr1);
                        if (afPower[0] > afPower[1]) {
                            if (pChr->GetTeamNo() == 0) {
                                m_nWinner = m_nChr1;
                            } else {
                                m_nWinner = m_nChr2;
                            }
                        } else {
                            if (pChr->GetTeamNo() == 1) {
                                m_nWinner = m_nChr1;
                            } else {
                                m_nWinner = m_nChr2;
                            }
                        }
                    }
                }

                // A human player in the fight shakes the camera.
                int bPad = 0;
                for (long i = 0; i < 8; i++) {
                    if (m_bJoin[i] && CChrMan::GetChrCore(i)->IsPadCtrl()) {
                        bPad = 1;
                    }
                }
                if (bPad) {
                    CGame::StartCameraQuake(0x2F, -1, -1);
                }
            }
            break;
        }

        // The winning motion.
        case 1: {
            if (m_nChr1 < 0) {
                break;
            }
            float fFrm = CChrMan::GetChrCore(m_nChr1)->AnimGetFrame(3);
            if (m_Unk6C == 0) {
                m_nState = 3;
                for (long i = 0; i < 8; i++) {
                    if (m_bJoin[i] && i != m_nWinner) {
                        CChrMan::GetChrCore(i)->AnimPlay(3, 0x24, 0, 8.0f, 0.0f);
                    }
                }
            } else if (fFrm <= 112.0f && fFrm >= m_fWinMotFrm) {
                m_nState = 3;
                if (m_Unk6C != 0) {
                    m_nState = 2;
                    m_fTime = 20.0f;
                    m_fCallWait = 10.0f;
                    for (long i = 0; i < 8; i++) {
                        if (m_bJoin[i]) {
                            // (the original fetches both and uses neither)
                            CChrCore *pChr = CChrMan::GetChrCore(i);
                            CChrCore *pWinner = 0;
                            if (m_nWinner > 0) {
                                pWinner = CChrMan::GetChrCore(m_nWinner);
                            }
                            if (i != m_nWinner) {
                                CChrMan::GetChrCore(i)->AnimPlay(3, 0x24, 0, 8.0f, 0.0f);
                            }
                        }
                        m_fPower[i] = 0.0f;
                    }
                }
            }
            break;
        }

        // The second round: the first to act wins.
        case 2: {
            if (m_fCallWait > 0.0f) {
                m_fCallWait -= fRate;
                if (m_fCallWait <= 0.0f) {
                    CCockpitHockey::CreateObj(0x1E, 0);
                    m_fCallWait = 0.0f;
                }
            }
            m_fTime -= fRate;

            if (m_nFirst >= 0) {
                CCockpit::RemoveObj(0x1E);
                if (m_nWinner < 0) {
                    m_nWinner = m_nFirst;
                    m_nState = 3;
                } else if (m_nWinner == m_nFirst) {
                    m_nState = 3;
                } else {
                    m_nState = 4;
                    // Both teams acted, unless the two are team mates.
                    long nTeam = CChrMan::GetChrCore(m_nWinner)->GetTeamNo();
                    if (nTeam == CChrMan::GetChrCore(m_nFirst)->GetTeamNo()) {
                        m_nWinner = m_nFirst;
                        m_nState = 3;
                    }
                }
            } else if (m_fTime <= 0.0f) {
                m_fTime = 0.0f;
                CCockpit::RemoveObj(0x1E);

                float afPower[2];
                float afMax[2];
                long anBest[2];
                afPower[0] = 0.0f;
                afPower[1] = 0.0f;
                for (long nTeam = 0; nTeam < 2; nTeam++) {
                    afMax[nTeam] = -1.0f;
                    anBest[nTeam] = 0;
                }
                for (long i = 0; i < 8; i++) {
                    if (m_bJoin[i]) {
                        long nTeam = CChrMan::GetChrCore(i)->GetTeamNo();
                        float fPower = m_fPower[i];
                        afPower[nTeam] += fPower;
                        if (afMax[nTeam] < fPower) {
                            afMax[nTeam] = fPower;
                            anBest[nTeam] = i;
                        }
                    }
                }

                if (afPower[0] > 0.0f && afPower[1] > 0.0f) {
                    m_nState = 4;
                } else if (afPower[0] > 0.0f) {
                    m_nState = 3;
                    m_nWinner = anBest[0];
                } else if (afPower[1] > 0.0f) {
                    m_nState = 3;
                    m_nWinner = anBest[1];
                } else {
                    m_nState = 3;
                    m_nWinner = -1;
                }
            }
            break;
        }

        case 3:
            if (m_nWinner < 0) {
                Reset();
            } else {
                m_nState = 5;
            }
            break;

        // Both teams acted: the fight starts over.
        case 4:
            m_nState = 0;
            m_nFirst = -1;
            m_nWinner = -1;
            m_Unk6C = 0;
            for (long i = 0; i < 8; i++) {
                m_fPower[i] = 0.0f;
            }
            m_fTime = 600.0f;
            m_bCanJoin = 1;
            m_fWinPoint = bcs::BCS_RandRange(70.0f, 120.0f);
            m_fCallWait = 0.0f;
            m_nRound++;
            break;

        // Waits for the winner's motion.
        case 5:
            if (m_nWinner >= 0) {
                CChrCore *pChr = CChrMan::GetChrCore(m_nWinner);
                if (pChr->AnimGetMotID(3, 0) == 0x38 || pChr->GetState() != 0x56) {
                    m_nState++;
                }
            }
            break;

        // Waits until nobody is in state 0x56 any more.
        case 6: {
            int bEnd = 1;
            for (long i = 0; i < 8; i++) {
                if (m_bJoin[i] && CChrMan::GetChrCore(i)->GetState() == 0x56) {
                    bEnd = 0;
                }
            }
            if (bEnd) {
                Reset();
            }
            break;
        }
    }

    if (CheckEndFighting()) {
        m_nState = 3;
        m_nWinner = -1;
        m_bShakeIcon = 0;
        CCockpit::RemoveObj(0x1072);
        CCockpit::RemoveObj(0x1E);
        CCockpitHockey::ClearFightGaugeIcon(0, 0);
        CCockpitHockey::ClearFightGaugeIcon(1, 0);
    }
}

void CFighting::CheckbShakeIconDisp()
{
    int bDisp = 0;

    for (long i = 0; i < 8; i++) {
        if (m_bJoin[i]) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (!pChr->IsDisable() &&
                (CGame::g_GameWork.m_Unk151 == 0 || CGame::g_GameWork.m_nUnk00 == pChr->GetTeamNo()) &&
                pChr->IsPadCtrl()) {
                bDisp = 1;
            }
        }
    }

    if (!bDisp) {
        if (m_bShakeIcon) {
            m_bShakeIcon = 0;
            CCockpit::RemoveObj(0x1072);
        }
    } else if (!m_bShakeIcon) {
        m_bShakeIcon = 1;
        CCockpitHockey::CreateObj(0x1072, 0);
    }
}

void CFighting::UpdateFunc(float)
{
}

void CFighting::Main()
{
    MainFunc(GetFrameRateF());
}

int CFighting::CheckEndFighting()
{
    long nJoin = 0;
    for (long i = 0; i < 8; i++) {
        if (m_bJoin[i]) {
            nJoin++;
        }
    }

    CBall *pBall = CBallMan::GetBallPtr(0);
    for (long i = 0; i < 8; i++) {
        if (!m_bJoin[i]) {
            continue;
        }

        if (m_fEndWait > 0.0f) {
            m_fEndWait -= GetFrameRateF();
            if (m_fEndWait <= 0.0f) {
                m_fEndWait = 0.0f;
            }
        }

        CChrCore *pChr = CChrMan::GetChrCore(i);
        float fDist = GetDistXZ(pChr->GetPos(), pBall->GetPos());

        // The more fighters, the farther they may be from the puck.
        float fAdd = nJoin - 2;
        if (fAdd < 0.0f) {
            fAdd = 0.0f;
        }

        if (pChr->IsHockeyDamage() || (pChr->GetStatusFlag() & 0x10000) || (pChr->GetStatusFlag() & 0x20000)) {
            return 1;
        }
        if (4.0f + fAdd < fDist && m_fEndWait <= 0.0f) {
            return 1;
        }
    }
    return 0;
}

void CFighting::CheckFightingStart()
{
    if (m_bCanJoin == 0) {
        return;
    }
    if (m_fJoinWait > 0.0f) {
        return;
    }

    CBall *pBall = CBallMan::GetBallPtr(0);
    for (long i = 0; i < 8; i++) {
        if (m_bJoin[i]) {
            continue;
        }
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr->IsDisable()) {
            continue;
        }

        if (GetDistXZ(pChr->GetPos(), pBall->GetPos()) <= 2.2f && static_cast<CChrCoreHockey *>(pChr)->MethdAddFighting_()) {
            m_fTime = 300.0f;
            for (long j = 0; j < 8; j++) {
                if (m_bJoin[j]) {
                    m_fPower[j] = 0.0f;
                    // (the original's 0.1 is one bit above 0.1f, as
                    // 6.0f * (1.0f / 60.0f) gives: six frames, guessed)
                    CChrCore *pOther = CChrMan::GetChrCore(j);
                    pOther->SetHockeySkateWork(pChr->GetRotY(), 6.0f * (1.0f / 60.0f), 20.0f, -0.006f);
                }
            }
            m_bJoin[i] = 1;
        }
    }
}

namespace CHR_MAN_HOCKEY {

CChrManHockeyCS::~CChrManHockeyCS()
{
}

void CChrManHockeyCS::Create_()
{
    CreateObj_();
    CreateDriver_();
    CreateMarker_();
    SetTeamLeader(0, 0, 0);
}

long CChrManHockeyCS::CheckGenChrCoreNum_()
{
    return m_nChrCoreNum;
}

void CChrManHockeyCS::CreateDriver_()
{
    if (m_bMission) {
        MissionCreateDriver_();
    } else {
        KSCreateDriver_();
    }
}

void CChrManHockeyCS::KSCreateDriver_()
{
    long padId[8];
    long camId[8];
    long drvId[8];

    SetPadIdAssign_(8, padId);
    SetCamIdAssign_(8, camId);
    SetDrvIdAssign_(8, drvId);

    // The order in which the characters take the teams' slots.
    long order[4] = { 0, 2, 1, 3 };

    for (long i = 0; i < m_nChrCoreNum; i++) {
        CChrCore *pChr = m_ppChrCore[order[i]];
        if (pChr->IsDisable()) {
            continue;
        }

        pChr->DriverSetEntry(0, new (0) CChrDrvPadHockey);
        pChr->DriverSetEntry(1, new (0) CComDrvHockeyCS);
        pChr->SetTeamNo(order[i]);
        pChr->SetPadID(padId[i]);
        pChr->SetCamNo(camId[i]);
        if (drvId[i] == 1) {
            pChr->DriverConnect(0, 0, 0, 0, 0);
        } else {
            pChr->DriverConnect(1, 0, 0, 0, 0);
        }
        pChr->OnStatusFlag(0x100);
    }
}

void CChrManHockeyCS::MissionCreateDriver_()
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

        pChr->DriverSetEntry(0, new (0) CChrDrvPadHockey);
        pChr->DriverSetEntry(1, new (0) CComDrvHockeyCS);
        pChr->SetTeamNo(i & 1);
        pChr->SetPadID(padId[i]);
        pChr->SetCamNo(camId[i]);
        if (drvId[i] == 1) {
            pChr->DriverConnect(0, 0, 0, 0, 0);
        } else {
            pChr->DriverConnect(1, 0, 0, 0, 0);
        }
        if (i < 2) {
            pChr->OnStatusFlag(0x100);
        }
    }
}

void CChrManHockeyCS::CreateMarker_()
{
    for (long i = 0; i < m_nChrCoreNum; i++) {
        if (m_ppChrCore[i]->IsDisable()) {
            continue;
        }
        if (CGame::g_GameWork.GetGameType() != 2) {
            continue;
        }

        if (m_bMission) {
            long nTeam = m_ppChrCore[i]->GetTeamNo();
            new (0) CChrMarkerHockey(this, i, 2, nTeam);
            new (0) CChrMarkerHockey(this, i, 6, nTeam);
            new (0) CChrMarkerHockey(this, i, 1, nTeam);
            new (0) CChrMarkerHockey(this, i, 7, nTeam);
            new (0) CChrMarkerHockeyArrow(this, i, nTeam);
        } else {
            new (0) CChrMarkerHockey(this, i, 2, i & 3);
            new (0) CChrMarkerHockey(this, i, 6, i & 3);
            new (0) CChrMarkerHockey(this, i, 1, i & 3);
            new (0) CChrMarkerHockey(this, i, 7, i & 3);
            new (0) CChrMarkerHockeyArrow(this, i, i & 3);
        }
    }
}

void CChrManHockeyCS::AllReset_()
{
}

void CChrManHockeyCS::AllSetState_(long nState, long nArg1, long nArg2)
{
    long nWinTeam;
    if (nState == 1) {
        if (!m_bMission) {
            // Time over: the team with the most goals wins, and nobody when
            // every team has the same score.
            long nTop = 1;
            long nCount = 0;
            for (long nTeam = 0; nTeam < CGame::g_GameWork.m_nTeamNum; nTeam++) {
                long nScore = CGame::GetTeamScore(nTeam);
                if (nScore >= nTop) {
                    nTop = nScore;
                    nCount++;
                }
            }
            if (nCount == CGame::g_GameWork.m_nTeamNum) {
                nTop = 9999;
            }

            for (long nTeam = 0; nTeam < CGame::g_GameWork.m_nTeamNum; nTeam++) {
                nWinTeam = nTop == CGame::GetTeamScore(nTeam) ? nTeam : -1;
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    if (m_ppChrCore[i] != 0 && !m_ppChrCore[i]->IsDisable() && nTeam == m_ppChrCore[i]->GetTeamNo()) {
                        m_ppChrCore[i]->MethdTimeOver(nWinTeam);
                    }
                }
            }
        }
    } else {
        CChrMan::AllSetState_(nState, nArg1, nArg2);
    }
}

void CChrManHockeyCS::InitLeader_(int bConnect)
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
            pChr->DriverConnect(pChr->DriverGetNo(), 0, 0, 0, 0);
        }
    }
}

int CChrManHockeyCS::CanbeTurnOver_()
{
    int bResult = 0;

    if (CChrMan::CanbeTurnOver_()) {
        bResult = 1;
        for (long i = 0; i < m_nChrCoreNum; i++) {
            CChrCore *pChr = m_ppChrCore[i];
            if (pChr != 0 && !pChr->IsDisable()) {
                // (states 0x4E and 6 are not named yet)
                switch (pChr->GetState()) {
                    case 0x4E:
                        bResult = 0;
                        break;
                    case 6:
                        if (pChr->GetComLevelWork() != 4) {
                            bResult = 0;
                        }
                        break;
                }
            }
        }
    }
    return bResult;
}

void CChrManHockeyCS::DispFunc()
{
}

} // namespace CHR_MAN_HOCKEY

} // namespace msw
