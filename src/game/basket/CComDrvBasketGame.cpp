// msw::CComDrvBasketGame: ActInterrupt and ThkMain, the two hooks CComDrv
// calls every frame (break off the running action for something more
// urgent; pick the next action when none runs), with their per-situation
// parts (loose ball, offence, defence, star state, gimmicks, thrown items).
// A file of its own with its own float constants; the name is a guess.
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CSimpleSortBase.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CComDrvBasketGame.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManBasket.hpp"
#include "game/msw/CComManBasketGame.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CItemBallBasket.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/CNpcGhostBasketBase.hpp"

namespace CComDrvBasketGame_cpp {
// uncalled stand-in / pool-off stand-in: the original addresses every float
// constant of this file with its own lis, which this compiler does when the
// first .rodata object of the file is weak (a function-local const). The
// module link strips this function and its constant.
void UnusedPoolOff()
{
    const long k = 0;
}
}

// 70 degrees in radians, unused (as in CComManBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace {
    // (guessed helper) rPos1 - rPos2.
    inline bcs::FVEC3 Diff(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 diff;
        return *bcs::FVEC3Sub(&diff, &rPos1, &rPos2);
    }

    // (guessed helper) The distance of two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 d = Diff(rPos1, rPos2);
        return bcs::BCS_Vec_GetLenXZ(d);
    }

    // (guessed helper) The distance of two points.
    inline float Dist(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 d = Diff(rPos1, rPos2);
        return PSVECMag((const Vec *)&d);
    }

    typedef bcs::CSimpleSortBase<32>::SORT SORT;

    // (guessed helper) The computer manager of the basketball game.
    inline msw::CComManBasketGame *GetComManBK()
    {
        return static_cast<msw::CComManBasketGame *>(msw::CComMan::GetInstance());
    }
}

// Most functions keep the computer level as `const float &fLv =
// GetComLvF();`, a reference bound to the returned temporary. That is a
// guess at the original's form: it rounds the kept value with frsp before
// every use (and saves the register with psq_st), which a plain float local
// does not reproduce. ActInterrupCommonThrowItem_ has no frsp, so it uses a
// plain float.

namespace msw {

// Breaks off the running action for something more urgent.
void CComDrvBasketGame::ActInterrupt()
{
    m_fUnk18C += m_pChr->GetFrameStep();
    if (m_pChr->GetState() == 0x26) {
        return;
    }
    if (CGame::g_GameWork.m_nFlag18 & 0x8) {
        return;
    }
    if (IsCombination()) {
        return;
    }
    if (ActInterrupGimmick_()) {
        return;
    }
    if (ActInterrupCommonThrowItem_()) {
        return;
    }
    if (ActInterrupStateStar_()) {
        return;
    }

    if (CGame::g_GameWork.m_nFlag18 & 0x1) {
        m_nUnk188 = 0;
        m_nUnk184 = 0;
        ActInterrupLooseBall_();
    } else if (m_nFlag17C & 0x800) {
        ActInterrupSp_();
    } else if (IsMyTeamHoldBall(0) || CGame::g_GameWork.GetUserTeam() == m_nMyTeam) {
        m_nUnk188 = 0;
        m_nUnk184 = 0;
        ActInterruptOffense_();
    } else {
        ActInterruptDefense_();
    }
}

// Picks the next action when none is running.
void CComDrvBasketGame::ThkMain()
{
    if (CGame::g_GameWork.m_nFlag18 & 0x8) {
        ThkMainJumpBall_();
        return;
    }
    if ((m_nFlag17C & 0x800) && CChrManBasket::GetInstance()->GetArtsChr() == 0) {
        m_nFlag17C &= ~0x800;
    }

    if (CGame::g_GameWork.m_nFlag18 & 0x1) {
        if (m_pChr->GetStatusFlag() & 0x20000) {
            ThkMainDefenseStar_();
        } else {
            ThkMainLooseBall_();
        }
    } else if (m_nFlag17C & 0x800) {
        ThkMainSp_();
    } else if (CGame::g_GameWork.GetUserTeam() == m_nMyTeam) {
        if (m_pChr->GetStatusFlag() & 0x20000) {
            ThkMainOffenseStar_();
        } else {
            ThkMainOffense_();
        }
    } else if (m_pChr->IsNowBlockWork() && GetCurrentActType() == 5) {
        Reset();
    } else if (m_pChr->GetStatusFlag() & 0x20000) {
        ThkMainDefenseStar_();
    } else {
        ThkMainDefense_();
    }
}

// The ball is loose: throw an item, or go for the ball when no team mate
// is nearer to it.
void CComDrvBasketGame::ActInterrupLooseBall_()
{
    bcs::FVEC3 target;
    if (JudgeItemThrow(target)) {
        Reset();
        ActCallItemThrow(target);
        return;
    }
    if (GetRootActType() == 0x14) {
        return;
    }
    long nAct = GetCurrentActType();
    if (nAct != 1 && nAct != 5) {
        return;
    }
    CBall *pBall = CBallMan::GetBallPtr(0);
    if (pBall != 0 && pBall->IsActive()) {
        float fDist = DistXZ(pBall->GetPos(), m_pChr->GetPos());
        if (3.5f < fDist) {
            return;
        }
        int bNearest = 1;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            const CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == m_nMyTeam &&
                !(pChr->GetStatusFlag() & 0x1400) && pChr != m_pChr && !CChrManBasket::IsBadConditon(pChr) &&
                !(DistXZ(pBall->GetPos(), pChr->GetPos()) > fDist)) {
                bNearest = 0;
                break;
            }
        }
        if (bNearest) {
            Reset();
            ActCallGetBall();
        }
    }
}

