// msw::CChrCoreBasket's block cut, side step and smash cut states: the
// defender's moves against the ball holder. The block cut swipes at the
// nearest ball (or the one in the direction asked for), the side step
// dodges sideways, and the smash cut is a spinning jump (or, for some
// characters, a straight run) with an attack collision.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "nw4r/math/types.h"

namespace unit_2FD1C_cpp {
// uncalled stand-in / pool-off stand-in: the original addresses every float
// constant of this file with its own lis, which this compiler does when the
// first .rodata object of the file is weak (a function-local const). The
// module link strips this function and its constant.
void UnusedPoolOff()
{
    const long k = 0;
}
}

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it (see CBallManBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace {

    // bcs::FVEC3 derives from nw4r::math::VEC3 in the original, and the
    // vector differences go through VEC3's operators; here FVEC3 is a
    // separate struct of the same layout, so the operands are reinterpreted.
    typedef const nw4r::math::VEC3 &VEC3_CREF;

    // (guessed helper) An angle brought into (-pi, pi].
    inline float WrapRadPi(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        if (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        return rad;
    }

    // (guessed helper) An angle brought into [0, 2 pi).
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

    // (guessed helper) Degrees to radians.
    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    // (guessed helper) A colour variant kept within [0, nMax]: the
    // variants of CChrCoreBasketSlime::GetArtsEffectUnit (3) and
    // CChrCoreBasketYoshi::GetEffectUnit (4).
    inline long ClampColor(long nColor, long nMax)
    {
        return nColor > nMax ? nMax : (nColor < 0 ? 0 : nColor);
    }
}

// x kept within [min, max]. A macro: the original computes each bound only
// on the branch that needs it.
#define CLAMP(x, min, max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))

// The block cut's work (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_BLOCK_CUT {
    long m_nUnk00;                // 0x00
    long m_nBallNo;               // 0x04: the ball the cut turns to
    unsigned long m_nFlag;        // 0x08: bit 2 set when the cut turns to a ball
    float m_fFrame;               // 0x0C: frames since the cut began
    // 0x10: the direction the driver asked for (MethdBlockCut), -1 for
    // none.
    float m_fRotY;
    bcs::FVEC3 m_BallPos;         // 0x14: where that ball was
    long m_nUnk20;                // 0x20
    CChrCore::DIR_MOVE m_DirMove; // 0x24: the lunge of the cut
};

// ---- Block cut ----------------------------------------------------------

// The block cut: a swipe at a ball, to the right or left as the ball lies.
// With a direction asked for, the ball nearest to it (within 60 degrees)
// is the target; otherwise the nearest ball on the court.
void msw::CChrCoreBasket::CtorStateBlockCut()
{
    m_UnkEC |= 0x10;
    SetCollBodyPushPower_(2);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(1);

    CBall *pTarget;
    STATE_WORK_BLOCK_CUT *pWork = FsmqGetWork<STATE_WORK_BLOCK_CUT>();
    long nMot = 0x65;
    pWork->m_fFrame = 0.0f;
    pTarget = 0;
    if (pWork->m_fRotY != -1.0f) {
        float fMinRot = 9999.9f;
        for (long i = 0; i < CBallMan::GetBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall && pBall->IsActive()) {
                float fRot = WrapRadPi(GetRotY_ToPosXZ(pBall->GetPos()) - pWork->m_fRotY);
                if (__fabsf(fRot) < __fabsf(fMinRot)) {
                    fMinRot = fRot;
                    pTarget = pBall;
                }
            }
        }
        if (pTarget && __fabsf(fMinRot) <= 1.0471976f) {
            pWork->m_nBallNo = pTarget->GetBallNo();
            pWork->m_BallPos = pTarget->GetPos();
            pWork->m_nFlag |= 4;
        }
    }
    if (!pTarget) {
        float fMinDist = 9999.9f;
        for (long i = 0; i < CBallMan::GetBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall && pBall->IsActive()) {
                bcs::FVEC3 dist = reinterpret_cast<VEC3_CREF>(pBall->GetPos()) - reinterpret_cast<VEC3_CREF>(GetPos());
                float fDist = bcs::BCS_Vec_GetLenXZ(dist);
                if (fDist < fMinDist) {
                    fMinDist = fDist;
                    pTarget = pBall;
                }
            }
        }
    }
    if (pTarget) {
        float fRotY = GetRotY();
        if (WrapRadPi(GetRotY_ToPosXZ(pTarget->GetPos()) - fRotY) < 0.0f) {
            nMot *= -1;
        }
    }

    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(0);
    AnimPlay(3, nMot, 0, 2.0f, 0.0f);
    PlayVoice(0x1E, 0);
    PlaySe(0xC7C, 1, -1, 0.0f, 1.0f, 0.0f);
    Util_SetDirMoveParam(pWork->m_DirMove, GetRotY());

    CCollAtkUnit *pAtk = CollAtkGetColl_(0);
    pAtk->GetCollUnit()->OffFlag10(1);
    pAtk->m_nTmpNoHitFlag = 0;
    pAtk->m_nNoHitFlag = 0;
    pAtk->GetCollUnit()->SetCollType(0);

    Eff_Stop(0xB);
    long nEff = 0x5ED;
    if (nMot < 0) {
        nEff = 0x5EE;
    }
    Eff_CreateAttach(0xB, nEff, m_nEffNode, 0, 8, 3, 0);
    Eff_SetRotateY(0xB, GetRotY());
    switch (GetChrIdS()) {
        case 0x10: {
            const CChrCoreBasketSlime::SLIME_COL_SET *pCol =
                CChrCoreBasketSlime::GetArtsEffectUnit(ClampColor(m_nColor, 2));
            Eff_SetPointLightColor(0xB, pCol->m_LightColor0, pCol->m_LightColor1);
            break;
        }
        case 4: {
            const CChrCoreBasketYoshi::YOSHI_COL_SET *pCol =
                CChrCoreBasketYoshi::GetEffectUnit(ClampColor(m_nColor, 3));
            Eff_SetPointLightColor(0xB, pCol->m_Color0, pCol->m_Color1);
            break;
        }
    }

    // The other team's drivers learn the cut's direction and the ball's
    // (in degrees).
    long nPlayerNo = m_nPlayerNo;
    float fBallRot = 57.295776f * GetRotY_ToPosXZ(pWork->m_BallPos);
    float fRot = 57.295776f * GetRotY();
    Util_SendMsgToEnemy(static_cast<CChrDrv::MSG_ID>(0x15), fRot, fBallRot, nPlayerNo, 0);
    CollAtkGetColl_(0)->m_nNoHitFlag = 0;
}

