// msw::CNpcGhostBasketBase: the ghost (Boo) of the basketball game's haunted
// court. It floats in front of the goal its team attacks, catches loose
// balls and shots, and passes them to its team; a hit makes it vanish for a
// while.
#include "game/msw/CNpcGhostBasketBase.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CComManBasketGame.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CStageBasket06.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/math/triangular.h"

// Set by the file's static initialiser and used by nothing in the file (see
// game/basket/CBallManBasket.cpp).
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

namespace msw {

    namespace {
        // The yaw ranges (radians, relative to the goal's yaw) of the four
        // areas in front of a goal, from one side to the other.
        const float s_fAreaRot[4][2] = {
            {-1.5707964f, -0.7853982f},
            {-0.7853982f, 0.0f},
            {0.0f, 0.7853982f},
            {0.7853982f, 1.5707964f},
        };

        // The angle brought into [0, 2 pi).
        inline float RoundRad(float fRad)
        {
            while (fRad > 6.2831855f) {
                fRad -= 6.2831855f;
            }
            while (fRad < 0.0f) {
                fRad += 6.2831855f;
            }
            return fRad;
        }

        // A random float between the two bounds, in either order.
        inline float RandRangeSort(float fMin, float fMax)
        {
            if (fMin > fMax) {
                float fTmp = fMin;
                fMin = fMax;
                fMax = fTmp;
            }
            return bcs::BCS_RandRange(fMin, fMax);
        }

        inline float Clamp(float fValue, float fMin, float fMax)
        {
            return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
        }

        // rTo - rFrom. (name guessed; the paired-single subtraction goes to
        // a temporary that is then copied out)
        inline bcs::FVEC3 VecSub(const bcs::FVEC3 &rTo, const bcs::FVEC3 &rFrom)
        {
            bcs::FVEC3 vSub;
            return *bcs::FVEC3Sub(&vSub, &rTo, &rFrom);
        }

        // Not zero for a character index.
        inline int IsChrIdx(long nChr)
        {
            int bResult = 0;
            if ((unsigned long)nChr <= 5) {
                bResult = 1;
            }
            return bResult;
        }
    }

    inline long CNpcGhostBasketBase::GetArea_() const
    {
        const bcs::FVEC3 &rGoal = CGame::GetGoalPos(GetTargetGoalIdx());
        float fGoalRot = CGame::GetGoalYRot(GetTargetGoalIdx());
        float fRot = RoundRad(GetRotY_FromPosXZ(rGoal) - fGoalRot);
        if (fRot > 3.1415927f) {
            fRot -= 6.2831855f;
        }
        for (long i = 0; i < 4; i++) {
            if (s_fAreaRot[i][0] <= fRot && s_fAreaRot[i][1] >= fRot) {
                return i;
            }
        }
        return 0;
    }

    // Defined here, first in the file, and expanded into CreateGhost_; the
    // out-of-line copy is stripped by the module link (nothing calls it).
    // Its 0.0f is the first float literal of the original's .rodata.
    CNpcGhostBasketBase::CNpcGhostBasketBase(CChrMan *pMan, long nType, long nTeam)
        : CNpcGhost(pMan, nType)
        , m_nFlag(0)
        , m_fPassCallWait(0.0f)
        , m_fFloatPhase(0.0f)
        , m_fDamageWait(0.0f)
    {
        m_nTeamNo = nTeam;
    }

    CNpcGhostBasketBase::~CNpcGhostBasketBase()
    {
    }

    void CNpcGhostBasketBase::Create()
    {
        CNpcGhost::Create();

        m_pBodyColl = new (0) CBodyCollision(4, 3, 0, 0);

        // Unit 0: pushes characters away.
        m_pBodyColl->SetPushEnable(0);
        m_pBodyColl->SetCollType(0, 2);
        m_pBodyColl->ResetTargetFlag(0, 0x14);
        m_pBodyColl->SetEnable(0);
        m_pBodyColl->SetBodyPushCallbackAll(CallbackBodyPush, this);

        // Unit 1: catches balls.
        m_pBodyColl->SetCatchEnable(1);
        m_pBodyColl->SetCollType(1, 2);
        m_pBodyColl->SetBallCatchCallback(1, CallbackBallCatch, this);
        m_pBodyColl->SetEnable(1);

        // Unit 2: catches shots in flight.
        m_pBodyColl->SetCatchEnable(2);
        m_pBodyColl->SetCollType(2, 0);
        m_pBodyColl->SetBallCatchCallback(2, CallbackBallCatch2, this);
        m_pBodyColl->SetEnable(2);

        // Unit 3: takes hits.
        m_pBodyColl->SetBodyEnable(3);
        m_pBodyColl->SetEnable(3);
        m_pBodyColl->SetCollType(3, 2);
        m_pBodyColl->SetTargetFlag(3, 6);
        m_pBodyColl->SetBodyDamageCallbackAll(CallbackBodyDamage, this);
    }

    void CNpcGhostBasketBase::Reset()
    {
        static const float fBaseDist = 3.0f;

        m_vPos = bcs::FVEC3(0.0f, 0.0f, GetTargetGoalIdx() == 0 ? -fBaseDist : fBaseDist);
        m_vRot.y = RoundRad(GetTargetGoalIdx() == 0 ? 0.0f : 3.1415927f);
        m_fFloatPhase = 0.0f;
        m_Unk214 = 0.0f;
        m_Unk218 = 0.0f;
        m_Unk21C = 0.0f;
        AnimPlay(1, 1, 8.0f, 0.0f);
        DetachBall();
        m_fAlpha = 0.0f;
        m_fDamageWait = 0.0f;
        m_fPassCallWait = 0.0f;
        m_nFlag = 0;
        ChangeStateSleep_(-1.0f);
    }

