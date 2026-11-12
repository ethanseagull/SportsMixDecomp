// msw::CChrCoreBasket's dunk states: the dunk, the rebound dunk and the
// alley-oop (all three share the dunk's work and steps), the lay-up, and
// CChrCore::CallbackMotTrg_AtkOff_ (empty). The file ends in its static
// initialiser.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/CGoalObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CComManBasketGame.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CGameGuideManBasket.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/CStageMan.hpp"

namespace {

    // The dunk's motions, by kind of dunk (STATE_WORK_DUNK::m_nType): the
    // start, the run-up loop, the jump, the dunk, the hang and the landing.
    // Each comes as a pair, the motion and the mirrored one (a negative id).
    enum {
        DUNK_MOT_START,
        DUNK_MOT_LOOP,
        DUNK_MOT_JUMP,
        DUNK_MOT_DUNK,
        DUNK_MOT_HANG,
        DUNK_MOT_LAND,
        DUNK_MOT_NUM
    };
    const short s_DunkMot[4][DUNK_MOT_NUM][2] = {
        { { 0x52, -0x52 }, { 0x56, -0x56 }, { 0x55, -0x55 }, { 0x5D, -0x5D }, { 0x60, -0x60 }, { 0x63, -0x63 } },
        { { 0x54, -0x53 }, { 0x5A, -0x57 }, { 0x5B, -0x58 }, { 0x5E, -0x5E }, { 0x61, -0x61 }, { 0x64, -0x64 } },
        { { 0x52, -0x52 }, { 0x56, -0x56 }, { 0x59, -0x59 }, { 0x5C, -0x5C }, { 0x5F, -0x5F }, { 0x62, -0x62 } },
        { { 0x53, -0x54 }, { 0x57, -0x5A }, { 0x58, -0x5B }, { 0x5E, -0x5E }, { 0x61, -0x61 }, { 0x64, -0x64 } },
    };

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

    // (guessed helper) An angle brought into (-pi, pi].
    inline float WrapRadPi(float rad)
    {
        rad = WrapRad(rad);
        if (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        return rad;
    }

    // (guessed helper) Not zero when character type nType is neither 5
    // nor 6.
    inline int IsOtherThanType5And6(long nType)
    {
        int bOther = 0;
        if (nType < 5 || nType > 6) {
            bOther = 1;
        }
        return bOther;
    }

    // (guessed helper) Not zero while the character's motion plays
    // mirrored: the second index of s_DunkMot.
    inline int IsMotFlip(const msw::CChrCore *pChr)
    {
        return pChr->GetMdl()->IsAnmFlipPlay(3, 0);
    }

    // (guessed helper) rOut = v * fScale by way of a temporary.
    inline void ScaleVec(bcs::FVEC3 &rOut, const bcs::FVEC3 &v, float fScale)
    {
        bcs::FVEC3 product;
        bcs::FVEC3Scale(&product, &v, fScale);
        rOut = product;
    }

    // (guessed helper) rOut = a - b by way of a temporary, as
    // bcs::FVEC3Sum does a sum (most likely an operator- in the original).
    inline void VecSub(bcs::FVEC3 &rOut, const bcs::FVEC3 &a, const bcs::FVEC3 &b)
    {
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &a, &b);
        rOut = diff;
    }

    // (guessed helper) v flattened onto the XZ plane and scaled to length
    // 1 (zero when it has no length there).
    inline void NormalizeXZ(bcs::FVEC3 &v)
    {
        bcs::FVEC3 xz(v.x, 0.0f, v.z);
        bcs::BCS_Vec_Normalize(v, xz);
    }

    // 70 degrees in radians. Not used here: nearly every BasketF file has
    // this object and the static initialiser that fills it (see
    // CBallManBasket.cpp).
    const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);
}

// The work of the dunk, the rebound dunk and the alley-oop in the state
// queue (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_DUNK {
    long m_nStep;            // 0x00: 0, 1 run-up, 2-4 dunk, 5 landing, 6-7 missed and dropping
    long m_nType;            // 0x04: the row of s_DunkMot
    long m_nUnk08;           // 0x08
    long m_nLoop;            // 0x0C: run-up loops left
    // 0x10: 0x1 may still turn into a lay-up, 0x2 the jump has started,
    // 0x8 a partner runs along (m_pPartner), 0x20 and 0x40 jumping to the
    // dunk, 0x80 the alley-oop's pass has been asked for, 0x400 the run-up
    // is timed instead of counted in loops
    unsigned long m_nFlag;
    float m_fFrame;          // 0x14: frames since the run-up began
    float m_fRestFrame;      // 0x18: frames left to the dunk
    float m_fFrameMax;       // 0x1C: frames of the run-up and jump
    float m_fSpeedRate;      // 0x20
    float m_fTime;           // 0x24: frames in the state
    float m_fMotSpeed;       // 0x28: the play speed of the motions
    unsigned char m_Unk2C[0x30 - 0x2C];
    bcs::FVEC3 m_Mid;        // 0x30: the point homed in on (y: its range)
    bcs::FVEC3 m_Target;     // 0x3C: the point the dunk aims at
    bcs::FVEC3 m_Vel;        // 0x48: the move while dropping
    bcs::FVEC3 m_StartPos;   // 0x54: where the dunk started
    bcs::FVEC3 m_GoalPos;    // 0x60: the goal dunked at
    CChrCoreBasket *m_pPartner; // 0x6C
};