// The cut turns to its ball (or the direction asked for) and lunges with
// the first motion, then plays the second and ends.
void msw::CChrCoreBasket::LoopStateBlockCut()
{
    STATE_WORK_BLOCK_CUT *pWork = FsmqGetWork<STATE_WORK_BLOCK_CUT>();
    pWork->m_fFrame += GetFrameStep();
    if (pWork->m_nFlag & 4) {
        float fRotY = GetRotY();
        CBall *pBall = CBallMan::GetBallPtr(pWork->m_nBallNo);
        if (pBall) {
            fRotY = GetRotY_ToPosXZ(pBall->GetPos());
        }
        RotateYTo_(fRotY, 0.27925268f);
    } else if (pWork->m_fRotY != -1.0f) {
        RotateYRate_(pWork->m_fRotY, 0.2f);
    }

    SubStateNeedBlockWork_(0, 0.0f);
    switch (AnimGetMotID(3, 0)) {
        case 0x65: {
            if (IsAnimEnd(3)) {
                if (AnimGetMotIDEx(3) == 0x65) {
                    AnimPlay(3, 0x66, 0, 0.0f, 0.0f);
                } else {
                    AnimPlay(3, -0x66, 0, 0.0f, 0.0f);
                }
            }
            pWork->m_DirMove.m_fRotY = GetRotY();
            CC_DATA::CHR_PARAM::MOVE_PARAM move;
            move.m_nType = 2;
            move.m_fFrame = 2.0f;
            move.m_fFrameMax = 26.0f;
            move.m_fDist = 1.5f;
            Util_DoDirMove(move, AnimGetFrame(3), pWork->m_DirMove, 1);
            break;
        }
        case 0x66:
            if (IsAnimEnd(3)) {
                FsmqDoChain();
            }
            break;
    }
}

