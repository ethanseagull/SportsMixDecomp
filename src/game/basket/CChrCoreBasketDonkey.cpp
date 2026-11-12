// msw::CChrCoreBasketDonkey: Donkey Kong in the basketball game.
//
// The special shot (the air trick) is a jump to a point beside the goal,
// a landing that shakes the camera and sends out a shock wave
// (CC07_BASKET::CcDonkeyQuake), a high jump back towards the goal, a wait
// in the air while the other side may answer (a block, answer 4), a flight
// onto the ring and the common dunk. While he flies onto the ring, whoever
// of the other team runs into him has his own dunk blocked.
//
// msw::CC07_BASKET::CcDonkeyQuake: the shock wave, an attack collision
// around the landing point that hits for one frame.
#include "game/msw/CChrCoreBasket.hpp"

#include <math.h>
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CUnivTask.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"

namespace {
    // (name guessed) The size of the shock wave's collision across the
    // ground.
    const float QUAKE_SIZE = 5.2f;

    // Stand-in for an unknown user of 0.5 and -1, which the original keeps
    // in .rodata after the shock wave's constants but nothing reads (likely
    // stripped code, as in CChrCoreBasketDiddy.cpp). See notes/decisions.md,
    // "Unreferenced data kept by dead references".
    inline void UnusedReferences(float f)
    {
        if (f == 0.5f) {
        }
        if (f == -1.0f) {
        }
    }
}

namespace msw {
    namespace CC07_BASKET {

        // The shock wave of Donkey Kong's landing: an attack collision at the
        // landing point, on for the first frame only; the task lives 100
        // frames (with its sound). 0x5C bytes. (member names guessed)
        class CcDonkeyQuake : public bcs::CUnivTaskEx {
            public:
                CcDonkeyQuake(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos);
                virtual ~CcDonkeyQuake();
                virtual void Main();

            private:
                // Stops the sound, if any, fading it out over fFade frames.
                void StopSe_(float fFade)
                {
                    if (m_hSe) {
                        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hSe);
                        if (pTask && !pTask->IsDelete()) {
                            static_cast<bcs::CSndTask *>(pTask)->Stop(fFade);
                        }
                        m_hSe = 0;
                    }
                }

                bcs::FVEC3 m_Pos;          // 0x34
                unsigned long m_hSe;       // 0x40, a sound task id
                CAtkCollision m_AtkColl;   // 0x44
                unsigned long m_hEffect;   // 0x50
                long m_nStep;              // 0x54
                float m_fLife;             // 0x58, frames left
        };
    }
}

namespace {
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

    // (name guessed) rIn flattened onto the XZ plane and scaled to length 1
    // (zero when it has no XZ part).
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(rIn.x, 0.0f, rIn.z));
    }

    // (name guessed) The length of rVec.
    inline float VecLen(const bcs::FVEC3 &rVec)
    {
        return bcs::FVEC3Len(&rVec);
    }

    // (guessed helpers) An angle brought into [0, 2 pi); a value kept
    // within [fMin, fMax].
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

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    // Set by the file's static initialiser and used by nothing in the file;
    // see CBallManBasket.cpp for the same angle.
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

msw::CChrCoreBasketDonkey::CChrCoreBasketDonkey()
{
}

msw::CChrCoreBasketDonkey::~CChrCoreBasketDonkey()
{
}

// The lay-up (state 0x25) turns its attack collision on at the trigger.
void msw::CChrCoreBasketDonkey::CallbackMotTrg_AtkOn_(long, long)
{
    if (GetState() == 0x25) {
        SetCCLayUpAtkCol();
    }
}

void msw::CChrCoreBasketDonkey::CallbackMotTrg_AtkOff_(long, long)
{
    CollAtkGetColl_(0)->GetCollUnit()->OffFlag10(1);
}