// The lay-up's work in the state queue (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_LAYUP {
    long m_nStep;            // 0x00
    unsigned long m_nFlag;   // 0x04: 0x1 from the right, 0x2 from the left
    float m_fShotFrame;      // 0x08: the motion frame the ball leaves at
    float m_fRotY;           // 0x0C: the yaw of the run-in
    float m_fHoldFrame;      // 0x10: frames left to hang at the top
    float m_fTopY;           // 0x14: the height of the top
    bcs::FVEC3 m_LandVel;    // 0x18
    bcs::FVEC3 m_Target;     // 0x24
    bcs::FVEC3 m_Vel;        // 0x30: the move per frame
};

namespace {

    // (guessed helper) Not zero when the dunk cannot be made: the
    // character is off the point, the goal's boss Packun keeps his mouth
    // shut, or the goal is busy with another dunk.
    inline int IsDunkMiss(msw::CChrCoreBasket *pChr, msw::CChrCoreBasket::STATE_WORK_DUNK *pWork)
    {
        if (!pChr->IsInnerXZ(pWork->m_Mid, 0.3f) || !pChr->IsInnerXZ(pChr->GetTargetGoalPos(), 1.0f)) {
            return 1;
        }
        long nGoal = pChr->GetTargetGoalIdx();
        msw::CNpcBossPackun *pBoss = msw::CChrManBasket::GetInstance()->GetBossPackun_(nGoal);
        if (pBoss != 0 && !pBoss->IsMouthOpen()) {
            return 1;
        }
        nGoal = pChr->GetTargetGoalIdx();
        CGoalObj *pGoal = msw::CStageMan::GetInstance()->GetGoal(nGoal);
        if (pGoal != 0 && pGoal->IsNowDunkMot()) {
            return 1;
        }
        if (pChr->IsHoldBall()) {
            pChr->SetCollBodyPushRank_(5);
            pChr->SetCollBodyPushPower_(5);
        }
        return 0;
    }
}

// ---- Dunk ---------------------------------------------------------------

void msw::CChrCoreBasket::CtorStateDunk()
{
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    m_UnkEC &= ~0x10;
    STATE_WORK_DUNK *pWork = FsmqGetWork<STATE_WORK_DUNK>();
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();

    pWork->m_nType = 0;
    long nMot = s_DunkMot[pWork->m_nType][DUNK_MOT_START][(m_nStatusFlag >> 2) & 1];
    float fFrame = pParam->m_nDunkFrame;
    if (CGame::g_GameWork.m_nGameType == 0) {
        fFrame *= CComManBasketGame::GetDunkFrameRate(this);
    }
    SubStateDunkCtor_(pWork, fFrame, pWork->m_nType, pParam->m_fDunkHeight, nMot);
    pWork->m_nFlag |= 1;

    CBall *pBall = GetAttachBall();
    if (pBall != 0) {
        CGame::SendMessage(0x34, GetPlayerNo(), 3, pBall->GetBallNo());
    }
    if (m_nBasketFlag & 0x800) {
        m_CombIcon.ClearIcon(1);
    }
    if (CChrManBasket::GetInstance()->IsScoreBookCountChr_(this)) {
        m_nBasketFlag |= 0x8000;
    }
    CGameBasket::GetInstance()->StopEffect_(0.0f, 0.0f, 0.0f);
    Util_SendMsgToAll(static_cast<CChrDrv::MSG_ID>(0x30), GetPlayerNo(), 3, 0, 0);
}

void msw::CChrCoreBasket::LoopStateDunk()
{
    STATE_WORK_DUNK *pWork = FsmqGetWork<STATE_WORK_DUNK>();

    if (pWork->m_nFlag & 0x40) {
        pWork->m_pPartner = 0;
        pWork->m_nFlag &= ~8;
    }
    if (SubStateDunkCmnAskLayupBranch_(pWork)) {
        return;
    }
    // Asks the driver once to let go of the run-up, when the character is
    // away from the goal.
    if (pWork->m_nStep == 0 || pWork->m_nStep == 1) {
        static const float fMinDist = 0.5f;
        bcs::FVEC3 diff;
        VecSub(diff, GetPos(), pWork->m_GoalPos);
        if (!(fMinDist > bcs::BCS_Vec_GetLenXZ(diff)) && !(pWork->m_nFlag & 2) &&
            DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x2B), 0, 0, 0, 0) != 0) {
            if (CChrManBasket::GetInstance()->IsScoreBookCountChr_(this)) {
                m_nBasketFlag |= 0x8000;
            } else {
                m_nBasketFlag &= ~0x8000;
            }
            pWork->m_nFlag |= 2;
        }
    }
    SubStateDunkCmnMove_(pWork);
}

void msw::CChrCoreBasket::DtorStateDunk()
{
    if ((m_nBasketFlag & 8) && !(m_nBasketFlag & 0x10)) {
        CGame::SendMessage(0x32, GetPlayerNo(), 0, 0);
    }
    m_nBasketFlag &= ~0x20018;
    m_nStatusFlag &= ~0x43000000;
    Eff_Stop(10);
    Eff_Stop(11);
    Eff_Stop(12);
    Eff_StopForce(11);
    WSE_Stop(0, 0.0f);
    WSE_Stop(1, 0.0f);
    ArtsOptDestroy();
    if (m_nBasketFlag & 0x800) {
        SetStateCombinationPlay(0);
    }
    SetRiverFallEnable(0);
}

