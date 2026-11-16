// msw::CChrCoreBasket's jump block, swat and jump to the upper floor
// states: the defender's jumps against a shot or a dunk. The jump block
// turns towards the ball (or the other team, for a rebound) and blocks
// with its defence collision for a window of frames; the swat hops at the
// ball; the jump to the upper floor is a spinning jump with an attack
// collision that can turn into an air battle with the character of an
// event. The file also holds CChrCore::IsHoldItem.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/types.h"

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it (see CBallManBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace {

    // bcs::FVEC3 derives from nw4r::math::VEC3 in the original, and the
    // vector sums go through VEC3's operators; here FVEC3 is a separate
    // struct of the same layout, so the operands are reinterpreted.
    typedef const nw4r::math::VEC3 &VEC3_CREF;
    typedef nw4r::math::VEC3 &VEC3_REF;

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

    // (guessed helper) rVec moved fRate of the way to rTo.
    inline void Approach(bcs::FVEC3 &rVec, const bcs::FVEC3 &rTo, float fRate)
    {
        rVec.x += fRate * (rTo.x - rVec.x);
        rVec.y += fRate * (rTo.y - rVec.y);
        rVec.z += fRate * (rTo.z - rVec.z);
    }

    // (guessed helper) A colour variant kept within [0, nMax].
    inline long ClampColor(long nColor, long nMax)
    {
        return nColor > nMax ? nMax : (nColor < 0 ? 0 : nColor);
    }
}

// x kept within [min, max]. A macro: the original computes each bound only
// on the branch that needs it.
#define CLAMP(x, min, max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))

// The jump block's work (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_JUMP_BLOCK {
    long m_nStep;                    // 0x00: 0 up, 1 down, 2 landing, 0x63 done
    long m_nUnk04;                   // 0x04
    // 0x08: 0x40 once falling, 0x200 in a dunk battle, 0x400 a rebound,
    // 0x400000 may jump to the upper floor, 0x800000 swat asked for.
    unsigned long m_nFlag;
    long m_nUnk0C;                   // 0x0C
    float m_fRotY;                   // 0x10: the yaw to turn to (negative: none)
    float m_fFrame;                  // 0x14: frames in the air
    float m_fRotYAsked;              // 0x18: the yaw the driver asked for (negative: none)
    float m_fDunkBattleFrame;        // 0x1C: frames left of the dunk battle
    long m_nUnk20;                   // 0x20
    float m_fFrameAll;               // 0x24: frames since the state began
    unsigned char m_Unk28[0x38 - 0x28];
    float m_fWait;                   // 0x38: frames to wait in the air
    unsigned char m_Unk3C[0x40 - 0x3C];
    float m_fReboundRotY;            // 0x40: the yaw to the other team
    CChrCoreBasket *m_pDunker;       // 0x44: the dunker of the battle
    unsigned long m_hBall;           // 0x48: task id of the ball
    CChrCore::DEF_COLL m_Def;        // 0x4C
};

// The swat's work (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_SWAT_IT {
    long m_nStep;                    // 0x00: 0 in the air, 1 landing
    float m_fRotY;                   // 0x04: the yaw asked for (flag 2)
    // 0x08: bit 0 cleared each frame, bit 1 when a yaw is asked for.
    unsigned long m_nFlag;
};

// ---- Jump block ---------------------------------------------------------