void msw::CChrCoreBasket::DtorStateBlockCut()
{
    CollAtkSetEnable_(0, 0);
    ArtsOptDestroy();
}

// ---- Side step ----------------------------------------------------------

// The side step: a dodge sideways from the ball holder, with an effect
// half-way between the two and a turn towards the holder.
void msw::CChrCoreBasket::CtorStateSideStep()
{
    AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x34 : 0x34, 0, 4.0f, 0.0f);

    float fRotY = GetRotY();
    bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
    CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
    if (pHolder) {
        bcs::FVEC3 dist = reinterpret_cast<VEC3_CREF>(static_cast<const CChrCore *>(pHolder)->GetPos()) -
                          reinterpret_cast<VEC3_CREF>(GetPos());
        ofs.Set(dist.x * 0.5f, 0.0f, dist.z * 0.5f);
        fRotY = GetRotY_ToPosXZ(static_cast<const CChrCore *>(pHolder)->GetPos());
    }

    // The effect turns from the side the character guards, at most 40
    // degrees either way.
    float fBase = GetGuardGoalPos().z > 0.0f ? 3.1415927f : 0.0f;
    float fRot = WrapRadPi(fRotY - fBase);
    float fMax = 0.6981317f;
    if (__fabsf(fRot) >= fMax) {
        if (fRot < 0.0f) {
            fRot = -fMax;
        } else {
            fRot = fMax;
        }
    }
    fRot += fBase;

    Eff_Stop(0xB);
    Eff_CreateAttach(0xB, 0x4EF, m_nEffNodeSpike, 0, 0, 3, 0);
    Eff_SetRotateY(0xB, fRot);
    Eff_SetTrans(0xB, ofs);
    if (IsPadCtrl()) {
        static_cast<CGameBasket *>(CGame::m_pInstance)->CameraQuake_(1, m_nPlayerNo);
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x2E), 0, 0, 0, 0);
}

void msw::CChrCoreBasket::LoopStateSideStep()
{
    if (IsAnimEnd(3)) {
        FsmqDoChain();
    }
    float fRotY = GetRotY();
    CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
    if (pHolder) {
        fRotY = GetRotY_ToPosXZ(static_cast<const CChrCore *>(pHolder)->GetPos());
    }
    RotateYRate_(fRotY, 0.2f);
}

void msw::CChrCoreBasket::DtorStateSideStep()
{
}

// ---- Smash cut ----------------------------------------------------------

// The smash cut. Characters 8, 5 and 12 run straight (the first motion,
// the second looped until the run's frames are filled, the third), with
// the yaw towards the ball (or the ball holder of the other team);
// everyone else jumps into a spin (SubStateSmashCut_).
void msw::CChrCoreBasket::CtorStateSmashCut()
{
    STATE_WORK_SMASH_CUT *pWork = FsmqGetWork<STATE_WORK_SMASH_CUT>();
    pWork->m_nStep = 0;
    pWork->m_nUnk04 = 0;
    pWork->m_fUnk10 = 0.0f;
    pWork->m_fFrameNum = 0.0f;

    CCollAtkUnit *pAtk = CollAtkGetColl_(0);
    pAtk->m_nNoHitFlag = 0;
    pAtk->GetCollUnit()->SetAtkType(0x20000);

    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    pWork->m_fUnk24 = pParam->m_nSmashCutUnk1CC;
    pWork->m_fUnk28 = pParam->m_nSmashCutUnk1D0;
    pWork->m_fRadius = pParam->m_fSmashCutRadius;
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);

    switch (GetChrIdS()) {
        case 8:
        case 5:
        case 0xC: {
            pWork->m_fRotY = GetRotY();
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall) {
                pWork->m_fRotY = GetRotY_ToPosXZ(pBall->GetPos());
            }

            // The run lasts the parameter's frames: the loop motion is
            // repeated until the three motions fill them, and all three are
            // sped up to end on time.
            const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam2 = GetCcParam2_();
            long nStart = AnimGetResFrameNum(0x6E);
            long nLoop = AnimGetResFrameNum(0x6F);
            float fEnd = AnimGetResFrameNum(0x70);
            pWork->m_fHoldFrame = 1.0f;
            long nFrame = nStart + (nLoop + fEnd);
            pWork->m_fFrameNum = pParam2->m_nSmashCutJumpFrame;
            if (nFrame < (long)pWork->m_fFrameNum) {
                long nRest = (long)pWork->m_fFrameNum - nFrame;
                long nAdd = nRest / nLoop;
                if (nRest % nLoop != 0) {
                    nAdd++;
                }
                pWork->m_fHoldFrame += nAdd;
                nFrame += nLoop * nAdd;
            }
            pWork->m_fAnimSpeed = nFrame / pWork->m_fFrameNum;

            pWork->m_fMoveRotY = GetRotY();
            CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
            if (pHolder && pHolder->GetTeamNo() != GetTeamNo()) {
                pWork->m_fMoveRotY = GetRotY_ToPosXZ(static_cast<const CChrCore *>(pHolder)->GetPos());
            }
            AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x6E : 0x6E, 0, 4.0f, 0.0f);
            AnimSetSpeed(3, pWork->m_fAnimSpeed);
            pWork->m_fSteerRotY = -1.0f;
            break;
        }
    }
    WSE_Play(0, 8, 1, -1);
    PlayVoice(0x1E, 0);
}

