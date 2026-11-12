// msw::CChrCoreBasketNinja: the ninja in the basketball game.
//
// His special shot (the air trick) splits him in three before the goal:
// two copies of him and of the ball (CC14_BASKET::CcNinjaClone) stand to
// his left and right, held by a task (CC14_BASKET::CcNinjaFactory). While
// the three hang in the air the player may send the ninja one place to the
// side (MoveTargetMarkerPos), where he swaps places with the copy standing
// there; the computer picks a side at random. Then all three come down on
// the goal; the copies vanish in a puff once the real ninja dunks, or when
// someone runs into them.
//
// Positions of the three are numbered 0 (left), 1 (middle) and 2 (right);
// the copies start at 0 and 2, so the ninja's own place is always
// 3 - (copy 0's + copy 1's).
#include "game/msw/CChrCoreBasket.hpp"

#include <string.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CMdlClone.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/EF/CEffect.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameTask.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/math/triangular.h"

// Set by the file's static initialiser and used by nothing in the file; see
// CChrCoreBasketMario.cpp for the same angle.
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);

    // (name guessed) From fFrom to fTo by fRate (0 to 1).
    inline float LerpF(float fFrom, float fTo, float fRate)
    {
        float fOfs = fRate * bcs::BCS_Abs(fFrom - fTo);
        return fFrom - fOfs;
    }

    // (name guessed) r1 - r2 in the XZ plane, as its length.
    inline float DistXZ(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 diff;

        bcs::FVEC3Sub(&diff, &r1, &r2);
        return bcs::BCS_Vec_GetLenXZ(diff);
    }

    // (name guessed) r1 + r2.
    inline bcs::FVEC3 VecAdd(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 sum;

        bcs::FVEC3Sum(sum, r1, r2);
        return sum;
    }

    // (name guessed) The length of rVec in the XZ plane.
    inline float LenXZ(const bcs::FVEC3 &rVec)
    {
        bcs::FVEC3 xz(rVec.x, 0.0f, rVec.z);
        return bcs::FVEC3Len(&xz);
    }
}

namespace msw {
    namespace CC14_BASKET {

        // One copy of the ninja and of the ball he holds: two models copied
        // from the originals, with their shadows and outlines, a body
        // collision and up to seven effects. 0x48 bytes, kept in the
        // CcNinjaFactory. (member names guessed)
        class CcNinjaClone {
            public:
                // The effect slots: 0 to 5 hold effects on the copy's body
                // (EFF_ID), 6 the light of the special shot on the ball.
                enum { EFF_NUM = 7, EFF_LIGHT = 6, MDL_NUM = 2 };

                CcNinjaClone();
                ~CcNinjaClone();

                // Makes the copies of pChr's model and of pBall's; pBody is
                // the owner's body unit, whose size the copy's collision takes.
                void Create(CChrCore *pChr, const CCollBodyUnit *pBody, CBall *pBall);
                void Release();
                void Main_(float fRate);
                // Starts the copy's vanishing: once the special shot runs
                // (bit 0 of the flags) and only once.
                void SetUpDisappear();
                // One step of the vanishing (fade out, stretch); 1 at its end.
                int OnDisappear(float fRate);
                // A body that pushes the copy makes it vanish, and is pushed
                // back.
                static void CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                             bcs::FVEC3 *pPush);

                // (names guessed) Bit 1 of the flags is set while the copy
                // vanishes, bit 2 once it is gone.
                unsigned long IsDisappear() const { return (m_nFlag & 6) != 0; }
                int IsGone() const { return (m_nFlag >> 2) & 1; }
                void OnFlag(unsigned long nBit) { m_nFlag |= nBit; }

                // Where the copy stands; null once it is gone.
                const bcs::FVEC3 *GetPos() const
                {
                    if (((m_nFlag >> 2) & 1) == 1) {
                        return 0;
                    }
                    return &m_pMdl[0]->m_vTrans;
                }

                // Stops the effect in slot nSlot.
                void StopEffect(long nSlot)
                {
                    if (m_hEff[nSlot] != 0) {
                        CEffectCtrl::StopAttachEffect(m_hEff[nSlot]);
                        m_hEff[nSlot] = 0;
                    }
                }

                // Starts the effect of slot nSlot on the copy, unless it
                // vanishes.
                void CreateEffect(long nSlot)
                {
                    if (IsDisappear() != 1) {
                        StopEffect(nSlot);
                        const long EFF_ID[EFF_NUM] = {0xA6, 0xA7, 0xA9, 0xA8, 0x22, 0xAB, -1};
                        m_hEff[nSlot] = CEffectCtrl::CreateAttachEffectAuto(EFF_ID[nSlot], m_pMdl[0], 3);
                    }
                }

                // Starts the light on the copy's ball, nFrame frames into it.
                void CreateLightEffect(unsigned long nFrame)
                {
                    if (IsDisappear() != 1) {
                        StopEffect(EFF_LIGHT);
                        m_hEff[EFF_LIGHT] = CEffectCtrl::CreateAttachEffect(0x5A4, m_pMdl[1], 0, 0, 0, 0, 0, 3,
                                                                            nFrame, 0, 0);
                    }
                }

                // The frames the light on the copy's ball has played, in
                // rFrame (left alone when there is none); then removes it.
                void EraseLightEffect(float &rFrame)
                {
                    unsigned long hLight = m_hEff[EFF_LIGHT];
                    CEffectCluster *pCluster = CEffectCtrl::GetObject(hLight);
                    if (pCluster != 0) {
                        pCluster->GetEffectNum();
                        bcs::EF::CEffect *pEffect = pCluster->FindEffect("baki00_light00");
                        if (pEffect != 0) {
                            rFrame = pEffect->GetCurrentFrame();
                        }
                    }
                    if (m_hEff[EFF_LIGHT] != 0) {
                        CEffectCtrl::EraseAttachEffect(m_hEff[EFF_LIGHT]);
                        m_hEff[EFF_LIGHT] = 0;
                    }
                }

                bcs::CMdlClone *GetMdl() const { return m_pMdl[0]; }

            private:
                // Makes model nMdl (0 the ninja, 1 the ball) from pSrc, with
                // its shadow and outline.
                void CreateMdl_(long nMdl, bcs::CMdlObj *pSrc);

