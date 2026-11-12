// msw::CChrCoreVolley's damage state (with the motion of each kind of
// damage), its dizzy state, the paralysis effect, the fall into the water
// and the rebirth after it. In the binary this follows the neutral, run and
// block jump states (game/volley/CChrCoreVolley.cpp) but is a file of its
// own: its literals are pooled again. Written from BossF's
// game/boss/unit_1C248.cpp and DodgeF's game/dodge/unit_3AAC8.cpp (the fall,
// rebirth and paralysis) and checked against VolleyF.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/triangular.h"

// The damage state's work (CChrCore's; names guessed).
struct msw::CChrCore::STATE_WORK_DAMAGE {
    long m_nStep;            // 0x00
    unsigned long m_nSubFlag; // 0x04: bit 0 once the landing effect is out
    // 0x08: the damage flags; the low byte is the kind of damage (see
    // CtorStateDamage), bits 8-15 a sub kind.
    unsigned long m_nFlag;
    long m_nType;            // 0x0C: the kind of damage, from m_nFlag
    int m_bAir;              // 0x10: hit in the air
    float m_fRotY;           // 0x14: the direction the hit came from
    int m_bBack;             // 0x18: hit from behind (selects the motion)
    float m_fDamage;         // 0x1C
    float m_fTime;           // 0x20: frames of the spin (kind 5)
    float m_fDownTime;       // 0x24: frames to stay down
};

namespace {

    // (names guessed) The time of the dizzy condition after a hit of kind
    // 12, and a value nothing here reads. The original reads the time from
    // memory at the start of .rodata: an array (a scalar const would be
    // folded into a literal).
    const float s_afDizzyTime12[2] = { 120.0f, 0.0f };

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

    // The dizzy state's work (names guessed).
    struct STATE_WORK_DIZZY {
        long m_nStep;          // 0x00
        unsigned long m_nFlag; // 0x04: bits 8-15 the kind of dizziness
        long m_nType;          // 0x08: 1 can be shaken off, 2 a fixed time
        float m_fTime;         // 0x0C: frames left
    };

    // The fall state's work (names guessed).
    struct STATE_WORK_FALL {
        long m_nStep;  // 0x00: 0 falling, then waiting
        float m_fTime; // 0x04: frames to wait under water
        int m_bSplash; // 0x08: once the character is under water
    };

    // The rebirth state's work (names guessed).
    struct STATE_WORK_REBIRTH {
        long m_nStep;  // 0x00
        float m_fTime; // 0x04: frames left (negative: none)
    };

    // (guessed helper) fBase plus a random part of fRange.
    inline float RandAdd(float fBase, float fRange)
    {
        float fRand = bcs::BCS_RandMax(1.0f);
        return fBase + fRand * fRange;
    }

    // (guessed helper) The rate of a damage's hit back that leaves a
    // large damage's speed and height.
    const float LDMG_SPEED_RATE = 0.33333334f;

    enum {
        STATE_DIZZY = msw::CChrCore::STATE_DIZZY,
        STATE_FALL = msw::CChrCore::STATE_FALL,
        STATE_REBIRTH = msw::CChrCore::STATE_REBIRTH
    };
}

namespace {

    // The damage motions by kind (Util_GetDamageMotID): for a hit from
    // the front and from behind, the start, the fall, the getting up and
    // the lying down.
    const short s_DamageMotNormal[2][4] = { { 79, 80, 82, 81 }, { 87, 88, 90, 89 } };
    const short s_DamageMotType1[2][4] = { { 83, 84, 86, 85 }, { 91, 92, 94, 93 } };
    const short s_DamageMotType3[2][4] = { { 95, 95, 96, 82 }, { 95, 95, 96, 82 } };
    const short s_DamageMotType4[2][4] = { { 104, 104, 106, 105 }, { 104, 104, 106, 105 } };
    const short s_DamageMotType5[2][4] = { { 98, 98, 98, 98 }, { 98, 98, 98, 98 } };
    const short s_DamageMotType15[2][4] = { { 24, 24, 25, 85 }, { 26, 26, 27, 85 } };
    const short s_DamageMotType16[2][4] = { { 48, 48, 49, 1 }, { 48, 48, 49, 1 } };
}

