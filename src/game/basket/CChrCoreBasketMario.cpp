// msw::CChrCoreBasketMario: Mario in the basketball game.
//
// The special shot (the air trick) is a dunk of its own: a hop back away
// from the goal (further the nearer Mario stands to it), a jump in two
// steps to a point half a unit behind the ring, a wait in the air while the
// other side may answer (a block, answer 4, sends Mario straight down onto
// the ring), a short flight onto the ring and the common dunk. While Mario
// dunks, whoever of the other team runs into him is hit.
//
// The file also holds CChrCoreBasket's blocker check, run after every step
// of a trick.
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

    // (names guessed) r1 - r2 and r1 + r2 returned by value, as NW4R's VEC3
    // operators (the original copies each result out of a temporary).
    inline bcs::FVEC3 VecSub(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3 diff;

        FVEC3Diff(diff, r1, r2);
        return diff;
    }

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

    // (name guessed) From fFrom to fTo by fRate (0 to 1).
    inline float LerpF(float fFrom, float fTo, float fRate)
    {
        float fOfs = fRate * __fabsf(fFrom - fTo);
        return fFrom - fOfs;
    }

    // (name guessed) An angle brought into [0, 2 pi).
    inline float WrapRad(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    // (name guessed) The speed, in percent, below which a blocker wins
    // against the trick of character nChrId: 15 for some characters, 0 for
    // the others. (As a helper; written out in the blocker check, that
    // function has too many statements to be inlined into
    // LoopStateAirTrick, where the original has it inlined.)
    inline float GetBlockRate(long nChrId)
    {
        float fRate = 0.0f;
        switch (nChrId) {
            case 0:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
                fRate = 15.0f;
                break;
        }
        return fRate;
    }
}

msw::CChrCoreBasketMario::CChrCoreBasketMario()
{
}

msw::CChrCoreBasketMario::~CChrCoreBasketMario()
{
}

void msw::CChrCoreBasketMario::CtorStateAirTrick()
{
    SubStateAirTrickCmnCtor_();
}