// The jump block. Without an item, a character that may take a rebound
// turns to the middle of the other team; otherwise the jump turns to the
// direction asked for when the ball lies 30 to 135 degrees off it (else
// to the ball). Condition 4 plays its own motion from the current frame.
void msw::CChrCoreBasket::CtorStateJumpBlock()
{
    STATE_WORK_JUMP_BLOCK *pWork = FsmqGetWork<STATE_WORK_JUMP_BLOCK>();
    pWork->m_Def.m_nStep = 0;
    pWork->m_fWait = 0.0f;
    pWork->m_nFlag &= ~8;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(4);
    m_nStatusFlag &= ~0x1F001C00;
    SetMoveMotStep_(0);
    SetMoveMotType(0);
    m_fJumpUnk220 = 0.0f;
    SetCollDefParam_(&pWork->m_Def, 8);
    Eff_Stop(0xA);
    if (CChrManBasket::GetInstance()->IsScoreBookCountChr_(this)) {
        m_nBasketFlag |= 0x8000;
    }

    if (!IsHoldItem() && m_nComLevel != 4) {
        int bRebound = CGame::g_GameWork.GetGameType() == 2 ? 0 : CanbeRebound();
        if (bRebound) {
            long nMot = 8;
            bcs::FVEC3 sum(0.0f, 0.0f, 0.0f);
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                const CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable() && pChr->GetTeamNo() != GetTeamNo()) {
                    sum.x += pChr->GetPos().x - GetPos().x;
                    sum.z += pChr->GetPos().z - GetPos().z;
                }
            }
            bcs::FVEC3 target = reinterpret_cast<VEC3_CREF>(sum) + reinterpret_cast<VEC3_CREF>(GetPos());
            pWork->m_fReboundRotY = GetRotY_ToPosXZ(target);
            if (WrapRadPi(pWork->m_fReboundRotY - GetRotY()) < 0.0f) {
                nMot = -8;
            }
            AnimPlay(3, nMot, 0, 4.0f, 0.0f);
            pWork->m_nFlag |= 0x400;
        } else {
            long nMot = 3;
            if (!(CChrManBasket::GetInstance()->GetFlagAC() & 2)) {
                int bAsked = pWork->m_fRotYAsked >= 0.0f;
                CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
                if (pBall && bAsked) {
                    // (the goal is looked up but not used)
                    if (CGame::g_GameWork.GetGameType() == 2) {
                        GetTargetGoalPos();
                    } else {
                        GetGuardGoalPos();
                    }
                    float fRotY = GetRotY();
                    float fRot = __fabsf(WrapRadPi(GetRotY_ToPosXZ(pBall->GetPos()) - fRotY));
                    if (fRot > 0.5235988f && fRot < 2.3561945f) {
                        pWork->m_fRotY = pWork->m_fRotYAsked;
                    } else {
                        bAsked = 0;
                        pWork->m_fRotY = GetRotY_ToPosXZ(pBall->GetPos());
                    }
                }
                if (bAsked) {
                    nMot = 0x67;
                }
            }
            if (m_pMdl->IsAnmFlipPlay(3, 0)) {
                nMot = -nMot;
            }
            AnimPlay(3, nMot, 0, 4.0f, 0.0f);
            switch (GetChrIdS()) {
                case 9:
                    Eff_CreateSimple(0xA, 0x647, GetPos(), GetRotY(), 3, 0, 0);
                    break;
                case 0xB:
                    Eff_CreateSimple(0xA, 0x65C, GetPos(), GetRotY(), 3, 0, 0);
                    break;
                case 0xF:
                    Eff_CreateSimple(0xA, 0x683, GetPos(), GetRotY(), 3, 0, 0);
                    break;
            }
            WSE_Play(0, 5, 0, -1);
        }
    } else if (m_nComLevel == 4) {
        float fFrame = AnimGetFrame(3);
        int bFlip = m_pMdl->IsAnmFlipPlay(3, 0);
        AnimPlay(3, bFlip ? -0x9A : 0x9A, 1, 0.0f, fFrame);
    } else {
        AnimSetSpeed(3, 1.5f);
    }

    pWork->m_pDunker = 0;
    pWork->m_fDunkBattleFrame = 0.0f;
    SetRiverFallEnable(1);
    m_nStatusFlag |= 0x400;
    PlayVoice(0x20, 0);
}

// Bit 1 of the status flags.
int msw::CChrCore::IsHoldItem() const
{
    return (m_nStatusFlag >> 1) & 1;
}

