// msw::CChrCoreBasketSaboten: the cactus (Saboten) in the basketball game.
//
// His special shot (the air trick) is a dunk of his own: a hop towards a
// point in front of the goal, a needle fired at the top of it
// (CC13_BASKET::CcSabotenNeedleBasket), a wait in the air while the other
// side may answer, then a curved flight onto the ring and the common dunk.
// While he dunks, whoever runs into him is blocked, and the others are put
// back at the point he hopped to.
//
// msw::CC13_BASKET::CcSabotenNeedleBasket: the needle. The base class
// (msw::CC_CMN::SABOTEN::CcNeedle, in main.dol) does the work; this class
// only gives its three tuning values.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcNeedle.hpp"

namespace msw {
    namespace CC13_BASKET {

        class CcSabotenNeedleBasket : public CC_CMN::SABOTEN::CcNeedle {
            public:
                CcSabotenNeedleBasket(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos, float fSpeed)
                    : CcNeedle(pParent, pOwner, rPos, fSpeed)
                {
                }
                virtual ~CcSabotenNeedleBasket() {}

            protected:
                virtual float GetCollHitEffectFrm_() const { return 300.0f; }
                virtual float GetCollHitRadius_() const { return 6.5f; }
                virtual int GetCororinDmgEffectID_() const { return 0x59E; }
        };
    }
}

// Set by the file's static initialiser and used by nothing in the file; see
// CBallManBasket.cpp for the same angle.
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);

    // (name guessed) rOut = r1 - r2 by way of a temporary, as
    // bcs::FVEC3Sum (see CChrCoreBasketKinopio.cpp).
    inline void FVEC3Diff(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 diff;

        bcs::FVEC3Sub(&diff, &r1, &r2);
        rOut = diff;
    }
}

msw::CChrCoreBasketSaboten::CChrCoreBasketSaboten()
{
}

msw::CChrCoreBasketSaboten::~CChrCoreBasketSaboten()
{
}

void msw::CChrCoreBasketSaboten::CtorStateAirTrick()
{
    SubStateAirTrickCmnCtor_();
}