// The steps of the trick (pWork->m_nStep):
//  0  waits for the arts event, then hops back along Mario's yaw, from
//     2.5 units (at the three-point line or further) to 6 (under the goal);
//  1  the hop; at its end, the first part of the jump to half a unit
//     behind the ring;
//  2  that part (15% of the way); then the second (48%);
//  3  the second part; at its end Mario waits in the air (step 6) for 30
//     frames, moving on by 12%;
//  6  the wait, ended early by the driver's answer 4 (then Mario goes
//     straight onto the ring); then the flight to the ring starts;
//  7  the flight to the end of its motion, then the dunk (step 16);
//  16 to 18 the common dunk;
//  others: falls until the motion ends, then the state is left.
// While flag 0x40 of the work is set the trick is held: the motion and the
// speed at 0x14 stand still for the frame.
void msw::CChrCoreBasketMario::LoopStateAirTrick()
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
                bcs::FVEC3 dir;
                FVEC3Diff(dir, GetPos(), GetTargetGoalPos());
                float fRate = LenXZ(VecSub(GetTargetGoalPos(), GetPos())) / GetCourt3PointSize();
                fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
                // (As a const local the far end is a .rodata object of its
                // own, which the original has not; but with it every
                // constant of the file is loaded with its own lis, as in
                // the original. Same as CChrCoreBasketMii00.cpp.)
                const float fLenMax = 6.0f;
                float fLen = LerpF(fLenMax, 2.5f, fRate);

                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)0, (m_nStatusFlag >> 2) & 1), 0, 8.0f, 0.0f);
                pWork->m_fMoveFrameNum = AnimGetFrameNum(3);
                pWork->m_fRotY = GetRotY();
                bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY + 3.1415927f);
                pWork->m_MoveVec.x = dir.x * fLen / pWork->m_fMoveFrameNum;
                pWork->m_MoveVec.y = 0.0f;
                pWork->m_MoveVec.z = dir.z * fLen / pWork->m_fMoveFrameNum;
                AnimSetSpeed(3, 1.0f);
                Util_SetJumpParam(pWork->m_fMoveFrameNum, GetPos().y, 1.0f, 2.0f);
                pWork->m_nStep = 1;

                ArtsEff_StopAll();
                ArtsOptAnimSetSpeed(1.0f);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x13, 0x66);
                Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
                int bAlt = (m_nStatusFlag >> 2) & 1;
                pWork->m_nMotId[0] = GetAirTrickMotID((AIR_TIRCK_FLOW)7, bAlt);
                pWork->m_nMotId[1] = GetAirTrickMotID((AIR_TIRCK_FLOW)8, bAlt);
                pWork->m_nMotId[2] = GetAirTrickMotID((AIR_TIRCK_FLOW)15, bAlt);
                PlaySeSPStart();
                PlaySePos(0x3E9, 0, 0, -1, 0.0f, 1.0f, 0.0f);
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
                AnimSetSpeed(3, 1.0f);
                float fFrameNum = AnimGetFrameNum(3);
                pWork->m_fMoveFrameNum = fFrameNum;
                pWork->m_fMoveFrameNum = pWork->m_fMoveFrameNum + AnimGetResFrameNum(0x80) + 30.0f;
                pWork->m_fMoveFrameNum += AnimGetResFrameNum(0x83);

                bcs::FVEC3 ofs;
                bcs::BCS_VecZ_RotY(ofs, 0.5f, GetRotY_ToGoal_NonOffset() + 3.1415927f);
                pWork->m_MoveVec = VecAdd(GetTargetGoalPos(), ofs);
                pWork->m_fMoveLen = LenXZ(VecSub(pWork->m_MoveVec, GetPos()));
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveFrame = 0.0f;
                pWork->m_fMoveUnk18 = 15.0f * pWork->m_fMoveLen / 100.0f / fFrameNum;
                pWork->m_fRotY = GetRotY_ToGoal_NonOffset();
                Util_SetJumpParam(fFrameNum, GetPos().y, 1.5f, 3.0f);

                ArtsEff_Stop((ARTS_EFF_SLOT)0x13);
                ArtsOptAnimSetSpeed(1.0f);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x14, 0x67);
                SE_Play(4, 0x3EF, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nStep = 2;
            }
            break;
        }

        case 2:
            SubStateAirTrickMove_(pWork, 55.0f, 45.0f);
            if (IsAnimEnd(3) == 1) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)2, (m_nStatusFlag >> 2) & 1), 0, 0.0f, 0.0f);
                float fFrameNum = AnimGetFrameNum(3);
                Util_SetJumpParam(fFrameNum, GetPos().y, 4.2f, 4.2f);
                pWork->m_nStep = 3;
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveUnk18 = 48.0f * pWork->m_fMoveLen / 100.0f / fFrameNum;
                ArtsEff_Stop((ARTS_EFF_SLOT)0x14);
                ArtsOptAnimSetSpeed(1.0f);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x15, 0x68);
            }
            break;

        case 3:
            SubStateAirTrickMove_(pWork, 85.0f, 15.0f);
            if (IsAnimEnd(3) == 1) {
                pWork->m_fWaitFrame = 30.0f;
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)5, (m_nStatusFlag >> 2) & 1), 1, 0.0f, 0.0f);
                Util_SetJumpParam(30.0f, GetPos().y, 4.2f, 4.2f);
                pWork->m_nStep = 6;
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveUnk18 = 12.0f * pWork->m_fMoveLen / 100.0f / 30.0f;
                ArtsEff_Stop((ARTS_EFF_SLOT)0x15);
                ArtsOptAnimSetSpeed(1.0f);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x1D, 0x6A);
                ArtsEff_PlayAttackSign();
                PlaySePos(0xC11, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            }
            break;

        case 6: {
            SubStateAirTrickMove_(pWork, 50.0f, 50.0f);
            long nAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x21, pWork->m_fWaitFrame, 30, 0, 0);
            if (pWork->m_fWaitFrame <= 0.0f || nAnswer == 4) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)6, (m_nStatusFlag & 4) != 0), 0, 0.0f, 0.0f);
                float fFrameNum = AnimGetFrameNum(3);
                Util_SetJumpParam(fFrameNum, GetPos().y, GetTargetGoalPos().y, 4.2f);
                pWork->m_nStep = 7;
                pWork->m_fMoveUnk18 = 25.0f * pWork->m_fMoveLen / 100.0f / fFrameNum;
                if (nAnswer == 4) {
                    // Blocked: straight down onto the ring.
                    pWork->m_nFlag |= 0x80;
                    Util_SetJumpParam(fFrameNum, GetPos().y, GetTargetGoalPos().y, 0.1f + GetTargetGoalPos().y);
                    pWork->m_fMoveUnk18 = LenXZ(VecSub(pWork->m_MoveVec, GetPos())) / fFrameNum;
                }
                pWork->m_fMoveRate = 0.0f;

                PlaySePos(0x3F0, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                PlayVoice(0x24, 0);
                ArtsEff_Stop((ARTS_EFF_SLOT)0x1D);
                ArtsOptAnimSetSpeed(1.0f);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x1E, 0x69);
                Eff_CreateAttach(0x1F, 0x513, m_nEffNodeDunk, 0, 0, 3, 0);
                Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
            }
            break;
        }

        case 7:
            SubStateAirTrickMove_(pWork, 5.0f, 95.0f);
            if (IsAnimEnd(3) == 1) {
                bcs::FVEC3 pos(GetPos());
                pos.y = GetTargetGoalPos().y;
                SetPos(pos);
                pWork->m_nStep = 16;
                FsmqRetry();
                ArtsEff_Stop((ARTS_EFF_SLOT)0x18);
                ArtsEff_Stop((ARTS_EFF_SLOT)0x1E);
                ArtsEff_Stop((ARTS_EFF_SLOT)0x1F);
                ResultAirTrickBattle(1);
                SE_Stop(4, 0.0f);
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

void msw::CChrCoreBasketMario::DtorStateAirTrick()
{
    CChrCoreBasket::DtorStateAirTrick();
}

// In the trick, while one of its dunk motions plays.
int msw::CChrCoreBasketMario::IsAirTrickDunkMode() const
{
    if (GetState() != 0x27) {
        return 0;
    }
    switch (AnimGetMotID(3, 0)) {
        case 0x7F:
        case 0x80:
        case 0x8D:
        case 0x83:
            break;
        default:
            return 0;
    }
    return 1;
}

// After every step of an air trick: forgets the blocker once it no longer
// blocks (status bit 0x2000), and while the trick climbs (steps 2, 3, 6
// and 7) a blocker that took the speed at 0x14 down to 15% (some
// characters) or to 0 wins: both tricks end and this character is hit,
// from behind.
void msw::CChrCoreBasket::SubStateAirTrickCmnBlockerCheck_(STATE_WORK_AIR_TRICK *pWork)
{
    if (pWork->m_pBlocker != 0 && !(pWork->m_pBlocker->m_nStatusFlag & 0x2000)) {
        pWork->m_pBlocker = 0;
        ResultAirTrickBattle(1);
    }
    switch (pWork->m_nStep) {
        case 2:
        case 3:
        case 6:
        case 7: {
            float fRate = GetBlockRate(m_nChrId);
            if (pWork->m_fSpeed <= fRate / 100.0f) {
                pWork->m_pBlocker->ResultAirTrickBattle(1);
                pWork->m_pBlocker = 0;
                Eff_Stop(0xB);
                Eff_Stop(0xC);
                MethdDamage(0, WrapRad(GetRotY() + 3.1415927f));
            }
            break;
        }
    }
}

// During the dunk, a character of the other team that runs into Mario is
// hit (damage kind 0, from Mario's direction).
void msw::CChrCoreBasketMario::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
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