    inline int CNpcGhostBasketBase::CanbeGetLooseBall_(const CBall *pBall) const
    {
        if (pBall == 0) {
            return 0;
        }
        if (!CanbeCatch()) {
            return 0;
        }
        if (IsHoldBall()) {
            return 0;
        }
        switch (GetState()) {
        case STATE_DISAPPEARS:
        case STATE_APPEARANCE:
        case STATE_PASS:
        case STATE_GET_LOOSE_BALL:
        case STATE_MOT_PLAY:
        case STATE_CATCH:
        case STATE_SLEEP:
            return 0;
        }
        return IsBallNear_(pBall) != 0;
    }

    inline int CNpcGhostBasketBase::IsBallNear_(const CBall *pBall) const
    {
        if (!pBall->IsActive()) {
            return 0;
        }
        if (!IsLooseBallMode()) {
            return 0;
        }
        bcs::FVEC3 vDiff = VecSub(pBall->GetPos(), m_vPos);
        bcs::FVEC3 vXZ(vDiff.x, 0.0f, vDiff.z);
        return !(bcs::FVEC3Len(&vXZ) > BASKET_PRM_S06_LOOSE_BALL_GET_DIST);
    }

    inline int CNpcGhostBasketBase::CheckAppearance_()
    {
        int bWake;
        if (!CChrManBasket::GetInstance()->IsGhostAppearEnable()) {
            bWake = 0;
        } else if (GetState() == STATE_SLEEP) {
            bWake = FsmqGetWork<STATE_WORK_SLEEP>()->m_nFlag & 1;
        } else {
            bWake = 0;
        }
        if (!bWake) {
            return 0;
        }
        STATE_WORK_APPEARANCE *pWork = static_cast<STATE_WORK_APPEARANCE *>(FsmqSetState(STATE_APPEARANCE));
        pWork->m_fWait = 80.0f;
        pWork->m_vPos = GetMoveTgtPosRand();
        pWork->m_vPos.y = 0.0f;
        return 1;
    }

    void CNpcGhostBasketBase::ActInterrup()
    {
        if (CGame::m_pInstance->GetState() != 5) {
            return;
        }

        // The lights over the goal the ghost's team attacks drive it away.
        CStageBasket06 *pStage = static_cast<CStageBasket06 *>(CStageMan::GetInstance());
        if (pStage->Illumi_IsOn(GetTargetGoalIdx())) {
            ChangeStateDisappears_();
            return;
        }

        if (CheckAppearance_()) {
            return;
        }

        if (IsHoldBall()) {
            m_fPassCallWait -= GetFrameRateF();
            if (m_fPassCallWait <= 0.0f) {
                m_fPassCallWait = 0.0f;
                ChangeStatePass_(-1);
            }
        }

        CBall *pBall = CBallMan::GetBallPtr(0);
        if (CanbeGetLooseBall_(pBall)) {
            STATE_WORK_GET_LOOSE_BALL *pWork =
                static_cast<STATE_WORK_GET_LOOSE_BALL *>(FsmqSetState(STATE_GET_LOOSE_BALL));
            pWork->m_hBall = pBall->GetId();
        }
    }

    void CNpcGhostBasketBase::MainFunc()
    {
        static const float fMax = 60.0f;

        ActInterrup();

        if (m_fDamageWait > 0.0f) {
            m_fDamageWait -= GetFrameRateF();
            if (m_fDamageWait < 0.0f) {
                m_fDamageWait = 0.0f;
            }
        }

        FsmqExec();

        // Float up and down.
        float fSpMin = BASKET_PRM_S06_MOVE_Y_SP_MIN;
        float fSpMax = BASKET_PRM_S06_MOVE_Y_SP_MAX;
        float fHeight = BASKET_PRM_S06_MOVE_HEIGHT;
        if (fSpMin > fSpMax) {
            float fTmp = fSpMin;
            fSpMin = fSpMax;
            fSpMax = fTmp;
        }
        float fSpeed = bcs::BCS_RandRange(fSpMin, fSpMax);
        m_fFloatPhase += fSpeed * GetFrameRateF();
        if (m_fFloatPhase >= fMax) {
            m_fFloatPhase -= fMax;
        }
        float fRad = RoundRad(6.2831855f * (m_fFloatPhase / fMax));
        float fY = 0.5f * (fHeight - fHeight * nw4r::math::SinRad(fRad));
        m_pMdl->SetTransOffset(bcs::FVEC3(0.0f, fY, 0.0f));

        // Stay inside the court.
        bcs::FVEC3 vHit;
        bcs::FVEC3 vNormal;
        float fRadius = m_pBodyColl->GetRadius(0);
        bcs::FVEC3 vFrom = m_vPos;
        float fTime;
        if (CCollMan::DetectMapHit(vHit, vNormal, fTime, vFrom, vFrom, fRadius, 3, 0x1000000)) {
            m_vPos = vHit;
        }

        if (m_pMdl) {
            m_pMdl->m_vTrans = m_vPos;
            m_pMdl->SetRotXYZ(m_vRot.x, m_vRot.y, m_vRot.z);
            m_pMdl->UpdateFrame(GetFrameRateF());
            CalcRoot_();
        }
        CheckDisp_();
        UpDateCollBody_();
    }