                CBodyCollision *m_pBody;            // 0x00
                const CCollBodyUnit *m_pOwnerBody;  // 0x04
                CChrCore *m_pChr;                   // 0x08
                unsigned long m_nFlag;              // 0x0C
                unsigned long m_hEff[EFF_NUM];      // 0x10
                float m_fDisappear;                 // 0x2C, 0 to 1 while the copy vanishes
                bcs::CMdlClone *m_pMdl[MDL_NUM];    // 0x30
                bcs::CMdlShadow *m_pShadow[MDL_NUM]; // 0x38
                bcs::CMdlRef *m_pRef[MDL_NUM];      // 0x40
        };

        // The task that holds the two copies during the special shot. Its
        // states follow the shot: SP_1_A while the ninja jumps, SPLoop while
        // the three hang in the air, SP_2_A while they come down, and
        // Disappear, which ends the task once both copies are gone. 0x134
        // bytes. (member names guessed)
        class CcNinjaFactory : public CGameTask, public bcs::CFsm<CcNinjaFactory> {
            public:
                enum { STATE_SP_1_A, STATE_SP_LOOP, STATE_SP_2_A, STATE_DISAPPEAR, STATE_NUM };
                enum { CLONE_NUM = 2 };

                CcNinjaFactory(bcs::CTask *pParent, CChrCore *pChr, const CCollBodyUnit *pBody, CBall *pBall);
                virtual ~CcNinjaFactory();
                virtual void Main();

                void CtorStateSP_1_A();
                void LoopStateSP_1_A();
                void DtorStateSP_1_A();
                void CtorStateSPLoop();
                void LoopStateSPLoop();
                void DtorStateSPLoop();
                void CtorStateSP_2_A();
                void LoopStateSP_2_A();
                void DtorStateSP_2_A();
                void CtorStateDisappear();
                void LoopStateDisappear();
                void DtorStateDisappear();

                // The state of the task's state machine.
                long GetFsmState() const { return m_nState; }

                // The ninja's own place (0 to 2).
                long GetOwnerPos() const { return 3 - (m_nClonePos[0] + m_nClonePos[1]); }

                // Sends the ninja one place left (nMove 4) or right (any
                // other) and swaps him with the copy standing there. The
                // lights on the two balls change hands: fFrame is how far
                // the ninja's has played, rFrame gets the copy's. 1 when the
                // ninja moved, 0 when he stands at the edge already.
                int ChangePosition(long nMove, float fFrame, float &rFrame)
                {
                    long nPos = GetOwnerPos();
                    long nNewPos;
                    if (nMove == 4) {
                        nNewPos = nPos - 1;
                        if (nNewPos < 0) {
                            nNewPos = 0;
                        }
                    } else {
                        nNewPos = nPos + 1;
                        if (nNewPos > 2) {
                            nNewPos = 2;
                        }
                    }
                    if (nNewPos != nPos) {
                        for (long i = 0; i < CLONE_NUM; i++) {
                            if (nNewPos == m_nClonePos[i]) {
                                m_nClonePos[i] = nPos;
                                m_Clone[i].EraseLightEffect(rFrame);
                                m_Clone[i].CreateLightEffect(fFrame);
                                return 1;
                            }
                        }
                    }
                    return 0;
                }

                // Places the ninja and the copies around m_BasePos while
                // the ninja's jump or dunk motion plays: side by side across
                // the line to the goal, spreading out as the jump goes on,
                // all at the same distance from the goal and facing it.
                void UpdatePosition()
                {
                    if (m_pChr == 0) {
                        return;
                    }
                    float fRate = 1.0f;
                    switch (m_pChr->AnimGetMotID(3, 0)) {
                        case 0x7E: {
                            float fFrameNum = m_pChr->AnimGetFrameNum(3);
                            float f = m_pChr->AnimGetFrame(3) / fFrameNum;
                            fRate = f > 1.0f ? 1.0f : (f < 0.0f ? 0.0f : f);
                        }
                        // fall through
                        case 0x8D:
                        case 0x83: {
                            const bcs::FVEC3 &rGoal = m_pChr->GetTargetGoalPos();
                            bcs::FVEC3 dir;
                            float fDist = GetDirXZ_(dir, m_BasePos, rGoal);
                            bcs::BCS_Vec_Normalize(dir, dir);
                            float fWidth = 3.5f * fRate;

                            for (long i = 0; i < CLONE_NUM; i++) {
                                if (m_nClonePos[i] == 1) {
                                    if (m_Clone[i].GetMdl() != 0) {
                                        m_Clone[i].GetMdl()->m_vTrans = m_BasePos;
                                    }
                                } else {
                                    bcs::FVEC3 pos;
                                    GetSidePos_(pos, dir, i == 0 ? 1.5707964f : -1.5707964f, fWidth, fDist, rGoal);
                                    if (m_Clone[i].GetMdl() != 0) {
                                        m_Clone[i].GetMdl()->m_vTrans = pos;
                                    }
                                }

                                const bcs::FVEC3 *pPos = m_Clone[i].GetPos();
                                float fRotY;
                                if (pPos != 0) {
                                    fRotY = GetRotYTo_(*pPos, rGoal);
                                } else {
                                    fRotY = 0.0f;
                                }
                                bcs::FVEC3 rot(0.0f, fRotY, 0.0f);
                                if (m_Clone[i].GetMdl() != 0) {
                                    m_Clone[i].GetMdl()->SetRotXYZ(rot.x, rot.y, rot.z);
                                }
                            }

                            long nPos = GetOwnerPos();
                            if (nPos == 1) {
                                m_pChr->SetPos(m_BasePos);
                            } else {
                                bcs::FVEC3 pos;
                                GetSidePos_(pos, dir, nPos == 0 ? 1.5707964f : -1.5707964f, fWidth, fDist, rGoal);
                                m_pChr->SetPos(pos);
                            }
                            m_pChr->SetRotY(m_pChr->GetRotY_ToPosXZ(rGoal));
                            break;
                        }
                    }
                }

