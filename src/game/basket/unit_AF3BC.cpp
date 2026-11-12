// msw::CComDrvBasketGame's frame and message handlers (OnMain, OnInit,
// OnGameMsgProc). A file of its own, with its own float constants; the file
// name is not known. The shape follows DodgeF's game/dodge/unit_A40EC.cpp.
#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComDrvBasketGame.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManBasket.hpp"
#include "game/msw/CComManBasketGame.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItemBall.hpp"
#include "game/msw/CItemBallBasket.hpp"

namespace unit_AF3BC_cpp {
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
    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (names guessed) An angle brought into [0, 2 pi), and into (-pi, pi].
    inline float RoundRad(float fRad)
    {
        while (fRad > PI2) {
            fRad -= PI2;
        }
        while (fRad < 0.0f) {
            fRad += PI2;
        }
        return fRad;
    }

    inline float RoundRadPI(float fRad)
    {
        fRad = RoundRad(fRad);
        if (fRad > PI) {
            fRad -= PI2;
        }
        return fRad;
    }

    // (guessed helper) rPos1 - rPos2.
    inline bcs::FVEC3 Diff(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 diff;
        return *bcs::FVEC3Sub(&diff, &rPos1, &rPos2);
    }

    // (guessed helper) The length of rVec in the XZ plane.
    inline float LenXZ(const bcs::FVEC3 &rVec)
    {
        bcs::FVEC3 xz(rVec.x, 0.0f, rVec.z);
        return PSVECMag((const Vec *)&xz);
    }

    // (guessed helper) The distance of two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 d = Diff(rPos1, rPos2);
        return LenXZ(d);
    }

    // (guessed helper) How far the character has to turn to face rPos,
    // in (-pi, pi].
    inline float GetTurnToPos(const msw::CChrCore *pChr, const bcs::FVEC3 &rPos)
    {
        float fRot = pChr->GetRotY();
        return RoundRadPI(pChr->GetRotY_ToPosXZ(rPos) - fRot);
    }

    // The work of the actions that turn to a point (actions 0xB and 0xC).
    struct ACT_WORK_LOOK : msw::CComDrv::ACT_WORK {
        unsigned char m_Unk1C[0x20 - 0x1C];
        bcs::FVEC3 m_Pos; // 0x20, in the court's frame
    };

    // The work of action 0x21: its first own word is its answer to message
    // 0x2C.
    struct ACT_WORK_ANSWER : msw::CComDrv::ACT_WORK {
        long m_nAnswer; // 0x1C
    };
}