int msw::CChrCoreVolley::Util_GetDamageMotID(long nType, long nBack, long nPart)
{
    switch (nType) {
        case 1:
            return s_DamageMotType1[nBack][nPart];
        case 3:
            return s_DamageMotType3[nBack][nPart];
        case 4:
            return s_DamageMotType4[nBack][nPart];
        case 5:
            return s_DamageMotType5[nBack][nPart];
        case 14:
            return 0x67;
        case 15:
            return s_DamageMotType15[nBack][nPart];
        case 16:
            return s_DamageMotType16[nBack][nPart];
        default:
            return s_DamageMotNormal[nBack][nPart];
    }
}

void msw::CChrCoreVolley::CtorStateDamage()
{
    STATE_WORK_DAMAGE *pWork = FsmqGetWork<STATE_WORK_DAMAGE>();

    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(0);
    pWork->m_nType = pWork->m_nFlag & 0xFF;
    pWork->m_nStep = 0;

    switch (pWork->m_nType) {
        case 0:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorMDmg_(pWork, 0.075f, 0.2f);
            break;
        case 1:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorLDmg_(pWork, 0.15f, 0.3f);
            break;
        case 2:
            SubStateDamageCtorDizzy_(pWork);
            break;
        case 4:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorTumble_(pWork);
            break;
        case 3:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorBurn_(pWork);
            break;
        case 5:
            // Spun round and thrown up.
            SetCondition(0, 0.0f, 0);
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
            DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
            break;
        case 13:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorMame_(pWork);
            break;
        case 15:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorReceiveDmg_(pWork);
            break;
        case 16:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorSpikeMiss_(pWork);
            break;
    }
    CombinationEnd_();
}