// The steps of the trick (pWork->m_nStep):
//  0  waits for the arts event, then hops towards a point in front of the
//     goal (0.7 of the three-point radius out, on the line to the cactus);
//  1  the hop; at the end of its motion the needle is fired;
//  2  at the end of the next motion, a jump that moves on 4 along his yaw
//     to the goal;
//  3  that jump; at its end he waits in the air (step 6) for 60 frames;
//  6  the wait, ended early by the driver's answer 4; then the flight to
//     the ring starts;
//  7  the flight; from 0.9 of its motion the jump aims at the ring's height
//     (step 8);
//  8  the flight to the end of the motion, then the dunk (step 16);
//  16 to 18 the common dunk;
//  others: falls until the motion ends, then the state is left.
void msw::CChrCoreBasketSaboten::LoopStateAirTrick()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();

    switch (pWork->m_nStep) {
        case 0:
            if (!CGame::IsArtsEventMode() && !CGame::IsReqArtsEvent()) {
                // The height of the hop and of the set-back point. (As a
                // const local it is also a .rodata object of its own, which
                // the original has not; but with it every constant of the
                // file is loaded with its own lis, as in the original.)
                const float fSetBackY = 1.2f;
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)0, (m_nStatusFlag & 4) != 0), 0, 8.0f, 0.0f);
                AnimSetSpeed(3, 1.0f);

                bcs::FVEC3 dir;
                FVEC3Diff(dir, GetPos(), GetTargetGoalPosOffZ_());
                bcs::BCS_Vec_Normalize(dir, dir);
                float fDist = 0.7f * GetCourt3PointSize();
                const bcs::FVEC3 &rGoal = GetTargetGoalPos();
                bcs::FVEC3 target(rGoal.x + dir.x * fDist, 0.0f, rGoal.z + dir.z * fDist);

                const bcs::FVEC3 &rPos = GetPos();
                pWork->m_fMoveFrameNum = AnimGetFrameNum(3);
                m_SetBackPos.x = target.x;
                m_SetBackPos.y = fSetBackY;
                m_SetBackPos.z = target.z;
                pWork->m_MoveVec.x = (target.x - rPos.x) / pWork->m_fMoveFrameNum;
                pWork->m_MoveVec.y = 0.0f;
                pWork->m_MoveVec.z = (target.z - rPos.z) / pWork->m_fMoveFrameNum;
                Util_SetJumpParam(pWork->m_fMoveFrameNum, GetPos().y, fSetBackY, 3.0f);

                ArtsEff_StopAll();
                Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x13, 0x9F);
                int bAlt = (m_nStatusFlag >> 2) & 1;
                pWork->m_nMotId[0] = GetAirTrickMotID((AIR_TIRCK_FLOW)7, bAlt);
                pWork->m_nMotId[1] = GetAirTrickMotID((AIR_TIRCK_FLOW)8, bAlt);
                pWork->m_nMotId[2] = GetAirTrickMotID((AIR_TIRCK_FLOW)15, bAlt);
                m_nStatusFlag |= 0x400;
                PlaySeSPStart();
                SE_PlayArtsJumpSE(0);
                pWork->m_nStep = 1;
            }
            break;

        case 1: {
            float fStep = m_fTimeScale * m_fFrameStep;
            bcs::FVEC3 pos(GetPos());
            pos.x += pWork->m_MoveVec.x * fStep;
            pos.z += pWork->m_MoveVec.z * fStep;
            SetPos(pos);
            Util_DoJumpMove(fStep);
            if (IsAnimEnd(3) == 1) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)1, (m_nStatusFlag >> 2) & 1), 0, 8.0f, 0.0f);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x14, 0xA1);
                new (0x100) CC13_BASKET::CcSabotenNeedleBasket(CGame::GetTempParent(), this, GetPos(), 75.0f);
                pWork->m_nStep = 2;
            }
            break;
        }

        case 2:
            if (IsAnimEnd(3) == 1) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)2, (m_nStatusFlag & 4) != 0), 0, 8.0f, 0.0f);
                float fFrameNum = AnimGetFrameNum(3);
                pWork->m_fMoveFrameNum = fFrameNum;
                pWork->m_fMoveFrameNum += 60.0f;
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveFrame = 0.0f;
                pWork->m_fRotY = GetRotY_FromPosXZ(GetTargetGoalPos());
                pWork->m_fMoveLen = 4.0f;
                pWork->m_fMoveUnk18 = 60.0f * pWork->m_fMoveLen / 100.0f / fFrameNum;

                bcs::FVEC3 move;
                bcs::BCS_VecZ_RotY(move, 1.0f, pWork->m_fRotY);
                move.x *= pWork->m_fMoveLen;
                move.z *= pWork->m_fMoveLen;
                bcs::FVEC3 end;
                bcs::FVEC3Sum(end, GetPos(), move);
                pWork->m_MoveVec = end;

                Util_SetJumpParam(AnimGetFrameNum(3), GetPos().y, 3.0f, 2.5f);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x15, 0xA2);
                pWork->m_nStep = 3;
            }
            break;

        case 3:
            SubStateAirTrickMove_(pWork, 50.0f, 50.0f);
            if (IsAnimEnd(3) == 1) {
                pWork->m_fWaitFrame = 60.0f;
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)5, (m_nStatusFlag >> 2) & 1), 1, 0.0f, 0.0f);
                Util_SetJumpParam(60.0f, GetPos().y, 2.0f, 3.0f);
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveUnk18 = 40.0f * pWork->m_fMoveLen / 100.0f / 60.0f;
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x1D, 0xA0);
                ArtsEff_PlayAttackSign();
                pWork->m_nStep = 6;
            }
            break;

        case 6: {
            SubStateAirTrickMove_(pWork, 50.0f, 50.0f);
            long nAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x21, pWork->m_fWaitFrame, 60, 0, 0);
            if (pWork->m_fWaitFrame <= 0.0f || nAnswer == 4) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)6, (m_nStatusFlag & 4) != 0), 0, 8.0f, 0.0f);
                float fFrameNum = AnimGetFrameNum(3);
                pWork->m_fMoveFrameNum = fFrameNum;
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveFrame = 0.0f;
                pWork->m_MoveVec = GetTargetGoalRing_();
                pWork->m_fRotY = GetRotY_ToPosXZ(pWork->m_MoveVec);
                bcs::FVEC3 diff;
                FVEC3Diff(diff, pWork->m_MoveVec, GetPos());
                pWork->m_fMoveLen = bcs::FVEC3Len(&diff);
                pWork->m_fMoveUnk18 = 0.0f;

                PlayVoice(0x24, 0);
                PlaySePos(0x46D, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                SE_Play(4, 0x46E, 0, -1, 0.0f, 1.0f, 0.0f);
                Util_SetJumpParam(0.9f * fFrameNum, GetPos().y, 3.8f, GetPos().y + (3.8f - GetPos().y) * 0.5f);
                ArtsEff_Stop((ARTS_EFF_SLOT)0x1D);
                Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
                ArtsEff_CreateAttach((ARTS_EFF_SLOT)0x1E, 0x59B, m_nEffNodeDunk, 0);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x18, 0xA3);
                pWork->m_nStep = 7;
            }
            break;
        }

        case 7: {
            float fFrame = AnimGetFrame(3);
            float fFrameNum = AnimGetFrameNum(3);
            if (fFrame >= 0.9f * fFrameNum) {
                Util_SetJumpParam(fFrameNum - fFrame, GetPos().y, GetTargetGoalPos().y, 3.8f);
                pWork->m_nStep = 8;
            }
        }
            // fall through
        case 8: {
            float fFrame = AnimGetFrame(3);
            float fStep = fFrame - pWork->m_fMoveUnk18;
            pWork->m_fMoveUnk18 = fFrame;
            pWork->m_fMoveFrame += fStep;
            bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
            SubBSplineMove_(pWork->m_fMoveFrame, pWork->m_fMoveFrameNum, pWork->m_fMoveRate, pWork->m_fMoveLen,
                            pWork->m_fRotY, -1.0f, ofs);
            Util_DoJumpMove(fStep);
            if (IsAnimEnd(3) == 1) {
                bcs::FVEC3 pos(GetPos().x, GetTargetGoalPos().y, GetPos().z);
                SetPos(pos);
                FsmqRetry();
                ArtsEff_Stop((ARTS_EFF_SLOT)0x1E);
                SE_Stop(4, 0.0f);
                pWork->m_nStep = 16;
            }
            break;
        }

        case 16:
        case 17:
        case 18:
            SubStateAirTrickCmnDunk_(pWork);
            break;

        default:
            Util_DoJumpMove(m_fTimeScale * m_fFrameStep);
            if (!(m_nStatusFlag & 0xC00) && IsAnimEnd(3) == 1) {
                m_nStatusFlag &= ~0x20;
                FsmqNext();
                FsmqRetry();
            }
            break;
    }
}