// The driver's team has the ball.
void CComDrvBasketGame::ActInterruptOffense_()
{
    if (m_pChr->IsHoldBall()) {
        long nRoot = GetRootActType();
        long nState = m_pChr->GetState();
        const float &fLv = GetComLvF();
        int bBossShot = 0;
        float fStep = m_pChr->GetFrameStep();

        // On the boss Packun stage: a jump shot every so often.
        if (CGame::g_GameWork.GetStage() == 7) {
            m_fUnk1D8 += fStep;
            if (!(CComMan::GetDatF(0xEF7, fLv) > m_fUnk1D8)) {
                bBossShot = 1;
                m_fUnk1D8 = 0.0f;
            }
        }

        // A dunk now and then.
        m_fUnk194 += fStep;
        if (m_fUnk194 >= CComMan::GetDatI(0x12B, fLv)) {
            m_fUnk194 = 0.0f;
            if (CanbeDunk()) {
                long nRate = CComMan::GetDatI(IsAllComTeam() ? 0x133 : 0x13B, fLv);
                switch (CGame::g_GameWork.GetStage()) {
                    case 7: {
                        CNpcBossPackun *pBoss = CChrManBasket::GetInstance()->GetBossPackun_(m_nUnk8C);
                        if (pBoss != 0) {
                            int bAllCom = IsAllComTeam();
                            if (pBoss->GetState() == 0) {
                                nRate = CComMan::GetDatI(bAllCom ? 0xEAF : 0xEB7, fLv);
                            } else if (pBoss->IsMouthOpen()) {
                                nRate = CComMan::GetDatI(bAllCom ? 0xE9F : 0xEA7, fLv);
                            } else {
                                nRate = CComMan::GetDatI(bAllCom ? 0xE8F : 0xE97, fLv);
                            }
                        }
                        break;
                    }
                    case 5:
                        if (static_cast<CChrCoreBasket *>(m_pChr)->CanbeDunkMissDist()) {
                            long nMiss = CComMan::GetDatI(0xF73, fLv);
                            if (LotRate() >= nMiss) {
                                nRate = 0;
                            }
                        }
                        break;
                }
                if (LotRate() < nRate && MethdDunk()) {
                    Reset();
                    ActCallDunk();
                    return;
                }
            }
        }

        if (CGame::g_GameWork.GetStage() == 7) {
            if ((nRoot < 0x1D || nRoot > 0x1E) && nRoot != 0x30 && CanbeActCallJumpShoot() && bBossShot) {
                long nRate = CComMan::GetDatI(IsAllComTeam() ? 0xEE7 : 0xEEF, 0L);
                if (LotRate() < nRate) {
                    float fDist = DistXZ(m_MyPos, m_TargetGoalPos);
                    if (!(fDist > CComMan::GetDatF(0xED7, fLv))) {
                        unsigned long nBits = GetEmTeamEnableBit();
                        float fWidth = CComMan::GetDatF(0xEDF, fLv) * 0.5f;
                        if (!CComManBasket::IsExistTarget(m_MyPos, m_TargetGoalPos, fWidth, fDist, nBits)) {
                            Reset();
                            ActCallJumpShoot();
                        }
                    }
                }
            }
        } else if ((nRoot < 0x1D || nRoot > 0x1E) && nRoot != 0x30 && CGame::g_GameWork.GetStage() == 10 && CanbeActCallJumpShoot() &&
                   (m_nFlag17C & 0x80)) {
            // On the street stage: a jump shot from a bonus panel.
            m_nFlag17C &= ~0x80;
            long nRate = CComMan::GetDatF(0xF9B, GetComLvF());
            if (CanbeJumpShoot() && LotRate() < nRate && JudgeShootCourseFree()) {
                Reset();
                ActCallJumpShoot();
                return;
            }
        }

        switch (nRoot) {
            case 0x30:
            case 0x1D:
                if (LotRateCancelPass()) {
                    Reset();
                    ActCallCancelPass(0);
                    return;
                }
                break;
            case 0x1E:
                if (LotRateCancelLayUp()) {
                    Reset();
                    ActCallCancelLayUp();
                    return;
                }
                break;
            case 0x2B:
            case 0x2D:
                if (m_pChr != 0 && m_pChr->IsBattle()) {
                    Reset();
                    ActCallBlockade();
                    return;
                }
                break;
            default:
                if (nState == 0x34 && m_pChr->IsBattle() &&
                    static_cast<CChrCoreBasket *>(m_pChr)->GetDashActRate() >= 0.7f &&
                    static_cast<CComManBasketGame *>(CComMan::GetInstance())->CheckDashToTrickMoveActInter_(m_pChr)) {
                    long nRate = CComMan::GetDatI(0x18B, fLv);
                    if (LotRate() < nRate) {
                        Reset();
                        ActCallSpinMove();
                        return;
                    }
                }
                break;
        }

        if (m_nFlag17C & 0x400) {
            MethdDash();
        }
        ActInterruptItemShootCheck_();
        return;
    }

    // A team mate holds the ball.
    IsMyTeamHoldBall(0);
    m_fUnk1D8 = 0.0f;
    m_fUnk1A8 = 0.0f;
    m_fUnk194 = 0.0f;
    long nRoot = GetRootActType();
    GetCurrentActType();
    if (nRoot == 0x3E) {
        return;
    }
    if (JudgeBombShootEsp()) {
        return;
    }
    if (JudgeReboundOrReboundDunk()) {
        return;
    }
    if (ActInterrupAlleyoopforLuiMan_()) {
        return;
    }
    bcs::FVEC3 target;
    if (JudgeItemThrow(target)) {
        Reset();
        ActCallItemThrow(target);
        return;
    }

    const CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
    if (pHolder != 0 && pHolder != m_pChr) {
        if (DistXZ(pHolder->GetPos(), m_pChr->GetPos()) >= CComMan::GetDatF(0x312, GetComLvF())) {
            return;
        }
        int bFar = DistXZ(m_pChr->GetPos(), m_TargetGoalPos) > 7.5f;
        if (bFar && GetRootActType() == 0xB) {
            Reset();
            ActCallGoOffense2();
            return;
        }
        long nAct = GetCurrentActType();
        if (nAct != 0xB && nAct != 0xC) {
            Reset();
            ActCallGoOffense();
        }
    }
}

