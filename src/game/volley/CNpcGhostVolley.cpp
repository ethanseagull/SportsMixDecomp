// msw::CNpcGhostVolley: the ghost (Boo) of volleyball stage 6. It floats
// to and fro along the net on the side of its team, puts up a wall
// (a defence collision unit) against the opponents' shots, and catches and
// spikes the balls that come down near it on its own side. It leaves when
// the stage's lights come on or after it stayed its time, and comes back
// when the game lets it.
#include "game/msw/CNpcGhostVolley.hpp"

#include <math.h>

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/FVEC4.hpp"
#include "bcs/New.hpp"
#include "game/msw/BallVolleySpikeShotArg.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrDrv.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStageVolley06.hpp"
#include "nw4r/math/triangular.h"

// x brought into [lo, hi]. A macro, as the original reads x again for the
// lower bound (a field the expression just stored).
#define GHOST_CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))

namespace msw {

    namespace {
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

        inline float Clamp(float fValue, float fMin, float fMax)
        {
            return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
        }

        inline float Max(float fA, float fB)
        {
            return fA < fB ? fB : fA;
        }

        inline float Min(float fA, float fB)
        {
            return fA > fB ? fB : fA;
        }

        inline void Swap(float &rA, float &rB)
        {
            float fTmp = rA;
            rA = rB;
            rB = fTmp;
        }

        inline float Lerp(float fFrom, float fTo, float fRate)
        {
            return fFrom + fRate * (fTo - fFrom);
        }

        // The yaw fRate of the way from fFrom to fTo, the short way round.
        inline float LerpRad(float fFrom, float fTo, float fRate)
        {
            fFrom = RoundRad(fFrom);
            fTo = RoundRad(fTo);
            float fDiff = RoundRad(fTo - fFrom);
            if (fDiff >= 3.1415927f) {
                fDiff -= 6.2831855f;
            }
            return RoundRad(fFrom + Lerp(0.0f, fDiff, fRate));
        }

        // Moves rPos for fFrame frames at rVel, falling at fGravity, and
        // lets rVel fall with it.
        inline void MoveByVel(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fGravity, float fFrame)
        {
            float fFall = fGravity * fFrame;
            bcs::FVEC3 vMove;
            bcs::FVEC3Scale(&vMove, &rVel, fFrame);
            vMove.y += 0.5f * (fFall * fFrame);
            rVel.y += fFall;
            bcs::FVEC3Add(&rPos, &rPos, &vMove);
        }
    }

    inline float CNpcGhostVolley::GetNetSideZ_() const
    {
        bcs::FVEC3 vNet = CStageMan::GetInstance()->GetNetPos();
        if (CGame::GetTargetGoalIdx(m_nTeamNo)) {
            vNet.z -= 1.0f;
        } else {
            vNet.z += 1.0f;
        }
        return vNet.z;
    }