            private:
                // rDir = rFrom - rTo in the XZ plane; returns its length.
                static float GetDirXZ_(bcs::FVEC3 &rDir, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
                {
                    bcs::FVEC3 diff;
                    bcs::FVEC3Sub(&diff, &rFrom, &rTo);
                    rDir.Set(diff.x, 0.0f, diff.z);
                    bcs::FVEC3 len(rDir);
                    return bcs::FVEC3Len(&len);
                }

                // The yaw from rFrom towards rTo, in [0, 2 pi].
                static float GetRotYTo_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
                {
                    bcs::FVEC3 to(rFrom);
                    to.x = rTo.x - rFrom.x;
                    to.z = rTo.z - rFrom.z;
                    return bcs::ROUND_PI(nw4r::math::Atan2Rad(to.x, to.z));
                }

                // The point fWidth to the side (fRotY from rDir) of
                // m_BasePos, brought to fDist from rGoal.
                void GetSidePos_(bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir, float fRotY, float fWidth, float fDist,
                                 const bcs::FVEC3 &rGoal)
                {
                    bcs::BCS_Vec_RotY(rPos, rDir, fRotY);
                    rPos.x = rPos.x * fWidth + m_BasePos.x;
                    rPos.y = m_BasePos.y;
                    rPos.z = rPos.z * fWidth + m_BasePos.z;
                    bcs::FVEC3Sub(&rPos, &rPos, &rGoal);
                    bcs::FVEC3 xz(rPos.x, 0.0f, rPos.z);
                    bcs::BCS_Vec_Normalize(rPos, xz);
                    rPos.x = rPos.x * fDist + rGoal.x;
                    rPos.y = m_BasePos.y;
                    rPos.z = rPos.z * fDist + rGoal.z;
                }

            public:
                CChrCore *m_pChr;           // 0x7C, the ninja
                long m_nClonePos[CLONE_NUM]; // 0x80, the places of the copies
                float m_fFrame;             // 0x88, cleared when the copies start to vanish
                bcs::FVEC3 m_Unk8C;         // 0x8C, cleared by the constructor only
                bcs::FVEC3 m_BasePos;       // 0x98, the middle place
                CcNinjaClone m_Clone[CLONE_NUM]; // 0xA4
        };
    }
}

namespace msw {

    // Hit, the ninja shows himself again (as the body-visible trigger
    // would).
    void CChrCoreBasketNinja::CtorStateDamage()
    {
        bcs::CAnmObjMotBase::MOT_TRG trg;

        memset(&trg, 0, sizeof(trg));
        trg.m_nType = 0x23;
        trg.m_nParam0 = 0;
        CChrCore::CallbackMotTrg(&trg, this);
        CChrCoreBasket::CtorStateDamage();
    }

    void CChrCoreBasketNinja::DtorStateTrickMove()
    {
        bcs::CAnmObjMotBase::MOT_TRG trg;

        memset(&trg, 0, sizeof(trg));
        trg.m_nType = 0x23;
        trg.m_nParam0 = 0;
        CChrCore::CallbackMotTrg(&trg, this);
        CChrCoreBasket::DtorStateTrickMove();
    }

    void CChrCoreBasketNinja::CtorStateDribbleMiss()
    {
        CChrCoreBasket::CtorStateDribbleMiss();

        bcs::CAnmObjMotBase::MOT_TRG trg;
        memset(&trg, 0, sizeof(trg));
        trg.m_nType = 0x23;
        CChrCore::CallbackMotTrg(&trg, this);
    }