void msw::CChrCoreVolley::LoopStateDamage()
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
            SetInvincible(30.0f);
            if (pWork->m_nStep == 0) {
                if (Util_DoJumpMove(m_fTimeScale * m_fFrameStep)) {
                    long nMot = Util_GetDamageMotID(3, pWork->m_bBack, 2);
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
        case 13:
            bEnd = SubStateDamageLoopMame_(pWork);
            break;
        case 15:
            bEnd = SubStateDamageLoopReceiveDmg_(pWork);
            break;
        case 16:
            bEnd = SubStateDamageLoopSpikeMiss_(pWork);
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

void msw::CChrCoreVolley::DtorStateDamage()
{
    Eff_StopGroup(3);
    m_nStatusFlag &= ~0x40000000;
    m_UnkEC &= ~0x4;
    SetSmashDmg(0);
    m_afCountWork[1] = 50.0f;
}

void msw::CChrCoreVolley::SetSmashDmg(int bSmash)
{
    m_bCheckLanding = bSmash;
}

void msw::CChrCoreVolley::SubStateDamageCtorMDmg_(STATE_WORK_DAMAGE *pWork, float fSpeed, float fUp)
{
    m_nStatusFlag |= 0x40000000;
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

    if (pWork->m_bBack) {
        fSpeed *= -1.0f;
    }
    bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
    m_fJumpVelY = fUp;
    m_fJumpAccelY = -0.025f;
    if (CGame::g_GameWork.GetStage() == 5) {
        SetSmashDmg(1);
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

int msw::CChrCoreVolley::SubStateDamageLoopMDmg_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
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
                    SetSmashDmg(0);
                    pWork->m_nStep++;
                    break;
                }
                case 2:
                    // Fell off the court.
                    if (MethdFall()) {
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
                unsigned long nFlag = m_nStatusFlag;
                m_nStatusFlag = nFlag & ~0xC00;
                if ((nFlag & 0x80000) || pWork->m_fDownTime > 0.0f) {
                    long nMot = Util_GetDamageMotID(0, pWork->m_bBack, 3);
                    if (nMot >= 0) {
                        AnimPlay(3, nMot, 1, 4.0f, 0.0f);
                        Eff_CreateAttach(0, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                    }
                    pWork->m_nStep = 2;
                } else {
                    AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 2), 0, 4.0f, 0.0f);
                    pWork->m_nStep = 3;
                }
            }
            break;
        case 2:
            if (!(m_nStatusFlag & 0x80000)) {
                float fStep = m_fFrameStep * GetCcParam_()->m_Unk3DC;
                pWork->m_fDownTime -= fStep;
                if (m_nComLevel == 1) {
                    pWork->m_fDownTime = 0.0f;
                }
                if (!(pWork->m_fDownTime > 0.0f)) {
                    Eff_Stop(0);
                    AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 2), 0, 0.0f, 0.0f);
                    pWork->m_nStep = 3;
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

void msw::CChrCoreVolley::SubStateDamageCtorLDmg_(STATE_WORK_DAMAGE *pWork, float fSpeed, float fUp)
{
    m_nStatusFlag |= 0x40000000;
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

    if (pWork->m_bBack) {
        fSpeed *= -1.0f;
    }
    // The hit back keeps its direction with a third of its length.
    bcs::FVEC3 vel;
    bcs::BCS_VecZ_RotY(vel, fSpeed, GetRotY());
    vel.y = fUp;
    bcs::BCS_Vec_Normalize(vel, vel);
    nw4r::math::VEC3Scale(reinterpret_cast<nw4r::math::VEC3 *>(&vel), reinterpret_cast<nw4r::math::VEC3 *>(&vel),
                          LDMG_SPEED_RATE);
    m_fJumpVelX = vel.x;
    m_fJumpVelY = vel.y;
    m_fJumpVelZ = vel.z;
    m_fJumpAccelY = -0.025f;
    if (CGame::g_GameWork.GetStage() == 5) {
        SetSmashDmg(1);
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

int msw::CChrCoreVolley::SubStateDamageLoopLDmg_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
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
                    SetSmashDmg(0);
                    pWork->m_nStep++;
                    break;
                }
                case 2:
                    // Fell off the court.
                    if (MethdFall()) {
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
                bcs::FVEC3 ofs;
                bcs::FVEC3 velXZ(m_fJumpVelX, 0.0f, m_fJumpVelZ);
                if (bcs::FVEC3Len(&velXZ) <= 0.1f) {
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
                unsigned long nFlag = m_nStatusFlag;
                m_nStatusFlag = nFlag & ~0xC00;
                if ((nFlag & 0x80000) || pWork->m_fDownTime > 0.0f) {
                    long nMot = Util_GetDamageMotID(1, pWork->m_bBack, 3);
                    if (nMot >= 0) {
                        AnimPlay(3, nMot, 1, 4.0f, 0.0f);
                        Eff_CreateAttach(0, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                    }
                    pWork->m_nStep = 2;
                } else {
                    AnimPlay(3, Util_GetDamageMotID(1, pWork->m_bBack, 2), 0, 4.0f, 0.0f);
                    AnimSetSpeed(3, 0.6f);
                    pWork->m_nStep = 3;
                }
            }
            break;
        case 2:
            if (!(m_nStatusFlag & 0x80000)) {
                float fStep = m_fTimeScale * (m_fFrameStep * GetCcParam_()->m_Unk3DC);
                pWork->m_fDownTime -= fStep;
                if (m_nComLevel == 1) {
                    pWork->m_fDownTime = 0.0f;
                }
                if (!(pWork->m_fDownTime > 0.0f)) {
                    long nMot = Util_GetDamageMotID(1, pWork->m_bBack, 2);
                    Eff_Stop(0);
                    AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                    AnimSetSpeed(3, 0.6f);
                    pWork->m_nStep = 3;
                }
            }
            break;
        default:
            if (IsAnimEnd(3) && !(m_nStatusFlag & 0x80000)) {
                return 1;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreVolley::SubStateDamageCtorDizzy_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    SetInvincible(0.0f);
    float fSpeed = 0.075f;
    float fUp = 0.2f;
    switch ((pWork->m_nFlag >> 8) & 0xFF) {
        case 12:
            pWork->m_bAir = 1;
            break;
        case 5:
            pWork->m_bAir = 1;
            fSpeed = 0.25f;
            fUp = 0.3f;
            break;
    }
    long nMsg = 10;
    if (pWork->m_bAir == 0) {
        // Dizzy on the spot.
        m_UnkEC |= 0x10;
        SetCollBodyPushPower_(5);
        SetCollBodyPushRank_(1);
        pWork->m_nStep = 99;
        switch ((pWork->m_nFlag >> 8) & 0xFF) {
            case 3:
                PlayVoice(0x8F, 0);
                break;
            case 4:
                PlayVoice(0x90, 0);
                nMsg = 5;
                break;
            default:
                PlayVoice(0x8D, 0);
                break;
        }
        DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), nMsg, 1, 0, 0);
    } else {
        SubStateDamageCtorMDmg_(pWork, fSpeed, fUp);
    }
}

int msw::CChrCoreVolley::SubStateDamageLoopDizzy_(STATE_WORK_DAMAGE *pWork)
{
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
                    SetSmashDmg(0);
                    pWork->m_nStep++;
                    break;
                }
                case 2:
                    // Fell off the court.
                    if (MethdFall()) {
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
                switch ((pWork->m_nFlag >> 8) & 0xFF) {
                    case 12:
                        pWork->m_fDownTime = 0.0f;
                        break;
                    case 5:
                        pWork->m_fDownTime = 1.0f;
                        break;
                }
                if ((m_nStatusFlag & 0x80000) || pWork->m_fDownTime > 0.0f) {
                    long nMot = Util_GetDamageMotID(0, pWork->m_bBack, 3);
                    if (nMot >= 0) {
                        AnimPlay(3, nMot, 1, 4.0f, 0.0f);
                        Eff_CreateAttach(0, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                    }
                    pWork->m_nStep = 2;
                } else {
                    AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 2), 0, 4.0f, 0.0f);
                    pWork->m_nStep = 3;
                }
            }
            break;
        case 2: {
            float fStep = m_fFrameStep * GetCcParam_()->m_Unk3DC;
            pWork->m_fDownTime -= fStep;
            if (m_nComLevel == 1) {
                pWork->m_fDownTime = 0.0f;
            }
            if (!(pWork->m_fDownTime > 0.0f)) {
                Eff_Stop(0);
                AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 2), 0, 0.0f, 0.0f);
                pWork->m_nStep = 3;
            }
            break;
        }
        case 3:
            if (!IsAnimEnd(3)) {
                break;
            }
        default: {
            // Dizzy from here on: the condition by the kind of hit, or
            // (kind 3) the dizzy state for good.
            switch ((pWork->m_nFlag >> 8) & 0xFF) {
                case 3: {
                    FsmqCutNext();
                    STATE_WORK_DIZZY *pDizzy = static_cast<STATE_WORK_DIZZY *>(FsmqPushWork(STATE_DIZZY));
                    pDizzy->m_nFlag = pWork->m_nFlag;
                    pDizzy->m_nType = 2;
                    pDizzy->m_fTime = 9999.0f;
                    SetCondition(5, 9999.0f, 0);
                    return 1;
                }
                case 4:
                    SetCondition(6, 180.0f, 0);
                    break;
                case 5:
                    SetCondition(7, 260.0f, 0);
                    break;
                case 8:
                    SetCondition(8, 30.0f, 8);
                    break;
                case 12:
                    SetCondition(8, s_afDizzyTime12[0], 12);
                    break;
                default:
                    SetCondition(7, 60.0f, 0);
                    break;
            }
            FsmqCutNext();
            return 1;
        }
    }
    return 0;
}