namespace msw {

void CComDrvBasketGame::BeforeMsgFunc(MSG_ID id, _CHR_MSG *pMsg)
{
    CComDrvBasket::BeforeMsgFunc(id, pMsg);
}

void CComDrvBasketGame::EndMsgFunc(MSG_ID id, _CHR_MSG *pMsg)
{
    CComDrvBasket::EndMsgFunc(id, pMsg);
}

long CComDrvBasketGame::OnMain(_CHR_MSG *pMsg)
{
    return OnMainSub_(pMsg);
}

// Every frame: counts the timers down, then, unless the character is busy,
// scores the panels of the court and runs the action in progress (or
// thinks of the next one).
long CComDrvBasketGame::OnMainSub_(_CHR_MSG *pMsg)
{
    CComDrv::OnMain(pMsg);

    if (m_fUnk1BC > 0.0f) {
        m_fUnk1BC -= m_pChr->m_fFrameStep;
        if (m_fUnk1BC < 0.0f) {
            m_fUnk1BC = 0.0f;
        }
    }
    if (m_pChr->IsHoldBall() || !m_pChr->IsHaveStockItem()) {
        m_fUnk1B0 = 0.0f;
    }
    if (m_fUnk1D4 > 0.0f) {
        m_fUnk1D4 -= m_pChr->m_fFrameStep;
        if (m_fUnk1D4 < 0.0f) {
            m_fUnk1D4 = 0.0f;
        }
    }
    if (m_fUnk190 > 0.0) {
        m_fUnk190 -= m_pChr->m_fFrameStep;
        if (m_fUnk190 < 0.0f) {
            m_fUnk190 = 0.0f;
        }
    }
    m_fUnk1C4 -= m_pChr->m_fFrameStep;
    if (m_fUnk1C4 <= 0.0f) {
        m_fUnk1C8 = -1.0f;
        m_fUnk1C4 = 30.0f;
    }

    m_nFlag17C &= ~0x400;
    if (m_pChr->IsHoldBall()) {
        long nWait = CComMan::GetDatI(0x263, GetComLvF());
        m_fUnk1A0 += m_pChr->m_fFrameStep;
        if (m_fUnk1A0 >= nWait) {
            float fScale = CComMan::GetDatF(0x273, (long)m_pChr->GetChrTypeID());
            long nRate = CComMan::GetDatI(0x26B, GetComLvF());
            if (LotRate() < (long)(nRate * fScale)) {
                m_nFlag17C |= 0x400;
            }
        }
    } else {
        m_fUnk1A0 = 0.0f;
    }

    CChrCore *pChr = m_pChr;
    if (pChr->GetFlagEC() & 0x68000000) {
        return 0;
    }
    if (static_cast<CChrCoreBasket *>(pChr)->m_nBasketFlag & 0x80) {
        return 0;
    }
    switch (pChr->GetState()) {
        case CChrCore::STATE_BLOCK_CUT:
        case CChrCore::STATE_DROP_CUT:
        case CChrCore::STATE_PASS:
        case CChrCore::STATE_AERIAL_PASS:
        case CChrCore::STATE_PASS_CATCH:
        case CChrCore::STATE_ARTS_PASS:
        case CChrCore::STATE_REBOUND_DUNK:
        case CChrCore::STATE_LAY_UP:
        case CChrCore::STATE_AIR_TRICK:
            return 0;
        case CChrCore::STATE_DAMAGE:
            Reset();
            return 0;
        case CChrCore::STATE_SWAT_IT:
            m_nFlag17C &= ~0x8;
            return 0;
    }

    // On the street stage the panel the ball holder stands on counts.
    if (CGame::g_GameWork.GetStage() == 10) {
        CStage10Base::CPanelInfoBase *pRide = pChr->IsHoldBall() ? CComManBasketGame::GetJStreetPanelRide() : 0;
        if (m_nFlag17C & 0x100) {
            if (!pRide) {
                m_nFlag17C &= ~0x180;
            }
        } else if (pRide) {
            m_nFlag17C |= 0x180;
        }
    }

    ActInterrupt();

    // Score the panels: higher the nearer the goal.
    ClearPanelData();
    ClearPanelDisableFlg();
    const bcs::FVEC3 &rGoal = m_pChr->GetTargetGoalPos();
    float fMax = -9999.0f;
    float fMin = 9999.0f;
    float fLeft, fRight, fTop, fBottom;
    GetFreeAreaRect(fLeft, fRight, fTop, fBottom);
    bcs::FVEC3 aCorner[4] = {
        bcs::FVEC3(fLeft, 0.0f, fTop),
        bcs::FVEC3(fRight, 0.0f, fTop),
        bcs::FVEC3(fRight, 0.0f, fBottom),
        bcs::FVEC3(fLeft, 0.0f, fBottom),
    };
    for (long i = 0; i < 4; i++) {
        float fDis = DistXZ(rGoal, aCorner[i]);
        if (fDis < fMin) {
            fMin = fDis;
        }
        if (fDis > fMax) {
            fMax = fDis;
        }
    }
    long nLeft, nRight, nTop, nBottom;
    GetFreeAreaRect(nLeft, nRight, nTop, nBottom);
    float fMyDis = DistXZ(rGoal, m_pChr->GetPos());
    float fRange = fMax - fMin;
    for (long w = nLeft; w <= nRight; w++) {
        for (long h = nTop; h <= nBottom; h++) {
            float fX0, fX1, fZ0, fZ1;
            GetPanelRect(w, h, fX0, fX1, fZ0, fZ1);
            bcs::FVEC3 center((fX1 - fX0) * 0.5f + fX0, 0.0f, (fZ1 - fZ0) * 0.5f + fZ0);
            float fRate = 1.0f - (DistXZ(rGoal, center) - fMin) / fRange;
            if (fRate > 1.0f) {
                fRate = 1.0f;
            } else if (fRate < 0.0f) {
                fRate = 0.0f;
            }
            SetPanelDate(w, h, GetPanelDate(w, h) + 100.0f * fRate);
        }
    }

    // ... and lower next to the other characters.
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        const CChrCore *pOther = CChrMan::GetChrCore(i);
        if (pOther && !pOther->IsDisable() && pOther != m_pChr) {
            long nW = GetPanelWIdx(pOther->GetPos().x);
            long nH = GetPanelHIdx(pOther->GetPos().z);
            for (long h = nH - 1; h <= nH + 1; h++) {
                for (long w = nW - 1; w <= nW + 1; w++) {
                    if (w == nW && h == nH) {
                        SetPanelDate(w, h, 0);
                    } else {
                        signed char nDate = GetPanelDate(w, h) - 50;
                        if (nDate < 0) {
                            nDate = 0;
                        }
                        SetPanelDate(w, h, nDate);
                    }
                }
            }
        }
    }