    // Trigger 0x23 hides (parameter not zero) or shows the ninja's body.
    // Bit 0x10000000 of the status flags is set while it is hidden (by the
    // CChrCore code the trigger goes on to). When the body changes in
    // state 0x25 or 0x1E a puff of smoke is left at his feet.
    void CChrCoreBasketNinja::CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *pTrg)
    {
        long bHide = pTrg->m_nParam0;
        int bChange = 1;
        if (!IsBodyHideTrg_(bHide) && !IsBodyShowTrg_(bHide)) {
            bChange = 0;
        }
        if (bChange && (long)pTrg->m_nType == 0x23) {
            int bShow = bHide == 0;
            switch (GetState()) {
                case 0x25: {
                    long nEff = 0x678;
                    if (bShow) {
                        nEff = 0x679;
                    }
                    bcs::FVEC3 rot(0.0f, GetRotY(), 0.0f);
                    Eff_CreateSimple(nEff, GetPos(), rot, 3, 0, 0);
                    break;
                }
                case 0x1E: {
                    long nEff = 0x676;
                    if (bShow) {
                        nEff = 0x677;
                    }
                    bcs::FVEC3 rot(0.0f, GetRotY(), 0.0f);
                    Eff_CreateSimple(nEff, GetPos(), rot, 3, 0, 0);
                    break;
                }
            }
        }
        CChrCoreBasket::CallbackMotTrg_(pTrg);
    }

    // The ball the ninja holds hides with him (ball flag 4; the basket
    // flag 0x4000 records that it was hidden this way).
    void CChrCoreBasketNinja::CallbackMotTrg_BodyVisible_(long bHide, long)
    {
        CBallBasket *pBall = static_cast<CBallBasket *>(GetAttachBall());
        if (pBall != 0) {
            if (bHide != 0) {
                pBall->OnBasketFlag(0x4000);
                pBall->OnFlag(4);
            } else {
                if (pBall->GetBasketFlag() & 0x4000) {
                    pBall->OffFlag(4);
                }
                pBall->OffBasketFlag(0x4000);
            }
        }
    }

    CChrCoreBasketNinja::CChrCoreBasketNinja()
    {
        m_hFactory = 0;
        m_hSpSe = 0;
    }

    CChrCoreBasketNinja::~CChrCoreBasketNinja()
    {
        DestroyClone_();
        StopSpSe_(0.0f);
    }

    // During the special shot, the middle of the ninja and his copies.
    void CChrCoreBasketNinja::GetSpRootPos(bcs::FVEC3 &rPos) const
    {
        rPos = GetPos();
        if (GetState() == 0x27) {
            bcs::FVEC3 sum(GetPos());
            long nNum = 1;
            for (long i = 0; i < CC14_BASKET::CcNinjaFactory::CLONE_NUM; i++) {
                const bcs::FVEC3 *pPos = GetClonePos(i);
                if (pPos != 0) {
                    nNum++;
                    sum.x += pPos->x;
                    sum.y += pPos->y;
                    sum.z += pPos->z;
                }
            }
            if (nNum > 1) {
                rPos.x = sum.x /= nNum;
                rPos.y = sum.y /= nNum;
                rPos.z = sum.z /= nNum;
            }
        }
    }

    void CChrCoreBasketNinja::CtorStateAirTrick()
    {
        m_nSpMove = 0;
        SubStateAirTrickCmnCtor_();
    }

    // The steps of the special shot (pWork->m_nStep):
    //  0  waits for the arts event, then jumps towards the goal, 5 to 10
    //     units (nearer the goal, shorter), kept 3 units inside the court
    //     and within 20 degrees of the line out of the nearer goal; the
    //     copies are made;
    //  1  the jump; at its end the three hang in the air (step 6) for 60
    //     frames, and the computer picks a side if the player did not;
    //  6  the wait, ended early by the driver's answer 4; then the three
    //     fly to 1.5 units before the goal;
    //  7  that flight; at its end the copies vanish and the dunk starts;
    //  8  the dunk: the ball is shot at frame 49; at the end a hop back;
    //  9  the hop; on landing the state's common end (step 0x16);
    //  others: falls until the motion ends, then the state is left.
    void CChrCoreBasketNinja::LoopStateAirTrick()
    {
        STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
        CChrManBasket::ClearPlayerIcon(this, 0);

        switch (pWork->m_nStep) {
            case 0: {
                if (CGame::IsArtsEventMode() || CGame::IsReqArtsEvent()) {
                    break;
                }
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)0, IsHaveLeft()), 0, 8.0f, 0.0f);
                AnimSetSpeed(3, 1.0f);

                float f3Point = GetCourt3PointSize();
                float fRate = DistXZ(GetTargetGoalPos(), GetPos()) / f3Point;
                fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
                float fLen = LerpF(10.0f, 5.0f, fRate);

                pWork->m_fMoveFrameNum = AnimGetFrameNum(3);
                pWork->m_fRotY = GetRotY_ToPosXZ(GetTargetGoalPosOffZ_());
                bcs::FVEC3 dir;
                bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY + bcs::DEG_TO_RAD(180.0f));
                pWork->m_MoveVec.x = dir.x * fLen + GetPos().x;
                pWork->m_MoveVec.y = 0.0f;
                pWork->m_MoveVec.z = dir.z * fLen + GetPos().z;

                // Within the court.
                float fLimit = bcs::BCS_Abs(CStageMan::GetInstance()->GetCourtFront()) - 3.0f;
                float fZ = pWork->m_MoveVec.z;
                pWork->m_MoveVec.z = fZ > fLimit ? fLimit : (fZ < -fLimit ? -fLimit : fZ);

                // Within 20 degrees of the line out of the nearer goal.
                long anGoal[2];
                anGoal[0] = GetTargetGoalIdx();
                anGoal[1] = GetGuardGoalIdx();
                float fMaxRot = bcs::DEG_TO_RAD(20.0f);
                float fGuardDist = bcs::BCS_Abs(pWork->m_MoveVec.z - GetGuardGoalPos().z);
                int bGuard = !(bcs::BCS_Abs(pWork->m_MoveVec.z - GetTargetGoalPos().z) < fGuardDist);
                long nGoal = anGoal[bGuard];
                bcs::FVEC3 goal(CGame::GetGoalPos(nGoal));
                float fGoalRot = CGame::GetGoalYRot(nGoal);
                float fRot = bcs::ROUND_PI(CChrManBasket::GetRotY_Pos1ToPos2XZ(goal, pWork->m_MoveVec) - fGoalRot);
                if (fRot > 3.1415927f) {
                    fRot -= 6.2831855f;
                }
                if (bcs::BCS_Abs(fRot) > fMaxRot) {
                    float fDist = DistXZ(goal, pWork->m_MoveVec);
                    if (!(fRot > 0.0f)) {
                        fMaxRot = -fMaxRot;
                    }
                    bcs::FVEC3 ofs;
                    bcs::BCS_VecZ_RotY(ofs, 1.0f, bcs::ROUND_PI(fMaxRot + fGoalRot));
                    pWork->m_MoveVec.x = ofs.x * fDist + goal.x;
                    pWork->m_MoveVec.y = 0.0f;
                    if (!bGuard) {
                        pWork->m_MoveVec.z = ofs.z * fDist + goal.z;
                    }
                }

                SetRotY(GetRotY_FromPosXZ(pWork->m_MoveVec));
                pWork->m_MoveVec.x = (pWork->m_MoveVec.x - GetPos().x) / pWork->m_fMoveFrameNum;
                pWork->m_MoveVec.z = (pWork->m_MoveVec.z - GetPos().z) / pWork->m_fMoveFrameNum;
                Util_SetJumpParam(pWork->m_fMoveFrameNum, GetPos().y, 2.65f, 3.1f);

                ArtsEff_StopAll();
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x13, 0xA5);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x1E, 0xA6);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x1F, 0xA7);
                Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
                CreateClone_();
                SetArtsBasePos_(GetPos());
                PlaySeSPStart();
                PlaySePos(0x471, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nStep = 1;
                break;
            }

            case 1: {
                float fRate = GetWholeFrameRate();
                bcs::FVEC3 *pBase = GetArtsBasePos_();
                SetPos(*pBase);
                Util_DoJumpMove(fRate);
                pBase->x += pWork->m_MoveVec.x * fRate;
                pBase->z += pWork->m_MoveVec.z * fRate;
                pBase->y = GetPos().y;
                WholeUpDatePosition_(*pBase);
                DriverSendMessage((CChrDrv::MSG_ID)0x28, 0, 0, 0, 0);
                if (IsAnimEnd(3)) {
                    AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)5, IsHaveLeft()), 1, 0.0f, 0.0f);
                    pWork->m_fWaitFrame = 60.0f;
                    ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x1D, 0xA8);
                    ArtsEff_Stop((ARTS_EFF_SLOT)0x1E);
                    ArtsEff_CreateAttach((ARTS_EFF_SLOT)0x1E, 0x5A4, GetVNode(4), 0);
                    ChangeStateClone_(CC14_BASKET::CcNinjaFactory::STATE_SP_LOOP);
                    StopSpSe_(0.0f);
                    m_hSpSe = PlayMoveSe(0x476, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
                    ArtsEff_PlayAttackSign();

                    // A puff at the feet of each copy as it appears.
                    bcs::FVEC3 node;
                    GetVNodePos(1, node);
                    static const long NUM = CC14_BASKET::CcNinjaFactory::CLONE_NUM;
                    for (long i = 0; i < NUM; i++) {
                        const bcs::FVEC3 *pPos = GetClonePos(i);
                        if (pPos != 0) {
                            CEffectCtrl::SimpleEffect(0x4D7, bcs::FVEC3(pPos->x, node.y, pPos->z), 3, 0, 0);
                        }
                    }

                    // The computer's choice of a place.
                    if (m_nSpMove == 0) {
                        static const long NUM = 3;
                        long anMove[NUM] = {1, 2, 4};
                        m_nSpMove = anMove[bcs::BCS_RandMax(NUM)];
                    }
                    if (m_nSpMove == 2 || m_nSpMove == 4) {
                        CC14_BASKET::CcNinjaFactory *pFactory =
                            static_cast<CC14_BASKET::CcNinjaFactory *>(bcs::CTaskMan::GetTask(m_hFactory));
                        if (pFactory != 0) {
                            float fFrame = 10.0f;
                            float fCloneFrame = fFrame;
                            CEffectCluster *pCluster = CEffectCtrl::GetObject(Eff_GetHandle(0x1E));
                            if (pCluster != 0) {
                                pCluster->GetEffectNum();
                                bcs::EF::CEffect *pEffect = pCluster->FindEffect("baki00_light00");
                                if (pEffect != 0) {
                                    fFrame = 1.0f + pEffect->GetCurrentFrame();
                                }
                            }
                            if (pFactory->ChangePosition(m_nSpMove, fFrame, fCloneFrame) == 1) {
                                ArtsEff_Erase((ARTS_EFF_SLOT)0x1E);
                                ArtsEff_CreateAttach((ARTS_EFF_SLOT)0x1E, 0x5A4, GetVNode(4), fCloneFrame);
                                WholeUpDatePosition_();
                            }
                        }
                    }
                    pWork->m_nStep = 6;
                }
                break;
            }

            case 6: {
                pWork->m_fWaitFrame -= GetWholeFrameRate();
                long nAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x21, pWork->m_fWaitFrame, 60, 0, 0);
                if (pWork->m_fWaitFrame <= 0.0f || nAnswer == 4) {
                    AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)6, IsHaveLeft()), 0, 0.0f, 0.0f);
                    SetPos(*GetArtsBasePos_());
                    float fFrameNum = AnimGetFrameNum(3);
                    pWork->m_fMoveFrameNum = fFrameNum;
                    pWork->m_fMoveFrameNum += AnimGetResFrameNum(0x84);

                    bcs::FVEC3 ofs;
                    bcs::BCS_VecZ_RotY(ofs, 1.5f, GetRotY_FromPosXZ(GetTargetGoalPos()));
                    pWork->m_MoveVec = VecAdd(GetTargetGoalPos(), ofs);
                    float fLen = DistXZ(pWork->m_MoveVec, GetPos());
                    pWork->m_fMoveLen = fLen;
                    pWork->m_fMoveRate = 0.0f;
                    pWork->m_fMoveFrame = 0.0f;
                    pWork->m_fMoveUnk18 = 73.0f * fLen / 100.0f / fFrameNum;
                    pWork->m_fRotY = GetRotY_ToGoal_NonOffset();
                    Util_SetJumpParam(fFrameNum, GetPos().y, 3.2f, 3.6f);

                    Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
                    ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x18, 0xA9);
                    ArtsEff_Stop((ARTS_EFF_SLOT)0x1F);
                    ChangeStateClone_(CC14_BASKET::CcNinjaFactory::STATE_SP_2_A);
                    WholeUpDatePosition_(GetPos());
                    StopSpSe_(0.0f);
                    m_hSpSe = PlayMoveSe(0x478, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
                    PlaySePos(0x477, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                    PlayVoice(0x24, 0);
                    pWork->m_nStep = 7;
                }
                break;
            }

            case 7:
                SetPos(*GetArtsBasePos_());
                SubStateAirTrickMove_(pWork, 70.0f, 30.0f);
                WholeUpDatePosition_(GetPos());
                if (IsAnimEnd(3) == 1) {
                    AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)7, IsHaveLeft()), 0, 0.0f, 0.0f);
                    float fFrameNum = AnimGetFrameNum(3);

                    bcs::FVEC3 ofs;
                    bcs::BCS_VecZ_RotY(ofs, 1.5f, GetRotY_FromPosXZ(GetTargetGoalPos()));
                    pWork->m_MoveVec = VecAdd(GetTargetGoalPos(), ofs);
                    float fLen = DistXZ(pWork->m_MoveVec, GetPos());
                    pWork->m_fMoveLen = fLen;
                    pWork->m_fMoveRate = 0.0f;
                    pWork->m_fMoveFrame = 0.0f;
                    pWork->m_fMoveUnk18 = fLen / fFrameNum;
                    pWork->m_fRotY = GetRotY_ToGoal_NonOffset();
                    Util_SetJumpParam(fFrameNum, GetPos().y, 3.55f, 5.6f);

                    ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x19, 0xAA);
                    ArtsEff_Stop((ARTS_EFF_SLOT)0x1E);
                    StopSpSe_(0.0f);
                    DestroyClone_();
                    pWork->m_nStep = 8;
                }
                break;

            case 8:
                if (IsHaveBall() == 1 && AnimGetFrame(3) >= 49.0f) {
                    static const long nMode = 9;
                    static const float fShootRate = 1.0f;
                    static_cast<CBallBasket *>(GetAttachBall())->SetArtsShootSpeed(0.17f);
                    Util_ShootBall(nMode, fShootRate, 1, 0);
                }
                SubStateAirTrickMove_(pWork, 40.0f, 60.0f);
                if (IsAnimEnd(3) == 1) {
                    AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)8, IsHaveLeft()), 0, 0.0f, 0.0f);
                    float fFrameNum = AnimGetFrameNum(3);
                    pWork->m_fMoveRate = 0.0f;

                    // A hop back, away from the goal.
                    bcs::FVEC3 dir;
                    pWork->m_fRotY = bcs::ROUND_PI(pWork->m_fRotY + bcs::DEG_TO_RAD(180.0f));
                    bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY);
                    pWork->m_MoveVec.x = 0.05f * dir.x;
                    pWork->m_MoveVec.y = 0.0f;
                    pWork->m_MoveVec.z = 0.05f * dir.z;
                    Util_SetJumpParam(fFrameNum, GetPos().y, 0.0f, 5.8f);
                    pWork->m_nStep = 9;
                }
                break;

            case 9: {
                // m_fMoveRate keeps the motion frame of the last update.
                float fFrame = AnimGetFrame(3);
                float fStep = fFrame - pWork->m_fMoveRate;
                pWork->m_fMoveRate = fFrame;
                bcs::FVEC3 move;
                bcs::FVEC3Scale(&move, &pWork->m_MoveVec, fStep);
                AddPos(move);
                if (Util_DoJumpMove(fStep) || IsAnimEnd(3)) {
                    AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)0xF, IsHaveLeft()), 0, 0.0f, 0.0f);
                    AnimSetSpeed(3, 1.0f);
                    SetCollBodyPushRank_(1);
                    SetCollBodyPushPower_(5);
                    bcs::FVEC3 dir;
                    bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY);
                    static const float fLen = 0.15f;
                    m_fJumpVelX = fLen * dir.x;
                    m_fJumpVelZ = fLen * dir.z;
                    ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x21, 0xAC);
                    pWork->m_nStep = 0x16;
                }
                break;
            }

            default:
                Util_DoJumpMove(m_fTimeScale * m_fFrameStep);
                if (!GetFlag(0xC00) && IsAnimEnd(3) == 1) {
                    ResetFlag(0x20);
                    FsmqDoChain();
                }
                break;
        }
    }

    void CChrCoreBasketNinja::DtorStateAirTrick()
    {
        CChrManBasket::DispPlayerIconSetCore(this);
        StopSpSe_(0.0f);
        DestroyClone_();
        CChrCoreBasket::DtorStateAirTrick();
    }

    // Makes the task of the copies, which start to the ninja's left and
    // right.
    void CChrCoreBasketNinja::CreateClone_()
    {
        DestroyClone_();
        CC14_BASKET::CcNinjaFactory *pFactory =
            new CC14_BASKET::CcNinjaFactory(GetParentTask(), this, CollBodyGetColl_(0), GetAttachBall());
        if (pFactory != 0) {
            const long anPos[CC14_BASKET::CcNinjaFactory::CLONE_NUM] = {0, 2};
            for (long i = 0; i < CC14_BASKET::CcNinjaFactory::CLONE_NUM; i++) {
                pFactory->m_nClonePos[i] = anPos[i];
            }
            m_hFactory = pFactory->GetId();
        }
    }

    // Lets the copies vanish (the task ends itself afterwards).
    void CChrCoreBasketNinja::DestroyClone_()
    {
        if (m_hFactory != 0) {
            CC14_BASKET::CcNinjaFactory *pFactory =
                static_cast<CC14_BASKET::CcNinjaFactory *>(bcs::CTaskMan::GetTask(m_hFactory));
            if (pFactory != 0 && pFactory->GetFsmState() != CC14_BASKET::CcNinjaFactory::STATE_DISAPPEAR &&
                pFactory->IsDelete() != 1) {
                pFactory->ChangeState(CC14_BASKET::CcNinjaFactory::STATE_DISAPPEAR);
            }
            m_hFactory = 0;
        }
    }

    void CChrCoreBasketNinja::ChangeStateClone_(long nState)
    {
        if (m_hFactory != 0) {
            CC14_BASKET::CcNinjaFactory *pFactory =
                static_cast<CC14_BASKET::CcNinjaFactory *>(bcs::CTaskMan::GetTask(m_hFactory));
            if (pFactory != 0) {
                pFactory->ChangeState(nState);
            }
        }
    }

    // During the special shot, while the ninja holds the ball, the stick
    // picks his move: ahead (1) within 45 degrees of straight on, back (3)
    // beyond 135, else left (2) or right (4).
    int CChrCoreBasketNinja::MoveTargetMarkerPos(float fRotY)
    {
        if (GetState() != 0x27) {
            return 0;
        }
        if (!IsHoldBall()) {
            return 0;
        }
        while (fRotY > 6.2831855f) {
            fRotY -= 6.2831855f;
        }
        while (fRotY < 0.0f) {
            fRotY += 6.2831855f;
        }
        if (fRotY > 3.1415927f) {
            fRotY -= 6.2831855f;
        }
        static const float f45Rad = 0.7853982f;
        float fAbs = __fabsf(fRotY);
        long nMove;
        if (fAbs < f45Rad) {
            nMove = 1;
        } else if (fAbs > 3.0f * f45Rad) {
            nMove = 3;
        } else if (fRotY > 0.0f) {
            nMove = 2;
        } else {
            nMove = 4;
        }
        m_nSpMove = nMove;
        return 1;
    }

    // While the three come down on the goal (steps 7 and 8), a character
    // of the other side running into the ninja blocks the dunk (in step 8
    // only one in its star state, condition 1).
    void CChrCoreBasketNinja::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (GetState() == 0x27) {
            STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
            if ((pWork->m_nStep == 7 || pWork->m_nStep == 8) && pOther->GetOwnerKind() == 1) {
                CChrCore *pChr = CChrMan::GetChrCore(pOther->m_nOwnerIdx);
                if (pChr != 0 && !pChr->IsDisable() && (pWork->m_nStep != 8 || pChr->GetComLevelWork() == 1)) {
                    CheckAirTrickDunkBlock_(pChr);
                }
            }
        }
        CChrCoreBasket::CallbackBodyPush_(pBody, pOther, pPush);
    }

    const bcs::FVEC3 *CChrCoreBasketNinja::GetClonePos(long nClone) const
    {
        if (m_hFactory != 0) {
            CC14_BASKET::CcNinjaFactory *pFactory =
                static_cast<CC14_BASKET::CcNinjaFactory *>(bcs::CTaskMan::GetTask(m_hFactory));
            if (pFactory != 0) {
                return pFactory->m_Clone[nClone].GetPos();
            }
        }
        return 0;
    }

    bcs::FVEC3 *CChrCoreBasketNinja::GetArtsBasePos_()
    {
        if (m_hFactory != 0) {
            CC14_BASKET::CcNinjaFactory *pFactory =
                static_cast<CC14_BASKET::CcNinjaFactory *>(bcs::CTaskMan::GetTask(m_hFactory));
            if (pFactory != 0) {
                return &pFactory->m_BasePos;
            }
        }
        return 0;
    }

    void CChrCoreBasketNinja::WholeUpDatePosition_()
    {
        if (m_hFactory != 0) {
            CC14_BASKET::CcNinjaFactory *pFactory =
                static_cast<CC14_BASKET::CcNinjaFactory *>(bcs::CTaskMan::GetTask(m_hFactory));
            if (pFactory != 0) {
                pFactory->UpdatePosition();
            }
        }
    }

    void CChrCoreBasketNinja::WholeUpDatePosition_(const bcs::FVEC3 &rPos)
    {
        if (m_hFactory != 0) {
            CC14_BASKET::CcNinjaFactory *pFactory =
                static_cast<CC14_BASKET::CcNinjaFactory *>(bcs::CTaskMan::GetTask(m_hFactory));
            if (pFactory != 0) {
                pFactory->m_BasePos = rPos;
                pFactory->UpdatePosition();
            }
        }
    }

    void CChrCoreBasketNinja::SetArtsBasePos_(const bcs::FVEC3 &rPos)
    {
        if (m_hFactory != 0) {
            CC14_BASKET::CcNinjaFactory *pFactory =
                static_cast<CC14_BASKET::CcNinjaFactory *>(bcs::CTaskMan::GetTask(m_hFactory));
            if (pFactory != 0) {
                pFactory->m_BasePos = rPos;
            }
        }
    }

    void CChrCoreBasketNinja::StopSpSe_(float fFrames)
    {
        StopSe(m_hSpSe, fFrames);
        m_hSpSe = 0;
    }

    // In the special shot, while its dunk motion plays.
    int CChrCoreBasketNinja::IsAirTrickDunkMode() const
    {
        if (GetState() != 0x27) {
            return 0;
        }
        return (unsigned long)(AnimGetMotID(3, 0) - 0x83) <= 1;
    }
}