void msw::CChrCoreBasket::SubStateDunkCtor_(STATE_WORK_DUNK *pWork, float fFrame, long nType, float fHeight,
                                            long nMot)
{
    pWork->m_nStep = 0;
    pWork->m_nFlag = 0;
    pWork->m_nType = nType;
    pWork->m_fFrame = 0.0f;
    pWork->m_fRestFrame = fFrame;
    pWork->m_fFrameMax = fFrame;
    pWork->m_nUnk08 = 0;
    pWork->m_fTime = 0.0f;
    pWork->m_fSpeedRate = 1.0f;
    pWork->m_pPartner = 0;
    pWork->m_StartPos = GetPos();
    pWork->m_GoalPos = GetTargetGoalPos();

    // On stage 5 a dunk from far away aims at a point nine units out from
    // the start towards the goal.
    if (CGame::g_GameWork.GetStage() == 5) {
        static const float fDist = 9.0f;
        bcs::FVEC3 diff;
        VecSub(diff, pWork->m_GoalPos, pWork->m_StartPos);
        if (bcs::BCS_Vec_GetLenXZ(diff) > fDist) {
            NormalizeXZ(diff);
            pWork->m_GoalPos.x = fDist * diff.x + pWork->m_StartPos.x;
            pWork->m_GoalPos.z = fDist * diff.z + pWork->m_StartPos.z;
        }
    }

    GetAirTrickTargetPos(pWork->m_Target, pWork->m_Mid, pWork->m_fFrame, pWork->m_fRestFrame);
    SetMoveMotStep_(0);
    SetMoveMotType(0);
    m_fJumpUnk220 = 0.0f;
    float fRest = pWork->m_fRestFrame;
    // (not used) How far into the running shot's circle the character
    // stands.
    float fDist = GetDist_ToPosXZ(pWork->m_Mid);
    float fIn = fDist > GetCourtRunShootSize() ? GetCourtRunShootSize() : (fDist < 0.0f ? 0.0f : fDist);
    float fRate = fIn * (1.0f / GetCourtRunShootSize());
    fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
    float fFrom = GetPosY();
    Util_SetJumpParam(fRest + 1.0f, fFrom, GetTargetGoalPos().y, fHeight);
    Util_DoJumpMove(m_fFrameStep);
    AnimPlay(3, nMot, 0, 8.0f, 0.0f);

    // The run-up loops until the start, loop and jump motions fill the
    // frames (or, timed, plays them faster to fit).
    int nFlip = nMot < 0;
    long nStart = AnimGetResFrameNum(s_DunkMot[pWork->m_nType][DUNK_MOT_START][nFlip]);
    long nLoop = AnimGetResFrameNum(s_DunkMot[pWork->m_nType][DUNK_MOT_LOOP][nFlip]);
    long nJump = AnimGetResFrameNum(s_DunkMot[pWork->m_nType][DUNK_MOT_JUMP][nFlip]);
    long nTotal = nStart + nLoop + nJump;

    switch (pWork->m_nType) {
        case 0:
            if (GetCcParam2_()->m_bDunkLoop == 0) {
                pWork->m_nFlag |= 0x400;
            }
            WSE_Play(0, 0, 0, -1);
            break;
        case 1:
        case 3:
            if (GetCcParam2_()->m_bAlleyoopLoop == 0) {
                pWork->m_nFlag |= 0x400;
            }
            break;
    }

    if (pWork->m_nFlag & 0x400) {
        if (nStart + nJump > static_cast<long>(pWork->m_fFrameMax)) {
            pWork->m_nLoop = 0;
            pWork->m_fMotSpeed = (nStart + nJump) / pWork->m_fFrameMax;
        } else {
            pWork->m_fMotSpeed = 1.0f;
        }
    } else {
        pWork->m_nLoop = 1;
        if (nTotal < static_cast<long>(pWork->m_fFrameMax)) {
            long nRest = static_cast<long>(pWork->m_fFrameMax) - nTotal;
            long nCount = nRest / nLoop;
            if (nRest % nLoop != 0) {
                nCount++;
            }
            pWork->m_nLoop += nCount;
            nTotal += nLoop * nCount;
        }
        pWork->m_fMotSpeed = nTotal / pWork->m_fFrameMax;
    }
    AnimSetSpeed(3, pWork->m_fMotSpeed);
    PlayVoice(0x15, 0);
    m_nBasketFlag &= ~0x20000;
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    SetCollBodyPushPower_(1);
    SetCollBodyPushRank_(1);
    SetRiverFallEnable(1);
}

int msw::CChrCoreBasket::SubStateDunkCmnAskLayupBranch_(STATE_WORK_DUNK *pWork)
{
    if (!(pWork->m_nFlag & 1)) {
        return 0;
    }
    if (pWork->m_fTime < GetChangeStateDisableInput()) {
        return 0;
    }
    if (!(m_nStatusFlag & 1) || (m_UnkEC & 0x0A000000) || !(m_UnkEC & 0x24000000)) {
        pWork->m_nFlag &= ~1;
        return 0;
    }
    long nLayUp = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x20), 0, 0, 0, 0);
    if (nLayUp > -1 && MethdLayUp(nLayUp)) {
        m_nBasketFlag &= ~0x18;
        FsmqRetry();
        return 1;
    }
    return 0;
}