// Characters 0x11 and 9 only play the first and last motions in place.
void msw::CChrCoreBasket::LoopStateSmashCut()
{
    STATE_WORK_SMASH_CUT *pWork = FsmqGetWork<STATE_WORK_SMASH_CUT>();
    pWork->m_fUnk10 += GetFrameStep();
    switch (GetChrIdS()) {
        case 8:
        case 5:
        case 0xC:
            SubStateSmashCutStraight_(pWork);
            break;
        case 0x11:
        case 9:
            switch (pWork->m_nStep) {
                case 0:
                    AnimPlay(3, !m_pMdl->IsAnmFlipPlay(3, 0) ? 0x6E : -0x6E, 0, 4.0f, 0.0f);
                    pWork->m_nStep++;
                    // fall through
                case 1:
                    if (!IsAnimEnd(3)) {
                        break;
                    }
                    AnimPlay(3, !m_pMdl->IsAnmFlipPlay(3, 0) ? 0x71 : -0x71, 0, 4.0f, 0.0f);
                    pWork->m_nStep++;
                    // fall through
                case 2:
                    if (IsAnimEnd(3)) {
                        FsmqDoChain();
                    }
                    break;
            }
            break;
        default:
            SubStateSmashCut_(pWork);
            break;
    }
}

void msw::CChrCoreBasket::DtorStateSmashCut()
{
    WSE_Stop(0, 0.0f);
    Eff_Stop(0xB);
    ArtsOptDestroy();
    CollAtkGetColl_(0)->GetCollUnit()->OffFlag10(1);
}