// Whether to shoot the stocked item now, by its kind.
void CComDrvBasketGame::ActInterruptItemShootCheck_()
{
    long nType = m_pChr->GetStockItemType();
    if (nType < 0) {
        m_fUnk1A8 = 0.0f;
        return;
    }
    if (!IsAllComTeam()) {
        return;
    }
    if (!CanbeActCallItemShoot()) {
        return;
    }
    if (GetRootActType() == 0x30) {
        return;
    }
    const float &fLv = GetComLvF();
    m_fUnk1A8 += m_pChr->GetFrameStep();
    if (m_fUnk1A8 < CComMan::GetDatI(0x193, fLv)) {
        return;
    }
    m_fUnk1A8 = 0.0f;
    if (m_pChr->GetComLevelWork() == 1) {
        return;
    }

    switch (nType) {
        case CItem::KIND_BOMB: {
            long nRate = CComMan::GetDatI(IsAllComTeam() ? 0x1F3 : 0x1FB, fLv);
            if (LotRate() > nRate) {
                return;
            }
            float fDist = CComManBasket::GetDistXZ(m_TargetGoalPos, m_MyPos);
            float fMin = CComMan::GetDatF(0x1E3, fLv);
            float fMax = CComMan::GetDatF(0x1EB, fLv);
            if (fDist < fMin || fDist > fMax) {
                return;
            }
            break;
        }
        case CItem::KIND_GSHELL: {
            float fMin = CComMan::GetDatF(0x19B, fLv);
            float fDist = CComManBasket::GetDistXZ(m_TargetGoalPos, m_MyPos);
            if (fMin > fDist) {
                return;
            }
            long nRate = CComMan::GetDatI(IsAllComTeam() ? 0x1A3 : 0x1AB, fLv);
            if (LotRate() > nRate) {
                return;
            }
            float fWidth = CComMan::GetDatF(0x1B3, fLv) * 0.5f;
            if (IsExistChrToTarget(m_TargetGoalPos, fWidth, fDist, GetEmTeamEnableBit())) {
                return;
            }
            break;
        }
        case CItem::KIND_RSHELL: {
            float fMin = CComMan::GetDatF(0x1BB, fLv);
            if (fMin > CComManBasket::GetDistXZ(m_TargetGoalPos, m_MyPos)) {
                return;
            }
            long nRate = CComMan::GetDatI(IsAllComTeam() ? 0x1C3 : 0x1CB, fLv);
            if (LotRate() > nRate) {
                return;
            }
            float fRange0 = CComMan::GetDatF(0x1D3, GetComLvF()) * 0.5f;
            float fRange1 = CComMan::GetDatF(0x1DB, GetComLvF()) * 0.5f;
            if (CheckItemShootDir(CItemBallBasket::GetCurveValue(CItem::KIND_RSHELL), fRange0, fRange1) == -1) {
                return;
            }
            break;
        }
        case CItem::KIND_TOADSTOOL: {
            float fMin = CComMan::GetDatF(0x22B, fLv);
            float fDist = CComManBasket::GetDistXZ(m_TargetGoalPos, m_MyPos);
            if (fMin > fDist) {
                return;
            }
            long nRate = CComMan::GetDatI(IsAllComTeam() ? 0x233 : 0x23B, fLv);
            if (LotRate() > nRate) {
                return;
            }
            float fWidth = CComMan::GetDatF(0x243, fLv) * 0.5f;
            if (IsExistChrToTarget(m_TargetGoalPos, fWidth, fDist, GetEmTeamEnableBit())) {
                return;
            }
            break;
        }
        case CItem::KIND_BANANA: {
            float fMin = CComMan::GetDatF(0x203, fLv);
            if (fMin > CComManBasket::GetDistXZ(m_TargetGoalPos, m_MyPos)) {
                return;
            }
            long nRate = CComMan::GetDatI(IsAllComTeam() ? 0x20B : 0x213, fLv);
            if (LotRate() > nRate) {
                return;
            }
            float fRange0 = CComMan::GetDatF(0x21B, GetComLvF()) * 0.5f;
            float fRange1 = CComMan::GetDatF(0x223, GetComLvF()) * 0.5f;
            if (CheckItemShootDir(CItemBallBasket::GetCurveValue(CItem::KIND_BANANA), fRange0, fRange1) == -1) {
                return;
            }
            break;
        }
        default:
            return;
    }
    Reset();
    ActCallItemShoot(-1);
}

