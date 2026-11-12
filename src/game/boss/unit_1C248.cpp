// msw::CChrCoreBoss's damage state (with the motion of each kind of damage)
// and its dizzy state. In the binary this follows the neutral and run
// states (game/boss/unit_18E08.cpp) but is a file of its own: its literals
// are pooled again.
#include "game/msw/CChrCoreBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/triangular.h"

// The damage state's work (CChrCore's; names guessed, the fields as
// game/boss/CChrCoreBossMethod.cpp's MethdDamage fills them in).
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

    // The damage motions by kind (Util_GetDamageMotID): for a hit from
    // the front and from behind, the start, the fall, the getting up and
    // the lying down (-1 for none... values as in the original table).
    const short s_DamageMotNormal[2][4] = { { 111, 112, 114, 113 }, { 119, 120, 122, 121 } };
    const short s_DamageMotType2[2][4] = { { 111, 112, 114, 113 }, { 119, 120, 122, 121 } };
    const short s_DamageMotType1[2][4] = { { 115, 116, 118, 117 }, { 123, 124, 126, 125 } };
    const short s_DamageMotType3[2][4] = { { 127, 127, 128, 114 }, { 127, 127, 128, 114 } };
    const short s_DamageMotType4[2][4] = { { 136, 136, 138, 137 }, { 136, 136, 138, 137 } };
    const short s_DamageMotType5[2][4] = { { 130, 130, 130, 130 }, { 130, 130, 130, 130 } };
    const short s_DamageMotType17[2][4] = { { 82, 82, 83, 117 }, { 84, 84, 85, 117 } };
    const short s_DamageMotType18[2][4] = { { 99, 99, 100, 1 }, { 99, 99, 100, 1 } };

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
}

int msw::CChrCoreBoss::Util_GetDamageMotID(long nType, long nBack, long nPart)
{
    switch (nType) {
        case 1:
            return s_DamageMotType1[nBack][nPart];
        case 2:
            return s_DamageMotType2[nBack][nPart];
        case 3:
            return s_DamageMotType3[nBack][nPart];
        case 4:
            return s_DamageMotType4[nBack][nPart];
        case 5:
            return s_DamageMotType5[nBack][nPart];
        case 17:
            return s_DamageMotType17[nBack][nPart];
        case 18:
            return s_DamageMotType18[nBack][nPart];
        default:
            return s_DamageMotNormal[nBack][nPart];
    }
}

void msw::CChrCoreBoss::CtorStateDamage()
{
    STATE_WORK_DAMAGE *pWork = FsmqGetWork<STATE_WORK_DAMAGE>();

    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(0);
    pWork->m_nType = pWork->m_nFlag & 0xFF;
    pWork->m_nStep = 0;
    if (m_nStatusFlag & 0x80000) {
        // After time over every heavier hit is a plain knock down.
        switch (pWork->m_nType) {
            case 2:
            case 3:
            case 4:
            case 5:
            case 13:
            case 17:
            case 18:
                pWork->m_nType = 1;
                break;
        }
    }

    switch (pWork->m_nType) {
        case 0:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorMDmg_(pWork);
            break;
        case 1:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorLDmg_(pWork);
            break;
        case 2:
            SubStateDamageCtorDizzy_(pWork);
            break;
        case 4:
            // Tumbling.
            SetCondition(0, 0.0f, 0);
            m_nStatusFlag |= 0x40000000;
            m_UnkEC &= ~0x10;
            SetCollBodyPushPower_(5);
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            SetCollBodyPushRank_(1);
            PlayVoice(0x92, 0);
            pWork->m_bBack = 0;
            AnimPlay(3, Util_GetDamageMotID(4, 0, 0), 0, 0.0f, 0.0f);
            m_fJumpVelX = 0.0f;
            m_fJumpVelY = 0.0f;
            m_fJumpVelZ = 0.0f;
            m_fJumpAccelY = -0.025f;
            DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
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
        case 17:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorReceiveDmg_(pWork);
            break;
        case 18:
            SetCondition(0, 0.0f, 0);
            SubStateDamageCtorSpikeMiss_(pWork);
            break;
    }
    CGame::SendMessage(0x63, 1, GetPlayerNo(), 0);
}

