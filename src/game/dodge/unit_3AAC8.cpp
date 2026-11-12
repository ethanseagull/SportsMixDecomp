// msw::CChrCoreDodge's damage state (with the motion of each kind of
// damage), its dizzy state, the spin stagger state, the fall into the river
// of the river court and the rebirth after it, the paralysis effect and the
// sliding state. The same states as BasketF's game/basket/unit_31D0C.cpp,
// written again from the DodgeF code: the dodgeball versions differ in most
// details (lives, the party game, the pinball stage).
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallDodge.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/DodgePrm.hpp"
#include "nw4r/math/triangular.h"

// (names guessed) Damage times in frames: how long a damage keeps the
// character invincible, the blow off of damage kind 0x15 and the blow off
// of the party game's last life. The original reads them from memory at the
// start of .rodata: one-element arrays (a scalar const would be folded into
// a literal).
static const float s_fDmgInvincibleTime[1] = { 30.0f };
static const float s_fBlowOffTime15[1] = { 20.0f };
static const float s_fBlowOffTime[1] = { 200.0f };

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

// The damage state's work (CChrCore's; names guessed).
struct msw::CChrCore::STATE_WORK_DAMAGE {
    long m_nStep;             // 0x00
    unsigned long m_nSubFlag; // 0x04: bit 0 once the landing effect is out
    // 0x08: the damage flags; the low byte is the kind of damage (see
    // CtorStateDamage), bits 8-15 a sub kind, bit 0x200000 a hit by an
    // arts shot.
    unsigned long m_nFlag;
    long m_nType;             // 0x0C: the kind of damage, from m_nFlag
    int m_bAir;               // 0x10: hit in the air
    float m_fRotY;            // 0x14: the direction the hit came from
    int m_bBack;              // 0x18: hit from behind (selects the motion)
    float m_fSpeed;           // 0x1C
    float m_fTime;            // 0x20: frames of the spin (kind 5), or left to lie down
    float m_fDownTime;        // 0x24: frames to stay down
};

namespace {

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