void msw::CChrCoreBasketSaboten::DtorStateAirTrick()
{
    Eff_StopForce(0x18);
    CChrCoreBasket::DtorStateAirTrick();
}

// In the trick, while motion 0x83 plays.
int msw::CChrCoreBasketSaboten::IsAirTrickDunkMode() const
{
    if (GetState() != 0x27) {
        return 0;
    }
    return AnimGetMotID(3, 0) == 0x83;
}

// During the flight to the ring (steps 7 and 8), a character that runs into
// the cactus has its own dunk blocked.
void msw::CChrCoreBasketSaboten::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
{
    if (GetState() == 0x27) {
        long nStep = FsmqGetWork<STATE_WORK_AIR_TRICK>()->m_nStep;
        if ((nStep == 7 || nStep == 8) && pOther->GetOwnerKind() == 1) {
            CheckAirTrickDunkBlock_(CChrMan::GetChrCore(pOther->m_nOwnerIdx));
        }
    }
    CChrCoreBasket::CallbackBodyPush_(pBody, pOther, pPush);
}

// Once the trick is under way, the point the cactus hopped to.
int msw::CChrCoreBasketSaboten::GetSpSetBackPos(bcs::FVEC3 &rPos) const
{
    if (GetState() != 0x27) {
        return 0;
    }
    if (FsmqGetWork<STATE_WORK_AIR_TRICK>()->m_nStep == 0) {
        return 0;
    }
    rPos = m_SetBackPos;
    return 1;
}