// The other team has the ball.
void CComDrvBasketGame::ActInterruptDefense_()
{
    if (m_nUnk188) {
        int bHit = 0;
        CChrCore *pHolder = CComMan::GetHoldBallChrCore(0);
        if (pHolder != 0) {
            bHit = static_cast<CChrCoreBasket *>(m_pChr)->CutHitCheck(pHolder);
        }
        m_nUnk188 = bHit;
    }

    long nRoot = GetRootActType();
    GetCurrentActType();
    switch (nRoot) {
        case 0x39:
        case 0x3A:
        case 0x43:
        case 0x35:
            return;
    }
    if (nRoot == 0x3E) {
        return;
    }
    if (JudgeBombShootEsp()) {
        return;
    }
    if (JudgeSmashCut()) {
        return;
    }
    if (JudgeCut()) {
        return;
    }
    if (JudgeMissCut()) {
        return;
    }
    if (JudgeShootBlock()) {
        return;
    }
    if (JudgeAlleyoopPassBlock()) {
        return;
    }
    if (JudgeDunkBlock()) {
        return;
    }
    if (JudgeAlleyoopBlock()) {
        return;
    }
    if (JudgeJumpUpper()) {
        return;
    }
    if (JudgeJumpCut()) {
        return;
    }
    bcs::FVEC3 target;
    if (JudgeItemThrow(target)) {
        Reset();
        ActCallItemThrow(target);
        return;
    }
    if (nRoot == 0x11) {
        return;
    }
    if (!GetComManBK()->CanbeActCallGoDefOnBaller_(m_nMyTeam)) {
        return;
    }
    const CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
    if (pHolder == 0) {
        return;
    }
    long nState = pHolder->GetState();
    if (nState != 0x22 && nState != 0x28 && nState != 0x24 && nState != 0x26) {
        // Go for the holder when at least one computer team mate is near it
        // as well.
        float fRange = CComMan::GetDatF(0x432, GetComLvF());
        if (DistXZ(m_pChr->GetPos(), pHolder->GetPos()) > fRange) {
            return;
        }
        long nNum = 1;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            const CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == m_pChr->GetTeamNo() && pChr != m_pChr &&
                !(DistXZ(pChr->GetPos(), pHolder->GetPos()) > fRange) && CComManBasket::IsComDrv(pChr)) {
                nNum++;
            }
        }
        if (nNum >= 2) {
            Reset();
            ActCallGoDefOnBaller();
        }
    }
}

// In the star state: now and then go for a "?" panel (a green shell on
// the beach).
int CComDrvBasketGame::ActInterrupStateStar_()
{
    if (!(m_pChr->GetStatusFlag() & 0x20000)) {
        return 0;
    }
    float fLv = GetComLvF();
    long nWait = CComMan::GetDatI(0x563, fLv);
    m_fUnk1C0 += m_pChr->GetFrameStep();
    if (nWait > m_fUnk1C0) {
        return 0;
    }
    m_fUnk1C0 = 0.0f;
    long nRoot = GetRootActType();
    if (nRoot == 0x15 || nRoot == 0x49) {
        return 0;
    }
    long nRate = CComMan::GetDatI(0x55B, fLv) * GetHatenaPanelStageRate_();
    if (LotRate() > nRate) {
        return 0;
    }
    if (CGame::g_GameWork.GetStage() == 2) {
        if (!CanbeActGetGShell()) {
            return 0;
        }
        Reset();
        ActCallGetGShell();
    } else {
        if (!CanbeActGetHatenaPanel()) {
            return 0;
        }
        Reset();
        ActCallGetHatenaPanel();
    }
    return 1;
}

// On the bubble stage: get out of a bubble that is about to catch the
// driver.
int CComDrvBasketGame::ActInterrupGimmick_()
{
    if (CGame::g_GameWork.GetStage() == 11) {
        if (GetRootActType() == 0x4A) {
            return 1;
        }
        long nState = m_pChr->GetState();
        if (nState == 1 || nState == 0x19) {
            for (CChrObstacle *pObstacle = CComMan::ObstacleSearch(0); pObstacle != 0;
                 pObstacle = CComMan::ObstacleSearch(pObstacle)) {
                if (pObstacle->GetObstacleType() == 0xF) {
                    float fRange = 1.2f + pObstacle->GetAttackRadius();
                    if (DistXZ(pObstacle->GetPos(), m_pChr->GetPos()) > fRange) {
                        break;
                    }
                    Reset();
                    ActCallGoEspBubble(pObstacle->GetId());
                    return 1;
                }
            }
        }
    }
    return 0;
}