namespace msw {
    namespace CC14_BASKET {

        CcNinjaClone::CcNinjaClone()
        {
            m_pBody = 0;
            m_pOwnerBody = 0;
            m_pChr = 0;
            m_nFlag = 0;
            m_fDisappear = 0.0f;
            for (long i = 0; i < MDL_NUM; i++) {
                m_pMdl[i] = 0;
                m_pShadow[i] = 0;
                m_pRef[i] = 0;
            }
            for (long i = 0; i < EFF_NUM; i++) {
                m_hEff[i] = 0;
            }
        }

        CcNinjaClone::~CcNinjaClone()
        {
            Release();
        }

        void CcNinjaClone::Create(CChrCore *pChr, const CCollBodyUnit *pBody, CBall *pBall)
        {
            m_pChr = pChr;
            m_pOwnerBody = pBody;
            CreateMdl_(0, pChr->GetMdl());
            CreateMdl_(1, pBall->GetMdlObj());

            m_pBody = new (0) CBodyCollision(1, 1, 2, m_pMdl[0]->GetHandle());
            m_pBody->SetPushRank(0, 5);
            m_pBody->SetPushPower(0, 6);
            m_pBody->ResetTargetFlag(0, 0x14);
            m_pBody->SetNoHitFlagAll(1 << m_pChr->GetPlayerNo());
            m_pBody->SetPushEnable(0);
            m_pBody->SetEnable(0);
            m_pBody->SetIndexAll(m_pChr->GetPlayerNo());
            m_pBody->SetBodyPushCallbackAll(CallbackBodyPush, this);

            CreateEffect(0);
            CreateEffect(1);
            CreateEffect(4);
        }