    if (m_nFlag17C & 0x20) {
        m_fUnk1CC += m_pChr->m_fFrameStep;
        if (m_fUnk1CC >= CComMan::GetDatF(0x53B, GetComLvF())) {
            m_fUnk1CC = 0.0f;
            m_nFlag17C &= ~0x20;
        }
    } else {
        m_fUnk1CC += m_pChr->m_fFrameStep;
        if (m_fUnk1CC >= CComMan::GetDatF(0x543, GetComLvF())) {
            m_fUnk1CC = 0.0f;
            m_nFlag17C |= 0x20;
        }
    }
    if (m_nFlag17C & 0x40) {
        m_fUnk1DC += m_pChr->m_fFrameStep;
        if (m_fUnk1DC >= CComMan::GetDatF(0xFAB, GetComLvF())) {
            m_fUnk1DC = 0.0f;
            m_nFlag17C &= ~0x40;
        }
    } else {
        m_fUnk1DC += m_pChr->m_fFrameStep;
        if (m_fUnk1DC >= CComMan::GetDatF(0xFB3, GetComLvF())) {
            m_fUnk1DC = 0.0f;
            m_nFlag17C |= 0x40;
        }
    }

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

void CComDrvBasketGame::OnConnect(_CHR_MSG *pMsg)
{
    CComDrvBasket::OnConnect(pMsg);
}

void CComDrvBasketGame::OnDisconnect(_CHR_MSG *pMsg)
{
    CComDrvBasket::OnDisconnect(pMsg);
}

long CComDrvBasketGame::OnInit(_CHR_MSG *pMsg)
{
    long nRet = CComDrvBasket::OnInit(pMsg);
    InitScript();
    return nRet;
}

// The game's questions and news. (message numbers only; their names are
// not known)
long CComDrvBasketGame::OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = CComDrvBasket::OnGameMsgProc(id, pMsg);
    switch (id) {
        case 0x12:
            Reset();
            break;
        case 0x4:
            Reset();
            m_nUnk188 = 0;
            break;
        case 0x18:
            m_nFlag17C |= 0x800;
            Reset();
            break;
        case 0x9:
            Reset();
            m_nFlag17C &= ~0x800;
            break;
        case 0xE: {
            // Stop what the driver is doing, unless it is turning to a point
            // in front of it.
            int bReset = 1;
            int bMine = pMsg->m_nParam1 == m_pChr->GetPlayerNo();
            switch (GetRootActType()) {
                case 0xB:
                    if (bMine && GetCurrentActType() == 0xB) {
                        bcs::FVEC3 pos = CComMan::Local2World(static_cast<ACT_WORK_LOOK *>(GetCurrentActWork())->m_Pos, -1);
                        if (fabsf(GetTurnToPos(m_pChr, pos)) < PI / 2.0f) {
                            bReset = 0;
                        }
                    }
                    break;
                case 0xC:
                    if (bMine && GetCurrentActType() == 0xC) {
                        bcs::FVEC3 pos = CComMan::Local2World(static_cast<ACT_WORK_LOOK *>(GetCurrentActWork())->m_Pos, -1);
                        if (fabsf(GetTurnToPos(m_pChr, pos)) < PI / 2.0f) {
                            bReset = 0;
                        }
                    }
                    break;
            }
            if (bReset) {
                Reset();
                if (bMine) {
                    long nMin = CComMan::GetDatI(0x54B, GetComLvF());
                    long nMax = CComMan::GetDatI(0x54B, GetComLvF());
                    if (nMin > 0 && nMax > 0) {
                        ActCallWaitRand(nMin, nMax);
                    }
                }
            }
            break;
        }
        case 0x6:
            switch (GetRootActType()) {
                case 0xB:
                case 0xC:
                case 0x3B:
                    break;
                default:
                    Reset();
                    break;
            }
            break;
        case 0x7:
            if (pMsg->m_nParam0 == m_pChr->GetPlayerNo()) {
                m_fUnk1D4 = CComMan::GetDatI(0xA0, GetComLvF());
            }
            MsgSomeBodyShoot(pMsg);
            break;
        case 0x13: {
            int bFlag = (m_nFlag17C & 0x800) != 0;
            m_nUnk188 = 0;
            long nAct = GetRootActType();
            if (nAct == 0x20 || nAct == 0x34 || nAct == 0x38 || nAct == 0x3C) {
                if (bFlag) {
                    Reset();
                }
            } else {
                Reset();
                m_nFlag17C &= ~0x1C;
            }
            m_nFlag17C &= ~0x800;
            break;
        }
        case 0x14:
            m_nFlag17C &= ~0x1C;
            if (GetRootActType() != 0x3C) {
                Reset();
            }
            break;
        case 0x10:
            if (pMsg->m_nParam0 >= 0) {
                m_fUnk1BC = CComMan::GetDatI(0x513, GetComLvF());
            }
            break;
        case 0x2D:
            // Is the way of the shot free?
            switch (m_pChr->GetState()) {
                case CChrCore::STATE_JUMP_SHOOT: {
                    float fRange0 = -1.0f;
                    float fRange1 = -1.0f;
                    float fCurve = 0.0f;
                    CBall *pBall = m_pChr->GetAttachBall();
                    if (pBall) {
                        if (static_cast<CBallBasket *>(pBall)->GetBasketFlag() & 0x2000) {
                            CChrCoreBasket *pChr = static_cast<CChrCoreBasket *>(m_pChr);
                            if (pChr->IsJustShootTypeCurve()) {
                                fRange0 = fRange1 = 1.0f;
                                fCurve = pChr->GetJustShootCurveValue();
                            }
                        } else {
                            CItemBall *pItem = pBall->GetItemEffect_();
                            if (pItem) {
                                switch (pItem->GetType()) {
                                    case 5:
                                        fRange0 = CComMan::GetDatF(0x21B, GetComLvF()) * 0.5f;
                                        fRange1 = CComMan::GetDatF(0x223, GetComLvF()) * 0.5f;
                                        break;
                                    case 1:
                                        fRange0 = CComMan::GetDatF(0x1D3, GetComLvF()) * 0.5f;
                                        fRange1 = CComMan::GetDatF(0x1DB, GetComLvF()) * 0.5f;
                                        break;
                                }
                                fCurve = CItemBallBasket::GetCurveValue(pItem->GetType());
                            }
                        }
                    }
                    if (fRange0 != -1.0f) {
                        const CChrCore *apChr[3];
                        long nNum = CComManBasketGame::GetMemberNoBadCondition(1 - m_nMyTeam, apChr, 3, -1, 0);
                        nRet = CheckFreeShootCourseToCurve(nNum, apChr, fCurve, fRange0, fRange1);
                    }
                    break;
                }
                case CChrCore::STATE_AIR_TRICK:
                    nRet = CheckArtsShootDir();
                    break;
            }
            break;
        case 0x15:
            // A character of the other team may cut: maybe step aside.
            if (pMsg && CanbeActCutEsp()) {
                CChrCore *pEm = GetChrCore(pMsg->m_nParam2);
                if (pEm && pEm->GetTeamNo() != m_pChr->GetTeamNo()) {
                    float fLv = GetComLvF();
                    int bEsc = 0;
                    float fDis = CComManBasket::GetDistXZ(m_pChr, pEm);
                    if (fDis < CComMan::GetDatF(0x24B, fLv)) {
                        long nRate = CComMan::GetDatI(0x253, fLv);
                        bEsc = LotRate() < nRate;
                    }
                    if (!bEsc && static_cast<CChrCoreBasket *>(pEm)->CutHitCheck(m_pChr)) {
                        long nRate = CComMan::GetDatI(0x25B, fLv);
                        bEsc = LotRate() < nRate;
                    }
                    if (bEsc) {
                        float fDeg = RandRangeI(9, 18) * 10.0f;
                        if (RandMaxI(1)) {
                            fDeg = -fDeg;
                        }
                        float fRad = bcs::DEG_TO_RAD(fDeg);
                        float fRot = RoundRad(fRad + GetRotY_P1ToP2XZ(m_pChr, pEm));
                        bcs::FVEC3 dir;
                        bcs::BCS_VecZ_RotY(dir, 1.0f, fRot);
                        float fLen = RandRangeF(2.0f, 3.0f);
                        dir.x = fLen * dir.x + m_MyPos.x;
                        dir.y = 0.0f;
                        dir.z = fLen * dir.z + m_MyPos.z;
                        Reset();
                        bcs::FVEC3 target;
                        bcs::FVEC3Sum(target, dir, m_MyPos);
                        ActCallGoRun(target, GetAttainmentMoveMinDist(), 300.0f, 0);
                    }
                }
            }
            break;
        case 0x1A:
            if (pMsg) {
                nRet = LotRateJumpShoot(pMsg->m_nParam0, pMsg->m_nParam1, pMsg->m_nParam2);
            }
            break;
        case 0x2B:
            nRet = RandMaxI(999) < 20;
            break;
        case 0x2C:
            nRet = 0;
            if (GetRootActType() == 0x21) {
                nRet = static_cast<ACT_WORK_ANSWER *>(GetCurrentActWork())->m_nAnswer;
            }
            break;
        case 0x20:
            nRet = LotRateLayUp();
            break;
        case 0x25:
            nRet = 0;
            if (CanbeJumpCut() && LotRateSwatIt()) {
                nRet = 1;
            }
            break;
        case 0x52:
            if (GetRootActType() != 0x18 && pMsg && pMsg->m_nParam0 != m_nMyChrIdx) {
                Reset();
                ActCallPass(CChrMan::GetChrCore(pMsg->m_nParam0));
            }
            break;
        case 0x2A:
            nRet = LotRateUseItem(pMsg->m_nParam0, pMsg->m_nParam1, pMsg->m_nParam2);
            break;
        case 0x2E:
            Reset();
            ActCallMoveBlock();
            break;
        case 0x2F:
            Reset();
            ActCallAssistPlay();
            break;
        case 0x30:
            MsgShootStart(pMsg);
            m_nFlag17C &= ~0x8000;
            break;
        case 0x31:
            MsgReboundStart(pMsg);
            break;
        case 0x21:
            if (MsgArtsShootRelease(pMsg)) {
                nRet = 4;
            }
            break;
        case 0x28:
            nRet = MsgArtsDirInput(pMsg);
            break;
        case 0x23:
            nRet = 1;
            break;
        case 0x32:
            Reset();
            break;
        case 0x33:
            if (m_nMyTeam != CGame::g_GameWork.GetUserTeam() && pMsg->m_nParam0 == 1) {
                Reset();
                ActCallScriptI("ActSpMoogleDef..I.", pMsg->m_nParam1);
            }
            break;
        case 0x26: {
            nRet = 0;
            float fWait = CComMan::GetDatF(0x10A3, GetComLvF());
            m_fUnk1FC += m_pChr->m_fFrameStep;
            if (!(m_fUnk1FC < fWait)) {
                m_fUnk1FC = 0.0f;
                long nLv = (long)(0.5f + GetComLvF());
                long nMin = CComMan::GetDatI(0x1093, nLv);
                long nMax = CComMan::GetDatI(0x109B, nLv);
                long nVal = CComManBasket::GetRandValueI(nMin, nMax);
                nRet = (long)(409.6f * (0.01f * nVal));
            }
            break;
        }
    }
    return nRet;
}

}