// Thrown items: get out of the way of the nearest one that will hit the
// driver.
int CComDrvBasketGame::ActInterrupCommonThrowItem_()
{
    float fLv = GetComLvF();
    long nRoot = GetRootActType();
    int bWait = 1;
    m_fUnk1B4 += m_pChr->GetFrameStep();
    switch (GetCurrentActType()) {
        case 0x1B:
        case 0x1D:
        case 0x1E:
        case 0x20:
        case 0x21:
        case 0x37:
        case 0x38:
        case 0x3A:
        case 0x3F:
            bWait = 0;
            break;
    }
    if (bWait && m_fUnk1B4 < CComMan::GetDatF(0x740, fLv)) {
        return nRoot == 0x3C;
    }

    CItem *pNear = 0;
    m_fUnk1B4 = 0.0f;
    if (m_pChr->GetComLevelWork() != 1) {
        const bcs::FVEC3 &rMyPos = m_pChr->GetPos();
        float fNearDist = 9999.9f;
        for (CItem *pItem = CComMan::ThrowItem_GetFirst(); pItem != 0; pItem = CComMan::ThrowItem_GetNext(pItem)) {
            float fDist = DistXZ(rMyPos, pItem->GetPos());
            if (fDist > fNearDist) {
                continue;
            }

            // Far enough from where it comes down: no danger.
            float fSafe = 0.0f;
            if (pItem->GetTouchDownFrame() > 0.0f) {
                switch (pItem->GetItemType()) {
                    case CItem::KIND_TOADSTOOL:
                        fSafe = CComMan::GetDatF(0x730, fLv);
                        break;
                    case CItem::KIND_BOMB:
                        fSafe = CComMan::GetDatF(0x720, fLv);
                        break;
                    case CItem::KIND_BANANA:
                        fSafe = CComMan::GetDatF(0x728, fLv);
                        break;
                }
            }
            bcs::FVEC3 target;
            if (!pItem->GetThrowTargetPos(target)) {
                target = pItem->GetPos();
            }
            bcs::FVEC3 d = Diff(rMyPos, target);
            if (PSVECMag((const Vec *)&d) > fSafe) {
                // In the way of its flight?
                float fWidth = 1.0f;
                float fLen;
                switch (pItem->GetItemType()) {
                    case CItem::KIND_GSHELL:
                        fWidth = CComMan::GetDatF(0x700, fLv);
                        fLen = CComMan::GetDatF(0x708, fLv);
                        break;
                    case CItem::KIND_RSHELL:
                        fWidth = CComMan::GetDatF(0x710, fLv);
                        fLen = CComMan::GetDatF(0x718, fLv);
                        break;
                    default:
                        fLen = DistXZ(target, pItem->GetPos());
                        target = pItem->GetPos();
                        break;
                }
                const bcs::FVEC3 &rVel = pItem->GetVelocity();
                bcs::FVEC3 vel(rVel.x, 0.0f, rVel.z);
                bcs::FVEC3 dir;
                bcs::BCS_Vec_Normalize(dir, vel);
                bcs::FVEC3 end;
                end.x = dir.x * fLen + target.x;
                end.y = 0.0f;
                end.z = dir.z * fLen + target.z;
                if (!CComMan::IsPointInFaceXZ(rMyPos, target, end, fWidth, fLen)) {
                    continue;
                }
            }
            fNearDist = fDist;
            pNear = pItem;
        }
    }

    if (pNear != 0) {
        long nRate = CComMan::GetDatF(0x748, fLv);
        if (LotRate() < nRate) {
            // What to do: jump shot, pass, jump, run away, run anywhere,
            // wait.
            long aRate[6];
            switch (pNear->GetItemType()) {
                case CItem::KIND_GSHELL:
                case CItem::KIND_RSHELL:
                    aRate[0] = CComMan::GetDatF(0x750, fLv);
                    aRate[1] = CComMan::GetDatF(0x758, fLv);
                    aRate[2] = CComMan::GetDatF(0x760, fLv);
                    aRate[3] = CComMan::GetDatF(0x768, fLv);
                    aRate[4] = CComMan::GetDatF(0x770, fLv);
                    aRate[5] = CComMan::GetDatF(0x778, fLv);
                    break;
                case CItem::KIND_TOADSTOOL:
                case CItem::KIND_BANANA:
                    aRate[0] = 0;
                    aRate[1] = 0;
                    aRate[2] = 0;
                    aRate[3] = CComMan::GetDatF(0x780, fLv);
                    aRate[4] = CComMan::GetDatF(0x788, fLv);
                    aRate[5] = CComMan::GetDatF(0x790, fLv);
                    break;
                default:
                    aRate[0] = 0;
                    aRate[1] = CComMan::GetDatF(0x798, fLv);
                    aRate[2] = 0;
                    aRate[3] = CComMan::GetDatF(0x768, fLv);
                    aRate[4] = CComMan::GetDatF(0x770, fLv);
                    aRate[5] = CComMan::GetDatF(0x778, fLv);
                    break;
            }
            if (m_pChr->IsHoldBall()) {
                aRate[2] = 0;
            } else {
                aRate[0] = 0;
                aRate[1] = 0;
            }

            long nAct = GetCurrentActType();
            long nLot = LotProbability(aRate, 6);
            int bGo;
            switch (nLot) {
                case 0:
                    bGo = 0;
                    if (nAct != 0x1D && nAct != 0x18) {
                        bGo = 1;
                    }
                    break;
                case 1:
                    bGo = 0;
                    if (nAct != 0x1D && nAct != 0x18) {
                        bGo = 1;
                    }
                    break;
                case 2:
                    bGo = nAct != 0x37;
                    break;
                default:
                    bGo = 1;
                    switch (nAct) {
                        case 0x18:
                        case 0x1D:
                        case 0x37:
                            bGo = 0;
                            break;
                    }
                    break;
            }
            if (bGo) {
                Reset();
                ActCallItemEscape();
                switch (nLot) {
                    case 0:
                        ActCallJumpShoot();
                        break;
                    case 1:
                        ActCallPass();
                        break;
                    case 2:
                        ActCallJump();
                        break;
                    case 3:
                        ActCallGoEscape(pNear->GetId());
                        break;
                    case 4:
                        ActCallGoRunRand();
                        break;
                    case 5: {
                        float fMin = CComMan::GetDatF(0x7B8, fLv);
                        ActCallWait(CComManBasket::GetRandValueF(fMin, CComMan::GetDatF(0x7C0, fLv)));
                        break;
                    }
                }
            }
        }
    }
    return nRoot == 0x3C;
}

// On Luigi's Mansion: now and then an alley-oop through the ghost.
int CComDrvBasketGame::ActInterrupAlleyoopforLuiMan_()
{
    if (CGame::g_GameWork.GetStage() != 6) {
        return 0;
    }
    m_fUnk1A4 -= m_pChr->GetFrameStep();
    if (m_fUnk1A4 > 0.0f) {
        return 0;
    }
    CNpcBase *pGhost = static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetGhost_(m_pChr->GetTeamNo());
    if (pGhost == 0) {
        return 0;
    }
    if (!pGhost->IsHoldBall()) {
        return 0;
    }
    float fLv = GetComLvF();
    m_fUnk1A4 = CComMan::GetDatF(0x1003, fLv);
    if (!CanbeAlleyoop()) {
        return 0;
    }
    long nRate = CComMan::GetDatF(0x100B, fLv);
    if (LotRate() >= nRate) {
        return 0;
    }
    Reset();
    ActCallAlleyoop();
    return 1;
}