        void CcNinjaClone::Release()
        {
            for (long i = 0; i < EFF_NUM; i++) {
                StopEffect(i);
            }
            for (long i = 0; i < MDL_NUM; i++) {
                if (m_pRef[i] != 0) {
                    bcs::CScnObj::AppendDelayDeleteObject(m_pRef[i]);
                }
                m_pRef[i] = 0;
                if (m_pShadow[i] != 0) {
                    bcs::CScnObj::AppendDelayDeleteObject(m_pShadow[i]);
                }
                m_pShadow[i] = 0;
                if (m_pMdl[i] != 0) {
                    bcs::CScnObj::AppendDelayDeleteObject(m_pMdl[i]);
                }
                m_pMdl[i] = 0;
            }
            if (m_pBody != 0) {
                delete m_pBody;
            }
            m_pBody = 0;
            m_pChr = 0;
        }

        void CcNinjaClone::CreateMdl_(long nMdl, bcs::CMdlObj *pSrc)
        {
            nw4r::g3d::ResFile file = pSrc->GetResMdl().GetParent();

            nw4r::g3d::ResMdl mdl = file.GetResMdl(0);
            if (mdl.IsValid()) {
                bcs::CMdlClone *pMdl = new (0) bcs::CMdlClone(pSrc, mdl, 0, 0, 0);
                pMdl->OnFlag(0x102);
                pMdl->SetExBlendMode(1);
                pMdl->m_vTrans = pSrc->m_vTrans;
                pMdl->SetRotXYZ(pSrc->m_fRotX, pSrc->m_fRotY, pSrc->m_fRotZ);
                pMdl->CalcRootMtx();
                pMdl->CalcWorldMatrixAll(0);
                pMdl->OnFlag(0x4000);
                m_pMdl[nMdl] = pMdl;
                bcs::CRenderMan::PushBack(pMdl, 2);
            }

            nw4r::g3d::ResMdl shadow = file.GetResMdl(2);
            if (shadow.IsValid()) {
                m_pShadow[nMdl] = new bcs::CMdlShadow(m_pMdl[nMdl], shadow, 0x40, 0, 0);
                bcs::CRenderMan::PushBack(m_pShadow[nMdl], 2);
            }

            nw4r::g3d::ResMdl ref = file.GetResMdl(1);
            if (ref.IsValid()) {
                m_pRef[nMdl] = new bcs::CMdlRef(m_pMdl[nMdl], ref, 0, 0, 0);
                bcs::CRenderMan::PushBack(m_pRef[nMdl], 4);
            }

            // The copy of the ball takes the stage's ball colour.
            if (nMdl == 1) {
                bcs::BCS_Color color(0xFFFFFFFF);
                if (CBallMan::GetStageBallColor(color)) {
                    m_pMdl[nMdl]->SetMatTevColor((_GXTevRegID)1, color);
                }
            }
        }