    // (guessed helper) An angle brought into [-pi, pi].
    inline float WrapRadPi(float rad)
    {
        while (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        while (rad < -3.1415927f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    inline float Min(float a, float b)
    {
        return a > b ? b : a;
    }

    // (guessed) x brought into [lo, hi]; a macro: the original reads x
    // again for the second test.
#define CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))

    inline float Clamp(float x, float fMin, float fMax)
    {
        return x > fMax ? fMax : (x < fMin ? fMin : x);
    }

    // (guessed helper) The sub kinds of damage that leave no down effect
    // and make the character invincible to balls.
    inline int IsNoDownEffect(long nSub)
    {
        return nSub == 14 || nSub == 27 || nSub == 37 || nSub == 38;
    }

    // (guessed helper) fBase plus a random part of fRange.
    inline float RandAdd(float fBase, float fRange)
    {
        float fRand = bcs::BCS_RandMax(1.0f);
        return fBase + fRand * fRange;
    }

    // The dizzy state's work (names guessed).
    struct STATE_WORK_DIZZY {
        long m_nStep;          // 0x00
        unsigned long m_nFlag; // 0x04: bits 8-15 the kind of dizziness
        float m_fTime;         // 0x08: frames left
        float m_fRotY;         // 0x0C: the direction of the hit back
        float m_fFrame;        // 0x10: frames since the start
        unsigned char m_Unk14[0x3C - 0x14];
        // 0x3C: the hit back of the pinball (kind 12).
        msw::CC_DATA::CHR_PARAM::MOVE_PARAM m_Move;
        msw::CChrCore::DIR_MOVE m_DirMove; // 0x4C
    };

    // The fall state's work (names guessed).
    struct STATE_WORK_FALL {
        int bSplash; // 0x00: once the character is under water
    };

    // The rebirth state's work (names guessed).
    struct STATE_WORK_REBIRTH {
        float m_fTime; // 0x00: frames left (negative: none)
    };

    // The sliding state's work (names guessed).
    struct STATE_WORK_SLIDING {
        long m_nStep;       // 0x00
        unsigned char m_Unk04[0x08 - 0x04];
        float m_fRotY;      // 0x08: the direction of the slide
        float m_fRotYTrg;   // 0x0C: the direction the stick asks for
        float m_fSpeedRate; // 0x10
        int m_bAccel;       // 0x14: the stick is held this frame
    };

    // The state the dizzy state is (the table's entry).
    const long STATE_DIZZY = 16;
}

int msw::CChrCoreDodge::Util_GetDamageMotID(long nType, long nBack, long nPart)
{
    // The damage motions by kind of damage: for a hit from the front and
    // from behind, the start, the fall, the getting up and the lying down.
    static const short _ciDmgMotID[17][2][4] = {
        { { 85, 86, 88, 87 }, { 93, 94, 96, 95 } },
        { { 89, 90, 91, 92 }, { 97, 98, 99, 100 } },
        { { 108, 108, 108, 0 }, { 108, 108, 108, 0 } },
        { { 101, 102, 88, 0 }, { 101, 102, 88, 0 } },
        { { 110, 111, 112, 111 }, { 110, 111, 112, 111 } },
        { { 104, 104, 104, 0 }, { 104, 104, 104, 0 } },
    };

    switch (nType) {
        case 1:
            return _ciDmgMotID[1][nBack][nPart];
        case 2:
            return _ciDmgMotID[2][nBack][nPart];
        case 3:
            return _ciDmgMotID[3][nBack][nPart];
        case 4:
            return _ciDmgMotID[4][nBack][nPart];
        case 5:
            return _ciDmgMotID[5][nBack][nPart];
        default:
            return _ciDmgMotID[0][nBack][nPart];
    }
}

void msw::CChrCoreDodge::CtorStateDamage()
{
    STATE_WORK_DAMAGE *pWork = FsmqGetWork<STATE_WORK_DAMAGE>();

    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(0);
    long nType = (unsigned char)pWork->m_nFlag;
    pWork->m_nType = nType;
    pWork->m_nStep = 0;

    switch (nType) {
        case 0:
            SubStateDamageCtorMDmg_(pWork);
            break;
        case 1:
            SubStateDamageCtorLDmg_(pWork);
            break;
        case 2:
            SubStateDamageCtorDizzy_(pWork);
            break;
        case 4:
            // Tumbling.
            SetInvincible(s_fDmgInvincibleTime[0]);
            m_UnkEC &= ~0x10;
            SetCollBodyPushPower_(5);
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            SetCollBodyPushRank_(4);
            PlayVoice(0x92, 0);
            pWork->m_bBack = 1;
            AnimPlay(3, Util_GetDamageMotID(4, 1, 0), 0, 0.0f, 0.0f);
            if ((unsigned char)(pWork->m_nFlag >> 8) == 13) {
                PlaySePos(0xC56, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            }
            m_fJumpVelX = 0.0f;
            m_fJumpVelY = 0.0f;
            m_fJumpVelZ = 0.0f;
            m_fJumpAccelY = -0.025f;
            break;
        case 3: {
            // Burnt: thrown up.
            m_nStatusFlag |= 0x40000000;
            m_UnkEC &= ~0x10;
            SetCollBodyPushPower_(1);
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            SetCollBodyPushRank_(4);
            PlayVoice(0x8E, 0);
            AnimPlay(3, Util_GetDamageMotID(3, 0, 0), 1, 0.0f, 0.0f);
            float fY = GetPosY();
            Util_SetJumpParam(48.0f, fY, fY, 2.5f + fY);
            m_nStatusFlag |= 0x400;
            SetCondition(4, pWork->m_fDownTime, 0);
            break;
        }
        case 5:
            // Spun round and thrown up.
            m_nStatusFlag |= 0x40000000;
            m_UnkEC &= ~0x10;
            SetCollBodyPushPower_(1);
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            SetCollBodyPushRank_(4);
            PlayVoice(0x91, 0);
            AnimPlay(3, Util_GetDamageMotID(5, 0, 0), 0, 0.0f, 0.0f);
            Util_SetJumpParam(30.0f, 0.0f, 4.0f);
            m_fJumpVelX = 0.05f * nw4r::math::SinRad(pWork->m_fRotY);
            m_fJumpVelZ = 0.05f * nw4r::math::CosRad(pWork->m_fRotY);
            m_nStatusFlag |= 0x400;
            break;
        case 6:
            SubStateDamageCtorMDmg_(pWork);
            break;
    }
}

void msw::CChrCoreDodge::LoopStateDamage()
{
    STATE_WORK_DAMAGE *pWork = FsmqGetWork<STATE_WORK_DAMAGE>();

    int bEnd;
    switch (pWork->m_nType) {
        case 0:
            bEnd = SubStateDamageLoopMDmg_(pWork);
            break;
        case 1:
            bEnd = SubStateDamageLoopLDmg_(pWork);
            break;
        case 2:
            bEnd = SubStateDamageLoopDizzy_(pWork);
            break;
        case 3:
            // Burnt: thrown up, then the landing motion.
            SetInvincible(s_fDmgInvincibleTime[0]);
            if (pWork->m_nStep == 0) {
                if (Util_DoJumpMove(m_fTimeScale * m_fFrameStep)) {
                    long nMot = Util_GetDamageMotID(3, pWork->m_bBack, 1);
                    m_fJumpUnk220 = 9999.9f;
                    AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                    Util_SetEffect(static_cast<SET_EFFECT_TYPE>(1));
                    m_UnkEC |= 0x10;
                    SetCollBodyPushRank_(1);
                    m_nStatusFlag &= ~0xC00;
                    pWork->m_nStep++;
                }
            } else if (IsAnimEnd(3)) {
                bEnd = 1;
                break;
            }
            bEnd = 0;
            break;
        case 4:
            bEnd = SubStateDamageLoopTumble_(pWork);
            break;
        case 5:
            // Spinning until the time runs out on the ground.
            if (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                m_fJumpVelX *= 0.5f;
                m_fJumpVelZ *= 0.5f;
            }
            if (pWork->m_fTime > 0.0f) {
                float fRot = GetRotY();
                float fTime = pWork->m_fTime;
                if (fTime > 32.0f) {
                    fRot -= 0.7853982f;
                } else {
                    fRot -= 0.7853982f * nw4r::math::SinRad(1.5707964f * (0.03125f * fTime));
                }
                SetRotY(WrapRad(fRot));
                if (GetPosY() <= 0.0f) {
                    pWork->m_fTime -= m_fFrameStep;
                }
            } else {
                bEnd = 1;
                break;
            }
            bEnd = 0;
            break;
        case 6:
            bEnd = SubStateDamageLoopMDmg_(pWork);
            break;
        default:
            bEnd = 1;
            break;
    }

    if (bEnd) {
        m_nStatusFlag &= ~0xC00;
        m_UnkEC &= ~0x20;
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreDodge::DtorStateDamage()
{
    m_nStatusFlag &= ~0x40000000;
    m_UnkEC &= ~0x4;
    Eff_Stop(0x10);
    Eff_Stop(0x10);
    SetSmashDmg(0);
}

void msw::CChrCoreDodge::SubStateDamageCtorMDmg_(STATE_WORK_DAMAGE *pWork)
{
    int nSub = (unsigned char)(pWork->m_nFlag >> 8);
    if (pWork->m_nType != 6 && nSub != 0x15) {
        m_nStatusFlag |= 0x40000000;
    }
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(7), 8.0f);
    SetCollBodyPushRank_(4);
    PlayVoice(0x8A, 0);

    float fRot = WrapRad(GetRotY() - pWork->m_fRotY);
    if (fRot >= 1.5707964f && fRot <= 4.712389f) {
        SetRotY(WrapRad(3.1415927f + pWork->m_fRotY));
        pWork->m_bBack = 1;
    } else {
        SetRotY(WrapRad(pWork->m_fRotY));
        pWork->m_bBack = 0;
    }
    AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 0), 0, 0.0f, 0.0f);

    int bBlowOff = 0;
    m_bDmgBlowOff = 0;
    float fSpeed = 0.075f;
    if (CGame::g_GameWork.GetGameType() == 2 && GetLife() <= 0.0f) {
        // The last life of the party game: blown away.
        float fFrame = s_fBlowOffTime[0];
        if (1.0f > fFrame) {
            fFrame = 1.0f;
        }
        bBlowOff = 1;
        m_bDmgBlowOff = 1;
        float fRotY = WrapRad(3.1415927f + m_fCamRotY);
        pWork->m_fRotY = fRotY;
        SetRotY(fRotY);
        pWork->m_bBack = 0;
        AnimPlay(3, Util_GetDamageMotID(0, 0, 0), 0, 0.0f, 0.0f);
        fSpeed = 60.0f / fFrame;
        if (pWork->m_bBack) {
            fSpeed *= -1.0f;
        }
        bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
        float fVel, fGravity;
        bcs::BCS_Phys_CalcYVG(fFrame, 0.0f, 0.0f, 12.0f, fVel, fGravity);
        m_fJumpVelY = fVel;
        m_fJumpAccelY = fGravity;
        Eff_CreateAttach(0x10, 0x488, m_nEffNodeSpike, 0, 0, 3, 0);
        PlaySePos(0xDE3, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    } else if (nSub == 0x15) {
        float fFrame = s_fBlowOffTime15[0];
        if (1.0f > fFrame) {
            fFrame = 1.0f;
        }
        fSpeed = 3.3f / fFrame;
        if (pWork->m_bBack) {
            fSpeed *= -1.0f;
        }
        bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
        float fVel, fGravity;
        bcs::BCS_Phys_CalcYVG(fFrame, 0.0f, 0.0f, 1.2f, fVel, fGravity);
        m_fJumpVelY = fVel;
        m_fJumpAccelY = fGravity;
    } else {
        if (pWork->m_bBack) {
            fSpeed *= -1.0f;
        }
        bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
        m_fJumpVelY = 0.2f;
        m_fJumpAccelY = -0.025f;
    }
    if (!IsNoDownEffect((unsigned char)(pWork->m_nFlag >> 8)) && !bBlowOff) {
        Eff_CreateAttach(0x10, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
    }
    SetSmashDmg(1);
}

int msw::CChrCoreDodge::SubStateDamageLoopMDmg_(STATE_WORK_DAMAGE *pWork)
{
    int nSub = (unsigned char)(pWork->m_nFlag >> 8);
    if (pWork->m_nType != 6 && nSub != 0x15) {
        SetInvincible(s_fDmgInvincibleTime[0]);
    }
    if (IsNoDownEffect((unsigned char)(pWork->m_nFlag >> 8))) {
        SetInvincibleVsBall(s_fDmgInvincibleTime[0]);
    }
    switch (pWork->m_nStep) {
        case 0:
            switch (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                case 1: {
                    long nMot = Util_GetDamageMotID(0, pWork->m_bBack, 1);
                    m_fJumpUnk220 = 9999.9f;
                    Util_SetEffect(static_cast<SET_EFFECT_TYPE>(1));
                    AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                    m_UnkEC |= 0x10;
                    SetCollBodyPushPower_(5);
                    SetCollBodyPushRank_(1);
                    pWork->m_nStep++;
                    SetSmashDmg(0);
                    break;
                }
                case 2:
                    if (MethdFall()) {
                        // Fell into the river.
                        return 1;
                    }
                    break;
            }
            if (m_bDmgBlowOff && m_fJumpVelY < 0.0f) {
                pWork->m_nStep = 4;
                Eff_Stop(0x10);
                AnimSetSpeed(3, 0.0f);
            }
            break;
        case 1:
            m_fJumpVelX *= 0.875f;
            m_fJumpVelZ *= 0.875f;
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                m_nStatusFlag &= ~0xC00;
                pWork->m_fTime = pWork->m_fDownTime;
                AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 3), 1, 0.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (!IsDodgeInfield() || CGameDodge::GetChrRestLife(m_nPlayerNo) > 0) {
                if (pWork->m_fTime > 0.0f) {
                    pWork->m_fTime -= m_fFrameStep;
                } else {
                    AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 2), 0, 4.0f, 0.0f);
                    pWork->m_nStep++;
                }
            }
            break;
        default:
            if (IsAnimEnd(3)) {
                return 1;
            }
            break;
        case 4:
            // Blown away: the motion stays frozen.
            Eff_Stop(0x10);
            break;
    }
    return 0;
}

void msw::CChrCoreDodge::SubStateDamageCtorLDmg_(STATE_WORK_DAMAGE *pWork)
{
    if (pWork->m_nType != 6) {
        m_nStatusFlag |= 0x40000000;
    }
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(7), 8.0f);
    SetCollBodyPushRank_(4);
    PlayVoice(0x8B, 0);

    float fRot = WrapRad(GetRotY() - pWork->m_fRotY);
    if (fRot >= 1.5707964f && fRot <= 4.712389f) {
        SetRotY(WrapRad(3.1415927f + pWork->m_fRotY));
        pWork->m_bBack = 1;
    } else {
        SetRotY(WrapRad(pWork->m_fRotY));
        pWork->m_bBack = 0;
    }
    AnimPlay(3, Util_GetDamageMotID(1, pWork->m_bBack, 0), 0, 0.0f, 0.0f);

    float fSpeed = 0.15f;
    if (pWork->m_bBack) {
        fSpeed *= -1.0f;
    }
    bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
    m_fJumpVelY = 0.3f;
    m_fJumpAccelY = -0.025f;
    if (!IsNoDownEffect((unsigned char)(pWork->m_nFlag >> 8)) && !(pWork->m_nFlag & 0x200000)) {
        Eff_CreateAttach(0x10, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
    }
    SetSmashDmg(1);
}

int msw::CChrCoreDodge::SubStateDamageLoopLDmg_(STATE_WORK_DAMAGE *pWork)
{
    if (pWork->m_nType != 6) {
        SetInvincible(s_fDmgInvincibleTime[0]);
    }
    if (pWork->m_nFlag & 0x200000) {
        SetInvincibleVsBall(s_fDmgInvincibleTime[0]);
    }
    if (IsNoDownEffect((unsigned char)(pWork->m_nFlag >> 8))) {
        SetInvincibleVsBall(s_fDmgInvincibleTime[0]);
    }
    switch (pWork->m_nStep) {
        case 0:
            switch (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                case 1: {
                    long nMot = Util_GetDamageMotID(1, pWork->m_bBack, 1);
                    m_fJumpUnk220 = 9999.9f;
                    AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                    SetCollBodyPushPower_(5);
                    SetCollBodyPushRank_(1);
                    m_UnkEC |= 0x10;
                    pWork->m_nStep++;
                    SetSmashDmg(0);
                    break;
                }
                case 2:
                    if (MethdFall()) {
                        // Fell into the river.
                        return 1;
                    }
                    break;
            }
            break;
        case 1:
            m_fJumpVelX *= 0.875f;
            m_fJumpVelZ *= 0.875f;
            if (!(pWork->m_nSubFlag & 1)) {
                // (ofs: the offset of the landing effect; not used)
                bcs::FVEC3 velXZ(m_fJumpVelX, 0.0f, m_fJumpVelZ);
                if (bcs::FVEC3Len(&velXZ) <= 0.1f) {
                    bcs::FVEC3 ofs;
                    if (pWork->m_bBack) {
                        bcs::BCS_VecZ_RotY(ofs, -0.6f, GetRotY());
                    } else {
                        bcs::BCS_VecZ_RotY(ofs, 0.6f, GetRotY());
                    }
                    Util_SetEffect(static_cast<SET_EFFECT_TYPE>(2));
                    pWork->m_nSubFlag |= 1;
                }
            }
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                AnimPlay(3, Util_GetDamageMotID(1, pWork->m_bBack, 2), 1, 4.0f, 0.0f);
                m_nStatusFlag &= ~0xC00;
                pWork->m_fTime = pWork->m_fDownTime;
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (!IsDodgeInfield() || CGameDodge::GetChrRestLife(m_nPlayerNo) > 0 || !CGame::IsMainLoopState(0)) {
                if (pWork->m_fTime > 0.0f) {
                    pWork->m_fTime -= m_fFrameStep;
                } else {
                    // Getting up.
                    AnimPlay(3, Util_GetDamageMotID(1, pWork->m_bBack, 3), 0, 4.0f, 0.0f);
                    AnimSetSpeed(3, 0.6f);
                    pWork->m_nStep++;
                }
            }
            break;
        default:
            if (IsAnimEnd(3)) {
                return 1;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreDodge::SubStateDamageCtorDizzy_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    SetInvincible(0.0f);
    int bAir = pWork->m_bAir;
    switch ((pWork->m_nFlag >> 8) & 0xFF) {
        case 5:
            bAir = 1;
            break;
        case 12:
            bAir = 1;
            break;
    }
    if (bAir == 0) {
        m_UnkEC |= 0x10;
        SetCollBodyPushPower_(5);
        SetCollBodyPushRank_(1);
        PlayVoice(0x8D, 0);
        AnimPlay(3, Util_GetDamageMotID(2, 0, 0), 0, 0.0f, 0.0f);
        pWork->m_nStep = 99;
    } else {
        SubStateDamageCtorMDmg_(pWork);
    }
}

int msw::CChrCoreDodge::SubStateDamageLoopDizzy_(STATE_WORK_DAMAGE *pWork)
{
    int nSub = (unsigned char)(pWork->m_nFlag >> 8);
    if (pWork->m_nType != 6 && nSub != 0x15) {
        SetInvincible(s_fDmgInvincibleTime[0]);
    }
    if (IsNoDownEffect((unsigned char)(pWork->m_nFlag >> 8))) {
        SetInvincibleVsBall(s_fDmgInvincibleTime[0]);
    }
    switch (pWork->m_nStep) {
        case 0:
            switch (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                case 1: {
                    long nMot = Util_GetDamageMotID(0, pWork->m_bBack, 1);
                    m_fJumpUnk220 = 9999.9f;
                    Util_SetEffect(static_cast<SET_EFFECT_TYPE>(1));
                    AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                    m_UnkEC |= 0x10;
                    SetCollBodyPushPower_(5);
                    SetCollBodyPushRank_(1);
                    pWork->m_nStep++;
                    SetSmashDmg(0);
                    break;
                }
                case 2:
                    if (MethdFall()) {
                        // Fell into the river.
                        return 1;
                    }
                    break;
            }
            break;
        case 1:
            m_fJumpVelX *= 0.875f;
            m_fJumpVelZ *= 0.875f;
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                m_nStatusFlag &= ~0xC00;
                if ((unsigned char)(pWork->m_nFlag >> 8) == 5) {
                    pWork->m_fDownTime = 15.0f;
                }
                pWork->m_fTime = pWork->m_fDownTime;
                AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 3), 1, 0.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (!IsDodgeInfield() || CGameDodge::GetChrRestLife(m_nPlayerNo) > 0) {
                if (pWork->m_fTime > 0.0f) {
                    pWork->m_fTime -= m_fFrameStep;
                } else {
                    AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 2), 0, 4.0f, 0.0f);
                    pWork->m_nStep++;
                }
            }
            break;
        case 3:
            if (!IsAnimEnd(3)) {
                break;
            }
            pWork->m_nStep++;
        default:
            // Dizzy from here on: a condition, or a dizzy state of its own.
            if ((unsigned char)(pWork->m_nFlag >> 8) == 5) {
                SetCondition(8, 240.0f, 0);
                return 1;
            }
            FsmqCutNext();
            STATE_WORK_DIZZY *pDizzy = static_cast<STATE_WORK_DIZZY *>(FsmqPushState(STATE_DIZZY));
            pDizzy->m_nFlag = pWork->m_nFlag;
            pDizzy->m_fRotY = pWork->m_fRotY;
            return 1;
    }
    return 0;
}

int msw::CChrCoreDodge::SubStateDamageLoopTumble_(STATE_WORK_DAMAGE *pWork)
{
    if (pWork->m_nType != 6) {
        SetInvincible(s_fDmgInvincibleTime[0]);
    }
    if (IsNoDownEffect((unsigned char)(pWork->m_nFlag >> 8))) {
        SetInvincibleVsBall(s_fDmgInvincibleTime[0]);
    }
    switch (pWork->m_nStep) {
        case 0:
            if (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                m_fJumpUnk220 = 9999.9f;
                Util_SetEffect(static_cast<SET_EFFECT_TYPE>(1));
                m_UnkEC |= 0x10;
                SetCollBodyPushRank_(1);
                pWork->m_nStep++;
            }
            break;
        case 1:
            m_fJumpVelX *= 0.875f;
            m_fJumpVelZ *= 0.875f;
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                m_nStatusFlag &= ~0xC00;
                pWork->m_fTime = pWork->m_fDownTime;
                AnimPlay(3, Util_GetDamageMotID(4, pWork->m_bBack, 3), 1, 0.0f, 0.0f);
                pWork->m_nStep++;
                if (!IsNoDownEffect((unsigned char)(pWork->m_nFlag >> 8))) {
                    Eff_CreateAttach(0x10, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                }
            }
            break;
        case 2:
            if (!IsDodgeInfield() || CGameDodge::GetChrRestLife(m_nPlayerNo) > 0) {
                if (pWork->m_fTime > 0.0f) {
                    pWork->m_fTime -= m_fFrameStep;
                } else {
                    AnimPlay(3, Util_GetDamageMotID(4, pWork->m_bBack, 2), 0, 4.0f, 0.0f);
                    pWork->m_nStep++;
                }
            }
            break;
        default:
            if (IsAnimEnd(3)) {
                return 1;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreDodge::CtorStateDizzy()
{
    STATE_WORK_DIZZY *pWork = FsmqGetWork<STATE_WORK_DIZZY>();

    m_nStatusFlag &= ~0x40000000;
    m_UnkEC |= 0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushRank_(1);
    SetInvincible(0.0f);
    switch ((pWork->m_nFlag >> 8) & 0xFF) {
        case 3:
            AnimPlay(3, 0x6A, 1, 8.0f, 0.0f);
            pWork->m_fTime = 60.0f;
            Eff_CreateAttach(0x10, 0x524, m_nEffNodeSub, 0, 0, 3, 0);
            break;
        case 5:
            AnimPlay(3, 0x69, 1, 8.0f, 0.0f);
            pWork->m_fTime = 240.0f;
            break;
        case 12:
            // Hit by the pinball: knocked back and short of stamina.
            AnimPlay(3, 0x6C, 1, 8.0f, 0.0f);
            pWork->m_fTime = DODGE_PRM_S12_BALL_DIZZY_TIME;
            AddStamina(-DODGE_PRM_S12_BALL_STAMINA_DMG);
            pWork->m_Move.m_nType = 2;
            pWork->m_Move.m_fFrame = 0.0f;
            pWork->m_Move.m_fFrameMax = DODGE_PRM_S12_BALL_HIT_BACK_TIME;
            pWork->m_Move.m_fDist = DODGE_PRM_S12_BALL_HIT_BACK_DIST;
            break;
        default:
            AnimPlay(3, 0x6C, 1, 8.0f, 0.0f);
            pWork->m_fTime = 300.0f;
            break;
    }
    Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fRotY);
}

void msw::CChrCoreDodge::LoopStateDizzy()
{
    STATE_WORK_DIZZY *pWork = FsmqGetWork<STATE_WORK_DIZZY>();

    // Staggering along the direction of the jump.
    switch ((pWork->m_nFlag >> 8) & 0xFF) {
        case 3:
            if (m_fJumpRotY >= 0.0f) {
                bcs::FVEC3 move;
                bcs::BCS_VecZ_RotY(move, (1.0f / 60.0f) * m_fFrameStep, m_fJumpRotY);
                ClearRevise();
                AddPos(move);
            }
            break;
        case 5:
            if (m_fJumpRotY >= 0.0f) {
                bcs::FVEC3 move;
                bcs::BCS_VecZ_RotY(move, (1.0f / 60.0f) * m_fFrameStep, m_fJumpRotY);
                ClearRevise();
                AddPos(move);
            }
            break;
        case 12:
            if (m_fJumpRotY >= 0.0f) {
                bcs::FVEC3 move;
                bcs::BCS_VecZ_RotY(move, (DODGE_PRM_S12_BALL_DIZZY_SPEED / 60.0f) * m_fFrameStep, m_fJumpRotY);
                ClearRevise();
                AddPos(move);
            }
            break;
    }

    if (pWork->m_fTime > 0.0f) {
        pWork->m_fTime -= m_fTimeScale * m_fFrameStep;
        pWork->m_fFrame += m_fFrameStep;
        Util_DoDirMove(pWork->m_Move, pWork->m_fFrame, pWork->m_DirMove, 1);
        // Shaking the remote shortens it.
        float fShake = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x3D), 0, 0, 0, 0);
        if (fShake >= 512.0f) {
            pWork->m_fTime -= 0.0009765625f * (fShake * (m_fTimeScale * m_fFrameStep));
            AnimChgSpeed(3, 4.0f, 10.0f);
        } else {
            AnimChgSpeed(3, 1.0f, 30.0f);
        }
    } else {
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreDodge::DtorStateDizzy()
{
    Eff_StopGroup(2);
    Eff_Stop(0x10);
}

void msw::CChrCoreDodge::CtorStateSpinStagger()
{
    FsmqNext();
    FsmqRetry();
}

void msw::CChrCoreDodge::LoopStateSpinStagger() {}

void msw::CChrCoreDodge::DtorStateSpinStagger() {}

void msw::CChrCoreDodge::CtorStateFall()
{
    STATE_WORK_FALL *pWork = FsmqGetWork<STATE_WORK_FALL>();

    m_nStatusFlag |= 0x40000000;
    SetInvincibleVsBall(-1.0f);
    pWork->bSplash = 0;
}

void msw::CChrCoreDodge::LoopStateFall()
{
    STATE_WORK_FALL *pWork = FsmqGetWork<STATE_WORK_FALL>();

    // Falling into the water.
    float fStep = m_fTimeScale * m_fFrameStep;
    if (m_fJumpAccelY >= 0.0f) {
        m_fJumpAccelY = -0.025f;
    }
    bcs::FVEC3 move(0.0f, 0.0f, 0.0f);
    {
        // The move of this frame under the jump's speed and gravity.
        bcs::FVEC3 step;
        float fVelAdd = m_fJumpAccelY * fStep;
        bcs::FVEC3Scale(&step, &GetJumpVel(), fStep);
        m_fJumpVelY += fVelAdd;
        step.y += 0.5f * (fVelAdd * fStep);
        bcs::FVEC3Add(&move, &move, &step);
    }
    ClearRevise();
    AddPos(move);
    if (GetPos().y < -3.0f && !pWork->bSplash) {
        // Under water: the splash.
        bcs::FVEC3 pos = GetPos();
        bcs::FVEC3 rot = GetRot();
        pos.y = -3.0f;
        Eff_CreateSimple(0x14C, pos, rot, 3, 0, 0);
        pWork->bSplash = 1;
        PlaySePos(0xD29, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    }
    if (GetPos().y < -5.0f) {
        if (MethdRebirth()) {
        }
    }
}

void msw::CChrCoreDodge::DtorStateFall() {}

void msw::CChrCoreDodge::CtorStateRebirth()
{
    STATE_WORK_REBIRTH *pWork = FsmqGetWork<STATE_WORK_REBIRTH>();

    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    m_nStatusFlag &= ~0xC00;
    AnimPlay(3, 3, 1, 0.0f, 0.0f);
    SetMoveMotType(0);
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    pWork->m_fTime = -1.0f;
    if (m_pMdl) {
        m_pMdl->SetAlphaRateF(0.0f);
    }

    // The balls this character touched last are scored as thrown out.
    long nNum = CBallMan::GetBallNum();
    for (long i = 0; i < nNum; i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);
        if (pBall && pBall->IsActive() && pBall->GetTouchNum() > 0 &&
            static_cast<CBallDodge *>(pBall)->GetLastTouchChrIdx() == m_nPlayerNo) {
            pBall->TrgScore(-1);
        }
    }

    float fLife = GetLife();
    if (fLife > 0.0f) {
        static_cast<CGameDodge *>(CGame::m_pInstance)->AddChrLostScore_(m_nPlayerNo, fLife, -1);
        static_cast<CGameDodge *>(CGame::m_pInstance)->TrgOut_(-1, m_nTeamNo ^ 1, 0, 1, 0.0f, GetPos(), m_nPlayerNo);
    }
    m_nStatusFlag |= 0x40000000;
    SetInvincibleVsBall(-1.0f);
}

void msw::CChrCoreDodge::LoopStateRebirth()
{
    STATE_WORK_REBIRTH *pWork = FsmqGetWork<STATE_WORK_REBIRTH>();

    if (pWork->m_fTime >= 0.0f) {
        pWork->m_fTime -= m_fFrameStep;
        if (pWork->m_fTime <= 0.0f) {
            FsmqNext();
            FsmqRetry();
        }
    }
}

void msw::CChrCoreDodge::DtorStateRebirth()
{
    if (m_pMdl) {
        m_pMdl->SetAlphaRateF(1.0f);
    }
    m_nStatusFlag &= ~0x40000000;
    ResetInvincibleVsBall();
}

float msw::CChrCoreDodge::SubStateParalyzeSetEffect_()
{
    AnimPlay(3, 0x6D, 1, 8.0f, 0.0f);
    Eff_CreateAttach(10, 0x5B7, 0, 0, 0, 3, 0);
    // The size of the effect by character.
    float fScale = 1.0f;
    switch (m_nChrId) {
        case 6:
            fScale = 1.3f;
            break;
        case 7:
            fScale = 1.4f;
            break;
        case 8:
            fScale = 0.9f;
            break;
        case 10:
            fScale = 1.4f;
            break;
        case 11:
            fScale = 0.8f;
            break;
        case 12:
            fScale = 0.8f;
            break;
    }
    Eff_SetScale(10, fScale, fScale, fScale);
    return 30.0f;
}

void msw::CChrCoreDodge::SubStateParalyzeEndEffect_()
{
    Eff_Stop(10);
    if (m_nComLevel == 9) {
        m_fUnk248 = RandAdd(33.0f, 0.0f);
    }
}

void msw::CChrCoreDodge::CtorStateSliding()
{
    STATE_WORK_SLIDING *pWork = FsmqGetWork<STATE_WORK_SLIDING>();

    pWork->m_nStep = 0;
    pWork->m_fSpeedRate = 1.0f;
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 9, 1, 0, 0);
}

void msw::CChrCoreDodge::LoopStateSliding()
{
    STATE_WORK_SLIDING *pWork = FsmqGetWork<STATE_WORK_SLIDING>();

    int bEnd = 0;
    switch (pWork->m_nStep) {
        case 0:
            AnimPlay(3, 0x71, 0, 4.0f, 0.0f);
            SetMoveMotType(2);
            PlayVoice(0x92, 0);
            SE_PlayMove(0, 0x45A, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
            pWork->m_nStep = 1;
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x72, 1, 0.0f, 0.0f);
                pWork->m_nStep = 2;
            }
            break;
        case 2:
            if (!pWork->m_bAccel) {
                // Slowing down unless the stick is held.
                float fFrame = 99.0f;
                pWork->m_fSpeedRate -= (1.0f / Max(fFrame, 1.0f)) * m_fFrameStep;
                pWork->m_fSpeedRate = CLAMP(pWork->m_fSpeedRate, 0.0f, 1.0f);
                if (pWork->m_fSpeedRate <= 0.0f) {
                    SE_Stop(0, 8.0f);
                    AnimPlay(3, 0x73, 0, 0.0f, 0.0f);
                    pWork->m_nStep = 3;
                }
            } else {
                float fFrame = 99.0f;
                pWork->m_fSpeedRate += (1.0f / Max(fFrame, 1.0f)) * m_fFrameStep;
                pWork->m_fSpeedRate = CLAMP(pWork->m_fSpeedRate, 0.0f, 1.0f);
            }
            break;
        default:
            if (IsAnimEnd(3)) {
                bEnd = 1;
            }
            break;
    }

    float fRotYTrg = pWork->m_fRotYTrg;
    if (pWork->m_nStep < 3) {
        // Steering, by at most 8.5 degrees a frame.
        float fDiff = WrapRadPi(fRotYTrg - pWork->m_fRotY);
        float fMaxDeg = 8.5f;
        float fMax = bcs::DEG_TO_RAD(fMaxDeg);
        float fRot = Clamp(fDiff, -fMax, fMax);
        float fStep = fRot * m_fFrameStep;
        if (fDiff < 0.0f) {
            fDiff = Max(fStep, fDiff);
        } else {
            fDiff = Min(fStep, fDiff);
        }
        pWork->m_fRotY = WrapRad(pWork->m_fRotY + fDiff);
        RotateYRate_(fRotYTrg, 0.2f);
    }

    float fSpeed = 1.1f * GetPrmMoveSpeed();
    bcs::FVEC3 pos = GetPos();
    MoveRunXZ_(pWork->m_fRotY, fSpeed, pWork->m_fSpeedRate);
    pWork->m_bAccel = 0;
    if (bEnd) {
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreDodge::DtorStateSliding()
{
    SE_Stop(0, 0.0f);
}