// The jump ball: the team mate nearest to the ball jumps for it.
void CComDrvBasketGame::ThkMainJumpBall_()
{
    int bNearest = 1;
    CBall *pBall = CBallMan::GetBallPtr(0);
    float fDist = CComManBasket::GetDistXZ(m_pChr->GetPos(), pBall->GetPos());
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        const CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == m_nMyTeam && pChr != m_pChr &&
            fDist > CComManBasket::GetDistXZ(pChr->GetPos(), pBall->GetPos())) {
            bNearest = 0;
            break;
        }
    }
    if (bNearest) {
        ActCallJumpBall();
        return;
    }

    const float &fLv = GetComLvF();
    long aRate[2];
    aRate[0] = CComMan::GetDatI(0x4CA, fLv);
    aRate[1] = CComMan::GetDatI(0x4D2, fLv);
    switch (LotProbability(aRate, 2)) {
        case 0:
            ActCallGoRunRand();
            break;
        case 1: {
            long nMin = CComMan::GetDatI(0x4DA, fLv);
            ActCallWait(CComManBasket::GetRandValueI(nMin, CComMan::GetDatI(0x4E2, fLv)));
            break;
        }
    }
}

// The ball is loose: go for it, likelier the nearer the driver is to it
// among its team.
void CComDrvBasketGame::ThkMainLooseBall_()
{
    const float &fLv = GetComLvF();
    if (m_pChr->GetState() != 6) {
        CBall *pBall = CBallMan::GetBallPtr(0);
        const CChrCore *apChr[3] = {0, 0, 0};
        SORT aSort[32];
        SORT *pSort = aSort;
        const CChrCore **ppChr = apChr;
        long nNum = 0;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            const CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == m_nMyTeam && pChr->GetState() != 6) {
                float fDist = CComManBasket::GetDistXZ(pChr->GetPos(), pBall->GetPos());
                nNum++;
                *ppChr++ = pChr;
                pSort->m_nId = pChr->GetPlayerNo();
                pSort->m_fParam = fDist;
                pSort++;
            }
        }
        if (nNum != 1) {
            bcs::BCS_ShakerSort(aSort, nNum, bcs::CSimpleSortBase<32>::SortCmpParamF);
        }

        long nOrder = 0;
        for (long i = 0; i < nNum; i++) {
            if (aSort[i].m_nId == m_pChr->GetPlayerNo()) {
                nOrder = i;
                break;
            }
        }
        long aRate[3];
        aRate[0] = CComMan::GetDatI(0x49A, fLv);
        aRate[1] = CComMan::GetDatI(0x4A2, fLv);
        aRate[2] = CComMan::GetDatI(0x4AA, fLv);
        long nRate = aRate[nOrder];

        // More when the driver is the nearest to its own goal.
        if (nNum >= 2) {
            const bcs::FVEC3 &rGoal = m_pChr->GetGuardGoalPos();
            float fMyDist = CComManBasket::GetDistXZ(m_pChr->GetPos(), rGoal);
            int bNearest = 1;
            for (long i = 0; i < nNum; i++) {
                if (apChr[i] != m_pChr && fMyDist > CComManBasket::GetDistXZ(apChr[i]->GetPos(), rGoal)) {
                    bNearest = 0;
                    break;
                }
            }
            if (bNearest) {
                nRate += CComMan::GetDatI(0x4B2, fLv);
            }
        }
        if (LotRate() < nRate) {
            ActCallGetBall();
            return;
        }
    }

    long aRate[2];
    aRate[0] = CComMan::GetDatI(0x4BA, fLv);
    aRate[1] = CComMan::GetDatI(0x4C2, fLv);
    switch (LotProbability(aRate, 2)) {
        case 0:
            ActCallGoHome();
            break;
        case 1:
            ActCallGoRunRand();
            break;
    }
}