void msw::CChrCoreBasket::SubStateDunkCmnLandInit_(STATE_WORK_DUNK *pWork)
{
    pWork->m_nFlag &= ~1;
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
    if (m_nStatusFlag & 1) {
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    } else {
        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    }
    m_nStatusFlag &= ~0x20;
    m_nStatusFlag &= ~0x40000000;
    m_nStatusFlag &= ~0x01000000;
    long nMot = AnimGetMotIDEx(3);
    AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_LAND][nMot < 0], 0, 0.0f, 0.0f);
    pWork->m_nStep = 5;
}

void msw::CChrCoreBasket::SubStateDunkCmnMove_(STATE_WORK_DUNK *pWork)
{
    // static-order swap: the original's .rodata has fScale (declared in
    // step 6 below) before this one; ours emits them the other way round.
    static const float fSpeed = 1.1f;

    bcs::FVEC3 prevPos = GetPos();
    m_fJumpVelZ = 0.0f;
    m_fJumpVelX = 0.0f;
    float fTimeStep = m_fTimeScale * m_fFrameStep;
    pWork->m_fTime += fTimeStep;

    // The run-up motions: start, loop, then the jump.
    switch (AnimGetMotID(3, 0)) {
        case 0x52:
        case 0x53:
        case 0x54:
            if (!IsAnimEnd(3)) {
                break;
            }
            AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_LOOP][IsMotFlip(this)], 1, 0.0f, 0.0f);
            AnimSetSpeed(3, pWork->m_fMotSpeed);
            // fall through
        case 0x56:
        case 0x57:
        case 0x5A:
            if (pWork->m_nFlag & 0x400) {
                if (!(pWork->m_nFlag & 2)) {
                    float fRest = pWork->m_fFrameMax - pWork->m_fFrame;
                    if (fRest < 0.0f) {
                        fRest = 0.1f;
                    }
                    if (fRest >= AnimGetResFrameNum(s_DunkMot[pWork->m_nType][DUNK_MOT_JUMP][IsMotFlip(this)]) /
                                     pWork->m_fMotSpeed) {
                        break;
                    }
                }
            } else {
                if (!IsAnimLoop(3)) {
                    break;
                }
                if (--pWork->m_nLoop > 0) {
                    break;
                }
            }
            if (!(pWork->m_nFlag & 2)) {
                // (not used) How far the model has moved off the
                // character.
                bcs::FVEC3 trans;
                GetTransNodePos(trans);
                bcs::FVEC3 diff;
                VecSub(diff, GetPos(), trans);
                bcs::BCS_Vec_GetLenXZ(diff);
                AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_JUMP][IsMotFlip(this)], 0, 8.0f, 0.0f);
                AnimSetSpeed(3, pWork->m_fMotSpeed);
                break;
            }
            // fall through
        case 0x55:
            if (pWork->m_nFlag & 2) {
                // Near the goal: the jump of the dunk of kind 2.
                bcs::FVEC3 diff;
                VecSub(diff, pWork->m_GoalPos, GetPos());
                if (!(3.0f < bcs::BCS_Vec_GetLenXZ(diff))) {
                    float fRest = pWork->m_fFrameMax - pWork->m_fFrame;
                    if (fRest < 0.0f) {
                        fRest = 0.1f;
                    }
                    float fJump = AnimGetResFrameNum(s_DunkMot[2][DUNK_MOT_JUMP][IsMotFlip(this)]);
                    if (!(fRest > fJump)) {
                        pWork->m_nType = 2;
                        long nMot = s_DunkMot[pWork->m_nType][DUNK_MOT_JUMP][IsMotFlip(this)];
                        float fJumpSpeed = fJump / fRest;
                        if (fJumpSpeed > 2.0f) {
                            fJumpSpeed = 2.0f;
                        }
                        AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                        AnimSetSpeed(3, fJumpSpeed);
                        PlayVoice(0x16, 0);
                        WSE_Play(1, 1, 0, -1);
                        pWork->m_nFlag |= 0x40;
                        pWork->m_nFlag &= ~1;
                    }
                }
            }
            break;
    }

    switch (pWork->m_nStep) {
        case 0: {
            if (!(m_nBasketFlag & 8) && pWork->m_nType == 0) {
                Eff_Stop(11);
                switch (GetChrTypeID()) {
                    case 13:
                        Eff_CreateAttach(11, 0x66F, m_nEffNodeSpike, 0, 0, 3, 0);
                        break;
                    case 4:
                        Eff_CreateAttach(11, 0x61E, m_nEffNodeBlock, 0, 0, 3, 0);
                        break;
                }
            }
            m_nBasketFlag |= 8;
            long nMot = AnimGetMotID(3, 0);
            if (nMot < 0x52 || nMot > 0x54 || IsAnimEnd(3)) {
                pWork->m_nStep = 1;
            }
        }
            // fall through
        case 1: {
            if (GetChrId() == 4) {
                CEffectCluster *pEff = Eff_GetCluster(11);
                if (pEff != 0) {
                    bcs::FVEC3 rot(0.0f, GetRotY(), 0.0f);
                    pEff->SetRotateRad(&rot);
                }
            }
            GetAirTrickTargetPos(pWork->m_Target, pWork->m_Mid, pWork->m_fFrame, pWork->m_fRestFrame);
            float fStep = m_fTimeScale * (pWork->m_fSpeedRate * m_fFrameStep);
            pWork->m_fFrame += fStep;
            SubStateDunkCmnHomingMove_(FsmqGetWork<STATE_WORK_DUNK>());
            if (!(pWork->m_fFrame >= pWork->m_fFrameMax && (pWork->m_nFlag & 0x40)) && Util_DoJumpMove(fStep)) {
                SubStateDunkCmnLandInit_(pWork);
            }
            // Dunk once the frames are up (after a kind 2 jump, once its
            // motion has ended).
            if (pWork->m_fFrame >= pWork->m_fFrameMax) {
                if (!(pWork->m_nFlag & 2) ||
                    (AnimGetMotIDEx(3) == s_DunkMot[2][DUNK_MOT_JUMP][IsMotFlip(this)] && IsAnimEnd(3))) {
                    if (IsDunkMiss(this, pWork)) {
                        pWork->m_nStep = 6;
                        FsmqRetry();
                        return;
                    }
                    // At the goal, half a unit in front of it.
                    bcs::FVEC3 ofs;
                    bcs::BCS_VecZ_RotY(ofs, 1.0f, GetRotY_FromGoal());
                    ofs.x *= 0.5f;
                    ofs.z *= 0.5f;
                    bcs::FVEC3 pos;
                    bcs::FVEC3Sum(pos, GetTargetGoalPos(), ofs);
                    SetPos(pos);
                    pWork->m_nStep = 2;
                }
            }
            break;
        }
        case 2:
        case 3:
        case 4:
            if (SubStateDunkCmnDunk_(pWork) == 2 && MethdFall()) {
                FsmqDoChain();
                return;
            }
            break;
        case 6: {
            // Missed: drop from the goal.
            if (GetChrId() != 11) {
                ArtsOptDestroy();
            }
            pWork->m_nFlag &= ~1;
            float fBlend = GetChrId() == 10 ? 10.0f : 8.0f;
            AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_HANG][IsMotFlip(this)], 0, fBlend, 0.0f);
            m_fJumpVelX = 0.0f;
            m_fJumpVelY = 0.0f;
            m_fJumpVelZ = 0.0f;
            AnimSetSpeed(3, fSpeed);
            float fTop = GetPos().y * 0.5f;
            const bcs::FVEC3 &rPos = GetPos();
            Util_SetJumpParam(AnimGetFrameNum(3) / fSpeed, rPos.y, 0.0f, fTop);
            Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(6));
            SetCollBodyPushRank_(4);
            SetCollBodyPushPower_(1);
            Eff_Stop(11);
            {
                static const float fScale = 0.02f;
                bcs::FVEC3 dir;
                bcs::BCS_VecZ_RotY(dir, 1.0f, GetRotY());
                pWork->m_Vel.x = fScale * dir.x;
                pWork->m_Vel.z = fScale * dir.z;
            }
            // The ball drops off forwards, turned by up to 15 degrees.
            if (m_nStatusFlag & 1) {
                CBall *pBall = GetAttachBall();
                if (pBall != 0) {
                    DetachBall();
                    CGame::SetLooseBallMode(1);
                    bcs::FVEC3 vel;
                    bcs::FVEC3 front(0.0f, 0.0f, 1.0f);
                    bcs::BCS_Vec_RotX(vel, front, 1.134464f);
                    float fRot = 0.017453292f * bcs::BCS_RandRange(-15L, 15L);
                    bcs::BCS_Vec_RotY(vel, vel, GetRotY() + fRot);
                    bcs::BCS_Vec_Normalize(vel, vel);
                    bcs::FVEC3Scale(&vel, &vel, 0.2f);
                    static_cast<CBallBasket *>(pBall)->ReleaseBall(vel);
                }
            }
            pWork->m_nStep = 7;
        }
            // fall through
        case 7: {
            float fStep = m_fTimeScale * m_fFrameStep;
            bcs::FVEC3 move;
            ScaleVec(move, pWork->m_Vel, fStep);
            AddPos(move);
            int nJump = Util_DoJumpMove(fStep);
            if (nJump == 1) {
                SubStateDunkCmnLandInit_(pWork);
                SetCollBodyPushRank_(1);
                SetCollBodyPushPower_(5);
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                SetRiverFallEnable(0);
                pWork->m_nStep = 5;
            } else if (nJump == 2 && MethdFall()) {
                FsmqDoChain();
                return;
            }
            break;
        }
        default:
            SubStateDunkCmnLand_(pWork);
            break;
    }

    m_fJumpVelX = GetPos().x - prevPos.x;
    m_fJumpVelZ = GetPos().z - prevPos.z;
}