// Up (step 0), with the yaw turned to and an effect before the character;
// then down under gravity (step 1) until the ground, the landing motion
// (step 2) and the end. A dunk battle runs down with its own frames; the
// driver may turn the jump into a swat or a jump to the upper floor. The
// defence collision is on within the parameter's frames.
void msw::CChrCoreBasket::LoopStateJumpBlock()
{
    static const float fDist = 1.0f;
    STATE_WORK_JUMP_BLOCK *pWork = FsmqGetWork<STATE_WORK_JUMP_BLOCK>();
    pWork->m_fFrameAll += m_fFrameStep;

    float fGroundY;
    CEffectCluster *pEff = Eff_GetCluster(0xA);
    if (pEff) {
        bcs::FVEC3 dir;
        bcs::BCS_VecZ_RotY(dir, fDist, GetRotY());
        bcs::FVEC3 pos(dir.x + GetPos().x, 1.0f + GetPos().y, dir.z + GetPos().z);
        bcs::FVEC3 rot(0.0f, GetCameraToRadRange(0.87266463f), 0.0f);
        pEff->SetTranslate(&pos);
        pEff->SetRotateRad(&rot);
    }

    if ((pWork->m_nFlag & 0x400000) && MethdJumpUpper()) {
        return;
    }
    if (DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x25), 0, 0, 0, 0)) {
        pWork->m_nFlag |= 0x800000;
    }
    if ((pWork->m_nFlag & 0x800000) && MethdSwatIt()) {
        return;
    }

    float fStep = m_fFrameStep;
    if ((pWork->m_nFlag & 0x200) && pWork->m_fDunkBattleFrame > 0.0f && pWork->m_pDunker) {
        if (!pWork->m_pDunker->CheckDunkBattle()) {
            pWork->m_fDunkBattleFrame = 0.0f;
        }
        pWork->m_fDunkBattleFrame -= GetWholeFrameRate();
        if (pWork->m_fDunkBattleFrame < 0.0f) {
            pWork->m_fDunkBattleFrame = 0.0f;
            pWork->m_pDunker->EndDunkBattle();
            pWork->m_pDunker = 0;
        }
    }
    if (pWork->m_fWait > 0.0f) {
        pWork->m_fWait -= fStep;
        if (pWork->m_fWait > 0.0f) {
            return;
        }
        m_fJumpVelZ = 0.0f;
        m_fJumpVelY = 0.0f;
        m_fJumpVelX = 0.0f;
        pWork->m_fWait = 0.0f;
        pWork->m_fDunkBattleFrame = 0.0f;
        m_fJumpUnk224 = 0.0f;
    }

    switch (pWork->m_nStep) {
        case 0:
            m_nStatusFlag |= 0x400;
            if (pWork->m_fRotY >= 0.0f) {
                RotateYRate_(pWork->m_fRotY, 0.2f);
            }
            if (m_fJumpVelY <= 0.0f) {
                if (pWork->m_fDunkBattleFrame > 0.0f || m_fJumpUnk224 > 0.0f) {
                    m_fJumpVelY = 0.0f;
                    m_fJumpUnk224 -= fStep;
                    m_fJumpAccelY = 0.0f;
                } else {
                    pWork->m_nStep = 1;
                    m_fJumpAccelY = -0.025f;
                    m_nStatusFlag = (m_nStatusFlag & ~0x400) | 0x1000;
                    pWork->m_nFlag |= 0x40;
                }
            }
            // fall through
        case 1: {
            // (the ground is at height 0 here)
            fGroundY = 0.0f;
            pWork->m_fFrame += fStep;
            bcs::FVEC3 pos = GetPos();
            float fAccel = m_fJumpAccelY * fStep;
            nw4r::math::VEC3 move = reinterpret_cast<VEC3_CREF>(GetJumpVel()) * fStep;
            m_fJumpVelY += fAccel;
            move.y += 0.5f * (fAccel * fStep);
            reinterpret_cast<VEC3_REF>(pos) += move;
            ClearRevise();
            if ((pWork->m_nFlag & 0x40) && pos.y <= fGroundY && m_fJumpVelY < 0.0f) {
                if (!CanbeLanding() && MethdFall()) {
                    FsmqDoChain();
                    return;
                }
                pos.y = fGroundY;
                m_fJumpAccelY = -0.025f;
                m_fJumpVelY = 0.0f;
                pWork->m_fRotY = 0.0f;
                m_UnkEC |= 0x10;
                m_nStatusFlag = (m_nStatusFlag & ~0x400) | 0x800;
                PlayLandSe(0);
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                SetRiverFallEnable(0);
                pWork->m_nStep = 2;

                float fBlend = 4.0f;
                float fStart = 0.0f;
                long nLoop = 0;
                long nMot;
                switch (AnimGetMotID(3, 0)) {
                    case 0x9A:
                        fBlend = 0.0f;
                        nMot = 0x1E;
                        nLoop = 1;
                        fStart = AnimGetFrame(3);
                        pWork->m_nStep = 0x63;
                        break;
                    case 0x67:
                        nMot = 0x68;
                        break;
                    case 3:
                        nMot = 4;
                        break;
                    case 5:
                        nMot = 6;
                        break;
                    default:
                        nMot = 9;
                        break;
                }
                int bFlip = m_pMdl->IsAnmFlipPlay(3, 0);
                AnimPlay(3, bFlip ? -nMot : nMot, nLoop, fBlend, fStart);
                AnimSetSpeed(3, 1.0f);
            }
            SetPos(pos);
            break;
        }
        case 2:
            if (!IsAnimEnd(3) && !IsAnimLooped(3)) {
                break;
            }
            pWork->m_nStep = 0x63;
            // fall through
        default:
            m_nStatusFlag &= ~0x1F001C00;
            FsmqDoChain();
            break;
    }

    // The defence collision is on from the parameter's first frame to its
    // last (0 to 45 with status bit 0x10000).
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    int bAlt = (m_nStatusFlag >> 16) & 1;
    float fStart = bAlt ? 0 : pParam->m_nJumpBlockDefStart;
    float fEnd = bAlt ? 0x2D : pParam->m_nJumpBlockDefEnd;
    if (fStart <= pWork->m_fFrame && fEnd >= pWork->m_fFrame) {
        SetCollDefParam_(&pWork->m_Def, 8);
    } else {
        pWork->m_Def.m_nStep = 0;
    }
}

