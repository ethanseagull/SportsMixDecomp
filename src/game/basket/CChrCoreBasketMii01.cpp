// msw::CChrCoreBasketMii01: a Mii (second body type) in the basketball game.
// The same trick as CChrCoreBasketMii00.cpp with its own effects and sounds.
//
// The special shot (the air trick) is a dunk of its own: a hop back away
// from the goal (further the nearer the Mii stands to it), a jump to a point
// half a unit behind the ring, a wait in the air while the other side may
// answer (a block, answer 4, sends the Mii straight down onto the ring), a
// short flight onto the ring and the common dunk. While the Mii dunks,
// whoever of the other team runs into it is hit.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CGame.hpp"

// Set by the file's static initialiser and used by nothing in the file; see
// CBallManBasket.cpp for the same angle.
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);

    // (name guessed) rOut = r1 - r2 by way of a temporary, as
    // bcs::FVEC3Sum (see CChrCoreBasketSaboten.cpp).
    inline void FVEC3Diff(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 diff;

        bcs::FVEC3Sub(&diff, &r1, &r2);
        rOut = diff;
    }

    // (name guessed) The length of rVec in the XZ plane.
    inline float LenXZ(const bcs::FVEC3 &rVec)
    {
        bcs::FVEC3 xz(rVec.x, 0.0f, rVec.z);
        return bcs::FVEC3Len(&xz);
    }

    // (name guessed) From fFrom to fTo by fRate (0 to 1).
    inline float LerpF(float fFrom, float fTo, float fRate)
    {
        float fOfs = fRate * __fabsf(fFrom - fTo);
        return fFrom - fOfs;
    }
}

msw::CChrCoreBasketMii01::CChrCoreBasketMii01()
{
}

msw::CChrCoreBasketMii01::~CChrCoreBasketMii01()
{
}

void msw::CChrCoreBasketMii01::CtorStateAirTrick()
{
    SubStateAirTrickCmnCtor_();
}