// The driver's team attacks.
void CComDrvBasketGame::ThkMainOffense_()
{
    if (IsCombination()) {
        ActCallCombination();
        return;
    }

    MEMBER_INFO info;
    long nNum = CalcHoldBallScore(CGame::g_GameWork.GetUserTeam(), info);
    const float &fLv = GetComLvF();
    long nMyScore = 0;
    for (long i = 0; i < nNum; i++) {
        if (info.m_apChr[i] == m_pChr) {
            nMyScore = info.m_aScore[i];
        }
    }
    int bAssist = CComMan::GetComDrv(m_pChr) == 0 ? m_pChr->IsMoveAssist() : 0;

    if (m_pChr->IsHoldBall()) {
        // Pass to the best placed computer team mate.
        const CChrCore *pBest = info.m_apChr[0];
        if (pBest != m_pChr && !bAssist && CComManBasket::IsComDrv(pBest) &&
            3.0f < Dist(m_pChr->GetPos(), pBest->GetPos())) {
            bcs::FVEC3 pos = CComMan::World2Local(pBest->GetPos(), -1);
            if (CComManBasketGame::IsTrainCrossLine(m_MyPos, pos)) {
                ActCallShootCancelPass(pBest);
            } else {
                ActCallPass(pBest);
            }
            return;
        }

        // Luigi's Mansion: pass to the ghost.
        if (CGame::g_GameWork.GetStage() == 6 && IsAllComTeam()) {
            CNpcGhostBasketBase *pGhost =
                static_cast<CNpcGhostBasketBase *>(static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetGhost_(m_pChr->GetTeamNo()));
            if (pGhost != 0 && pGhost->CanbePassCatch()) {
                long nRate = CComMan::GetDatI(0xFBB, fLv);
                if (LotRate() < nRate) {
                    ActCallPassGhost();
                    return;
                }
            }
        }

        // The special shot, close enough to the middle line.
        if (CanbeAirTrick() && CComManBasket::IsAllComTeam(m_pChr->GetTeamNo())) {
            float fZ = CComMan::GetDatF(0x103, fLv);
            bcs::FVEC3 pos(0.0f, 0.0f, m_TargetGoalPos.z > 0.0f ? fZ : -fZ);
            float fDist = CComManBasket::GetDistXZ(m_pChr->GetPos(), pos);
            if (fDist <= CComMan::GetDatF(0x10B, fLv)) {
                long nRate = CComMan::GetDatI(0xFB, GetComLvF());
                if (LotRate() < nRate) {
                    ActCallAirTrick();
                    return;
                }
            }
        }

        if (GetBaseShootPointToGimmick() + CComManBasketGame::GetBaseShootPoint() > nMyScore) {
            if (!JudgeCombinationPlay() || !MethdCombination()) {
                ActCallBallKeep();
            }
            return;
        }

        if (!bAssist) {
            long nRate = CComMan::GetDatI(0xAC, GetChrTypeID());
            switch (CGame::g_GameWork.GetStage()) {
                case 5:
                    switch (GetDCruise3PointAreaState()) {
                        case -1:
                            nRate = nRate * CComMan::GetDatF(0xF5B, fLv);
                            break;
                        case 1:
                            nRate = nRate * CComMan::GetDatF(0xF53, fLv);
                            break;
                    }
                    break;
                case 7: {
                    CNpcBossPackun *pBoss = static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetBossPackun_(m_nUnk8C);
                    if (pBoss != 0) {
                        if (pBoss->GetState() == 0) {
                            nRate = nRate * CComMan::GetDatF(0xE4F, fLv);
                        } else if (pBoss->IsMouthOpen()) {
                            nRate = nRate * CComMan::GetDatF(0xE47, fLv);
                        } else {
                            nRate = nRate * CComMan::GetDatF(0xE3F, fLv);
                        }
                    }
                    break;
                }
            }
            if (LotRate() < nRate) {
                ActCallGoDunkArea();
                return;
            }
        }
        if (CanbeActCallJumpShoot()) {
            ActCallJumpShoot();
        }
        return;
    }

    // Luigi's Mansion: ask a team mate near the ghost for the ball.
    if (CGame::g_GameWork.GetStage() == 6 && IsAllComTeam() && info.m_apChr[0] == m_pChr) {
        CNpcBase *pGhost = static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetGhost_(m_pChr->GetTeamNo());
        if (pGhost != 0) {
            long nRate = CComMan::GetDatI(0xFE3, fLv);
            if (pGhost->IsHoldBall() && LotRate() < nRate) {
                float fRange = CComMan::GetDatF(0xFDB, fLv);
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    const CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() != m_pChr->GetTeamNo() &&
                        DistXZ(pChr->GetPos(), pGhost->GetPos()) < fRange && MethdPassRequest()) {
                        return;
                    }
                }
            }
        }
    }

    long nRate = CComMan::GetDatI(0x288, fLv) * GetHatenaPanelStageRate_();
    if (LotRate() < nRate) {
        if (CGame::g_GameWork.GetStage() == 2) {
            if (CanbeActGetGShell()) {
                ActCallGetGShell();
                return;
            }
        } else if (CComMan::QPanel_GetNear(m_MyPos, 1) && CanbeActGetHatenaPanel()) {
            ActCallGetHatenaPanel();
            return;
        }
    }
    if (SubThkMainOffenseGoDunkAreaforLuiman_()) {
        return;
    }

    if (DistXZ(m_pChr->GetPos(), m_TargetGoalPos) > 7.5f) {
        unsigned long nBits = GetEmTeamEnableBit();
        float fDist = DistXZ(m_MyPos, m_TargetGoalPos);
        if (!CComManBasket::IsExistTarget(m_MyPos, m_TargetGoalPos, CComMan::GetDatF(0x298, GetComLvF()), fDist, nBits)) {
            long nRate2 = CComMan::GetDatI(0x2A0, GetComLvF());
            if (LotRate() < nRate2) {
                ActCallGoOffense2();
                return;
            }
        }
    }
    long nRate3 = CComMan::GetDatI(0x290, fLv);
    if (LotRate() < nRate3) {
        ActCallGoOffense();
        return;
    }
    ActCallGoRunRand();
}

// The driver's team attacks in the star state.
void CComDrvBasketGame::ThkMainOffenseStar_()
{
    const float &fLv = GetComLvF();
    if (m_pChr->IsHoldBall()) {
        // Shoot the item, likelier the more of the star state is left.
        long nStar = m_pChr->GetStarRemainderRate();
        if (nStar != -1 && CanbeActCallItemShoot()) {
            long nHi = CComMan::GetDatI(0x56B, fLv);
            long nLo = CComMan::GetDatI(0x573, fLv);
            if (nHi < nLo) {
                long nTmp = nHi;
                nHi = nLo;
                nLo = nTmp;
            }
            long nRate = nLo + (nStar * (nHi - nLo)) / 100.0f;
            if (LotRate() < nRate) {
                ActCallItemShoot(-1);
                return;
            }
        }
        long nRate = CComMan::GetDatI(0x57B, fLv);
        if (LotRate() < nRate) {
            ActCallGoEnemy();
        } else {
            ActCallGoRunRand();
        }
        return;
    }

    if (SubThkMainOffenseGoDunkAreaforLuiman_()) {
        return;
    }
    long nRate = CComMan::GetDatI(0x58B, fLv);
    if (LotRate() < nRate) {
        ActCallGoOffense();
        return;
    }
    nRate = CComMan::GetDatI(0x583, fLv);
    if (LotRate() < nRate) {
        ActCallGoEnemy();
        return;
    }
    ActCallGoRunRand();
}