// The spinning cut: a jump turning by m_fRotY per frame (steps 1 to 3;
// with a second turn for characters that have one), then the fall (step
// 4), which turns back part of the way, and the landing. The motions
// follow the frames left: the first, the loop, and the last timed to end
// with the cut.
void msw::CChrCoreBasket::SubStateSmashCut_(STATE_WORK_SMASH_CUT *pWork)
{
    float fStep = m_fFrameStep;
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_nStep++;
            // fall through
        case 1: {
            m_nStatusFlag = (m_nStatusFlag & ~0x1C00) | 0x400;
            float fHold = pParam->m_nSmashCutHoldFrame;
            pWork->m_fFrameNum = pParam->m_nSmashCutJumpFrame + fHold;
            pWork->m_fFrameNum = pWork->m_fFrameNum + pParam->m_nSmashCutFallFrame;
            pWork->m_fHoldFrame = fHold + pParam->m_nSmashCutJumpFrame;
            pWork->m_fRotY = DegToRad(pParam->m_nSmashCutRotDeg);
            if (m_pMdl->IsAnmFlipPlay(3, 0)) {
                pWork->m_fRotY = -pWork->m_fRotY;
            }

            long nEff = -1;
            int bLeft = pWork->m_fRotY > 0.0f;
            switch (GetChrIdS()) {
                case 0:
                    nEff = 0x5F8;
                    if (bLeft) {
                        nEff = 0x5F7;
                    }
                    break;
                case 2:
                    nEff = 0x60B;
                    if (bLeft) {
                        nEff = 0x60A;
                    }
                    break;
                case 3:
                    nEff = 0x615;
                    if (bLeft) {
                        nEff = 0x614;
                    }
                    break;
                case 6:
                    nEff = 0x633;
                    if (bLeft) {
                        nEff = 0x632;
                    }
                    break;
                case 7:
                    nEff = 0x63A;
                    if (bLeft) {
                        nEff = 0x639;
                    }
                    break;
                case 0xA:
                    nEff = 0x652;
                    if (bLeft) {
                        nEff = 0x651;
                    }
                    break;
                case 0xB:
                    nEff = 0x65F;
                    if (bLeft) {
                        nEff = 0x65E;
                    }
                    break;
                case 0xE:
                    nEff = 0x67D;
                    if (bLeft) {
                        nEff = 0x67C;
                    }
                    break;
                case 0xD:
                    nEff = 0x66E;
                    break;
                case 0xF:
                    nEff = 0x688;
                    break;
                case 0x13:
                case 0x15:
                case 0x17:
                    nEff = 0x6A2;
                    if (bLeft) {
                        nEff = 0x6A1;
                    }
                    break;
                case 0x14:
                case 0x16:
                case 0x18:
                    nEff = 0x6AB;
                    if (bLeft) {
                        nEff = 0x6AA;
                    }
                    break;
            }
            if (nEff > 0) {
                unsigned long nNode = m_nEffNode;
                if (GetChrIdS() == 0xD) {
                    nNode = m_nEffNodeSpike;
                }
                Eff_Stop(0xB);
                Eff_CreateAttach(0xB, nEff, nNode, 0, 0, 3, 0);
            }

            float fHeight = pParam->m_fSmashCutHeight;
            Util_SetJumpParam(pParam->m_nSmashCutJumpFrame, GetPos().y, fHeight, fHeight * 0.5f);
            pWork->m_nStep++;
        }
            // fall through
        case 2:
        case 3:
            Util_DoJumpMove(fStep);
            if (m_fJumpVelY < 0.0f) {
                m_fJumpVelY = 0.0f;
                SetPosY(pParam->m_fSmashCutHeight);
                m_nStatusFlag |= 0x400;
            }
            float fTurn = fStep * pWork->m_fRotY;
            SetRotY(WrapRad(fTurn + GetRotY()));
            pWork->m_fHoldFrame -= fStep;
            if (pWork->m_fHoldFrame <= 0.0f) {
                if (pWork->m_nStep == 2 && pParam->m_nSmashCutTurnFrame > 0) {
                    // The second turn: facing away from the camera.
                    pWork->m_fRotY = 0.0f;
                    pWork->m_fHoldFrame = pParam->m_nSmashCutTurnFrame;
                    SetRotY(3.1415927f + m_fCamRotY);
                    pWork->m_nStep++;
                    if (GetChrId() == 0xD) {
                        Eff_Stop(0xB);
                    }
                } else {
                    m_nStatusFlag = (m_nStatusFlag & ~0x1C00) | 0x1000;
                    float fTop = GetPos().y * 0.5f;
                    Util_SetJumpParam(pParam->m_nSmashCutFallFrame, GetPos().y, 0.0f, fTop);
                    pWork->m_nStep = 4;
                }
            }
            break;
        case 4: {
            float fFall = CLAMP(pWork->m_fFrameNum - pWork->m_fUnk10, 0.0f, (float)pParam->m_nSmashCutFallFrame);
            float fRate = 1.0f - fFall / pParam->m_nSmashCutFallFrame;
            float fBack = DegToRad((float)pParam->m_nSmashCutRotDeg - (float)pParam->m_nSmashCutRotBackDeg);
            if (!m_pMdl->IsAnmFlipPlay(3, 0)) {
                fBack = -fBack;
            }
            float fTurn = fStep * (pWork->m_fRotY - fBack * fRate);
            SetRotY(WrapRad(fTurn + GetRotY()));
            if (Util_DoJumpMove(fStep) || pWork->m_fFrameNum <= pWork->m_fUnk10) {
                AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x71 : 0x71, 0, 4.0f, 0.0f);
                AnimSetSpeed(3, 1.0f);
                m_nStatusFlag = (m_nStatusFlag & ~0x1C00) | 0x800;
                pWork->m_nStep++;
            }
            break;
        }
        default:
            Util_DoJumpMove(fStep);
            if (IsAnimEnd(3)) {
                m_nStatusFlag &= ~0x1C00;
                FsmqDoChain();
                return;
            }
            break;
    }

    switch (AnimGetMotID(3, 0)) {
        case 0x70:
        case 0x71:
            break;
        case 0x6E:
            if (!IsAnimEnd(3)) {
                break;
            }
            AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x6F : 0x6F, 1, 4.0f, 0.0f);
            AnimSetSpeed(3, 1.0f);
            // fall through
        case 0x6F: {
            // The last motion starts when the frames left fit it.
            float fRest = pWork->m_fFrameNum - pWork->m_fUnk10;
            float fEnd = AnimGetResFrameNum(0x70);
            if (fRest <= fEnd) {
                AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x70 : 0x70, 0, 4.0f, 0.0f);
                float fSpeed = fEnd / fRest;
                if (fSpeed < 0.0f) {
                    fSpeed = 0.1f;
                }
                AnimSetSpeed(3, fSpeed);
            }
            break;
        }
        default: {
            AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x6E : 0x6E, 0, 4.0f, 0.0f);
            float fStart = AnimGetResFrameNum(0x6E);
            if (pWork->m_fFrameNum >= fStart + AnimGetResFrameNum(0x70)) {
                AnimSetSpeed(3, 1.0f);
            } else {
                float fSpeed = fStart / (pWork->m_fFrameNum * 0.5f);
                if (fSpeed < 0.0f) {
                    fSpeed = 0.1f;
                }
                AnimSetSpeed(3, fSpeed);
            }
            break;
        }
    }
}