void msw::CChrCoreBasket::DtorStateJumpBlock()
{
    SetRiverFallEnable(0);
    m_nBasketFlag &= ~0x8000;
    CollAtkSetEnable_(0, 0);
    Eff_StopForce(0xA);
    ArtsOptDestroy();
    WSE_Stop(0, 0.0f);
    WSE_Stop(1, 0.0f);
}

// ---- Swat ---------------------------------------------------------------

// The swat: a low hop forward with a swipe effect to the side of the
// motion.
void msw::CChrCoreBasket::CtorStateSwatIt()
{
    static const unsigned long uFlg = 8;
    static const float fHeightPlus = 0.3f;

    bcs::FVEC3 vel = GetJumpVel();
    Util_SetJumpParam(25.0f, GetPos().y, 0.0f, GetPos().y + fHeightPlus);
    Eff_Stop(0xB);
    long nMot = m_pMdl->IsAnmFlipPlay(3, 0) ? -0x69 : 0x69;
    long nEff = 0x5F0;
    if (nMot > 0) {
        nEff = 0x5EF;
    }
    Eff_CreateAttach(0xB, nEff, m_nEffNode, 0, uFlg, 3, 0);
    Eff_SetRotateY(0xB, GetRotY());
    if (GetChrId() == 4) {
        const CChrCoreBasketYoshi::YOSHI_COL_SET *pCol =
            CChrCoreBasketYoshi::GetEffectUnit(ClampColor(m_nColor, 3));
        Eff_SetPointLightColor(0xB, pCol->m_Color0, pCol->m_Color1);
    }
    AnimPlay(3, nMot, 0, GetCcParam2_()->m_nSwatItBlend, 0.0f);
    m_fJumpVelZ = 0.0f;
    m_fJumpVelX = 0.0f;
    PlayVoice(0x1E, 0);
    PlaySe(0xC7C, 1, -1, 0.0f, 1.0f, 0.0f);
    SetRiverFallEnable(1);
    if (CChrManBasket::GetInstance()->IsScoreBookCountChr_(this)) {
        m_nBasketFlag |= 0x8000;
    }
}

// In the air the swat turns to the ball (while it is within reach and in
// the first half of the motion, or to the yaw asked for) and moves
// forward; it ends with the landing motion.
void msw::CChrCoreBasket::LoopStateSwatIt()
{
    STATE_WORK_SWAT_IT *pWork = FsmqGetWork<STATE_WORK_SWAT_IT>();
    float fStep = GetWholeFrameRate();
    switch (pWork->m_nStep) {
        case 0: {
            float fRotY = GetRotY();
            float fFrame = AnimGetFrame(3);
            if (fFrame <= 0.5f * AnimGetFrameNum(3)) {
                CBall *pBall = CBallMan::GetBallPtr(0);
                if (pBall) {
                    bcs::FVEC3 dist = reinterpret_cast<VEC3_CREF>(GetPos()) - reinterpret_cast<VEC3_CREF>(pBall->GetPos());
                    float fDist = bcs::BCS_Vec_GetLenXZ(dist);
                    if (!(pWork->m_nFlag & 2) && fDist <= 3.0f) {
                        fRotY = GetRotY_ToPosXZ(pBall->GetPos());
                    }
                }
            }
            if (pWork->m_nFlag & 2) {
                fRotY = pWork->m_fRotY;
            }
            RotateYRate_(fRotY, 0.2f);

            float fSpeed = 0.1f * m_fFrameStep;
            bcs::FVEC3 move;
            bcs::BCS_VecZ_RotY(move, 1.0f, GetRotY());
            move.x *= fSpeed;
            move.z *= fSpeed;
            AddPos(move);

            long nRet = Util_DoJumpMove(fStep);
            if (nRet == 1) {
                m_UnkEC |= 0x10;
                m_nStatusFlag = (m_nStatusFlag & ~0x400) | 0x800;
                int bFlip = m_pMdl->IsAnmFlipPlay(3, 0);
                AnimPlay(3, bFlip ? -4 : 4, 0, 4.0f, 0.0f);
                PlayLandSe(0);
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                SetRiverFallEnable(0);
                pWork->m_nStep = 1;
            } else if (nRet == 2 && MethdFall()) {
                FsmqDoChain();
                return;
            }
            break;
        }
        case 1:
            Util_DoJumpMove(fStep);
            if (IsAnimEnd(3)) {
                m_nStatusFlag &= ~0x1C00;
                FsmqDoChain();
            }
            break;
    }
    pWork->m_nFlag &= ~1;
}