void msw::CChrCoreVolley::SubStateDamageCtorBurn_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(4);
    PlayVoice(0x8E, 0);
    AnimPlay(3, Util_GetDamageMotID(3, 0, 0), 0, 0.0f, 0.0f);
    float fY = GetPosY();
    Util_SetJumpParam(AnimGetFrameNum(3), fY, fY, 1.5f + fY);
    m_nStatusFlag |= 0x400;
    // Burning: the character weaves about for the down time (three
    // seconds for kind 9).
    long nMsg;
    unsigned char nSub = pWork->m_nFlag >> 8;
    m_fWeaveWidth = 0.7853982f;
    if (nSub == 9) {
        SetCondition(4, 180.0f, 0);
        nMsg = 8;
        m_fWeaveSpeed = 0.06981317f;
        m_fWeaveWidth = 0.7853982f;
    } else {
        SetCondition(4, pWork->m_fDownTime, 0);
        nMsg = 8;
        m_fWeaveSpeed = 0.06981317f;
        m_fWeaveWidth = 0.7853982f;
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), nMsg, 1, 0, 0);
}

void msw::CChrCoreVolley::SubStateDamageCtorTumble_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(1);
    float fInter = 0.0f;
    switch ((pWork->m_nFlag >> 8) & 0xFF) {
        case 2:
            PlayVoice(0x8E, 0);
            break;
        case 39:
            fInter = 8.0f;
        default:
            PlayVoice(0x92, 0);
            break;
    }
    pWork->m_bBack = 0;
    AnimPlay(3, Util_GetDamageMotID(4, 0, 0), 0, fInter, 0.0f);
    m_fJumpVelX = 0.0f;
    m_fJumpVelY = 0.0f;
    m_fJumpVelZ = 0.0f;
    m_fJumpAccelY = -0.025f;
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