    long CNpcGhostBasketBase::SendMessage(long nMsg, long nArg0, long nArg1, long nArg2, long nArg3)
    {
        switch (nMsg) {
        case 3:
        case 5:
        case 0xF:
            Reset();
            break;
        case 0x12:
            Reset();
            break;
        case 0xB:
            ChangeStateDisappears_();
            break;
        case 0x36:
            m_nFlag |= 1;
            break;
        case 0x24:
            switch (nArg0) {
            case 0:
            case 2:
                m_fDamageWait = 0.0f;
                m_nFlag &= ~1;
                break;
            case 3: {
                // A character of the ghost's team calls for the ball.
                CChrCore *pChr = CChrMan::GetChrCore(nArg1);
                if (pChr == 0) {
                    return 0;
                }
                if (pChr->GetTeamNo() != m_nTeamNo) {
                    return 0;
                }
                ChangeStatePass_(nArg1);
                break;
            }
            }
            break;
        }
        return CNpcGhost::SendMessage(nMsg, nArg0, nArg1, nArg2, nArg3);
    }

    bcs::CMultViewAlphaMdlObj *CNpcGhostBasketBase::NewMdlObj_(nw4r::g3d::ResMdl mdl, unsigned long flag)
    {
        CFgMdl *pMdl = new CFgMdl();
        if (pMdl) {
            pMdl->SetFadeDir(0.0f);
            pMdl->SetFadeAngle1(1.3962634f);
            pMdl->SetFadeAngle0(1.5707964f);
            pMdl->SetUnk218(1.0f);
            pMdl->SetAlphaTestNearRange(BASKET_PRM_S06_ALPHA_TEST_NEAR_RANG);
            pMdl->SetAlphaTestFarRange(BASKET_PRM_S06_ALPHA_TEST_FAR_RANG);
            pMdl->SetUnk214(0.0f);
            pMdl->SetExBlendMode(1);
            pMdl->CalcBoundingVolume();
            pMdl->Create(mdl, flag, 0);
            return pMdl;
        }
        return 0;
    }

    void CNpcGhostBasketBase::CallbackBallCatch2(CCollBodyUnit *pBody, CBall *pBall, void *pArg)
    {
        CNpcGhostBasketBase *pGhost = static_cast<CNpcGhostBasketBase *>(pArg);
        if (pGhost && !pBall->IsAttach()) {
            // Chase a shot of the ghost's own team.
            if (pGhost->CanbePursuit_() && pBall->IsShootMode()) {
                if (pGhost->m_nTeamNo == CChrMan::GetChrCore(pBall->GetShotChrIdx())->GetTeamNo()) {
                    STATE_WORK_PURSUIT *pWork =
                        static_cast<STATE_WORK_PURSUIT *>(pGhost->FsmqSetState(STATE_PURSUIT));
                    pWork->m_pBall = pBall;
                }
            }
        }
    }

    void CNpcGhostBasketBase::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (pOther->GetOwnerKind() == 1) {
            long nChr = pOther->m_nOwnerIdx;
            CChrCore *pChr = CChrMan::GetChrCore(nChr);
            if (pChr && (pChr->GetStatusFlag() & 0x20000)) {
                // A character dashing into the ghost knocks the ball out.
                CBall *pBall = GetAttachBall();
                if (pBall) {
                    pBall->SetChrReflectMode(nChr, 0.0f);
                    DetachBall();
                }
                ChangeStateDisappears_();
                return;
            }
        }