    inline int CNpcGhostVolley::IsOverNet_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rNet) const
    {
        int bOver = 0;
        if (CGame::GetTargetGoalIdx(m_nTeamNo)) {
            if (rPos.z > rNet.z) {
                bOver = 1;
            }
        } else if (rPos.z < rNet.z) {
            bOver = 1;
        }
        return bOver;
    }

    CNpcGhostVolley::CNpcGhostVolley(CChrMan *pMan, long nType)
        : CNpcGhost(pMan, nType)
        , m_fFloatPhase(0.0f)
        , m_fRotY(0.0f)
        , m_vHandOfs(0.0f, 0.0f, 0.0f)
        , m_fSpikeFrame(0.0f)
        , m_fStayTime(1800.0f)
    {
    }

    CNpcGhostVolley::~CNpcGhostVolley()
    {
    }

    void CNpcGhostVolley::Create()
    {
        CNpcGhost::Create();

        // Unit 0: the hand that catches the ball to spike (on only while
        // the spike can hit, see MainFunc).
        m_pBodyColl = new (0x100) CBodyCollision(1, 3, 0, 0);
        m_pBodyColl->SetCatchEnable(0);
        m_pBodyColl->SetCollType(0, 0);
        m_pBodyColl->SetRadius(0, 0.5f);
        m_pBodyColl->SetBallCatchCallback(0, CallbackBallCatch, this);
        m_pBodyColl->SetEnable(0);

        // The block wall.
        m_DefColl.Create(1, 5, 2, 0);
        m_DefColl.GetColl(0)->SetCollType(2);
        m_DefColl.GetColl(0)->SetShape(4);
        m_DefColl.GetColl(0)->SetSize(bcs::FVEC3(1.0f, 2.0f, 0.1f));
        m_DefColl.GetColl(0)->SetPos(m_vPos);
        m_DefColl.GetColl(0)->OnFlagA0(1);
        m_DefColl.GetColl(0)->SetReflectHitCallback(CallbackDefReflectHit, this);

        m_vHandOfs.Set(0.0f, 0.5f, 1.3f);
        m_fSpikeFrame = 8.0f + AnimGetResFrameNum(0xB);
    }

    long CNpcGhostVolley::SendMessage(long nMsg, long nArg0, long nArg1, long nArg2, long nArg3)
    {
        switch (nMsg) {
        case 1:
        case 3:
        case 5:
            Reset();
            m_fFloatPhase = 0.0f;
            m_vPos.y = 1.0f;
            m_nGhostFlag |= 1;
            FsmqSetState(STATE_SLEEP);
            break;
        case 0xF:
        case 0x12:
        case 0x13:
            Reset();
            m_nGhostFlag |= 1;
            FsmqSetState(STATE_HIDDEN);
            break;
        case 0x17:
            m_nGhostFlag |= 1;
            FsmqSetState(STATE_HIDDEN);
            break;
        case 0xD:
            if (!(m_nGhostFlag & 1)) {
                long nState = GetState();
                if (nState != STATE_SLEEP && nState != STATE_HIDDEN && nState != STATE_DISAPPEARS) {
                    FsmqSetState(STATE_DISAPPEARS);
                }
            }
            break;
        case 0xE:
        case 0x16:
            if (GetState() == STATE_HIDDEN) {
                FsmqGetWork<STATE_WORK_HIDDEN>()->m_bCanAppear = 1;
            }
            break;
        }
        return CNpcGhost::SendMessage(nMsg, nArg0, nArg1, nArg2, nArg3);
    }

    CBall *CNpcGhostVolley::GetTargetBall_() const
    {
        long nNum = CBallMan::GetBallNum();
        if (nNum <= 0) {
            return 0;
        }
        CBall *pTarget = CBallMan::GetBallPtr(0);
        if (nNum <= 1) {
            return pTarget;
        }
        float fMinDist = 9999980000.0f;
        for (long i = 0; i < nNum; i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall && !pBall->IsScored()) {
                bcs::FVEC3 vPos = pBall->GetPos();
                float fDist = bcs::FVEC3DistSq(&m_vPos, &vPos);
                if (fDist < fMinDist) {
                    fMinDist = fDist;
                    pTarget = pBall;
                }
            }
        }
        return pTarget;
    }

    void CNpcGhostVolley::MainFunc()
    {
        FsmqExec();

        if (m_pMdl) {
            m_pMdl->m_vTrans = m_vPos;
            m_pMdl->SetRotXYZ(m_vRot.x, m_vRot.y, m_vRot.z);
            m_pMdl->UpdateFrame(GetFrameRateF());
            CalcRoot_();
        }

        // The block wall stands in front of the ghost while it blocks.
        m_DefColl.SetDisableAll();
        if (GetState() == STATE_BLOCK) {
            m_DefColl.SetSizeAll(bcs::FVEC3(1.0f, 2.0f, 0.1f));
            bcs::FVEC3 vPos = m_vPos;
            bcs::FVEC3 vOfs(0.0f, 0.5f, 0.5f);
            bcs::BCS_Vec_RotY(vOfs, vOfs, m_fRotY);
            bcs::FVEC3Add(&vPos, &vPos, &vOfs);
            m_DefColl.SetPosAll(vPos);
            m_DefColl.SetYRotAll(0.0f);
            m_DefColl.SetEnableAll();
        }

        // The hand catches the ball while the spike can hit it.
        if (m_pBodyColl) {
            m_pBodyColl->SetDisableAll();
            if (GetState() == STATE_SPIKE) {
                STATE_WORK_SPIKE *pWork = FsmqGetWork<STATE_WORK_SPIKE>();
                if (pWork->m_bHitEnable) {
                    bcs::FVEC3 vPos = m_vPos;
                    bcs::FVEC3 vHand;
                    bcs::BCS_Vec_RotY(vHand, m_vHandOfs, pWork->m_fRotTo);
                    bcs::FVEC3Add(&vPos, &vPos, &vHand);
                    m_pBodyColl->SetPos(0, vPos);
                    m_pBodyColl->SetEnable(0);
                }
            }
        }

        CheckDisp_();
    }

    unsigned long CNpcGhostVolley::CallbackDefReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                         bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, void *pArg)
    {
        if (pArg == 0) {
            return 0;
        }
        return static_cast<CNpcGhostVolley *>(pArg)->CallbackDefReflectHit_(pDef, pBall, rPos, rVel);
    }

    unsigned long CNpcGhostVolley::CallbackDefReflectHit_(const CCollDefUnit *pDef, const CBall *pBall,
                                                          bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
    {
        PlaySePos(0xD35, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        return 0x701;
    }

    int CNpcGhostVolley::DoBlockCheck_()
    {
        CBall *pBall = GetTargetBall_();
        if (pBall == 0) {
            return 0;
        }
        if (m_nTeamNo != CGame::g_GameWork.GetUserTeam()) {
            return 0;
        }
        if (!pBall->IsShootMode()) {
            return 0;
        }
        if (pBall->IsItemBall()) {
            return 0;
        }

        // Only near the net.
        bcs::FVEC3 vNet = CStageMan::GetInstance()->GetNetPos();
        if (CGame::GetTargetGoalIdx(m_nTeamNo)) {
            if (m_vPos.z < vNet.z - 3.0f) {
                return 0;
            }
        } else if (m_vPos.z > 3.0f + vNet.z) {
            return 0;
        }

        // The shot comes from the other court.
        bcs::FVEC3 vBall = pBall->GetPos();
        if (!IsOverNet_(vBall, vNet)) {
            return 0;
        }

        bcs::FVEC3 vBlock;
        bcs::BCS_VecZ_RotY(vBlock, 2.0f, m_fRotY);
        bcs::FVEC3Add(&vBlock, &vBlock, &m_vPos);
        return bcs::FVEC3DistSq(&pBall->GetPos(), &vBlock) < 9.0f;
    }

    int CNpcGhostVolley::DoSpikeCheck_()
    {
        CBall *pBall = GetTargetBall_();
        if (pBall == 0) {
            return 0;
        }
        if (m_nTeamNo != CGame::g_GameWork.GetUserTeam()) {
            return 0;
        }
        if (pBall->IsShootMode()) {
            return 0;
        }
        if (pBall->IsItemBall()) {
            return 0;
        }

        // Only a ball on the ghost's side.
        bcs::FVEC3 vBall = pBall->GetPos();
        bcs::FVEC3 vNet = CStageMan::GetInstance()->GetNetPos();
        if (IsOverNet_(vBall, vNet)) {
            return 0;
        }

        // Where the ball will be when the spike hits: high enough and
        // within reach.
        bcs::FVEC3 vAfter;
        pBall->GetPosAfterFrame(vAfter, m_fSpikeFrame);
        if (vAfter.y <= 3.0f) {
            return 0;
        }
        bcs::FVEC3 vReach = m_vPos;
        vReach.y += 2.0f;
        return bcs::FVEC3DistSq(&vAfter, &vReach) < 9.0f;
    }

    void CNpcGhostVolley::CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (pBall && !pBall->IsScored() && pBall->CanbeCatch()) {
            AttachBall(pBall, m_nBallNode, 8.0f, 0x18);
        }
    }

    void CNpcGhostVolley::SpikeBall_(const bcs::FVEC3 &rTarget)
    {
        CBall *pBall = GetAttachBall();
        DetachBall();
        if (pBall == 0) {
            return;
        }

        long nGoal = CGame::GetGuardGoalIdx(m_nTeamNo);
        BALL::VOLLEY::SPIKE_SHOT_ARG arg;
        arg.m_nUnk0C = 0;
        arg.m_nUnk14 = 0;
        arg.m_nUnk20 = 1;
        arg.m_nUnk2C = m_nTeamNo;
        arg.m_nUnk30 = m_nTeamNo;
        arg.m_nUnk34 = nGoal;
        arg.m_fUnk40 = m_fRotY;
        arg.m_nUnk54 = PlayGetOutline_(0);
        arg.m_nUnkA0 = 0;
        arg.m_nUnkA4 = 0;
        arg.m_nUnk24 = 1;
        arg.m_fUnk70 = 100.0f;
        arg.m_fUnk58 = 18.0f;
        arg.m_fUnk5C = 34.0f;
        arg.m_fUnk60 = 34.0f;
        arg.m_fUnk68 = 1.0f;
        arg.m_fUnk6C = 25.0f;
        arg.m_fUnk7C = 0.0f;
        arg.m_fUnk80 = 0.0f;
        arg.m_fUnk84 = 0.0f;
        bcs::FVEC3 vTarget(rTarget.x, 0.0f, rTarget.z);
        arg.m_fUnk00 = vTarget.x;
        arg.m_fUnk04 = vTarget.y;
        arg.m_fUnk08 = vTarget.z;
        arg.m_nUnk44 = 0xC85;
        arg.m_nUnk4C = 0x4E0;
        pBall->SetVolleySpikeShotMode(arg);

        // The ball goes to the other team.
        long nOther = 1 - m_nTeamNo;
        CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x85, pBall->GetBallNo(), m_nTeamNo, m_nTeamNo, nOther);
        CGame::SetOffenseTeam(nOther);
        CChrMan::AllSendMsg((CChrDrv::MSG_ID)0x17, nOther, 0, 0, 0);
    }

    void CNpcGhostVolley::CtorStateSleep()
    {
        CNpcGhost::CtorStateSleep();
        Eff_EraseAll();
    }

    void CNpcGhostVolley::LoopStateSleep()
    {
    }

    void CNpcGhostVolley::DtorStateSleep()
    {
    }

    void CNpcGhostVolley::CtorStateIdle()
    {
        if (AnimGetMotID() != 1) {
            AnimPlay(1, 1, 8.0f, 0.0f);
        }
    }

    void CNpcGhostVolley::LoopStateIdle()
    {
        // Float up and down.
        m_fFloatPhase += 0.034906585f * GetFrameRateF();
        m_fFloatPhase = RoundRad(m_fFloatPhase);
        m_vPos.y = 0.5f * nw4r::math::SinRad(m_fFloatPhase) + 1.5f;

        CStageVolley06 *pStage = static_cast<CStageVolley06 *>(CStageMan::GetInstance());
        if (pStage->Illumi_IsOn(0) || m_fStayTime <= 0.0f) {
            FsmqSetState(STATE_DISAPPEARS);
        } else if (DoBlockCheck_()) {
            FsmqSetState(STATE_BLOCK);
        } else if (DoSpikeCheck_()) {
            FsmqSetState(STATE_SPIKE);
        } else {
            // Float to one end of the net or the other.
            bcs::FVEC3 vTarget = m_vPos;
            vTarget.z = GetNetSideZ_();
            vTarget.x = (bcs::BCS_RandMax(255L) & 1) ? -8.0f : 8.0f;
            FsmqCutQueue();
            STATE_WORK_MOVE *pWork = static_cast<STATE_WORK_MOVE *>(FsmqPushWork(STATE_MOVE));
            pWork->m_vTarget = vTarget;
            FsmqDoChain();
        }
    }

    void CNpcGhostVolley::DtorStateIdle()
    {
    }

    void CNpcGhostVolley::CtorStateMove()
    {
        FsmqGetWork<STATE_WORK_MOVE>()->m_fSpeed = 0.0f;
    }

    void CNpcGhostVolley::LoopStateMove()
    {
        CStageVolley06 *pStage = static_cast<CStageVolley06 *>(CStageMan::GetInstance());
        if (pStage->Illumi_IsOn(0) || m_fStayTime <= 0.0f) {
            FsmqSetState(STATE_DISAPPEARS);
            return;
        }

        STATE_WORK_MOVE *pWork = FsmqGetWork<STATE_WORK_MOVE>();
        pWork->m_vDir.x = pWork->m_vTarget.x - m_vPos.x;
        pWork->m_vDir.z = pWork->m_vTarget.z - m_vPos.z;
        pWork->m_vDir.y = 0.0f;
        if (pWork->m_vDir.x * pWork->m_vDir.x + pWork->m_vDir.z * pWork->m_vDir.z <= 1.0f) {
            FsmqSetState(STATE_IDLE);
            return;
        }

        bcs::BCS_Vec_Normalize(pWork->m_vDir, bcs::FVEC3(pWork->m_vDir.x, 0.0f, pWork->m_vDir.z));
        pWork->m_fSpeed += 0.0008333334f * GetFrameRateF();
        pWork->m_fSpeed = Min(0.025f, pWork->m_fSpeed);
        bcs::FVEC3 vVel;
        bcs::FVEC3Scale(&vVel, &pWork->m_vDir, pWork->m_fSpeed);
        MoveByVel(m_vPos, vVel, 0.0f, GetFrameRateF());

        // Float up and down, faster than when idle.
        m_fFloatPhase += 0.06981317f * GetFrameRateF();
        m_fFloatPhase = RoundRad(m_fFloatPhase);
        m_vPos.y = 0.5f * nw4r::math::SinRad(m_fFloatPhase) + 1.5f;

        RotateYRate(GetRotY_ToPosXZ(pWork->m_vTarget), 0.025f);
        m_fStayTime -= GetFrameRateF();

        if (DoBlockCheck_()) {
            FsmqSetState(STATE_BLOCK);
        } else if (DoSpikeCheck_()) {
            FsmqSetState(STATE_SPIKE);
        }
    }

    void CNpcGhostVolley::DtorStateMove()
    {
    }

    void CNpcGhostVolley::CtorStateDisappears()
    {
        FsmqGetWork<STATE_WORK_DISAPPEARS>()->m_nStep = 0;
    }

    void CNpcGhostVolley::LoopStateDisappears()
    {
        // The frame of the vanish motion the fade starts at.
        static const float fCheckFrame = 70.0f;

        STATE_WORK_DISAPPEARS *pWork = FsmqGetWork<STATE_WORK_DISAPPEARS>();
        switch (pWork->m_nStep) {
        case 0:
            PlaySePos(0xD34, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            AnimPlay(8, 0, 4.0f, 0.0f);
            pWork->m_nStep++;
        case 1: {
            float fFrame = AnimGetFrame();
            if (fFrame < fCheckFrame) {
                return;
            }
            float fRate = Clamp((fFrame - fCheckFrame) / (AnimGetFrameNum() - fCheckFrame), 0.0f, 1.0f);
            m_fAlpha = 1.0f - fRate;
            if (!IsAnimEnd()) {
                break;
            }
            pWork->m_nStep++;
        }
        default:
            FsmqSetState(STATE_HIDDEN);
            break;
        }
    }

    void CNpcGhostVolley::DtorStateDisappears()
    {
    }

    void CNpcGhostVolley::CtorStateAppearance()
    {
        FsmqGetWork<STATE_WORK_APPEARANCE>()->m_nStep = 0;
        m_fAlpha = 0.0f;

        // Comes back for either team, at the middle of the net.
        m_nTeamNo = bcs::BCS_RandMax(1L);
        m_vPos.x = 0.0f;
        m_vPos.y = 1.5f;
        m_vPos.z = GetNetSideZ_();
        m_fFloatPhase = 0.0f;
        if (CGame::GetTargetGoalIdx(m_nTeamNo)) {
            m_fRotY = 0.0f;
        } else {
            m_fRotY = 3.1415927f;
        }
        SetRotY(m_fRotY);
        m_nGhostFlag &= ~1;
    }

    void CNpcGhostVolley::LoopStateAppearance()
    {
        STATE_WORK_APPEARANCE *pWork = FsmqGetWork<STATE_WORK_APPEARANCE>();
        if (pWork->m_nStep == 0) {
            PlaySePos(0xD33, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            AnimPlay(0x10, 0, 4.0f, 0.0f);
            pWork->m_nStep++;
        }
        m_fAlpha = AnimGetFrame() / AnimGetFrameNum();
        if (IsAnimEnd()) {
            m_fAlpha = 1.0f;
            FsmqSetState(STATE_IDLE);
        }
    }

    void CNpcGhostVolley::DtorStateAppearance()
    {
    }

    void CNpcGhostVolley::CtorStateHidden()
    {
        STATE_WORK_HIDDEN *pWork = FsmqGetWork<STATE_WORK_HIDDEN>();
        pWork->m_nStep = 0;
        pWork->m_fWait = 60.0f;
        m_fFloatPhase = 0.0f;
        m_vPos.y = 1.5f;
        m_fStayTime = 1800.0f;
        m_nGhostFlag |= 1;
    }

    void CNpcGhostVolley::LoopStateHidden()
    {
        STATE_WORK_HIDDEN *pWork = FsmqGetWork<STATE_WORK_HIDDEN>();
        if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
            return;
        }
        if (!pWork->m_bCanAppear) {
            return;
        }
        CStageVolley06 *pStage = static_cast<CStageVolley06 *>(CStageMan::GetInstance());
        if (pStage->Illumi_IsOn(0)) {
            return;
        }
        pWork->m_fWait -= GetFrameRateF();
        if (pWork->m_fWait <= 0.0f) {
            FsmqSetState(STATE_APPEARANCE);
        }
    }

    void CNpcGhostVolley::DtorStateHidden()
    {
    }

    void CNpcGhostVolley::CtorStateBlock()
    {
        FsmqGetWork<STATE_WORK_BLOCK>()->m_nStep = 0;
    }

    void CNpcGhostVolley::LoopStateBlock()
    {
        STATE_WORK_BLOCK *pWork = FsmqGetWork<STATE_WORK_BLOCK>();
        int bEnd = 0;
        switch (pWork->m_nStep) {
        case 0:
            // Rise to block.
            AnimPlay(0xB, 0, 4.0f, 0.0f);
            pWork->m_vFrom = m_vPos;
            pWork->m_vTo = m_vPos;
            pWork->m_vTo.y = 3.0f;
            pWork->m_fRotFrom = m_vRot.y;
            pWork->m_fRotTo = m_fRotY;
            pWork->m_fFrameNum = AnimGetFrameNum();
            pWork->m_fFrameNum = Min(8.0f, pWork->m_fFrameNum);
            pWork->m_fFrame = 0.0f;
            pWork->m_nStep++;
            break;
        case 1: {
            pWork->m_fFrame += GetFrameRateF();
            pWork->m_fFrame = GHOST_CLAMP(pWork->m_fFrame, 0.0f, pWork->m_fFrameNum);
            float fRate = Clamp(pWork->m_fFrame / pWork->m_fFrameNum, 0.0f, 1.0f);
            bcs::FVEC3Lerp(&m_vPos, &pWork->m_vFrom, &pWork->m_vTo, fRate);
            m_vRot.y = LerpRad(pWork->m_fRotFrom, pWork->m_fRotTo, fRate);
            if (IsAnimEnd()) {
                AnimPlay(0xE, 1, 4.0f, 0.0f);
                pWork->m_fWait = 60.0f;
                pWork->m_nStep++;
            }
            break;
        }
        case 2:
            // Hold the block, then come down.
            pWork->m_fWait -= GetFrameRateF();
            if (pWork->m_fWait <= 0.0f) {
                AnimPlay(0xC, 0, 4.0f, 0.0f);
                pWork->m_vFrom = m_vPos;
                pWork->m_vTo = m_vPos;
                pWork->m_vTo.y = 1.5f;
                pWork->m_vTo.z = GetNetSideZ_();
                pWork->m_fFrameNum = AnimGetFrameNum();
                pWork->m_fFrame = 0.0f;
                pWork->m_nStep++;
            }
            break;
        case 3: {
            pWork->m_fFrame += GetFrameRateF();
            pWork->m_fFrame = GHOST_CLAMP(pWork->m_fFrame, 0.0f, pWork->m_fFrameNum);
            float fRate = Clamp(pWork->m_fFrame / pWork->m_fFrameNum, 0.0f, 1.0f);
            fRate = Clamp(pow(fRate, 2.0), 0.0f, 1.0f);
            bcs::FVEC3Lerp(&m_vPos, &pWork->m_vFrom, &pWork->m_vTo, fRate);
            if (pWork->m_fFrame >= pWork->m_fFrameNum) {
                bEnd = 1;
                pWork->m_nStep++;
            }
            if (!IsAnimEnd()) {
                break;
            }
            pWork->m_nStep++;
        }
        default:
            bEnd = 1;
            break;
        }

        if (bEnd) {
            FsmqSetState(STATE_IDLE);
        } else {
            RotateYRate(m_fRotY, 0.125f);
        }
    }

    void CNpcGhostVolley::DtorStateBlock()
    {
        m_fFloatPhase = 0.0f;
    }

    void CNpcGhostVolley::CtorStateSpike()
    {
        FsmqGetWork<STATE_WORK_SPIKE>()->m_nStep = 0;
    }

    void CNpcGhostVolley::LoopStateSpike()
    {
        STATE_WORK_SPIKE *pWork = FsmqGetWork<STATE_WORK_SPIKE>();
        int bEnd = 0;
        switch (pWork->m_nStep) {
        case 0: {
            // Rise to where the hand meets the ball.
            AnimPlay(0xB, 0, 4.0f, 0.0f);
            pWork->m_vFrom = m_vPos;
            pWork->m_vTo = m_vPos;
            pWork->m_fRotFrom = m_vRot.y;
            pWork->m_fRotTo = m_fRotY;
            pWork->m_fFrameNum = AnimGetFrameNum();
            pWork->m_fFrame = 0.0f;
            pWork->m_bHitEnable = 0;
            pWork->m_bSpiked = 0;
            CBall *pBall = GetTargetBall_();
            if (pBall) {
                bcs::FVEC3 vBall;
                pBall->GetPosAfterFrame(vBall, 8.0f + pWork->m_fFrameNum);
                pWork->m_fRotTo = CalcTargetPos_(vBall, pWork->m_vTarget);
                bcs::FVEC3 vHand;
                bcs::BCS_Vec_RotY(vHand, m_vHandOfs, pWork->m_fRotTo);
                bcs::FVEC3Sub(&vBall, &vBall, &vHand);
                pWork->m_vTo = vBall;
            }
            pWork->m_nStep++;
            break;
        }
        case 1: {
            pWork->m_fFrame += GetFrameRateF();
            pWork->m_fFrame = GHOST_CLAMP(pWork->m_fFrame, 0.0f, pWork->m_fFrameNum);
            float fRate = Clamp(pWork->m_fFrame / pWork->m_fFrameNum, 0.0f, 1.0f);
            bcs::FVEC3Lerp(&m_vPos, &pWork->m_vFrom, &pWork->m_vTo, fRate);
            m_vRot.y = LerpRad(pWork->m_fRotFrom, pWork->m_fRotTo, fRate);
            if (IsAnimEnd()) {
                AnimPlay(0xF, 0, 0.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        }
        case 2:
            // The spike: the hand catches the ball between frames 4 and 10
            // of the motion and hits it at once.
            pWork->m_bHitEnable = 0;
            if (IsHoldBall()) {
                SpikeBall_(pWork->m_vTarget);
                pWork->m_bSpiked = 1;
                PlaySePos(0xD35, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            } else if (!pWork->m_bSpiked) {
                float fFrame = AnimGetFrame();
                if (fFrame >= 4.0f && fFrame <= 10.0f) {
                    pWork->m_bHitEnable = 1;
                }
            }
            if (IsAnimEnd()) {
                // Come back down to the net.
                pWork->m_nStep++;
                AnimPlay(1, 1, 8.0f, 0.0f);
                pWork->m_vFrom = m_vPos;
                pWork->m_vTo = m_vPos;
                float fHeight = 1.5f;
                pWork->m_vTo.y = fHeight - 0.5f;
                pWork->m_vTo.z = GetNetSideZ_();
                pWork->m_fRotFrom = m_vRot.y;
                pWork->m_fRotTo = m_fRotY;
                pWork->m_fFrameNum = 60.0f;
                pWork->m_fFrame = 0.0f;
            }
            break;
        case 3: {
            pWork->m_fFrame += GetFrameRateF();
            pWork->m_fFrame = GHOST_CLAMP(pWork->m_fFrame, 0.0f, pWork->m_fFrameNum);
            float fRate = Clamp(pWork->m_fFrame / pWork->m_fFrameNum, 0.0f, 1.0f);
            fRate = Clamp(pow(fRate, 0.8f), 0.0f, 1.0f);
            bcs::FVEC3Lerp(&m_vPos, &pWork->m_vFrom, &pWork->m_vTo, fRate);
            m_vRot.y = LerpRad(pWork->m_fRotFrom, pWork->m_fRotTo, fRate);
            if (pWork->m_fFrame >= pWork->m_fFrameNum) {
                bEnd = 1;
                pWork->m_nStep++;
            }
            break;
        }
        default:
            bEnd = 1;
            break;
        }

        if (bEnd) {
            FsmqSetState(STATE_IDLE);
        }
    }

    void CNpcGhostVolley::DtorStateSpike()
    {
        m_fFloatPhase = -1.5707964f;
    }

    float CNpcGhostVolley::CalcTargetPos_(const bcs::FVEC3 &rBall, bcs::FVEC3 &rTarget)
    {
        bcs::FVEC3 vTarget;
        vTarget.x = rBall.x;
        vTarget.z = rBall.z;
        vTarget.y = 0.0f;

        // The opponents' court, near the ball's x, in the half nearer the
        // net.
        long nGoal = CGame::GetGuardGoalIdx(m_nTeamNo);
        bcs::FVEC4 rect;    // left, right, front, back
        CStageMan::GetInstance()->GetTeamCourtRect(nGoal, &rect.x, &rect.y, &rect.z, &rect.w);
        if (rect.x > rect.y) {
            Swap(rect.x, rect.y);
        }
        if (rect.z > rect.w) {
            Swap(rect.z, rect.w);
        }
        rect.x = Max(rBall.x - 4.0f, rect.x);
        rect.y = Min(4.0f + rBall.x, rect.y);
        if (nGoal) {
            rect.w = 0.5f * (rect.z + rect.w);
        } else {
            rect.z = 0.5f * (rect.z + rect.w);
        }

        vTarget.x = Lerp(rect.x, rect.y, bcs::BCS_RandMax(1.0f));
        vTarget.z = Lerp(rect.z, rect.w, bcs::BCS_RandMax(1.0f));
        rTarget = vTarget;

        bcs::FVEC3 vDir;
        vDir.x = vTarget.x - rBall.x;
        vDir.z = vTarget.z - rBall.z;
        return RoundRad(nw4r::math::Atan2Rad(vDir.x, vDir.z));
    }
}

// A weak copy in the original (VolleyF 0xC98B8, after this file's code);
// BasketF defines it out of line in game/basket/unit_581FC.cpp, so the
// shared header only declares it.
int msw::CNpcGhost::IsHoldBall() const
{
    return (m_nGhostFlag >> 1) & 1;
}