int msw::CChrCoreVolley::SubStateDamageLoopTumble_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    switch (pWork->m_nStep) {
        case 0:
            m_fJumpVelX *= 0.875f;
            m_fJumpVelZ *= 0.875f;
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                if (((pWork->m_nFlag >> 8) & 0xFF) == 2) {
                    PlaySePos(0xC56, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                }
                AnimPlay(3, Util_GetDamageMotID(4, pWork->m_bBack, 3), 1, 4.0f, 0.0f);
                if (pWork->m_fDownTime > 0.0f) {
                    Eff_CreateAttach(0, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                }
                pWork->m_nStep++;
            }
            break;
        case 1:
            if (!(m_nStatusFlag & 0x80000)) {
                float fStep = m_fTimeScale * (m_fFrameStep * GetCcParam_()->m_Unk3DC);
                pWork->m_fDownTime -= fStep;
                if (m_nComLevel == 1) {
                    pWork->m_fDownTime = 0.0f;
                }
                if (!(pWork->m_fDownTime > 0.0f)) {
                    long nMot = Util_GetDamageMotID(4, pWork->m_bBack, 2);
                    Eff_Stop(0);
                    AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                    pWork->m_nStep = 2;
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

void msw::CChrCoreVolley::SubStateDamageCtorMame_(STATE_WORK_DAMAGE *pWork)
{
    CChrCore::SubStateDamageCtorMame_(pWork);
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

int msw::CChrCoreVolley::SubStateDamageLoopMame_(STATE_WORK_DAMAGE *pWork)
{
    return CChrCore::SubStateDamageLoopMame_(pWork);
}

void msw::CChrCoreVolley::SubStateDamageCtorReceiveDmg_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(7), 8.0f);
    SetCollBodyPushRank_(4);
    PlayVoice(0x46, 0);
    SetRotY(WrapRad(3.1415927f + pWork->m_fRotY));
    pWork->m_bBack = 0;
    if (pWork->m_bAir != 0) {
        pWork->m_bBack = 1;
    }
    AnimPlay(3, Util_GetDamageMotID(15, pWork->m_bBack, 0), 0, 4.0f, 0.0f);
    Eff_Stop(0);

    // Knocked back by the parameters' distance and height, per second.
    float fBack = -0.016666668f * GetCcParam_()->m_Unk3D4;
    float fUp = 0.016666668f * GetCcParam_()->m_Unk3D8;
    bcs::BCS_VecZ_RotY(GetJumpVel(), fBack, GetRotY());
    m_fJumpVelY = fUp;
    m_fJumpAccelY = -0.025f;
    if (CGame::g_GameWork.GetStage() == 5) {
        SetSmashDmg(1);
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 6, 1, 0, 0);
}

int msw::CChrCoreVolley::SubStateDamageLoopReceiveDmg_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    switch (pWork->m_nStep) {
        case 0:
            switch (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                case 1:
                    if (IsAnimEnd(3)) {
                        Eff_Stop(0);
                        SetCollBodyPushPower_(5);
                        SetCollBodyPushRank_(1);
                        m_UnkEC |= 0x10;
                        if (pWork->m_fDownTime > 0.0f) {
                            long nMot = Util_GetDamageMotID(15, pWork->m_bBack, 3);
                            m_fJumpUnk220 = 9999.9f;
                            AnimPlay(3, nMot, 1, 0.0f, 0.0f);
                            Eff_CreateAttach(0, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                            pWork->m_nStep = 1;
                        } else {
                            long nMot = Util_GetDamageMotID(15, pWork->m_bBack, 2);
                            m_fJumpUnk220 = 9999.9f;
                            AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                            AnimSetSpeed(3, 0.6f);
                            pWork->m_nStep = 2;
                        }
                    } else {
                        m_fJumpVelX *= 0.95f;
                        m_fJumpVelZ *= 0.95f;
                    }
                    break;
                case 2:
                    // Fell off the court.
                    if (MethdFall()) {
                        return 1;
                    }
                    break;
            }
            break;
        case 1: {
            float fStep = m_fTimeScale * (m_fFrameStep * GetCcParam_()->m_Unk3DC);
            pWork->m_fDownTime -= fStep;
            if (m_nComLevel == 1) {
                pWork->m_fDownTime = 0.0f;
            }
            if (!(pWork->m_fDownTime > 0.0f)) {
                Eff_Stop(0);
                long nMot = Util_GetDamageMotID(15, pWork->m_bBack, 2);
                m_fJumpUnk220 = 9999.9f;
                AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                AnimSetSpeed(3, 0.6f);
                pWork->m_nStep = 2;
            }
            break;
        }
        default:
            if (IsAnimEnd(3)) {
                return 1;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreVolley::SubStateDamageCtorSpikeMiss_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(7), 8.0f);
    SetCollBodyPushRank_(4);

    float fRot = WrapRad(GetRotY() - pWork->m_fRotY);
    if (fRot >= 1.5707964f && fRot <= 4.712389f) {
        SetRotY(WrapRad(3.1415927f + pWork->m_fRotY));
        pWork->m_bBack = 1;
    } else {
        SetRotY(WrapRad(pWork->m_fRotY));
        pWork->m_bBack = 0;
    }
    AnimPlay(3, Util_GetDamageMotID(16, pWork->m_bBack, 0), 0, 0.0f, 0.0f);

    float fSpeed = 0.075f;
    if (pWork->m_bBack) {
        fSpeed *= -1.0f;
    }
    bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
    m_fJumpVelY = 0.2f;
    m_fJumpAccelY = -0.025f;
    if (CGame::g_GameWork.GetStage() == 5) {
        SetSmashDmg(1);
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

int msw::CChrCoreVolley::SubStateDamageLoopSpikeMiss_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    if (pWork->m_nStep == 0) {
        switch (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
            case 1: {
                long nMot = Util_GetDamageMotID(16, pWork->m_bBack, 2);
                m_fJumpUnk220 = 9999.9f;
                AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                SetCollBodyPushPower_(5);
                SetCollBodyPushRank_(1);
                m_UnkEC |= 0x10;
                pWork->m_nStep++;
                break;
            }
            case 2:
                // Fell off the court.
                if (MethdFall()) {
                    return 1;
                }
                break;
        }
    } else {
        m_fJumpVelX *= 0.875f;
        m_fJumpVelZ *= 0.875f;
        Util_DoMove(m_fTimeScale * m_fFrameStep);
        if (IsAnimEnd(3)) {
            return 1;
        }
    }
    return 0;
}

void msw::CChrCoreVolley::CtorStateDizzy()
{
    STATE_WORK_DIZZY *pWork = FsmqGetWork<STATE_WORK_DIZZY>();

    m_nStatusFlag &= ~0x40000000;
    m_UnkEC |= 0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushRank_(1);
    switch ((pWork->m_nFlag >> 8) & 0xFF) {
        case 3:
            AnimPlay(3, 0x64, 1, 8.0f, 0.0f);
            pWork->m_fTime = 1.0f;
            pWork->m_nType = 2;
            break;
        case 4:
            AnimPlay(3, 0x65, 1, 8.0f, 0.0f);
            pWork->m_fTime = 180.0f;
            break;
        case 5:
            AnimPlay(3, 0x63, 1, 8.0f, 0.0f);
            pWork->m_fTime = 260.0f;
            break;
        case 12:
            AnimPlay(3, 0x63, 1, 8.0f, 0.0f);
            pWork->m_fTime = s_afDizzyTime12[0];
            break;
        default:
            pWork->m_fTime = 60.0f;
            AnimPlay(3, 0x66, 1, 8.0f, 0.0f);
            break;
    }
}

void msw::CChrCoreVolley::LoopStateDizzy()
{
    STATE_WORK_DIZZY *pWork = FsmqGetWork<STATE_WORK_DIZZY>();

    if (pWork->m_fTime > 0.0f) {
        switch (pWork->m_nType) {
            case 0:
                pWork->m_fTime -= m_fTimeScale * m_fFrameStep;
                break;
            case 1: {
                // Shaking the remote shortens it.
                pWork->m_fTime -= m_fTimeScale * m_fFrameStep;
                float fShake = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x3D), 0, 0, 0, 0);
                if (fShake >= 512.0f) {
                    pWork->m_fTime -= 0.0009765625f * (fShake * (m_fTimeScale * m_fFrameStep));
                    AnimChgSpeed(3, 4.0f, 10.0f);
                } else {
                    AnimChgSpeed(3, 1.0f, 30.0f);
                }
                break;
            }
        }
    } else {
        SetInvincible(30.0f);
        FsmqNext();
        FsmqRetry();
    }
}

void msw::CChrCoreVolley::DtorStateDizzy()
{
    Eff_StopGroup(2);
    SetCondition(0, 0.0f, 0);
    m_afCountWork[1] = 50.0f;
}

float msw::CChrCoreVolley::SubStateParalyzeSetEffect_()
{
    AnimPlay(3, 0x67, 1, 8.0f, 0.0f);
    Eff_CreateAttach(10, 0x5F5, 0, 0, 0, 3, 0);
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
    return 33.0f;
}

void msw::CChrCoreVolley::SubStateParalyzeEndEffect_()
{
    Eff_Stop(10);
    if (m_nComLevel == 9) {
        m_fUnk248 = RandAdd(40.0f, 0.0f);
    }
}

void msw::CChrCoreVolley::CtorStateFall()
{
    STATE_WORK_FALL *pWork = FsmqGetWork<STATE_WORK_FALL>();

    m_nStatusFlag |= 0x40000000;
    pWork->m_nStep = 0;
    pWork->m_fTime = 60.0f;
    pWork->m_bSplash = 0;
}

void msw::CChrCoreVolley::LoopStateFall()
{
    STATE_WORK_FALL *pWork = FsmqGetWork<STATE_WORK_FALL>();

    if (pWork->m_nStep == 0) {
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
        if (GetPos().y < -3.0f && !pWork->m_bSplash) {
            // Under water: the splash.
            bcs::FVEC3 pos = GetPos();
            bcs::FVEC3 rot = GetRot();
            pos.y = -3.0f;
            Eff_CreateSimple(0x14C, pos, rot, 3, 0, 0);
            pWork->m_bSplash = 1;
            PlaySePos(0xD29, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        }
        if (GetPos().y < -5.0f) {
            SetPosY(-5.0f);
            pWork->m_nStep++;
        }
    } else {
        pWork->m_fTime -= m_fFrameStep;
        if (pWork->m_fTime <= 0.0f) {
            if (MethdRebirth()) {
            }
        }
    }
}

void msw::CChrCoreVolley::DtorStateFall()
{
    SetSmashDmg(0);
}

// Back on the court after a fall: in the middle of the character's half,
// half see-through while invincible.
void msw::CChrCoreVolley::CtorStateRebirth()
{
    STATE_WORK_REBIRTH *pWork = FsmqGetWork<STATE_WORK_REBIRTH>();

    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    m_nStatusFlag &= ~0xC00;
    AnimPlay(3, 1, 1, 0.0f, 0.0f);
    SetMoveMotType(0);
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetSmashDmg(0);
    bcs::FVEC4 rect;
    Util_GetGuardCourtRect_(rect);
    bcs::FVEC3 pos(0.5f * (rect.x + rect.y), 0.0f, 0.5f * (rect.z + rect.w));
    SetPos(pos);
    SetRotY(GetBlockTargetRot_());
    pWork->m_fTime = 60.0f;
    if (m_pMdl) {
        m_pMdl->SetAlphaRateF(0.5f);
    }
    m_nStatusFlag |= 0x40000000;
}

void msw::CChrCoreVolley::LoopStateRebirth()
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

void msw::CChrCoreVolley::DtorStateRebirth()
{
    if (m_pMdl) {
        m_pMdl->SetAlphaRateF(1.0f);
    }
    m_nStatusFlag &= ~0x40000000;
}

int msw::CChrCoreVolley::IsNowFalling() const
{
    switch (GetState()) {
        case STATE_FALL:
        case STATE_REBIRTH:
            return 1;
    }
    return 0;
}