void msw::CChrCoreBasket::SubStateDunkCmnHomingMove_(STATE_WORK_DUNK *pWork)
{
    // Count the frames down and home in on the point in front of the goal,
    // turning to the goal.
    float fMove = m_fFrameStep * (pWork->m_fSpeedRate * m_fTimeScale);
    float fDist = GetDist_ToPosXZ(pWork->m_Mid);
    pWork->m_fRestFrame -= fMove;
    float fRest = pWork->m_fRestFrame;
    if (fDist < pWork->m_Mid.y) {
        fRest = fRest > 8.0f ? 8.0f : fRest;
    }
    if (fRest < 1.0f) {
        fRest = 1.0f;
    }
    float fRate = fDist / fRest;
    if (fRate > 0.2f) {
        fRate = 0.2f;
    }
    unsigned long nFlag = pWork->m_nFlag;
    if ((nFlag & 8) && !(nFlag & 0x40)) {
        if (nFlag & 0x20) {
            fRate = 0.1f;
        } else if (nFlag & 8) {
            fRate = 0.08f;
        }
    }
    bcs::FVEC3 pos = GetPos(); // (not used)
    RotateYToPosRate_(pWork->m_GoalPos, 0.125f);
    Util_DoHomingMoveXZ(pWork->m_Mid, fRate * fMove);

    // The partner keeps alongside, the two body pushes apart.
    if ((pWork->m_nFlag & 8) && pWork->m_pPartner != 0) {
        float fRadius = GetPush1Radius();
        float fGap = pWork->m_pPartner->GetPush1Radius() + fRadius;
        bcs::FVEC3 dir;
        bcs::BCS_VecZ_RotY(dir, 1.0f, GetRotY_ToPosXZ(pWork->m_GoalPos));
        bcs::FVEC3 to;
        GetTransNodePos(to);
        to.x += dir.x * fGap;
        to.y = pWork->m_pPartner->GetPos().y;
        to.z += dir.z * fGap;
        bcs::FVEC3Sub(&to, &to, &pWork->m_pPartner->GetPos());
        bcs::FVEC3Scale(&to, &to, 0.3f);
        pWork->m_pPartner->AddPos(to);
    }
}