        // Keeps the collision on the copy's model and the ball's copy in
        // the copy's hand, and updates the two models.
        void CcNinjaClone::Main_(float fRate)
        {
            if (m_nFlag & 4) {
                return;
            }
            if (((m_nFlag >> 1) & 1) == 1 && OnDisappear(fRate) == 1) {
                Release();
                return;
            }
            if (m_pBody->IsEnable(0) == 1) {
                m_pBody->SetSize(0, m_pOwnerBody->m_Size.x, m_pOwnerBody->m_Size.y, m_pOwnerBody->m_Size.z);
                m_pBody->SetYRot(0, m_pMdl[0]->m_fRotY);
                m_pBody->SetPos(0, m_pMdl[0]->m_vTrans, m_pMdl[0]->m_vTrans);
            }

            bcs::CMdlClone *pBallMdl = m_pMdl[1];
            if (pBallMdl != 0 && m_pMdl[0] != 0) {
                bcs::CMdlObj *pChrMdl = m_pChr->GetMdl();
                if (pChrMdl != 0) {
                    bcs::FMTX34 mtx;
                    pChrMdl->GetNodeLocalMtx(m_pChr->GetVNode(4), mtx);
                    bcs::FVEC3 ofs(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                    bcs::BCS_Vec_RotY(ofs, ofs, m_pMdl[0]->m_fRotY);
                    bcs::FVEC3Add(&ofs, &ofs, &m_pMdl[0]->m_vTrans);
                    pBallMdl->m_vTrans = ofs;
                }
            }

            for (long i = 0; i < MDL_NUM; i++) {
                if (m_pMdl[i] != 0) {
                    m_pMdl[i]->OffFlag(2);
                    m_pMdl[i]->ObjProc(1, 0, 0);
                    m_pMdl[i]->OnFlag(2);
                }
            }
        }

        void CcNinjaClone::SetUpDisappear()
        {
            if ((m_nFlag & 1) && !(m_nFlag & 2)) {
                if (m_pMdl[0] != 0) {
                    m_pMdl[0]->OnCloneFlag(0x40000);
                }
                if (m_pMdl[1] != 0) {
                    m_pMdl[1]->OnCloneFlag(0x40000);
                }
                if (m_pBody != 0) {
                    m_pBody->SetDisableAll();
                }
                for (long i = 0; i < EFF_NUM; i++) {
                    StopEffect(i);
                }
                CreateEffect(5);
                CGameSnd::Play(0x4DE, GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
                m_nFlag |= 2;
            }
        }

        // The copy fades out in 16 frames, stretching up and thinning.
        int CcNinjaClone::OnDisappear(float fRate)
        {
            m_fDisappear += 0.0625f * fRate;
            if (m_fDisappear >= 1.0f) {
                m_fDisappear = 1.0f;
                m_nFlag |= 4;
                return 1;
            }
            for (long i = 0; i < MDL_NUM; i++) {
                bcs::CMdlClone *pMdl = m_pMdl[i];
                if (pMdl != 0) {
                    pMdl->SetAlphaRateF(1.0f - m_fDisappear);
                    pMdl->m_vScale.y = 2.0f * m_fDisappear + 1.0f;
                    pMdl->m_vScale.x = 1.0f - 0.8f * m_fDisappear;
                    pMdl->m_vScale.z = 1.0f - 0.8f * m_fDisappear;
                }
            }
            return 0;
        }

        void CcNinjaClone::CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                            bcs::FVEC3 *pPush)
        {
            if (pArg != 0) {
                static_cast<CcNinjaClone *>(pArg)->SetUpDisappear();
            }
            if (pPush != 0) {
                CChrCore *pChr = CChrMan::GetChrCore(pOther->m_nOwnerIdx);
                if (pChr != 0) {
                    pChr->AddPos(*pPush);
                }
            }
        }

        CcNinjaFactory::CcNinjaFactory(bcs::CTask *pParent, CChrCore *pChr, const CCollBodyUnit *pBody,
                                       CBall *pBall)
            : CGameTask(pParent, -1)
            , m_pChr(pChr)
            , m_fFrame(0.0f)
            , m_Unk8C(0.0f, 0.0f, 0.0f)
        {
            InitFsm(this, STATE_NUM);
            SetState(STATE_SP_1_A, &CcNinjaFactory::CtorStateSP_1_A, &CcNinjaFactory::LoopStateSP_1_A,
                     &CcNinjaFactory::DtorStateSP_1_A);
            SetState(STATE_SP_LOOP, &CcNinjaFactory::CtorStateSPLoop, &CcNinjaFactory::LoopStateSPLoop,
                     &CcNinjaFactory::DtorStateSPLoop);
            SetState(STATE_SP_2_A, &CcNinjaFactory::CtorStateSP_2_A, &CcNinjaFactory::LoopStateSP_2_A,
                     &CcNinjaFactory::DtorStateSP_2_A);
            SetState(STATE_DISAPPEAR, &CcNinjaFactory::CtorStateDisappear, &CcNinjaFactory::LoopStateDisappear,
                     &CcNinjaFactory::DtorStateDisappear);
            ChangeState(STATE_SP_1_A);

            for (long i = 0; i < CLONE_NUM; i++) {
                m_nClonePos[i] = 1;
                m_Clone[i].Create(m_pChr, pBody, pBall);
            }
        }

        CcNinjaFactory::~CcNinjaFactory()
        {
            for (long i = 0; i < CLONE_NUM; i++) {
                m_Clone[i].Release();
            }
        }

        void CcNinjaFactory::Main()
        {
            ExecState();
            for (long i = 0; i < CLONE_NUM; i++) {
                m_Clone[i].Main_(GetFrameRateF());
            }
        }

        void CcNinjaFactory::CtorStateSP_1_A() {}

        void CcNinjaFactory::LoopStateSP_1_A() {}

        void CcNinjaFactory::DtorStateSP_1_A() {}

        // The three hang in the air: each copy's body effect changes and
        // its ball gets the light.
        void CcNinjaFactory::CtorStateSPLoop()
        {
            for (long i = 0; i < CLONE_NUM; i++) {
                m_Clone[i].OnFlag(1);
                m_Clone[i].StopEffect(0);
                m_Clone[i].CreateEffect(3);
                m_Clone[i].CreateLightEffect(0);
            }
        }

        void CcNinjaFactory::LoopStateSPLoop() {}

        void CcNinjaFactory::DtorStateSPLoop()
        {
            for (long i = 0; i < CLONE_NUM; i++) {
                m_Clone[i].StopEffect(1);
                m_Clone[i].CreateEffect(2);
            }
        }

        void CcNinjaFactory::CtorStateSP_2_A()
        {
            for (long i = 0; i < CLONE_NUM; i++) {
                m_Clone[i].StopEffect(4);
            }
        }

        void CcNinjaFactory::LoopStateSP_2_A() {}

        void CcNinjaFactory::DtorStateSP_2_A() {}

        void CcNinjaFactory::CtorStateDisappear()
        {
            m_fFrame = 0.0f;
            for (long i = 0; i < CLONE_NUM; i++) {
                m_Clone[i].OnFlag(1);
                m_Clone[i].SetUpDisappear();
            }
        }

        // The task ends once both copies are gone.
        void CcNinjaFactory::LoopStateDisappear()
        {
            int bEnd = 1;
            for (long i = 0; i < CLONE_NUM; i++) {
                if (m_Clone[i].IsGone() != 1) {
                    bEnd = 0;
                }
            }
            if (bEnd == 1) {
                Delete();
            }
        }

        void CcNinjaFactory::DtorStateDisappear() {}
    }
}