// The steps of the trick (pWork->m_nStep):
//  0  waits for the arts event, then hops back along the Mii's yaw, from
//     2.5 units (at the three-point line or further) to 6 (under the goal);
//  1  the hop; at its end, a jump to half a unit behind the ring;
//  2  that jump; at its end the Mii waits in the air (step 6) for 40 frames;
//  6  the wait, ended early by the driver's answer 4 (then the Mii goes
//     straight onto the ring); then the flight to the ring starts;
//  7  the flight to the end of its motion, then the dunk (step 16);
//  16 to 18 the common dunk;
//  others: falls until the motion ends, then the state is left.
// While flag 0x40 of the work is set the trick is held: the motion and the
// speed at 0x14 stand still for the frame.
void msw::CChrCoreBasketMii01::LoopStateAirTrick()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    float fSpeed = pWork->m_fSpeed;

    if (pWork->m_nFlag & 0x40) {
        pWork->m_fSpeed = 0.0f;
        AnimSetSpeed(3, 0.0f);
    }

    switch (pWork->m_nStep) {
        case 0:
            if (!CGame::IsArtsEventMode() && !CGame::IsReqArtsEvent()) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)0, (m_nStatusFlag & 4) != 0), 0, 8.0f, 0.0f);
                AnimSetSpeed(3, 1.0f);

                bcs::FVEC3 dir;
                FVEC3Diff(dir, GetPos(), GetTargetGoalPos());
                bcs::FVEC3 toGoal;
                FVEC3Diff(toGoal, GetTargetGoalPos(), GetPos());
                float fRate = LenXZ(toGoal) / GetCourt3PointSize();
                fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
                // (As a const local the far end is a .rodata object of
                // its own, which the original has not; but with it every
                // constant of the file is loaded with its own lis, as in
                // the original. Same as CChrCoreBasketSaboten.cpp.)
                const float fLenMax = 6.0f;
                float fLen = LerpF(fLenMax, 2.5f, fRate);

                pWork->m_fMoveFrameNum = AnimGetFrameNum(3);
                pWork->m_fRotY = GetRotY();
                bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY + 3.1415927f);
                pWork->m_MoveVec.x = dir.x * fLen / pWork->m_fMoveFrameNum;
                pWork->m_MoveVec.y = 0.0f;
                pWork->m_MoveVec.z = dir.z * fLen / pWork->m_fMoveFrameNum;
                Util_SetJumpParam(pWork->m_fMoveFrameNum, GetPos().y, 1.0f, 2.0f);

                ArtsEff_StopAll();
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x13, 0xCD);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x1E, 0xCE);
                Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
                int bAlt = (m_nStatusFlag >> 2) & 1;
                pWork->m_nMotId[0] = GetAirTrickMotID((AIR_TIRCK_FLOW)7, bAlt);
                pWork->m_nMotId[1] = GetAirTrickMotID((AIR_TIRCK_FLOW)8, bAlt);
                pWork->m_nMotId[2] = GetAirTrickMotID((AIR_TIRCK_FLOW)15, bAlt);
                PlaySeSPStart();
                SE_PlayArtsJumpSE(0);
                pWork->m_nStep = 1;
                break;
            }
            // Wait while an arts event plays (the trick is not updated).
            return;

        case 1: {
            float fStep = m_fTimeScale * m_fFrameStep;
            bcs::FVEC3 pos(GetPos());
            pos.x += pWork->m_MoveVec.x * fStep;
            pos.z += pWork->m_MoveVec.z * fStep;
            SetPos(pos);
            Util_DoJumpMove(fStep);
            if (IsAnimEnd(3) == 1) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)1, (m_nStatusFlag & 4) != 0), 0, 0.0f, 0.0f);
                float fFrameNum = AnimGetFrameNum(3);
                pWork->m_fMoveFrameNum = fFrameNum;
                pWork->m_fMoveFrameNum += 40.0f;
                pWork->m_fMoveFrameNum += AnimGetResFrameNum(0x83);

                bcs::FVEC3 ofs;
                bcs::BCS_VecZ_RotY(ofs, 0.5f, GetRotY_ToGoal_NonOffset() + 3.1415927f);
                bcs::FVEC3 end;
                bcs::FVEC3Sum(end, GetTargetGoalPos(), ofs);
                pWork->m_MoveVec = end;

                bcs::FVEC3 diff;
                FVEC3Diff(diff, pWork->m_MoveVec, GetPos());
                pWork->m_fMoveLen = LenXZ(diff);
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveFrame = 0.0f;
                pWork->m_fMoveUnk18 = 65.0f * pWork->m_fMoveLen / 100.0f / fFrameNum;
                pWork->m_fRotY = GetRotY_ToGoal_NonOffset();
                Util_SetJumpParam(fFrameNum, GetPos().y, 4.2f, 3.0f);

                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x14, 0xCF);
                ArtsEff_Stop((ARTS_EFF_SLOT)0x1E);
                ArtsEff_CreateAttach((ARTS_EFF_SLOT)0x1E, 0x5E9, m_nEffNodeDunk, 0);
                PlaySePos(0x4A1, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                SE_Play(4, 0x49E, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nStep = 2;
            }
            break;
        }

        case 2:
            SubStateAirTrickMove_(pWork, 50.0f, 50.0f);
            if (IsAnimEnd(3) == 1) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)5, (m_nStatusFlag >> 2) & 1), 1, 0.0f, 0.0f);
                pWork->m_fWaitFrame = 40.0f;
                Util_SetJumpParam(40.0f, GetPos().y, 4.2f, 4.2f);
                pWork->m_nStep = 6;
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveUnk18 = 20.0f * pWork->m_fMoveLen / 100.0f / 40.0f;
                ArtsEff_PlayAttackSign();
                SE_Stop(4, 0.0f);
                SE_Play(4, 0x49F, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nStep = 6;
            }
            break;

        case 6: {
            SubStateAirTrickMove_(pWork, 50.0f, 50.0f);
            long nAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x21, pWork->m_fWaitFrame, 40, 0, 0);
            if (pWork->m_fWaitFrame <= 0.0f || nAnswer == 4) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)6, (m_nStatusFlag & 4) != 0), 0, 0.0f, 0.0f);
                float fFrameNum = AnimGetFrameNum(3);
                Util_SetJumpParam(fFrameNum, GetPos().y, GetTargetGoalPos().y, 4.2f);
                pWork->m_fMoveUnk18 = 15.0f * pWork->m_fMoveLen / 100.0f / fFrameNum;
                if (nAnswer == 4) {
                    // Blocked: straight down onto the ring.
                    pWork->m_nFlag |= 0x80;
                    Util_SetJumpParam(fFrameNum, GetPos().y, GetTargetGoalPos().y, GetTargetGoalPos().y + 0.1f);
                    bcs::FVEC3 diff;
                    FVEC3Diff(diff, pWork->m_MoveVec, GetPos());
                    pWork->m_fMoveUnk18 = LenXZ(diff) / fFrameNum;
                }
                pWork->m_fMoveRate = 0.0f;

                PlaySePos(0x4A0, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                PlayVoice(0x24, 0);
                Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x18, 0xD0);
                ArtsEff_Stop((ARTS_EFF_SLOT)0x1E);
                ArtsEff_CreateAttach((ARTS_EFF_SLOT)0x1E, 0x5EB, m_nEffNodeDunk, 0);
                pWork->m_nStep = 7;
            }
            break;
        }

        case 7:
            SubStateAirTrickMove_(pWork, 10.0f, 90.0f);
            if (IsAnimEnd(3) == 1) {
                bcs::FVEC3 pos(GetPos());
                pos.y = GetTargetGoalPos().y;
                SetPos(pos);
                FsmqRetry();
                ResultAirTrickBattle(1);
                ArtsEff_Stop((ARTS_EFF_SLOT)0x1E);
                SE_Stop(4, 0.0f);
                pWork->m_nStep = 16;
                return;
            }
            break;

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

    if (pWork->m_nFlag & 0x40) {
        pWork->m_fSpeed = fSpeed;
    }
    SubStateAirTrickCmnBlockerCheck_(pWork);
}

void msw::CChrCoreBasketMii01::DtorStateAirTrick()
{
    CChrCoreBasket::DtorStateAirTrick();
}

// In the trick, while one of its dunk motions plays.
int msw::CChrCoreBasketMii01::IsAirTrickDunkMode() const
{
    if (GetState() != 0x27) {
        return 0;
    }
    long nMotId = AnimGetMotID(3, 0);
    if (nMotId != 0x7F && nMotId != 0x8D && nMotId != 0x83) {
        return 0;
    }
    return 1;
}

// During the dunk, a character of the other team that runs into the Mii is
// hit (damage kind 0, from the Mii's direction).
void msw::CChrCoreBasketMii01::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
{
    if (GetState() == 0x27 && IsAirTrickDunkMode() && pOther->GetOwnerKind() == 1) {
        CChrCore *pChr = CChrMan::GetChrCore(pOther->m_nOwnerIdx);
        if (pChr != 0 && pChr->GetComLevelWork() == 1 && pChr->GetTeamNo() != GetTeamNo()) {
            if (MethdDamage(0, GetRotY_FromChr(pChr))) {
                return;
            }
        }
    }
    CChrCoreBasket::CallbackBodyPush_(pBody, pOther, pPush);
}