int msw::CChrCoreBasket::SubStateDunkCmnDunk_(STATE_WORK_DUNK *pWork)
{
    static const float fSpeed = 1.3f;

    int nJump = 0;
    switch (pWork->m_nStep) {
        case 2: {
            long nMot = s_DunkMot[pWork->m_nType][DUNK_MOT_DUNK][IsMotFlip(this)];
            Util_DunkStart(nMot, FsmqGetState() == STATE_DUNK ? 3 : 8, 8.0f, 1);
            Eff_Stop(10);
            Eff_Stop(11);
            pWork->m_nStep = 3;
            m_nBasketFlag |= 0x10;
        }
            // fall through
        case 3: {
            // Hang on until the motion ends, or let go at once when the
            // other side's ball holder jumps.
            int bLow = 0;
            if (!IsAnimEnd(3)) {
                int bWait = 1;
                CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(0);
                if (pHolder != 0) {
                    long nMot = pHolder->AnimGetMotID(3, 0);
                    if ((nMot >= 0x58 && nMot <= 0x59) || nMot == 0x55 || nMot == 0x5B) {
                        bWait = 0;
                        bLow = 1;
                    }
                }
                if (bWait) {
                    break;
                }
            }
            Util_DunkEnd(0);
            m_nStatusFlag &= ~0x40000000;
            m_nStatusFlag &= ~0x01000000;
            Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(6));
            SetCollBodyPushRank_(4);
            SetCollBodyPushPower_(1);
            AnimPlay(3, s_DunkMot[pWork->m_nType][DUNK_MOT_HANG][IsMotFlip(this)], 0, 8.0f, 0.0f);
            AnimSetSpeed(3, fSpeed);
            if (GetChrId() == 16 && !CChrManBasket::GetInstance()->IsCreateBossPackun_() && pWork->m_nType == 2) {
                m_fJumpVelY = -0.25f;
            } else {
                float fTop;
                if (bLow) {
                    fTop = GetPos().y * 0.5f;
                } else {
                    fTop = 0.5f + GetPos().y;
                }
                const bcs::FVEC3 &rPos = GetPos();
                Util_SetJumpParam(AnimGetFrameNum(3) / fSpeed, rPos.y, 0.0f, fTop);
                bcs::BCS_VecZ_RotY(pWork->m_Vel, 0.1f, WrapRad(3.1415927f + GetRotY()));
            }
            pWork->m_nStep = 4;
        }
            // fall through
        case 4: {
            float fStep = m_fTimeScale * m_fFrameStep;
            bcs::FVEC3 move;
            ScaleVec(move, pWork->m_Vel, fStep);
            AddPos(move);
            nJump = Util_DoJumpMove(fStep);
            if (nJump == 1) {
                SubStateDunkCmnLandInit_(pWork);
                SetCollBodyPushRank_(1);
                SetCollBodyPushPower_(5);
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                SetRiverFallEnable(0);
                pWork->m_nStep = 5;
            }
            break;
        }
    }
    return nJump;
}

void msw::CChrCoreBasket::SubStateDunkCmnLand_(STATE_WORK_DUNK *)
{
    float fStep = m_fTimeScale * m_fFrameStep;
    Util_DoJumpMove(fStep);
    if (!(m_nStatusFlag & 0xC00) && IsAnimEnd(3)) {
        long nMot = AnimGetMotID(3, 0);
        switch (nMot) {
            case 0x62:
                if (IsOtherThanType5And6(GetChrTypeID())) {
                    break;
                }
                // fall through
            case 0x64:
                if (GetChrId() != 1) {
                    // Landed facing away from the goal: turn round.
                    float fRotY = GetRotY();
                    fRotY = WrapRad(3.1415927f + fRotY);
                    Util_NodePatchRotY(fRotY);
                    SetRotY(fRotY);
                }
                break;
        }
        PlayLandSe(0);
        m_nStatusFlag &= ~0x20;
        FsmqDoChain();
    }
}

int msw::CChrCoreBasket::IsStateDunkStepStart() const
{
    long nState = FsmqGetState();
    if (nState != STATE_DUNK && nState != STATE_ALLEYOOP && nState != STATE_REBOUND_DUNK) {
        return 0;
    }
    return FsmqGetWork<STATE_WORK_DUNK>()->m_nStep == 0;
}

// ---- Rebound dunk -------------------------------------------------------