void msw::CChrCoreBasket::DtorStateSwatIt()
{
    SetRiverFallEnable(0);
    ArtsOptDestroy();
    CollAtkSetEnable_(0, 0);
    Eff_Stop(0xA);
    m_nBasketFlag &= ~0x8000;
}

// ---- Jump to the upper floor --------------------------------------------

// The jump to the upper floor: both attack collisions (1 and 2) get the
// jump's size and the down frames and power of attack kind 4.
void msw::CChrCoreBasket::CtorStateJumpUpper()
{
    STATE_WORK_JUMP_UPPER *pWork = FsmqGetWork<STATE_WORK_JUMP_UPPER>();
    pWork->m_nStep = 0;
    pWork->m_nPlayerBit = 1 << m_nPlayerNo;
    pWork->m_fFrame = 0.0f;
    pWork->m_fCount = 0.0f;
    pWork->m_nFlag = 0;
    pWork->m_fFrameFrac = 0.0f;
    SetMoveMotStep_(0);
    SetMoveMotType(0);
    m_fJumpUnk220 = 0.0f;
    WSE_Play(0, 7, 1, -1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    PlayVoice(0x1E, 0);
    if (CChrManBasket::GetInstance()->IsScoreBookCountChr_(this)) {
        m_nBasketFlag |= 0x8000;
    }
    SetRiverFallEnable(1);

    const long anColl[2] = { 1, 2 };
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    long nType = pParam->m_nJumpUpperCollType != 0;
    float fRadius = GetJumpCutColR();
    float fHeight = pParam->m_fJumpUpperCollHeight;
    for (long i = 0; i < 2; i++) {
        CCollAtkUnit *pAtk = CollAtkGetColl_(anColl[i]);
        pAtk->GetCollUnit()->SetCollType(nType);
        pAtk->GetCollUnit()->SetSize(fRadius, fHeight, 0.0f);
        pAtk->m_fDownFrame = GetAtkDownFrame(4);
        pAtk->m_fVanishPower = GetAtkVanishPower(4);
    }
}

// The spinning jump (steps 1 to 3, with a second turn for characters that
// have one), the fall (step 4), which turns back part of the way, and the
// landing (step 5). While the event's air battle runs (flag 0x200000)
// the character hangs on to the event character instead, slowing its
// fall with the driver's input. The motions follow the frames left.
void msw::CChrCoreBasket::LoopStateJumpUpper()
{
    STATE_WORK_JUMP_UPPER *pWork = FsmqGetWork<STATE_WORK_JUMP_UPPER>();
    float fStep = m_fFrameStep;
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    CHR_CORE_BASKET::JUMP_BLOCK_PARAM block;
    GetJumpBlockParam_(1, block);

    switch (pWork->m_nStep) {
        case 0:
            pWork->m_nStep++;
            // fall through
        case 1: {
            m_nStatusFlag = (m_nStatusFlag & ~0x1C00) | 0x400;
            float fHold = pParam->m_nJumpUpperHoldFrame;
            pWork->m_fFrameNum = pParam->m_nJumpUpperJumpFrame + fHold;
            pWork->m_fFrameNum = pWork->m_fFrameNum + pParam->m_nJumpUpperFallFrame;
            pWork->m_fHoldFrame = fHold + pParam->m_nJumpUpperJumpFrame;
            pWork->m_fRotY = DegToRad(pParam->m_nJumpUpperRotDeg);
            if (m_pMdl->IsAnmFlipPlay(3, 0)) {
                pWork->m_fRotY = -pWork->m_fRotY;
            }

            long nEff = -1;
            int bLeft = pWork->m_fRotY > 0.0f;
            switch (GetChrIdS()) {
                case 0:
                    nEff = 0x5FA;
                    if (bLeft) {
                        nEff = 0x5F9;
                    }
                    break;
                case 2:
                    nEff = 0x60D;
                    if (bLeft) {
                        nEff = 0x60C;
                    }
                    break;
                case 3:
                    nEff = 0x617;
                    if (bLeft) {
                        nEff = 0x616;
                    }
                    break;
                case 4:
                    nEff = 0x621;
                    if (bLeft) {
                        nEff = 0x620;
                    }
                    break;
                case 5:
                    nEff = 0x628;
                    if (bLeft) {
                        nEff = 0x627;
                    }
                    break;
                case 6:
                    nEff = 0x631;
                    if (bLeft) {
                        nEff = 0x630;
                    }
                    break;
                case 7:
                    nEff = 0x63C;
                    if (bLeft) {
                        nEff = 0x63B;
                    }
                    break;
                case 9:
                    nEff = 0x64B;
                    if (bLeft) {
                        nEff = 0x64A;
                    }
                    break;
                case 0xA:
                    nEff = 0x654;
                    if (bLeft) {
                        nEff = 0x653;
                    }
                    break;
                case 0xB:
                    nEff = 0x661;
                    if (bLeft) {
                        nEff = 0x660;
                    }
                    break;
                case 0xD:
                    nEff = 0x66D;
                    if (bLeft) {
                        nEff = 0x66C;
                    }
                    break;
                case 0xE:
                    nEff = 0x67B;
                    if (bLeft) {
                        nEff = 0x67A;
                    }
                    break;
                case 0xF:
                    nEff = 0x687;
                    if (bLeft) {
                        nEff = 0x686;
                    }
                    break;
                case 0x13:
                case 0x15:
                case 0x17:
                    nEff = 0x6A0;
                    if (bLeft) {
                        nEff = 0x69F;
                    }
                    break;
                case 0x14:
                case 0x16:
                case 0x18:
                    nEff = 0x6A9;
                    if (bLeft) {
                        nEff = 0x6A8;
                    }
                    break;
            }
            if (nEff > 0) {
                unsigned short nNode = m_nEffNode;
                if (GetChrIdS() == 0xD) {
                    nNode = m_nEffNodeSpike;
                }
                Eff_Stop(0xB);
                Eff_CreateAttach(0xB, nEff, nNode, 0, 0, 3, 0);
            }

            // Up to the jump block's height plus the parameter's, with the
            // top half-way.
            float fTop = pParam->m_fJumpUpperHeight + block.m_fHeight;
            Util_SetJumpParam(pParam->m_nJumpUpperJumpFrame, GetPos().y, fTop,
                              GetPos().y + (block.m_fHeight - GetPos().y) * 0.5f);
            pWork->m_nStep++;
        }
            // fall through
        case 2:
        case 3: {
            if (!(pWork->m_nFlag & 0x200000)) {
                Util_DoJumpMove(fStep);
                if (m_fJumpVelY < 0.0f) {
                    m_fJumpVelY = 0.0f;
                    SetPosY(pParam->m_fJumpUpperHeight + block.m_fHeight);
                    m_nStatusFlag |= 0x400;
                }
            }
            if (!(pWork->m_nFlag & 0x200000)) {
                float fTurn = fStep * pWork->m_fRotY;
                SetRotY(WrapRad(fTurn + GetRotY()));
            }
            if (pWork->m_nFlag & 0x200000) {
                // The air battle with the character of the event.
                CChrCoreBasket *pEvent =
                    (CChrManBasket::GetInstance()->GetFlagAC() >> 1) & 1
                        ? static_cast<CChrCoreBasket *>(CChrMan::GetChrCore(CGame::g_nEventChrIdx))
                        : 0;
                if (!pEvent) {
                    ResultAirTrickBattle(1);
                    FsmqDoChain();
                    return;
                }
                switch (pWork->m_nBattleStep) {
                    case 0: {
                        bcs::FVEC3 dist = reinterpret_cast<VEC3_CREF>(pEvent->GetTargetGoalPos()) -
                                          reinterpret_cast<VEC3_CREF>(GetPos());
                        float fDist = bcs::BCS_Vec_GetLenXZ(dist);
                        if (fDist < GetPush1Radius()) {
                            CChrCore *pHolder = CChrMan::Util_SearchHaveBallChr(0);
                            if (pHolder) {
                                pWork->m_fBattleFrame = static_cast<CChrCoreBasket *>(pHolder)->GetStaddleLastTime_();
                            } else {
                                pWork->m_fBattleFrame = 0.0f;
                            }
                            pEvent->MethdAirTrickDownSpeed(-1.0f);
                            pWork->m_nBattleStep = 1;
                        }
                    }
                        // fall through
                    case 1: {
                        if (AnimGetMotID(3, 0) == 0x6A && IsAnimEnd(3)) {
                            AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x6B : 0x6B, 1, 4.0f, 0.0f);
                            AnimSetSpeed(3, 1.0f);
                        }
                        if (GetChrIdS() == 1 || GetChrIdS() == 8) {
                            RotateYRate_(GetRotY_ToPosXZ(pEvent->GetPos()), 0.2f);
                        } else {
                            float fTurn = fStep * pWork->m_fRotY;
                            SetRotY(WrapRad(fTurn + GetRotY()));
                        }

                        bcs::FVEC3 dist = reinterpret_cast<VEC3_CREF>(pEvent->GetPos()) -
                                          reinterpret_cast<VEC3_CREF>(GetPos());
                        float fDist = bcs::BCS_Vec_GetLenXZ(dist);
                        bcs::FVEC3 ofs(0.0f, pEvent->GetPos().y - GetPos().y, fDist);
                        float fRate = GetWholeFrameRate();
                        Approach(m_BattleOfs, ofs, 0.1f);

                        // The driver's input slows the event character's
                        // fall while someone holds the ball.
                        long nInput = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x26), 0, 0, 0, 0);
                        if (nInput && CChrMan::Util_SearchHaveBallChr(0)) {
                            float fDown = nInput * 0.0009765625f;
                            if (fDown >= 0.4f) {
                                fDown = 0.4f;
                            }
                            pEvent->MethdAirTrickDownSpeed(-(fRate * (fDown * GetStaddleInputDownRate_())));
                        }
                        pWork->m_fBattleFrame -= fRate;
                        if (pWork->m_fBattleFrame <= 0.0f) {
                            pEvent->ResultAirTrickBattle(1);
                        }
                        return;
                    }
                }
            }

            pWork->m_fHoldFrame -= fStep;
            if (pWork->m_fHoldFrame <= 0.0f) {
                if (pWork->m_nStep == 2 && pParam->m_nJumpUpperTurnFrame > 0) {
                    // The second turn.
                    pWork->m_fHoldFrame = pParam->m_nJumpUpperTurnFrame;
                    pWork->m_nFlag |= 0x30000;
                    if (pParam->m_nJumpUpperTurnRate <= 0) {
                        // Facing away from the camera.
                        pWork->m_fRotY = 0.0f;
                        SetRotY(3.1415927f + m_fCamRotY);
                        if (GetChrId() == 0xD) {
                            Eff_Stop(0xB);
                        }
                    }
                    pWork->m_nStep++;
                } else {
                    m_nStatusFlag = (m_nStatusFlag & ~0x1C00) | 0x1000;
                    float fTop = GetPos().y * 0.5f;
                    Util_SetJumpParam(pParam->m_nJumpUpperFallFrame, GetPos().y, 0.0f, fTop);
                    pWork->m_nStep = 4;
                }
            }
            break;
        }
        case 4: {
            pWork->m_fHoldFrame += fStep;
            if (pWork->m_nFlag & 0x10000) {
                // The second turn slows down by the parameter's rate.
                pWork->m_fRotY *= pParam->m_nJumpUpperTurnRate / 100.0f;
                float fTurn = pWork->m_fRotY * fStep;
                SetRotY(WrapRad(fTurn + GetRotY()));
            } else if (pWork->m_fRotY > 0.0f) {
                float fFall =
                    CLAMP(pWork->m_fFrameNum - pWork->m_fFrame, 0.0f, (float)pParam->m_nJumpUpperFallFrame);
                float fRate = fFall / pParam->m_nJumpUpperFallFrame;
                float fBack = DegToRad(pParam->m_nJumpUpperRotDeg - pParam->m_nJumpUpperRotBackDeg);
                float fTurn = fStep * (pWork->m_fRotY - fBack * (1.0f - fRate));
                SetRotY(WrapRad(fTurn + GetRotY()));
            }
            long nRet = Util_DoJumpMove(fStep);
            if (nRet == 1) {
                int bFlip = m_pMdl->IsAnmFlipPlay(3, 0);
                AnimPlay(3, bFlip ? -0x6D : 0x6D, 0, 4.0f, 0.0f);
                AnimSetSpeed(3, 1.0f);
                m_nStatusFlag = (m_nStatusFlag & ~0x1C00) | 0x800;
                PlayLandSe(0);
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                SetRiverFallEnable(0);
                pWork->m_nStep++;
            } else if (nRet == 2 && MethdFall()) {
                FsmqDoChain();
                return;
            }
            break;
        }
        case 5:
            Util_DoJumpMove(fStep);
            if (IsAnimEnd(3)) {
                m_nStatusFlag &= ~0x1C00;
                FsmqDoChain();
                return;
            }
            break;
    }

    switch (AnimGetMotID(3, 0)) {
        case 0x6C:
        case 0x6D:
            break;
        case 0x6A: {
            if (!IsAnimEnd(3)) {
                break;
            }
            int bFlip = m_pMdl->IsAnmFlipPlay(3, 0);
            AnimPlay(3, bFlip ? -0x6B : 0x6B, 1, 4.0f, 0.0f);
            AnimSetSpeed(3, 1.0f);
        }
            // fall through
        case 0x6B: {
            // The last motion starts when the frames left fit it.
            float fRest = pWork->m_fFrameNum - pWork->m_fFrame;
            float fEnd = AnimGetResFrameNum(0x6C);
            if (fRest <= fEnd) {
                int bFlip = m_pMdl->IsAnmFlipPlay(3, 0);
                AnimPlay(3, bFlip ? -0x6C : 0x6C, 0, 4.0f, 0.0f);
                float fSpeed = fEnd / fRest;
                if (fSpeed < 0.0f) {
                    fSpeed = 0.1f;
                }
                AnimSetSpeed(3, fSpeed);
            }
            break;
        }
        default: {
            if (GetChrId() == 5) {
                float fRotY = WrapRad(3.1415927f + GetRotY());
                Util_NodePatchRotY(fRotY);
                SetRotY(fRotY);
            }
            int bFlip = m_pMdl->IsAnmFlipPlay(3, 0);
            AnimPlay(3, bFlip ? -0x6A : 0x6A, 0, 4.0f, 0.0f);
            float fStart = AnimGetResFrameNum(0x6A);
            if (pWork->m_fFrameNum >= fStart + AnimGetResFrameNum(0x6C)) {
                AnimSetSpeed(3, 1.0f);
            } else {
                float fSpeed = fStart / (0.5f * pWork->m_fFrameNum);
                if (fSpeed < 0.0f) {
                    fSpeed = 0.1f;
                }
                AnimSetSpeed(3, fSpeed);
            }
            break;
        }
    }

    // Whole frames: inside the parameter's window, flag 0x1000 is set at
    // the first, and every m_nJumpUpperSwapCount frames after it flags
    // 0x4000 and 0x8000 swap; outside both are cleared.
    float fWinStart = pParam->m_nJumpUpperWinStart;
    float fWinEnd = pParam->m_nJumpUpperWinEnd;
    pWork->m_fFrameFrac += fStep;
    while (pWork->m_fFrameFrac >= 1.0f) {
        pWork->m_fFrameFrac -= 1.0f;
        pWork->m_fFrame += 1.0f;
        if (pWork->m_fFrame >= fWinStart && pWork->m_fFrame <= fWinEnd) {
            if (!(pWork->m_nFlag & 0x1000)) {
                pWork->m_nFlag |= 0x5000;
            } else {
                if ((pWork->m_fCount += 1.0f) >= pParam->m_nJumpUpperSwapCount) {
                    if (pWork->m_nFlag & 0x4000) {
                        pWork->m_nFlag = (pWork->m_nFlag & ~0x4000) | 0x8000;
                    } else {
                        pWork->m_nFlag = (pWork->m_nFlag | 0x4000) & ~0x8000;
                    }
                    pWork->m_fCount = 0.0f;
                }
            }
        } else {
            pWork->m_nFlag &= ~0xC000;
        }
    }
}

void msw::CChrCoreBasket::DtorStateJumpUpper()
{
    WSE_Stop(0, 0.0f);
    SetRiverFallEnable(0);
    ArtsOptDestroy();
    Eff_StopForce(0xB);
    m_nBasketFlag &= ~0x8000;
    Eff_Stop(0xD);
    SE_Stop(7, 0.0f);
    m_nStatusFlag &= ~0x2000;
    m_nStatusFlag &= ~0x20000000;
    SetDetach_();
    SetEndure(0);
}