void msw::CChrCoreBasketDonkey::CtorStateAirTrick()
{
    // (Dead comparison, a guess: the original creates the shock wave's
    // size, 5.2, ahead of LoopStateAirTrick's static, most likely in code
    // the link stripped; without it the static comes first in .rodata and
    // the file loses its literal pool. See notes/decisions.md, "Dead float
    // comparison".)
    if (m_fTimeScale == QUAKE_SIZE) {
    }
    SubStateAirTrickCmnCtor_();
}

// The steps of the trick (pWork->m_nStep):
//  0  waits for the arts event, then jumps to a point beside the goal
//     (0.7 of the three-point radius out from the goal along the line from
//     the trick's axis, turned up to 30 degrees towards the middle);
//  1  that jump, moved by the motion's frames; on landing the shock wave
//     and the camera quake;
//  2  at the end of the landing, the high jump: 6 units along the yaw to
//     the goal;
//  3  the high jump; at its end Donkey Kong waits in the air (step 6);
//  6  the wait (60 frames), ended early by the driver's answer 4 (a block);
//     then the flight onto the ring starts;
//  7  the flight to the end of its motion, then the dunk (step 16);
//  16 to 18 the common dunk;
//  others: falls until the motion ends, then the state is left.
void msw::CChrCoreBasketDonkey::LoopStateAirTrick()
{
    static const float fTotalFrame = 60.0f;

    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();

    switch (pWork->m_nStep) {
        case 0:
            if (!CGame::IsArtsEventMode() && !CGame::IsReqArtsEvent()) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)0, (m_nStatusFlag & 4) != 0), 0, 8.0f, 0.0f);
                AnimSetSpeed(3, 1.2f);
                pWork->m_fMoveFrameNum = AnimGetFrameNum(3);

                bcs::FVEC3 axis;
                GetAirTrickTargetAxis_(axis);
                const bcs::FVEC3 &rPos = GetPos();
                bcs::FVEC3 dir;
                FVEC3Diff(dir, rPos, axis);
                NormalizeXZ(dir, dir);
                float fDist = 0.7f * GetCourt3PointSize();
                dir.x *= fDist;
                dir.z *= fDist;
                pWork->m_MoveVec = VecAdd(GetTargetGoalPos(), dir);

                // Turned towards the middle of the court, by up to 30
                // degrees as the point lies further to the side.
                const bcs::FVEC3 &rGoal = GetTargetGoalPos();
                float fRotY = GetRotY_FromPosXZ(rGoal);
                float fOfs = rGoal.z > 0.0f ? 3.1415927f : 0.0f;
                float fAngle = WrapRad(fRotY - fOfs);
                if (fAngle > 3.1415927f) {
                    fAngle -= 6.2831855f;
                }
                float fRate = Clamp(fabsf(fAngle) / 1.5707964f, 0.0f, 1.0f);
                if (fAngle > 0.0f) {
                    fRate = -fRate;
                }
                bcs::BCS_Vec_RotY(dir, dir, fRate * 0.5235988f);
                pWork->m_MoveVec = VecAdd(GetTargetGoalPos(), dir);
                pWork->m_fRotY = GetRotY_ToPosXZ(pWork->m_MoveVec);

                dir = VecSub(pWork->m_MoveVec, rPos);
                pWork->m_MoveVec.x = dir.x / pWork->m_fMoveFrameNum;
                pWork->m_MoveVec.y = 0.0f;
                pWork->m_MoveVec.z = dir.z / pWork->m_fMoveFrameNum;
                pWork->m_fMoveRate = 0.0f;
                Util_SetJumpParam(pWork->m_fMoveFrameNum, rPos.y, 0.0f, 3.5f);

                int bAlt = (m_nStatusFlag >> 2) & 1;
                pWork->m_nMotId[0] = GetAirTrickMotID((AIR_TIRCK_FLOW)7, bAlt);
                pWork->m_nMotId[1] = GetAirTrickMotID((AIR_TIRCK_FLOW)8, bAlt);
                pWork->m_nMotId[2] = GetAirTrickMotID((AIR_TIRCK_FLOW)15, bAlt);
                ArtsEff_StopAll();
                Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x13, 0x85);
                pWork->m_nStep = 1;
                PlaySeSPStart();
                PlaySePos(0x427, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                break;
            }
            // Wait while an arts event plays.
            return;

        case 1: {
            // m_fMoveRate holds the motion frame of the last update.
            float fFrame = AnimGetFrame(3);
            float fStep = fFrame - pWork->m_fMoveRate;
            pWork->m_fMoveRate = fFrame;
            bcs::FVEC3 pos(GetPos());
            pos.x += pWork->m_MoveVec.x * fStep;
            pos.z += pWork->m_MoveVec.z * fStep;
            SetPos(pos);
            RotateYRate_(pWork->m_fRotY, 0.2f);
            Util_DoJumpMove(fStep);
            if (IsAnimEnd(3) == 1) {
                new CC07_BASKET::CcDonkeyQuake(CGame::GetTempParent(), this, GetPos());
                CGame::StartCameraQuake(3, -1, -1);
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)1, (m_nStatusFlag >> 2) & 1), 0, 0.0f, 0.0f);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x14, 0x86);
                pWork->m_nStep = 2;
            }
            break;
        }

        case 2:
            if (IsAnimEnd(3) == 1) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)2, (m_nStatusFlag & 4) != 0), 0, 0.0f, 0.0f);
                AnimSetSpeed(3, 0.8f);
                float fFrameNum = AnimGetFrameNum(3);
                pWork->m_fMoveLen = 6.0f;
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveFrame = 0.0f;
                pWork->m_fRotY = GetRotY_FromPosXZ(GetTargetGoalPos());
                pWork->m_fMoveUnk18 = 90.0f * pWork->m_fMoveLen / 100.0f / fFrameNum;

                bcs::FVEC3 ofs;
                bcs::BCS_VecZ_RotY(ofs, 1.0f, pWork->m_fRotY);
                ofs.x *= pWork->m_fMoveLen;
                ofs.z *= pWork->m_fMoveLen;
                pWork->m_MoveVec = VecAdd(GetPos(), ofs);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x15, 0x87);
                Util_SetJumpParam(fFrameNum, GetPos().y, 3.0f, 2.0f);
                pWork->m_nStep = 3;
            }
            break;

        case 3:
            SubStateAirTrickMove_(pWork, 50.0f, 50.0f);
            if (IsAnimEnd(3) == 1) {
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)5, (m_nStatusFlag >> 2) & 1), 1, 0.0f, 0.0f);
                AnimSetSpeed(3, 1.0f);
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x1D, 0x88);
                pWork->m_fWaitFrame = fTotalFrame;
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveUnk18 = 10.0f * pWork->m_fMoveLen / 100.0f / fTotalFrame;
                Util_SetJumpParam(fTotalFrame, GetPos().y, 2.0f, 2.5f);
                ArtsEff_PlayAttackSign();
                pWork->m_nStep = 6;
            }
            break;

        case 6: {
            SubStateAirTrickMove_(pWork, 50.0f, 50.0f);
            long nAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x21, pWork->m_fWaitFrame, 60, 0, 0);
            if (pWork->m_fWaitFrame <= 0.0f || nAnswer == 4) {
                ArtsEff_Stop((ARTS_EFF_SLOT)0x1D);
                AnimPlay(3, GetAirTrickMotID((AIR_TIRCK_FLOW)6, (m_nStatusFlag & 4) != 0), 0, 0.0f, 0.0f);
                AnimSetSpeed(3, 1.2f);
                float fFrameNum = AnimGetFrameNum(3);
                Util_SetJumpParam(fFrameNum, GetPos().y, GetTargetGoalPos().y, 4.5f);
                if (nAnswer == 4) {
                    // Blocked.
                    pWork->m_nFlag |= 0x80;
                }
                pWork->m_fMoveFrameNum = fFrameNum;
                pWork->m_fMoveRate = 0.0f;
                pWork->m_fMoveFrame = 0.0f;
                pWork->m_MoveVec = GetTargetGoalRing_();
                pWork->m_fRotY = GetRotY_ToPosXZ(pWork->m_MoveVec);
                pWork->m_fMoveLen = VecLen(VecSub(pWork->m_MoveVec, GetPos()));
                pWork->m_fMoveUnk18 = pWork->m_fMoveLen / fFrameNum;
                ArtsEff_CreateAttachAuto((ARTS_EFF_SLOT)0x18, 0x89);
                Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
                SE_Play(4, 0x42F, 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nStep = 7;
            }
            break;
        }

        case 7:
            SubStateAirTrickMove_(pWork, 50.0f, 50.0f);
            if (IsAnimEnd(3) == 1) {
                SE_Stop(4, 0.0f);
                PlaySePos(0x42E, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                PlayVoice(0x24, 0);
                AnimSetSpeed(3, 1.0f);
                SetPos(bcs::FVEC3(GetPos().x, GetTargetGoalPos().y, GetPos().z));
                FsmqRetry();
                pWork->m_fUnk2C = 0.0f;
                pWork->m_nStep = 16;
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
}

void msw::CChrCoreBasketDonkey::DtorStateAirTrick()
{
    SE_Stop(4, 0.0f);
    CChrCoreBasket::DtorStateAirTrick();
}

// In the trick, while its dunk motion plays.
int msw::CChrCoreBasketDonkey::IsAirTrickDunkMode() const
{
    if (GetState() != 0x27) {
        return 0;
    }
    return AnimGetMotID(3, 0) == 0x83;
}

// During the flight onto the ring (step 7), a character that runs into
// Donkey Kong has its own dunk blocked.
void msw::CChrCoreBasketDonkey::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
{
    if (GetState() == 0x27 && FsmqGetWork<STATE_WORK_AIR_TRICK>()->m_nStep == 7 && pOther->GetOwnerKind() == 1) {
        CheckAirTrickDunkBlock_(CChrMan::GetChrCore(pOther->m_nOwnerIdx));
    }
    CChrCoreBasket::CallbackBodyPush_(pBody, pOther, pPush);
}

msw::CC07_BASKET::CcDonkeyQuake::CcDonkeyQuake(bcs::CTask *pParent, CChrCore *pOwner, const bcs::FVEC3 &rPos)
    : CUnivTaskEx(pParent, 0x17), m_hSe(0)
{
    m_hEffect = 0;
    m_nStep = 0;
    m_fLife = 100.0f;
    m_Pos = rPos;
    CGameSnd::Play(0x42C, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);

    float fSize = QUAKE_SIZE;
    m_AtkColl.Create(1, 4, 1, pOwner->GetMdl()->GetHandle());
    m_AtkColl.SetAtkTypeAll(0x502);
    m_AtkColl.SetTargetFlagAll(2);
    m_AtkColl.SetSizeAll(fSize, 0.2f, fSize);
    m_AtkColl.SetPosAll(m_Pos);
    m_AtkColl.SetEnableAll();
    m_AtkColl.SetAtkPowerAll(0.15f);
    m_AtkColl.SetHitEffectFrameAll(5.0f);

    StopSe_(5.0f);
    m_hSe = CGameSnd::Play(0x42D, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
    UnusedReferences(m_fLife);
}

msw::CC07_BASKET::CcDonkeyQuake::~CcDonkeyQuake()
{
    CEffectCtrl::StopAttachEffect(m_hEffect);
    m_hEffect = 0;
    StopSe_(5.0f);
}

// Frame 0 waits, frame 1 turns the collision off; then the task waits out
// its life and deletes itself.
void msw::CC07_BASKET::CcDonkeyQuake::Main()
{
    switch (m_nStep) {
        case 0:
            m_nStep++;
            break;
        case 1:
            m_AtkColl.SetDisableAll();
            m_nStep++;
            break;
        default:
            if (m_fLife <= 0.0f) {
                StopSe_(50.0f);
                Delete();
            } else {
                m_fLife -= GetFrameRateF();
            }
            break;
    }
}