void msw::CChrCoreBasket::CtorStateReboundDunk()
{
    m_UnkEC |= 0x10;
    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(0);
    Util_SendMsgToAll(static_cast<CChrDrv::MSG_ID>(0x30), GetPlayerNo(), 5, 0, 0);
    STATE_WORK_DUNK *pWork = FsmqGetWork<STATE_WORK_DUNK>();

    // Mirrored when the ball is on the left.
    int nFlip = 0;
    CBall *pBall = CBallMan::GetBallPtr(0);
    if (pBall != 0) {
        float fRotY = GetRotY();
        if (WrapRadPi(GetRotY_ToPosXZ(pBall->GetPos()) - fRotY) < 0.0f) {
            nFlip = 1;
        }
    }
    long nMot = s_DunkMot[pWork->m_nType][DUNK_MOT_START][nFlip];
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    SubStateDunkCtor_(pWork, pParam->m_nReboundDunkFrame, 0, pParam->m_fReboundDunkHeight, nMot);
}

void msw::CChrCoreBasket::LoopStateReboundDunk()
{
    LoopStateAlleyoop();
}

void msw::CChrCoreBasket::DtorStateReboundDunk()
{
    DtorStateDunk();
}

// ---- Alley-oop ----------------------------------------------------------

void msw::CChrCoreBasket::CtorStateAlleyoop()
{
    STATE_WORK_DUNK *pWork = FsmqGetWork<STATE_WORK_DUNK>();
    m_nStatusFlag &= ~0x1F000000;
    m_UnkEC &= ~0x10;
    m_nStatusFlag &= ~0x20;
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();

    // Straight at the goal (mirrored with the ball on the left), or the
    // dunks of kinds 1 and 3 with the ball off to the side.
    long nType = 0;
    long nFlip = 0;
    CBall *pBall = CBallMan::GetBallPtr(0);
    if (pBall != 0) {
        float fGoal = GetRotY_ToPosXZ(GetTargetGoalPos());
        float fRot = WrapRadPi(GetRotY_ToPosXZ(pBall->GetPos()) - fGoal);
        if (__fabsf(fRot) <= 1.3962634f) {
            if (fRot > 0.0f) {
                nFlip = 1;
                m_nStatusFlag |= 4;
            } else {
                nFlip = 0;
                m_nStatusFlag &= ~4;
            }
        } else {
            float fGoalRot = GetRotY_ToGoal();
            if (WrapRadPi(GetRotY_ToPosXZ(pBall->GetPos()) - fGoalRot) < 0.0f) {
                nType = 3;
            } else {
                nType = 1;
            }
        }
    }
    SubStateDunkCtor_(pWork, pParam->m_nAlleyoopFrame, nType, pParam->m_fAlleyoopHeight,
                      s_DunkMot[nType][DUNK_MOT_START][nFlip]);
    Util_SendMsgToAll(static_cast<CChrDrv::MSG_ID>(0x30), GetPlayerNo(), 8, 0, 0);
    if (m_nBasketFlag & 0x800) {
        m_CombIcon.ClearIcon(1);
        m_CombIconAlleyoop.SetIcon();
    }
}

void msw::CChrCoreBasket::LoopStateAlleyoop()
{
    STATE_WORK_DUNK *pWork = FsmqGetWork<STATE_WORK_DUNK>();

    // Asks for the pass once in the air.
    if (pWork->m_nStep == 0 || pWork->m_nStep == 1) {
        if (!(pWork->m_nFlag & 0x80) && FsmqGetState() == STATE_ALLEYOOP && GetPos().y >= 2.0 &&
            DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x2C), 0, 0, 0, 0) != 0) {
            CChrManBasket::GetInstance()->Util_SendPassRequest_(0, this);
        }
    } else {
        pWork->m_nFlag |= 0x80;
    }
    SubStateDunkCmnMove_(pWork);
}

void msw::CChrCoreBasket::DtorStateAlleyoop()
{
    DtorStateDunk();
    m_CombIconAlleyoop.ClearIcon(1);
}

// ---- Lay-up -------------------------------------------------------------

void msw::CChrCoreBasket::CtorStateLayUp()
{
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(5);
    Util_SendMsgToAll(static_cast<CChrDrv::MSG_ID>(0x30), GetPlayerNo(), 6, 0, 0);
    SE_PlayWrapAct(2, 1, -1);
    if (CChrManBasket::GetInstance()->IsScoreBookCountChr_(this)) {
        m_nBasketFlag |= 0x8000;
    }
    SetRiverFallEnable(1);
    CGameGuideManBasket::AddSuccessCount(this, 4);
}