// The straight cut: the first motion, the loop for m_fHoldFrame loops, the
// last; while running (step 0) the character turns towards the yaw the
// driver steers to (an eighth of the way each frame) and moves forward,
// until the run's frames are over.
void msw::CChrCoreBasket::SubStateSmashCutStraight_(STATE_WORK_SMASH_CUT *pWork)
{
    float fStep = m_fFrameStep;
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    switch (AnimGetMotID(3, 0)) {
        case 0x6E:
            if (IsAnimEnd(3)) {
                AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x6F : 0x6F, 1, 0.0f, 0.0f);
                AnimSetSpeed(3, pWork->m_fAnimSpeed);
            }
            break;
        case 0x6F:
            if (IsAnimLoop(3)) {
                pWork->m_fHoldFrame -= 1.0f;
                if (!(pWork->m_fHoldFrame > 0.0f)) {
                    AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x70 : 0x70, 0, 4.0f, 0.0f);
                    AnimSetSpeed(3, pWork->m_fAnimSpeed);
                }
            }
            break;
    }

    switch (pWork->m_nStep) {
        case 0: {
            if (pWork->m_fSteerRotY != -1.0f) {
                float fRot = WrapRad(pWork->m_fSteerRotY - GetRotY());
                float fRate = 0.125f;
                if (fRot < 3.1415927f) {
                    fRot = CLAMP(fRot * fRate, 0.0f, fRot);
                } else {
                    float fLeft = 6.2831855f - fRot;
                    fRot = CLAMP(-fRate * fLeft, -1.0f * fLeft, 0.0f);
                }
                pWork->m_fMoveRotY = WrapRad(fRot + GetRotY());
            }
            RotateYRate_(pWork->m_fMoveRotY, 0.2f);
            bcs::FVEC3 move;
            bcs::BCS_VecZ_RotY(move, 1.0f, GetRotY());
            float fSpeed = pParam->m_fSmashCutSpeed * fStep;
            move.x *= fSpeed;
            move.z *= fSpeed;
            AddPos(move);
            pWork->m_fFrameNum -= fStep;
            if (pWork->m_fFrameNum > 0.0f) {
                break;
            }
            AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x71 : 0x71, 0, 4.0f, 0.0f);
            pWork->m_nStep++;
        }
            // fall through
        default:
            if (IsAnimEnd(3)) {
                FsmqDoChain();
                return;
            }
            break;
    }
    pWork->m_fSteerRotY = -1.0f;
}