void msw::CChrCoreBoss::LoopStateDamage()
{
    STATE_WORK_DAMAGE *pWork = FsmqGetWork<STATE_WORK_DAMAGE>();

    // After time over the character gets up again once its down time has
    // run out, with some life back.
    if ((m_nStatusFlag & 0x80000) && !(CGame::g_GameWork.m_nFlag18 & 0x4306) && CGame::IsMainLoopState(1)) {
        m_fDownTime -= GetFrameStep();
        if (m_nComLevel == 1) {
            m_fDownTime = 0.0f;
        }
        if (m_fDownTime <= 0.0f) {
            pWork->m_fDownTime = 0.0f;
            m_fDownTime = 0.0f;
            m_nStatusFlag &= ~0x80000;
            if (m_fHP < 15.0f) {
                m_fHP = 15.0f;
            }
            m_fHP = m_fHPMax > m_fHP ? m_fHP : m_fHPMax;
        }
    }

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
        case 17:
            bEnd = SubStateDamageLoopReceiveDmg_(pWork);
            break;
        case 18:
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

void msw::CChrCoreBoss::DtorStateDamage()
{
    Eff_StopGroup(3);
    m_nStatusFlag &= ~0x40000000;
    m_UnkEC &= ~0x4;
}

void msw::CChrCoreBoss::SubStateDamageCtorMDmg_(STATE_WORK_DAMAGE *pWork)
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

    float fSpeed = 0.075f;
    if (pWork->m_bBack) {
        fSpeed *= -1.0f;
    }
    bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
    m_fJumpVelY = 0.2f;
    m_fJumpAccelY = -0.025f;
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

int msw::CChrCoreBoss::SubStateDamageLoopMDmg_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    switch (pWork->m_nStep) {
        case 0:
            if (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                long nMot = Util_GetDamageMotID(0, pWork->m_bBack, 1);
                m_fJumpUnk220 = 9999.9f;
                Util_SetEffect(static_cast<SET_EFFECT_TYPE>(1));
                AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                m_UnkEC |= 0x10;
                SetCollBodyPushPower_(5);
                SetCollBodyPushRank_(1);
                pWork->m_nStep++;
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
                float fStep = m_fFrameStep * GetCcParam_()->m_Unk1A8;
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

void msw::CChrCoreBoss::SubStateDamageCtorLDmg_(STATE_WORK_DAMAGE *pWork)
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

    float fSpeed = 0.15f;
    if (pWork->m_bBack) {
        fSpeed *= -1.0f;
    }
    bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
    m_fJumpVelY = 0.3f;
    m_fJumpAccelY = -0.025f;
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

int msw::CChrCoreBoss::SubStateDamageLoopLDmg_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    switch (pWork->m_nStep) {
        case 0:
            if (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                long nMot = Util_GetDamageMotID(1, pWork->m_bBack, 1);
                m_fJumpUnk220 = 9999.9f;
                AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                SetCollBodyPushPower_(5);
                SetCollBodyPushRank_(1);
                m_UnkEC |= 0x10;
                pWork->m_nStep++;
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
                float fStep = m_fTimeScale * (m_fFrameStep * GetCcParam_()->m_Unk1A8);
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

void msw::CChrCoreBoss::SubStateDamageCtorDizzy_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    SetInvincible(0.0f);
    if ((unsigned char)(pWork->m_nFlag >> 8) == 12) {
        pWork->m_bAir = 1;
    }
    long nMsg = 10;
    if (pWork->m_bAir == 0) {
        m_UnkEC |= 0x10;
        SetCollBodyPushPower_(5);
        SetCollBodyPushRank_(1);
        switch ((pWork->m_nFlag >> 8) & 0xFF) {
            case 3:
                PlayVoice(0x8F, 0);
                break;
            case 4:
                PlayVoice(0x90, 0);
                nMsg = 5;
                break;
            case 6:
                PlayVoice(0x93, 0);
                break;
            default:
                PlayVoice(0x8D, 0);
                break;
        }
        pWork->m_nStep = 3;
        DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), nMsg, 1, 0, 0);
        return;
    }
    SubStateDamageCtorMDmg_(pWork);
}

int msw::CChrCoreBoss::SubStateDamageLoopDizzy_(STATE_WORK_DAMAGE *pWork)
{
    switch (pWork->m_nStep) {
        case 0:
            if (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                long nMot = Util_GetDamageMotID(0, pWork->m_bBack, 1);
                m_fJumpUnk220 = 9999.9f;
                Util_SetEffect(static_cast<SET_EFFECT_TYPE>(1));
                AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                m_UnkEC |= 0x10;
                SetCollBodyPushPower_(5);
                SetCollBodyPushRank_(1);
                pWork->m_nStep++;
            }
            break;
        case 1:
            m_fJumpVelX *= 0.875f;
            m_fJumpVelZ *= 0.875f;
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                long nMot = Util_GetDamageMotID(0, pWork->m_bBack, 2);
                m_nStatusFlag &= ~0xC00;
                AnimPlay(3, nMot, 0, 4.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (!IsAnimEnd(3)) {
                break;
            }
        default:
            // Dizzy from here on (the condition), with the weave of kind 5.
            if ((unsigned char)(pWork->m_nFlag >> 8) == 5) {
                SetCondition(7, 60.0f, 0);
                m_fWeaveSpeed = 0.034906585f;
                m_fWeaveWidth = 1.0471976f;
            } else {
                SetCondition(7, 60.0f, 0);
            }
            FsmqCutNext();
            return 1;
    }
    return 0;
}

void msw::CChrCoreBoss::SubStateDamageCtorBurn_(STATE_WORK_DAMAGE *pWork)
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
    long nMsg = 10;
    m_nStatusFlag |= 0x400;
    unsigned char nSub = pWork->m_nFlag >> 8;
    if (nSub == 9 || nSub == 0x18) {
        SetCondition(4, pWork->m_fDownTime, 0);
        nMsg = 8;
        m_fWeaveSpeed = 0.06981317f;
        m_fWeaveWidth = 0.7853982f;
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), nMsg, 1, 0, 0);
}

int msw::CChrCoreBoss::SubStateDamageLoopTumble_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    switch (pWork->m_nStep) {
        case 0:
            m_fJumpVelX *= 0.875f;
            m_fJumpVelZ *= 0.875f;
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                AnimPlay(3, Util_GetDamageMotID(4, pWork->m_bBack, 3), 1, 4.0f, 0.0f);
                pWork->m_nStep++;
            }
            break;
        case 1:
            if (!(m_nStatusFlag & 0x80000)) {
                float fStep = m_fTimeScale * (m_fFrameStep * GetCcParam_()->m_Unk1A8);
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

void msw::CChrCoreBoss::SubStateDamageCtorMame_(STATE_WORK_DAMAGE *pWork)
{
    CChrCore::SubStateDamageCtorMame_(pWork);
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

int msw::CChrCoreBoss::SubStateDamageLoopMame_(STATE_WORK_DAMAGE *pWork)
{
    return CChrCore::SubStateDamageLoopMame_(pWork);
}

void msw::CChrCoreBoss::SubStateDamageCtorReceiveDmg_(STATE_WORK_DAMAGE *pWork)
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
    AnimPlay(3, Util_GetDamageMotID(17, pWork->m_bBack, 0), 0, 4.0f, 0.0f);
    Eff_Stop(0);

    // Knocked back by the parameters' distance and height, per second.
    float fBack = -0.016666668f * GetCcParam_()->m_Unk1A0;
    float fUp = 0.016666668f * GetCcParam_()->m_Unk1A4;
    bcs::BCS_VecZ_RotY(GetJumpVel(), fBack, GetRotY());
    m_fJumpVelY = fUp;
    m_fJumpAccelY = -0.025f;
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 6, 1, 0, 0);
}

int msw::CChrCoreBoss::SubStateDamageLoopReceiveDmg_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    switch (pWork->m_nStep) {
        case 0:
            if (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                if (IsAnimEnd(3)) {
                    Eff_Stop(0);
                    SetCollBodyPushPower_(5);
                    SetCollBodyPushRank_(1);
                    m_UnkEC |= 0x10;
                    if (pWork->m_fDownTime > 0.0f) {
                        long nMot = Util_GetDamageMotID(17, pWork->m_bBack, 3);
                        m_fJumpUnk220 = 9999.9f;
                        AnimPlay(3, nMot, 1, 0.0f, 0.0f);
                        pWork->m_nStep = 1;
                    } else {
                        long nMot = Util_GetDamageMotID(17, pWork->m_bBack, 2);
                        m_fJumpUnk220 = 9999.9f;
                        AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                        AnimSetSpeed(3, 0.6f);
                        pWork->m_nStep = 2;
                    }
                } else {
                    m_fJumpVelX *= 0.95f;
                    m_fJumpVelZ *= 0.95f;
                }
            }
            break;
        case 1:
            float fStep = m_fTimeScale * (m_fFrameStep * GetCcParam_()->m_Unk1A8);
            pWork->m_fDownTime -= fStep;
            if (m_nComLevel == 1) {
                pWork->m_fDownTime = 0.0f;
            }
            if (!(pWork->m_fDownTime > 0.0f)) {
                long nMot = Util_GetDamageMotID(17, pWork->m_bBack, 2);
                m_fJumpUnk220 = 9999.9f;
                AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                AnimSetSpeed(3, 0.6f);
                pWork->m_nStep = 2;
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

void msw::CChrCoreBoss::SubStateDamageCtorSpikeMiss_(STATE_WORK_DAMAGE *pWork)
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
    AnimPlay(3, Util_GetDamageMotID(18, pWork->m_bBack, 0), 0, 0.0f, 0.0f);

    float fSpeed = 0.075f;
    if (pWork->m_bBack) {
        fSpeed *= -1.0f;
    }
    bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
    m_fJumpVelY = 0.2f;
    m_fJumpAccelY = -0.025f;
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

int msw::CChrCoreBoss::SubStateDamageLoopSpikeMiss_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    if (pWork->m_nStep == 0) {
        if (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
            long nMot = Util_GetDamageMotID(18, pWork->m_bBack, 2);
            m_fJumpUnk220 = 9999.9f;
            AnimPlay(3, nMot, 0, 0.0f, 0.0f);
            SetCollBodyPushPower_(5);
            SetCollBodyPushRank_(1);
            m_UnkEC |= 0x10;
            pWork->m_nStep++;
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

void msw::CChrCoreBoss::CtorStateDizzy()
{
    STATE_WORK_DIZZY *pWork = FsmqGetWork<STATE_WORK_DIZZY>();

    m_nStatusFlag &= ~0x40000000;
    m_UnkEC |= 0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushRank_(1);
    switch ((pWork->m_nFlag >> 8) & 0xFF) {
        case 3:
            AnimPlay(3, 0x84, 1, 8.0f, 0.0f);
            pWork->m_fTime = 1.0f;
            pWork->m_nType = 2;
            break;
        case 4:
            AnimPlay(3, 0x85, 1, 8.0f, 0.0f);
            pWork->m_fTime = 0.0f;
            break;
        case 5:
            AnimPlay(3, 0x86, 1, 8.0f, 0.0f);
            pWork->m_fTime = 0.0f;
            break;
        case 12:
            AnimPlay(3, 0x83, 1, 8.0f, 0.0f);
            pWork->m_fTime = 0.0f;
            break;
        default:
            pWork->m_fTime = 60.0f;
            AnimPlay(3, 0x86, 1, 8.0f, 0.0f);
            break;
    }
}

void msw::CChrCoreBoss::LoopStateDizzy()
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

void msw::CChrCoreBoss::DtorStateDizzy()
{
    Eff_StopGroup(2);
    SetCondition(0, 0.0f, 0);
}