void msw::CChrCoreBasket::LoopStateLayUp()
{
    static float fTestFrame;

    STATE_WORK_LAYUP *pWork = FsmqGetWork<STATE_WORK_LAYUP>();
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    float fStep = m_fTimeScale * m_fFrameStep;

    switch (pWork->m_nStep) {
        case 0: {
            pWork->m_fTopY = pParam->m_fLayUpHeight + GetPos().y;
            const bcs::FVEC3 &rPos = GetPos();
            Util_SetJumpParam(pParam->m_nLayUpJumpFrame, GetPos().y, rPos.y, pWork->m_fTopY);
            int bRight = 1;
            m_nStatusFlag &= ~0x1000;
            pWork->m_fHoldFrame = pParam->m_nLayUpHoldFrame;

            // The side to lay up from: the one the target is on, seen from
            // the middle line (a coin toss right on it).
            if (!(pWork->m_nFlag & 1) && !(pWork->m_nFlag & 2)) {
                float fBase = pWork->m_Target.z > 0.0f ? 0.0f : 3.1415927f;
                float fRot = WrapRadPi(GetRotY_FromPosXZ(pWork->m_Target) - fBase);
                if (fRot > 0.0f) {
                    pWork->m_nFlag &= ~1;
                    pWork->m_nFlag |= 2;
                } else if (fRot < 0.0f) {
                    pWork->m_nFlag |= 1;
                    pWork->m_nFlag &= ~2;
                } else if (bcs::BCS_RandRange(0L, 200L) >= 100) {
                    pWork->m_nFlag &= ~1;
                    pWork->m_nFlag |= 2;
                } else {
                    pWork->m_nFlag |= 1;
                    pWork->m_nFlag &= ~2;
                }
            }
            long nMot;
            if (pWork->m_nFlag & 1) {
                nMot = 0x90;
            } else {
                nMot = 0x8E;
                bRight = 0;
            }
            PlayLayUpEffect((pWork->m_nFlag & 1) != 0);
            if (m_nStatusFlag & 4) {
                // The mirrored motion of the other side: -nOther, written
                // x - 2x as the original computes it (slwi/subf, not neg).
                // match-steering form, no effect on behaviour.
                long nOther = bRight ? 0x8E : 0x90;
                nMot = nOther - nOther * 2;
            }
            float fBlend = 8.0f;
            if (GetChrId() == 10) {
                fBlend = 4.0f;
            }
            AnimPlay(3, nMot, 0, fBlend, 0.0f);
            AnimSetSpeed(3, pParam->m_fLayUpMotSpeed);

            // Run in at an angle to the target.
            float fRot;
            if (bRight == 0) {
                fRot = -(0.017453292f * pParam->m_nLayUpRotDeg);
            } else {
                fRot = 0.017453292f * pParam->m_nLayUpRotDeg;
            }
            pWork->m_fRotY = WrapRad(GetRotY_ToPosXZ(pWork->m_Target) + fRot);
            SetRotY(pWork->m_fRotY);
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY);
            m_fJumpVelZ = 0.0f;
            m_fJumpVelX = 0.0f;
            pWork->m_Vel.x = dir.x * pParam->m_fLayUpSpeed;
            pWork->m_Vel.z = dir.z * pParam->m_fLayUpSpeed;
            pWork->m_Vel.y = 0.0f;
            pWork->m_fShotFrame = AnimSearchTrgFrame(AnimGetMotID(3, 0), 2, -1.0f);
            if (pWork->m_fShotFrame < 0.0f) {
                pWork->m_fShotFrame = 30.0f;
            }
            pWork->m_nStep = 1;
        }
            // fall through
        case 1: {
            if (IsHoldBall() && AnimGetFrame(3) >= pWork->m_fShotFrame) {
                Util_SendMsgToAll(static_cast<CChrDrv::MSG_ID>(7), GetPlayerNo(), 6, 0, 0);
                Util_ShootBall(6, Util_CalcShootRate(6), 1, 0);
            }
            bcs::FVEC3 move;
            ScaleVec(move, pWork->m_Vel, fStep);
            AddPos(move);
            // Hang at the top for a while.
            if (m_nStatusFlag & 0x1000) {
                pWork->m_fHoldFrame -= fStep;
                if (pWork->m_fHoldFrame > 0.0f) {
                    bcs::FVEC3 pos(GetPos().x, pWork->m_fTopY, GetPos().z);
                    SetPos(pos);
                    fTestFrame = 0.0f;
                    break;
                }
                pWork->m_fHoldFrame = 0.0f;
            }
            fTestFrame += m_fFrameStep;
            int nJump = Util_DoJumpMove(fStep);
            if (nJump == 1) {
                m_UnkEC |= 0x10;
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                m_nStatusFlag &= ~0x20;
                long nMot = AnimGetMotID(3, 0) == 0x90 ? 0x91 : 0x8F;
                if (IsMotFlip(this)) {
                    nMot = -nMot;
                }
                AnimPlay(3, nMot, 0, 8.0f, 0.0f);
                AnimSetSpeed(3, 1.0f);
                if (GetChrTypeID() == 17 || GetChrTypeID() == 3) {
                    ArtsOptDestroy();
                }
                PlayLandSe(0);
                SetRiverFallEnable(0);
                bcs::BCS_VecZ_RotY(pWork->m_LandVel, 0.1f, GetRotY());
                pWork->m_nStep = 2;
            } else if (nJump == 2) {
                if (MethdFall()) {
                    FsmqDoChain();
                }
            } else if (GetPos().y <= pWork->m_fTopY && m_fJumpVelY < 0.0f) {
                m_nStatusFlag |= 0x1000;
            }
            break;
        }
        default: {
            pWork->m_Vel.x *= 0.9f;
            pWork->m_Vel.z *= 0.9f;
            bcs::FVEC3 move;
            ScaleVec(move, pWork->m_Vel, fStep);
            AddPos(move);
            Util_DoJumpMove(fStep);
            if (IsAnimEnd(3)) {
                FsmqDoChain();
            }
            break;
        }
    }
}

void msw::CChrCoreBasket::DtorStateLayUp()
{
    SetRiverFallEnable(0);
    m_nBasketFlag &= ~0x8000;
    CallbackMotTrg_AtkOff_(0, 0);
    Eff_Stop(11);
    ArtsOptDestroy();
}

void msw::CChrCore::CallbackMotTrg_AtkOff_(long, long)
{
}