        if (pPush == 0) {
            return;
        }
        bcs::FVEC3 vFrom = m_vPos;
        bcs::FVEC3 vTo(vFrom.x + pPush->x, vFrom.y, vFrom.z + pPush->z);
        bcs::FVEC3 vHit;
        bcs::FVEC3 vNormal;
        float fTime;
        if (CCollMan::DetectMapHit(vHit, vNormal, fTime, vFrom, vTo, 0.0f, 3, 0)) {
            // Pushed into a wall: push the other one back.
            vNormal.x = vHit.x - vTo.x;
            vNormal.z = vHit.z - vTo.z;
            pOther->DoBodyPushCallback(pBody, &vNormal);
        } else {
            pOther->DoBodyPushCallback(pBody, 0);
        }
        m_vPos = vHit;
        if (m_pBodyColl) {
            vTo.x = vHit.x - vFrom.x;
            vTo.z = vHit.z - vFrom.z;
            m_pBodyColl->AddPosXZAll(vTo, vTo);
            m_pBodyColl->CalcAABBAll();
        }
    }

    void CNpcGhostBasketBase::CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (pBall && CanbeCatch() && !pBall->IsAttach()) {
            CGame::SetLooseBallMode(0);
            if (m_nTeamNo != CGame::g_GameWork.GetUserTeam()) {
                CChrManBasket::GetInstance()->SetClockOverTime_();
            }
            if (!pBall->IsArtsShootMode() && pBall->IsShootMode()) {
                CComManBasketGame::MsgShootCatchToGhost();
            }
            AttachBall(pBall, m_nBallNode, 8.0f, 0);
            CGameBasket::GetInstance()->SetOffenseTeam_(m_nTeamNo);
            m_fPassCallWait = 60.0f * BASKET_PRM_S06_PASS_CALL_WAIT_MINUTES;
            pBall->SetCollBodyRadius(BASKET_PRM_S06_DAMAGE_COLL_BALL_R);
            if (CanbeCatchState_()) {
                FsmqSetState(STATE_CATCH);
            }
        }
    }

    unsigned long CNpcGhostBasketBase::CallbackBallDamage_(CBall *pBall, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos)
    {
        if (BASKET_PRM_S06_DAMAGE_COLL_TYPE != 1 && BASKET_PRM_S06_DAMAGE_COLL_TYPE != 3) {
            return 0;
        }
        if (!IsHoldBall()) {
            return 0;
        }
        CheckDamage_(pBall, pAtk, rPos);
        return 1;
    }

    unsigned long CNpcGhostBasketBase::CallbackBodyDamage_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                           const bcs::FVEC3 &rPos)
    {
        if (BASKET_PRM_S06_DAMAGE_COLL_TYPE != 2 && BASKET_PRM_S06_DAMAGE_COLL_TYPE != 3) {
            return 0;
        }
        if (!IsHoldBall()) {
            return 0;
        }
        CheckDamage_(GetAttachBall(), pAtk, rPos);
        return 1;
    }

    // Expanded into both damage callbacks above; the out-of-line copy is
    // stripped by the module link (the original keeps only its two
    // constants, after the callbacks' literals).
    void CNpcGhostBasketBase::CheckDamage_(CBall *pBall, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos)
    {
        // static-order swap: declared in this order the values come out in
        // the original's order, but the "@0" suffix lands on fSpeedXZ
        // instead of fSpeedY (see notes/decisions.md).
        static const float fSpeedY = 0.13f;
        static const float fSpeedXZ = 0.04f;

        m_fDamageWait = 12.0f;
        DetachBall();
        long nChr = 0;
        CChrCore *pChr = 0;
        if (pAtk->GetOwnerKind() == 1) {
            nChr = pAtk->m_nOwnerNo;
            pChr = CChrMan::GetChrCore(nChr);
        }
        float fRotY = pChr ? pChr->GetRotY_FromPosXZ(m_vPos) : m_vRot.y;
        if (pBall) {
            static_cast<CBallBasket *>(pBall)->SetChrReflectMode(nChr, fRotY, fSpeedXZ, fSpeedY);
        }
        ChangeStateDisappears_();
    }

    // The idle motion: holding the ball or not, to the side the last
    // motion faced. Expanded where it is used; the out-of-line copy is
    // stripped by the module link (its 30.0f stays in .rodata here).
    void CNpcGhostBasketBase::PlayWaitMot_()
    {
        long nMot = AnimGetMotID();
        if (IsHoldBall()) {
            if (nMot != 9 && nMot != -9) {
                AnimPlay(AnimGetMotID() > 0 ? 9 : -9, 1, 30.0f, 0.0f);
            }
        } else if (nMot != 1 && nMot != -1) {
            AnimPlay(nMot > 0 ? 1 : -1, 1, 8.0f, 0.0f);
        }
    }

    // (name guessed) Eases fRate (0 to 1) so that it rises steeply first;
    // expanded into GetMoveTgtPosRand, the out-of-line copy is stripped by
    // the module link.
    float CNpcGhostBasketBase::CalcEaseRate_(float fRate)
    {
        return fRate * (1.0 + (1.0f - fRate));
    }

    void CNpcGhostBasketBase::UpDateCollBody_()
    {
        bcs::FVEC3 vPos;
        GetCompPos(vPos);
        m_pBodyColl->SetPosAll(vPos);

        float fRadius = BASKET_PRM_S06_PUSH1_COL_SIZE_R;
        float fHeight = BASKET_PRM_S06_PUSH1_COL_SIZE_HEIGHT;
        m_pBodyColl->SetYRot(3, m_vRot.y);
        m_pBodyColl->SetSize(3, fRadius, fHeight, fRadius);

        switch (GetState()) {
        case STATE_SLEEP:
        case STATE_DISAPPEARS:
        case STATE_APPEARANCE:
            m_pBodyColl->SetDisable(0);
            break;
        default: {
            m_pBodyColl->SetEnable(0);
            float fPushRadius = BASKET_PRM_S06_PUSH1_COL_SIZE_R;
            float fPushHeight = BASKET_PRM_S06_PUSH1_COL_SIZE_HEIGHT;
            m_pBodyColl->SetYRot(0, m_vRot.y);
            m_pBodyColl->SetSize(0, fPushRadius, fPushHeight, fPushRadius);
            m_pBodyColl->SetPushRank(0, 1);
            m_pBodyColl->SetPushPower(0, 0);
            break;
        }
        }

        m_pBodyColl->SetPos(1, vPos);
        m_pBodyColl->SetYRot(1, m_vRot.y);
        m_pBodyColl->SetSize(1, BASKET_PRM_S06_CATCH1_COL_R, BASKET_PRM_S06_CATCH1_COL_HEIGHT,
                             BASKET_PRM_S06_CATCH1_COL_R);

        switch (GetState()) {
        case STATE_IDLE:
        case STATE_MOVE: {
            bcs::FVEC3 vNode;
            GetVNodePos(1, vNode);
            m_pBodyColl->SetEnable(2);
            m_pBodyColl->SetPos(2, vNode);
            m_pBodyColl->SetRadius(2, BASKET_PRM_S06_CATCH2_COL_RADIUS);
            break;
        }
        default:
            m_pBodyColl->SetDisable(2);
            break;
        }
    }

    bcs::FVEC3 CNpcGhostBasketBase::GetMoveTgtPosRand() const
    {
        long nGoal = GetTargetGoalIdx();
        const bcs::FVEC3 &rGoal = CGame::GetGoalPos(nGoal);
        float fRot = CGame::GetGoalYRot(nGoal);

        // The chances of the four areas, by the area the ghost is in.
        long nProb[4][4] = {
            {0, 1, 2, 0},
            {1, 0, 1, 2},
            {2, 1, 0, 1},
            {0, 2, 1, 0},
        };
        long nArea = GetArea_();
        long nNext = bcs::BCS_LotProbability(nProb[nArea], 4);
        fRot += RandRangeSort(s_fAreaRot[nNext][0], s_fAreaRot[nNext][1]);

        bcs::FVEC3 vPos;
        CStageBasket06 *pStage = static_cast<CStageBasket06 *>(CStageMan::GetInstance());
        if (pStage->Illumi_IsOn(GetTargetGoalIdx())) {
            bcs::FVEC3 vDir;
            bcs::BCS_VecZ_RotY(vDir, 1.0f, fRot);
            float fRate = CalcEaseRate_(1.0f - Clamp(__fabsf(fRot) / 1.5707964f, 0.0f, 1.0f));
            float fRight = CStageMan::GetInstance()->GetTeamCourtRight(1 - nGoal) - BASKET_PRM_S06_RIGHT_OFF_MOVE_R2;
            float fRange = fRate * (__fabsf(rGoal.z) - BASKET_PRM_S06_RIGHT_OFF_MOVE_R2);
            if (fRight > 0.0f && fRight > fRange) {
                fRange = fRight;
            }
            float fDist = RandRangeSort(BASKET_PRM_S06_RIGHT_OFF_MOVE_R2, BASKET_PRM_S06_RIGHT_OFF_MOVE_R2 + fRange);
            vPos.y = 0.0f;
            vPos.x = vDir.x * fDist + rGoal.x;
            vPos.z = vDir.z * fDist + rGoal.z;
        } else {
            float fMin = BASKET_PRM_S06_RIGHT_OFF_MOVE_R1;
            float fMax = BASKET_PRM_S06_RIGHT_OFF_MOVE_R2;
            if (fMin > fMax) {
                float fTmp = fMin;
                fMin = fMax;
                fMax = fTmp;
            }
            float fDist = bcs::BCS_RandRange(fMin, fMax);
            bcs::FVEC3 vDir;
            bcs::BCS_VecZ_RotY(vDir, 1.0f, fRot);
            float fRate = (fDist - fMin) / (fMax - fMin);
            vPos.y = 0.0f;
            vPos.x = vDir.x * fDist + rGoal.x;
            vPos.z = vDir.z * fDist + rGoal.z;
            // Higher the nearer the goal.
            vPos.y = BASKET_PRM_S06_RIGHT_OFF_MOVE_R1_HEIGHT * (1.0f - Clamp(fRate, 0.0f, 1.0f));
        }

        bcs::FVEC3 vHit;
        bcs::FVEC3 vNormal;
        float fRadius = m_pBodyColl->GetRadius(0);
        bcs::FVEC3 vFrom = vPos;
        float fTime;
        if (CCollMan::DetectMapHit(vHit, vNormal, fTime, vFrom, vFrom, fRadius, 3, 0x1000000)) {
            vPos = vHit;
        }
        return vPos;
    }

    void CNpcGhostBasketBase::CtorStateSleep()
    {
        m_nGhostFlag |= 1;
        SetScale(bcs::FVEC3(0.0f, m_vScale.y, 0.0f));
        m_fAlpha = 0.0f;
    }

    void CNpcGhostBasketBase::LoopStateSleep()
    {
        STATE_WORK_SLEEP *pWork = FsmqGetWork<STATE_WORK_SLEEP>();
        switch (pWork->m_nStep) {
        case 0:
            pWork->m_fTimer -= GetFrameRateF();
            if (pWork->m_fTimer > 0.0f) {
                break;
            }
            pWork->m_nStep++;
        case 1:
            pWork->m_nFlag |= 1;
            pWork->m_nStep++;
            break;
        }
    }

    void CNpcGhostBasketBase::DtorStateSleep()
    {
        m_nGhostFlag &= ~1;
    }

    void CNpcGhostBasketBase::CtorStateIdle()
    {
        PlayWaitMot_();
    }

    void CNpcGhostBasketBase::LoopStateIdle()
    {
        ChangeStateMove_();
    }

    void CNpcGhostBasketBase::DtorStateIdle()
    {
    }

    void CNpcGhostBasketBase::CtorStateMove()
    {
        STATE_WORK_MOVE *pWork = FsmqGetWork<STATE_WORK_MOVE>();
        pWork->m_nStep = 0;
        pWork->m_fDir = bcs::BCS_RandMax(1L) == 0 ? 1.0f : -1.0f;
        PlayWaitMot_();
    }

    void CNpcGhostBasketBase::LoopStateMove()
    {
        STATE_WORK_MOVE *pWork = FsmqGetWork<STATE_WORK_MOVE>();
        switch (pWork->m_nStep) {
        case 0: {
            pWork->m_vTarget = GetMoveTgtPosRand();
            CStageBasket06 *pStage = static_cast<CStageBasket06 *>(CStageMan::GetInstance());
            pWork->m_bIllumiOn = pStage->Illumi_IsOn(GetTargetGoalIdx());
            pWork->m_nStep++;
        }
        case 1: {
            // Pick a new point when the lights over the goal went on or off.
            CStageBasket06 *pStage = static_cast<CStageBasket06 *>(CStageMan::GetInstance());
            if (pWork->m_bIllumiOn != pStage->Illumi_IsOn(GetTargetGoalIdx())) {
                pWork->m_nStep = 0;
            }
            if (MoveRun(pWork->m_vTarget, 0.8f, GetMoveSpeed_() / 60.0f)) {
                pWork->m_nStep = 0;
            }
            break;
        }
        }
    }

    void CNpcGhostBasketBase::DtorStateMove()
    {
    }

    void CNpcGhostBasketBase::CtorStateGetLooseBall()
    {
        PlayWaitMot_();
    }

    void CNpcGhostBasketBase::LoopStateGetLooseBall()
    {
        if (IsHoldBall() || !(CGame::g_GameWork.m_nFlag18 & 1) || !CanbeCatch()) {
            ChangeStateMove_();
            return;
        }
        STATE_WORK_GET_LOOSE_BALL *pWork = FsmqGetWork<STATE_WORK_GET_LOOSE_BALL>();
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
        if (pBall == 0) {
            ChangeStateMove_();
            return;
        }
        float fSpeed = GetMoveSpeed_();
        const bcs::FVEC3 &rBallPos = pBall->GetPos();
        bcs::FVEC3 vTarget(rBallPos.x, 0.0f, rBallPos.z);
        MoveRun(vTarget, 0.1f, fSpeed / 60.0f);
    }

    void CNpcGhostBasketBase::DtorStateGetLooseBall()
    {
    }

    void CNpcGhostBasketBase::CtorStateMotPlay()
    {
        STATE_WORK_MOT_PLAY *pWork = FsmqGetWork<STATE_WORK_MOT_PLAY>();
        pWork->m_nStep = 0;
        PlaySePos(0xD33, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    }

    void CNpcGhostBasketBase::LoopStateMotPlay()
    {
        STATE_WORK_MOT_PLAY *pWork = FsmqGetWork<STATE_WORK_MOT_PLAY>();
        RotateYRate(pWork->m_fRotY, 0.4f);
        switch (pWork->m_nStep) {
        case 0:
            AnimPlay(0xB, 0, 4.0f, 0.0f);
            pWork->m_nStep++;
        case 1:
            if (!IsAnimEnd()) {
                break;
            }
            AnimPlay(0xD, 1, 0.0f, 0.0f);
            pWork->m_nStep++;
        case 2:
            if (pWork->m_nLoop == -1) {
                break;
            }
            if (!IsAnimLoop()) {
                break;
            }
            if (--pWork->m_nLoop > 0) {
                break;
            }
            AnimPlay(0xC, 0, 0.0f, 0.0f);
            pWork->m_nStep++;
            break;
        default:
            if (IsAnimEnd()) {
                FsmqClear();
                FsmqSetState(STATE_IDLE);
            }
            break;
        }
    }

    void CNpcGhostBasketBase::DtroStateMotPlay()
    {
    }

    void CNpcGhostBasketBase::CtorStatePursuit()
    {
        STATE_WORK_PURSUIT *pWork = FsmqGetWork<STATE_WORK_PURSUIT>();
        pWork->m_nStep = 0;
    }

    void CNpcGhostBasketBase::LoopStatePursuit()
    {
        STATE_WORK_PURSUIT *pWork = FsmqGetWork<STATE_WORK_PURSUIT>();
        if (!pWork->m_pBall->IsShootMode()) {
            ChangeStateMove_();
            return;
        }
        MoveRun(pWork->m_pBall->GetPos(), 0.3f, GetMoveSpeed_() / 60.0f);
    }

    void CNpcGhostBasketBase::DtorStatePursuit()
    {
    }

    void CNpcGhostBasketBase::CtorStateDisappears()
    {
        STATE_WORK_DISAPPEARS *pWork = FsmqGetWork<STATE_WORK_DISAPPEARS>();
        pWork->m_nStep = 0;
        pWork->m_fTimer = 0.0f;
        pWork->m_fSleep = 60.0f * BASKET_PRM_S06_CUT_APPEND_MINUTES;
        pWork->m_fAlpha = m_fAlpha;
        pWork->m_fScale = m_vScale.x;
        PlaySePos(0xD34, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        AnimPlay(8, 0, 4.0f, 0.0f);
    }

    void CNpcGhostBasketBase::LoopStateDisappears()
    {
        static const float fCheckFrame = 70.0f;

        STATE_WORK_DISAPPEARS *pWork = FsmqGetWork<STATE_WORK_DISAPPEARS>();
        if (pWork->m_nStep == 0) {
            // Fade out over the end of the motion.
            float fFrame = AnimGetFrame();
            if (fFrame < fCheckFrame) {
                return;
            }
            float fRate = Clamp((fFrame - fCheckFrame) / (AnimGetFrameNum() - fCheckFrame), 0.0f, 1.0f);
            m_fAlpha = pWork->m_fAlpha * (1.0f - fRate);
            if (!IsAnimEnd()) {
                return;
            }
            pWork->m_nStep++;
        }
        ChangeStateSleep_(pWork->m_fSleep);
    }

    void CNpcGhostBasketBase::DtorStateDisappears()
    {
    }

    void CNpcGhostBasketBase::CtorStateAppearance()
    {
        STATE_WORK_APPEARANCE *pWork = FsmqGetWork<STATE_WORK_APPEARANCE>();
        pWork->m_nStep = 0;
        pWork->m_fTimer = 0.0f;
        m_vPos = pWork->m_vPos;
        m_fAlpha = 0.0f;
    }

    void CNpcGhostBasketBase::LoopStateAppearance()
    {
        STATE_WORK_APPEARANCE *pWork = FsmqGetWork<STATE_WORK_APPEARANCE>();
        if (pWork->m_nStep == 0) {
            pWork->m_fTimer += GetFrameRateF();
            if (pWork->m_fTimer < pWork->m_fWait) {
                return;
            }
            PlaySePos(0xD33, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            AnimPlay(0x10, 0, 4.0f, 0.0f);
            pWork->m_nStep++;
        }
        // Fade in with the motion.
        float fFrameNum = AnimGetFrameNum();
        m_fAlpha = (AnimGetFrame() / fFrameNum) * BASKET_PRM_S06_BW_ALPHA_RATE;
        if (IsAnimEnd()) {
            m_fAlpha = BASKET_PRM_S06_BW_ALPHA_RATE;
            FsmqClear();
            FsmqSetState(STATE_IDLE);
        }
    }

    void CNpcGhostBasketBase::DtorStateAppearance()
    {
    }

    void CNpcGhostBasketBase::CtorStatePass()
    {
    }

    void CNpcGhostBasketBase::LoopStatePass()
    {
        STATE_WORK_PASS *pWork = FsmqGetWork<STATE_WORK_PASS>();
        switch (pWork->m_nStep) {
        case 0: {
            pWork->m_fWait = 13.0f;
            CChrCore *pChr;
            if (IsChrIdx(pWork->m_nTarget)) {
                pChr = CChrMan::GetChrCore(pWork->m_nTarget);
            } else {
                pChr = 0;
            }
            pWork->m_pChr = static_cast<CChrCoreBasket *>(pChr);
            pWork->m_fFrame = 40.0f;
            pWork->m_nPassType = 5;
            if (pChr == 0) {
                // Throw the ball away somewhere around the ghost.
                bcs::FVEC3 vDir;
                bcs::BCS_VecZ_RotY(vDir, 1.0f, bcs::BCS_RandRange(0.0f, 6.2831855f));
                float fDist = bcs::BCS_RandRange(3.0f, 7.0f);
                pWork->m_vTarget.x = vDir.x * fDist + m_vPos.x;
                pWork->m_vTarget.y = 0.0f;
                pWork->m_vTarget.z = vDir.z * fDist + m_vPos.z;
            }
            AnimPlay(AnimGetMotID() > 0 ? 10 : -10, 0, 8.0f, 0.0f);
            pWork->m_nStep++;
        }
        case 1: {
            if (pWork->m_pChr) {
                pWork->m_vTarget = pWork->m_pChr->GetPos();
            }
            RotateYRate(GetRotY_ToPosXZ(pWork->m_vTarget), 0.4f);
            pWork->m_fWait -= GetFrameRateF();
            if (pWork->m_fWait > 0.0f) {
                break;
            }

            long nPassType = 5;
            CBall *pBall = GetAttachBall();
            if (pBall == 0) {
                ChangeStateMove_();
                break;
            }
            if (pWork->m_pChr) {
                if (pWork->m_pChr->GetState() == 0x26 && pBall) {
                    nPassType = 2;
                }
                pWork->m_vTarget = pWork->m_pChr->GetPos();
                // Aim where the receiver will be when the ball arrives.
                pWork->m_pChr->GetFuturePos(pWork->m_vTarget,
                                            pBall->GetPassFlyFrame(m_vPos, pWork->m_vTarget, nPassType));
                pWork->m_pChr->GetFuturePos(pWork->m_vTarget,
                                            pBall->GetPassFlyFrame(m_vPos, pWork->m_vTarget, nPassType));
                pWork->m_fFrame = 0.0f;
                pWork->m_nPassType = nPassType;
                pWork->m_pChr->MethdSetPassCatchState(pBall, 1, pWork->m_vTarget, nPassType);
            }
            PlaySePos(0xD35, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            float fFrame = pWork->m_fFrame;
            CChrCoreBasket *pChr = pWork->m_pChr;
            if (IsHoldBall()) {
                CBallBasket *pPassBall = static_cast<CBallBasket *>(GetAttachBall());
                DetachBall();
                pPassBall->SetPassMode(pWork->m_vTarget, nPassType, fFrame, -1, pChr ? pChr->GetPlayerNo() : -1, 1,
                                       1, 0);
                if (pChr && nPassType != 2) {
                    pChr->MethdSetPassCatchJumpWaitFrame(pWork->m_vTarget, fFrame);
                }
                pPassBall->OnBasketFlag(0x10000);
            }
            m_nFlag |= 1;
            pWork->m_nStep++;
            break;
        }
        case 2:
        case 3:
            if (!IsAnimEnd()) {
                break;
            }
            if (pWork->m_nTarget == -1) {
                // Thrown away: laugh.
                long nMotDir = AnimGetMotID() > 0 ? 4 : -4;
                float fRotY = m_vRot.y;
                long nLoop = BASKET_PRM_S06_PASS_MISS_TO_HAPPY_COUNT + 1;
                if (CanbeMotPlay_()) {
                    STATE_WORK_MOT_PLAY *pMotWork = static_cast<STATE_WORK_MOT_PLAY *>(FsmqSetState(STATE_MOT_PLAY));
                    pMotWork->m_nMotDir = nMotDir;
                    pMotWork->m_nLoop = nLoop;
                    pMotWork->m_fRotY = fRotY;
                }
            } else {
                ChangeStateMove_();
            }
            break;
        }
    }

    void CNpcGhostBasketBase::DtorStatePass()
    {
    }

    void CNpcGhostBasketBase::CtorStatePassCatch()
    {
    }

    void CNpcGhostBasketBase::LoopStatePassCatch()
    {
        STATE_WORK_PASS_CATCH *pWork = FsmqGetWork<STATE_WORK_PASS_CATCH>();
        switch (pWork->m_nStep) {
        case 0:
            PlayWaitMot_();
            pWork->m_fRotY = GetRotY_ToPosXZ(pWork->m_pBall->GetPos());
            pWork->m_nStep++;
        case 1:
            RotateYRate(pWork->m_fRotY, 0.2f);
            if (IsHoldBall()) {
                pWork->m_nStep++;
            } else if (!pWork->m_pBall->IsPassMode()) {
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (!CheckCatch_()) {
                if (GetState() != STATE_IDLE) {
                    FsmqSetState(STATE_IDLE);
                }
            }
            break;
        }
    }

    void CNpcGhostBasketBase::DtorStatePassCatch()
    {
    }

    void CNpcGhostBasketBase::CtorStateCatch()
    {
        STATE_WORK_CATCH *pWork = FsmqGetWork<STATE_WORK_CATCH>();
        pWork->m_nStep = 0;
        pWork->m_fTimer = 60.0f * BASKET_PRM_S06_PASS_CALL_WAIT_MINUTES;
    }

    void CNpcGhostBasketBase::LoopStateCatch()
    {
        STATE_WORK_CATCH *pWork = FsmqGetWork<STATE_WORK_CATCH>();
        switch (pWork->m_nStep) {
        case 0:
            AnimPlay(3, 0, 8.0f, 0.0f);
            pWork->m_nStep++;
        case 1:
            if (!IsAnimEnd()) {
                break;
            }
            AnimPlay(4, 0, 4.0f, 0.0f);
            pWork->m_nStep++;
        case 2:
            if (!IsAnimEnd()) {
                break;
            }
            PlayWaitMot_();
            pWork->m_nStep++;
        default:
            if (BASKET_PRM_S06_BALL_HOLD_MOVE == 1) {
                ChangeStateMove_();
            } else if (IsHoldBall()) {
                pWork->m_fTimer -= GetFrameRateF();
                if (pWork->m_fTimer <= 0.0f) {
                    pWork->m_fTimer = 0.0f;
                    ChangeStatePass_(-1);
                }
            } else {
                ChangeStateMove_();
            }
            break;
        }
    }

    void CNpcGhostBasketBase::DtorStateCatch()
    {
    }

    int CNpcGhostBasketBase::MoveRun(const bcs::FVEC3 &rTarget, float fDist, float fSpeed)
    {
        float fFrameRate = GetFrameRateF();

        // Turn towards the target, at most by the turn rate.
        float fRot = RoundRad(GetRotY_ToPosXZ(rTarget) - m_vRot.y);
        float fRotRate = BASKET_PRM_S06_MOVE_ROTATE_RATE;
        if (fRot < 4.1887903f) {
            fRot = Clamp(fRot * fRotRate * fFrameRate, 0.0f, fRot);
        } else {
            float fLeft = 6.2831855f - fRot;
            fRot = Clamp(-fRotRate * fLeft * fFrameRate, -1.0f * fLeft, 0.0f);
        }
        m_vRot.y = RoundRad(fRot + m_vRot.y);

        // Move forwards, as far as the target lies that way.
        float fStep = fSpeed * fFrameRate;
        bcs::FVEC3 vDiff = VecSub(rTarget, m_vPos);
        bcs::FVEC3 vDir;
        bcs::BCS_Vec_Normalize(vDir, vDiff);
        bcs::FVEC3 vFront;
        bcs::BCS_VecZ_RotY(vFront, 1.0f, m_vRot.y);
        m_vPos.x += fStep * (vFront.x * __fabsf(vDir.x));
        m_vPos.y += vDir.y * fStep;
        m_vPos.z += fStep * (vFront.z * __fabsf(vDir.z));

        bcs::FVEC3 vRest = VecSub(m_vPos, rTarget);
        return bcs::FVEC3Len(&vRest) <= fDist;
    }

    int CNpcGhostBasketBase::MoveRun(const bcs::FVEC3 &rTarget, float fDist, float fSpeed, float fRotY,
                                     float fRotRate)
    {
        bcs::FVEC3 vDir = VecSub(rTarget, m_vPos);
        bcs::BCS_Vec_Normalize(vDir, bcs::FVEC3(vDir.x, 0.0f, vDir.z));
        m_vPos.x += vDir.x * fSpeed;
        m_vPos.z += vDir.z * fSpeed;
        RotateYRate(fRotY, fRotRate);

        bcs::FVEC3 vRest = VecSub(m_vPos, rTarget);
        bcs::FVEC3 vRestXZ(vRest.x, 0.0f, vRest.z);
        return bcs::FVEC3Len(&vRestXZ) <= fDist;
    }

    int CNpcGhostBasketBase::CanbeCatch() const
    {
        if (IsHoldBall()) {
            return 0;
        }
        if (m_fDamageWait > 0.0f) {
            return 0;
        }
        if (m_nFlag & 1) {
            return 0;
        }
        if (!CGameBasket::GetInstance()->IsNowMainLoopEnable_()) {
            return 0;
        }
        switch (GetState()) {
        case STATE_DISAPPEARS:
        case STATE_APPEARANCE:
        case STATE_SLEEP:
            return 0;
        }
        return 1;
    }

    int CNpcGhostBasketBase::CanbePassCatch() const
    {
        return CanbeCatch() != 0;
    }

    int CNpcGhostBasketBase::MethdSetPassCatchState(CBall *pBall, const bcs::FVEC3 &rPos, long nPassType)
    {
        if (!CanbePassCatch()) {
            return 0;
        }
        STATE_WORK_PASS_CATCH *pWork = static_cast<STATE_WORK_PASS_CATCH *>(FsmqSetState(STATE_PASS_CATCH));
        pWork->m_nStep = 0;
        pWork->m_nPassType = nPassType;
        pWork->m_pBall = pBall;
        pWork->m_vPos = rPos;
        return 1;
    }

    CNpcGhostBasketBase *CChrManBasket::CreateGhost_(long nType, long nTeam)
    {
        CNpcGhostBasketBase *pGhost = new (0) CNpcGhostBasketBase(this, nType, nTeam);
        if (pGhost) {
            pGhost->Create();
            return pGhost;
        }
        return 0;
    }

    CNpcBase *CChrManBasket::GetGhost_(long nTeam)
    {
        return CChrMan::NpcSearch(CGameBasket::GetTeamGhostType(nTeam), 0);
    }
}