// The other team attacks.
void CComDrvBasketGame::ThkMainDefense_()
{
    const float &fLv = GetComLvF();

    // Luigi's Mansion: go after the other team's ghost when it holds the
    // ball, likelier the fewer team mates are nearer to it.
    if (CGame::g_GameWork.GetStage() == 6) {
        CNpcBase *pGhost = CChrManBasket::GetInstance()->GetGhost_(1 - m_pChr->GetTeamNo());
        if (pGhost != 0 && pGhost->IsHoldBall()) {
            long nNear = 0;
            const long aId[3] = {0x1013, 0x101B, 0x1023};
            float fMyDist = DistXZ(m_pChr->GetPos(), pGhost->GetPos());
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                const CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable() && pChr != m_pChr && pChr->GetTeamNo() == m_pChr->GetTeamNo() &&
                    DistXZ(pChr->GetPos(), pGhost->GetPos()) < fMyDist) {
                    nNear++;
                }
            }
            long nRate = CComMan::GetDatI(aId[nNear], fLv);
            if (LotRate() < nRate) {
                ActCallDefGhost();
                return;
            }
        }
    }

    // The team mates by their distance from the goal they guard.
    const CChrCore *apChr[3];
    SORT aSort[32];
    long nNum = CComManBasketGame::GetMemBer(m_nMyTeam, apChr, 3, -1, 0);
    for (long i = 0; i < nNum; i++) {
        const CChrCore *pChr = apChr[i];
        float fDist = 0.0f;
        if (pChr->GetState() == 6) {
            fDist += 9999.9f;
        }
        fDist += CComManBasket::GetDistXZ(m_GuardGoalPos, pChr->GetPos());
        aSort[i].m_nId = pChr->GetPlayerNo();
        aSort[i].m_fParam = fDist;
    }
    if (nNum != 1) {
        bcs::BCS_ShakerSort(aSort, nNum, bcs::CSimpleSortBase<32>::SortCmpParamF);
    }

    long nRate;
    if (aSort[0].m_nId == m_pChr->GetPlayerNo()) {
        nRate = CComMan::GetDatI(0x352, fLv);
    } else {
        nRate = CComMan::GetDatI(0x35A, fLv);
    }
    if (m_pMarkChr != 0 && m_pMarkChr->IsHoldBall()) {
        nRate = CComMan::GetDatI(0x34A, fLv);
    }
    nRate = nRate * GetHatenaPanelStageRate_();
    if (LotRate() < nRate) {
        if (CGame::g_GameWork.GetStage() == 2) {
            if (CanbeActGetGShell()) {
                ActCallGetGShell();
                return;
            }
        } else if (CComMan::QPanel_GetNear(m_MyPos, 1) && CanbeActGetHatenaPanel()) {
            ActCallGetHatenaPanel();
            return;
        }
    }

    bcs::FVEC3 home;
    GetHomePos(home);
    if (m_pMarkChr != 0 && m_pMarkChr->IsHoldBall()) {
        long nRate2 = CComMan::GetDatI(0x362, fLv);
        if (LotRate() < nRate2) {
            if (!IsActCallDefenseExt()) {
                ActCallGoDefense();
                return;
            }
            long aRate[3];
            aRate[0] = CComMan::GetDatI(0x36A, fLv);
            aRate[1] = CComMan::GetDatI(0x372, fLv);
            aRate[2] = CComMan::GetDatI(0x37A, fLv);
            switch (LotProbability(aRate, 3)) {
                case 0:
                    ActCallGoDefense();
                    return;
                case 1:
                    ActCallGoDefense2();
                    return;
                case 2:
                    ActCallGoDefense3();
                    return;
            }
        }
    } else {
        if (CChrMan::Util_SearchHoldBallChr(0) != 0) {
            long nRate2 = CComMan::GetDatI(0x38A, GetComLvF());
            if (LotRate() < nRate2) {
                ActCallGoDefPassCourse();
                return;
            }
        }
        long nRate2 = CComMan::GetDatI(0x382, fLv);
        if (LotRate() < nRate2) {
            ActCallGoDefense();
            return;
        }
    }
    ActCallGoRunRand();
}

// The other team attacks while the driver is in the star state.
void CComDrvBasketGame::ThkMainDefenseStar_()
{
    const float &fLv = GetComLvF();
    // The pHolder != 0 re-test is redundant; kept as a guess at the original (its duplicate beq), and it keeps this function out of line in ThkMain.
    CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
    if (pHolder == 0 || (pHolder != 0 && !(pHolder->GetStatusFlag() & 0x20000) && pHolder->GetTeamNo() != m_nMyTeam)) {
        long nRate = CComMan::GetDatI(0x593, fLv);
        if (LotRate() < nRate) {
            ActCallGoBall();
            return;
        }
    }
    long nRate = CComMan::GetDatI(0x59B, fLv);
    if (LotRate() < nRate) {
        ActCallGoEnemy();
        return;
    }
    ActCallGoRunRand();
}

// Luigi's Mansion: go to the dunk area, likelier the fewer team mates are
// nearer to the goal.
int CComDrvBasketGame::SubThkMainOffenseGoDunkAreaforLuiman_()
{
    if (CGame::g_GameWork.GetStage() != 6) {
        return 0;
    }
    if (!IsAllComTeam()) {
        return 0;
    }
    long nNear = 0;
    const long aId[3] = {0xFEB, 0xFF3, 0xFFB};
    float fMyDist = DistXZ(m_pChr->GetPos(), m_pChr->GetTargetGoalPos());
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        const CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr != m_pChr && pChr->GetTeamNo() == m_pChr->GetTeamNo() &&
            DistXZ(pChr->GetPos(), m_pChr->GetTargetGoalPos()) < fMyDist) {
            nNear++;
        }
    }
    long nRate = CComMan::GetDatI(aId[nNear], GetComLvF());
    if (LotRate() < nRate) {
        ActCallGoDunkArea();
        return 1;
    }
    return 0;
}

}
