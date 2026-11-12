// msw::CChrCoreBasket's damage state (with the motion of each kind of
// damage), its dizzy state, the fall into the river of the river court and
// the paralysis effect. Written after BossF's game/boss/unit_1C248.cpp, the
// same states of CChrCoreBoss.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CGameBasket.hpp"
#include "nw4r/math/triangular.h"

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it (see CBallBasketFruit.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

// The damage state's work (CChrCore's; names guessed).
struct msw::CChrCore::STATE_WORK_DAMAGE {
    long m_nStep;             // 0x00
    unsigned long m_nSubFlag; // 0x04: bit 0 once the landing effect is out
    // 0x08: the damage flags; the low byte is the kind of damage (see
    // CtorStateDamage), bits 8-15 a sub kind.
    unsigned long m_nFlag;
    long m_nType;             // 0x0C: the kind of damage, from m_nFlag
    int m_bAir;               // 0x10: hit in the air
    float m_fRotY;            // 0x14: the direction the hit came from
    int m_bBack;              // 0x18: hit from behind (selects the motion)
    float m_fSpeed;           // 0x1C: the speed of the knock back (0: the default)
    float m_fTime;            // 0x20: frames of the spin (kind 5)
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

    // The dizzy state's work (names guessed).
    struct STATE_WORK_DIZZY {
        long m_nStep;          // 0x00
        unsigned long m_nFlag; // 0x04: bits 8-15 the kind of dizziness
        long m_nType;          // 0x08: 1 can be shaken off, 2 a fixed time
        float m_fTime;         // 0x0C: frames left
    };

    // (guessed helper) fBase plus a random part of fRange.
    inline float RandAdd(float fBase, float fRange)
    {
        float fRand = bcs::BCS_RandMax(1.0f);
        return fBase + fRand * fRange;
    }

    // The fall state's work (names guessed).
    struct STATE_WORK_FALL {
        unsigned long m_nFlag; // 0x00: bit 0 once the character is under water
        long m_nStep;          // 0x04
    };

    // The state the dizzy state is (the table's entry).
    const long STATE_DIZZY = 16;
}

int msw::CChrCoreBasket::Util_GetDamageMotID(long nType, long nBack, long nPart)
{
    // The damage motions by kind of damage: for a hit from the front and
    // from behind, the start, the fall, the getting up and the lying down
    // (-1 for none).
    static const short _ciDmgMotID[17][2][4] = {
        { { 12, 13, 15, 14 }, { 20, 21, 23, 22 } },
        { { 16, 17, 19, 18 }, { 24, 25, 27, 26 } },
        { { -1, -1, -1, -1 }, { -1, -1, -1, -1 } },
        { { 28, 29, 15, -1 }, { 28, 29, 15, -1 } },
        { { 37, 37, 39, 38 }, { 37, 37, 39, 38 } },
        { { 31, 31, 31, -1 }, { 31, 31, 31, -1 } },
    };

    return _ciDmgMotID[nType][nBack][nPart];
}

void msw::CChrCoreBasket::CtorStateDamage()
{
    STATE_WORK_DAMAGE *pWork = FsmqGetWork<STATE_WORK_DAMAGE>();

    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(0);
    long nType = pWork->m_nFlag & 0xFF;
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
            SubStateDamageCtorTumble_(pWork);
            break;
        case 3:
            SubStateDamageCtorBurn_(pWork);
            break;
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
    }
}

void msw::CChrCoreBasket::LoopStateDamage()
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
                    long nMot = Util_GetDamageMotID(3, pWork->m_bBack, 1);
                    m_fJumpUnk220 = 9999.9f;
                    AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                    Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(1));
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

void msw::CChrCoreBasket::DtorStateDamage()
{
    Eff_Stop(0);
    SetRiverFallEnable(0);
    m_nBasketFlag &= ~0x10000;
    m_nStatusFlag &= ~0x40000000;
    m_UnkEC &= ~0x4;
}

void msw::CChrCoreBasket::SubStateDamageCtorMDmg_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(7), 8.0f);
    SetCollBodyPushRank_(4);
    if ((unsigned char)(pWork->m_nFlag >> 8) == 4) {
        PlayVoice(0x90, 0);
    } else {
        PlayVoice(0x8A, 0);
    }

    float fRot = WrapRad(GetRotY() - pWork->m_fRotY);
    if (fRot >= 1.5707964f && fRot <= 4.712389f) {
        SetRotY(WrapRad(3.1415927f + pWork->m_fRotY));
        pWork->m_bBack = 1;
    } else {
        SetRotY(WrapRad(pWork->m_fRotY));
        pWork->m_bBack = 0;
    }
    AnimPlay(3, Util_GetDamageMotID(0, pWork->m_bBack, 0), 0, 0.0f, 0.0f);

    float fSpeed = pWork->m_fSpeed;
    if (fSpeed <= 0.0f) {
        fSpeed = 0.075f;
    }
    if (pWork->m_bBack) {
        fSpeed *= -1.0f;
    }
    bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
    m_fJumpVelY = 0.2f;
    m_fJumpAccelY = -0.025f;
    SetRiverFallEnable(1);
}

int msw::CChrCoreBasket::SubStateDamageLoopMDmg_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(60.0f);
    switch (pWork->m_nStep) {
        case 0: {
            int nMove = Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (nMove == 1) {
                long nMot = Util_GetDamageMotID(0, pWork->m_bBack, 1);
                m_fJumpUnk220 = 9999.9f;
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(1));
                AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                m_UnkEC |= 0x10;
                SetCollBodyPushPower_(5);
                SetCollBodyPushRank_(1);
                SetRiverFallEnable(0);
                pWork->m_nStep++;
            } else if (nMove == 2 && MethdFall()) {
                // Fell into the river.
                return 1;
            }
            break;
        }
        case 1:
            m_fJumpVelX *= 0.875f;
            m_fJumpVelZ *= 0.875f;
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                unsigned long nFlag = m_nStatusFlag;
                m_nStatusFlag = nFlag & ~0xC00;
                if (!(nFlag & 0x20000) && ((m_nStatusFlag & 0x80000) || pWork->m_fDownTime > 0.0f)) {
                    long nMot = Util_GetDamageMotID(0, pWork->m_bBack, 3);
                    if (nMot >= 0) {
                        AnimPlay(3, nMot, 1, 4.0f, 0.0f);
                        Eff_CreateAttach(6, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                        m_fDownFrame = pWork->m_fDownTime;
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
                pWork->m_fDownTime -= GetFrameStep();
                if (m_nComLevel == 1) {
                    pWork->m_fDownTime = 0.0f;
                }
                if (!(pWork->m_fDownTime > 0.0f)) {
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

void msw::CChrCoreBasket::SubStateDamageCtorLDmg_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(7), 8.0f);
    SetCollBodyPushRank_(4);
    if ((unsigned char)(pWork->m_nFlag >> 8) == 8) {
        PlayVoice(0x91, 0);
    } else {
        PlayVoice(0x8B, 0);
        PlaySePos(0xC59, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    }

    float fRot = WrapRad(GetRotY() - pWork->m_fRotY);
    if (fRot >= 1.5707964f && fRot <= 4.712389f) {
        SetRotY(WrapRad(3.1415927f + pWork->m_fRotY));
        pWork->m_bBack = 1;
    } else {
        SetRotY(WrapRad(pWork->m_fRotY));
        pWork->m_bBack = 0;
    }
    AnimPlay(3, Util_GetDamageMotID(1, pWork->m_bBack, 0), 0, 0.0f, 0.0f);

    float fSpeed = pWork->m_fSpeed;
    if (fSpeed <= 0.0f) {
        fSpeed = 0.15f;
    }
    if (pWork->m_bBack) {
        fSpeed *= -1.0f;
    }
    bcs::BCS_VecZ_RotY(GetJumpVel(), fSpeed, GetRotY());
    m_fJumpVelY = 0.3f;
    m_fJumpAccelY = -0.025f;
    SetRiverFallEnable(1);
}

int msw::CChrCoreBasket::SubStateDamageLoopLDmg_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    switch (pWork->m_nStep) {
        case 0: {
            int nMove = Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (nMove == 1) {
                long nMot = Util_GetDamageMotID(1, pWork->m_bBack, 1);
                m_fJumpUnk220 = 9999.9f;
                AnimPlay(3, nMot, 0, 0.0f, 0.0f);
                SetCollBodyPushPower_(5);
                SetCollBodyPushRank_(1);
                m_UnkEC |= 0x10;
                SetRiverFallEnable(0);
                pWork->m_nStep++;
            } else if (nMove == 2 && MethdFall()) {
                // Fell into the river.
                return 1;
            }
            break;
        }
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
                    Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(2));
                    pWork->m_nSubFlag |= 1;
                }
            }
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                unsigned long nFlag = m_nStatusFlag;
                m_nStatusFlag = nFlag & ~0xC00;
                if ((nFlag & 0x80000) || pWork->m_fDownTime > 0.0f) {
                    AnimPlay(3, Util_GetDamageMotID(1, pWork->m_bBack, 3), 1, 4.0f, 0.0f);
                    Eff_CreateAttach(0, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                }
                pWork->m_nStep = 2;
            }
            break;
        case 2:
            if (m_nStatusFlag & 0x80000) {
                break;
            }
            float fStep = m_fFrameStep * m_fTimeScale;
            pWork->m_fDownTime -= fStep;
            if (m_nComLevel == 1) {
                pWork->m_fDownTime = 0.0f;
            }
            if (pWork->m_fDownTime > 0.0f) {
                break;
            }
            Eff_Stop(0);
            pWork->m_nStep++;
        case 3:
            // Getting up.
            AnimPlay(3, Util_GetDamageMotID(1, pWork->m_bBack, 2), 0, 4.0f, 0.0f);
            AnimSetSpeed(3, 1.0f);
            pWork->m_nStep++;
        default:
            if (IsAnimEnd(3)) {
                return 1;
            }
            break;
    }
    return 0;
}

void msw::CChrCoreBasket::SubStateDamageCtorDizzy_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    SetInvincible(0.0f);
    if (pWork->m_bAir == 0) {
        m_UnkEC |= 0x10;
        SetCollBodyPushPower_(5);
        SetCollBodyPushRank_(1);
        AnimPlay(3, Util_GetDamageMotID(2, 0, 0), 0, 0.0f, 0.0f);
        switch ((pWork->m_nFlag >> 8) & 0xFF) {
            case 3:
                SetInvincible(30.0f);
                PlayVoice(0x8F, 0);
                break;
            case 4:
                PlayVoice(0x90, 0);
                break;
            case 6:
                PlayVoice(0x93, 0);
                break;
            case 5:
                SubStateDamageCtorLDmg_(pWork);
                return;
            default:
                PlayVoice(0x8D, 0);
                break;
        }
        pWork->m_nStep = 4;
    } else {
        SubStateDamageCtorLDmg_(pWork);
    }
}

int msw::CChrCoreBasket::SubStateDamageLoopDizzy_(STATE_WORK_DAMAGE *pWork)
{
    switch (pWork->m_nStep) {
        case 0:
            if (Util_DoMove(m_fTimeScale * m_fFrameStep)) {
                long nMot = Util_GetDamageMotID(1, pWork->m_bBack, 1);
                m_fJumpUnk220 = 9999.9f;
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(1));
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
            if (!IsAnimEnd(3)) {
                break;
            }
            if (pWork->m_fDownTime > 0.0f) {
                AnimPlay(3, Util_GetDamageMotID(1, pWork->m_bBack, 3), 1, 4.0f, 0.0f);
                Eff_CreateAttach(6, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                m_fDownFrame = pWork->m_fDownTime;
            } else {
                m_fDownFrame = 0.0f;
            }
            m_nStatusFlag &= ~0xC00;
            pWork->m_nStep++;
        case 2:
            m_fDownFrame -= GetFrameStep();
            if (m_fDownFrame > 0.0f) {
                break;
            }
            AnimPlay(3, Util_GetDamageMotID(1, pWork->m_bBack, 2), 0, 4.0f, 0.0f);
            pWork->m_nStep++;
        case 3:
            if (!IsAnimEnd(3)) {
                break;
            }
            pWork->m_nStep++;
        default:
            // Dizzy from here on: by the sub kind, a dizzy state of its own
            // or a condition.
            switch ((pWork->m_nFlag >> 8) & 0xFF) {
                case 3: {
                    FsmqCutNext();
                    STATE_WORK_DIZZY *pDizzy = static_cast<STATE_WORK_DIZZY *>(FsmqPushState(STATE_DIZZY));
                    pDizzy->m_nFlag = pWork->m_nFlag;
                    pDizzy->m_nType = 2;
                    pDizzy->m_fTime = 9999.0f;
                    SetCondition(5, 9999.0f, 0);
                    return 1;
                }
                case 4:
                    SetCondition(6, 100.0f, 0);
                    break;
                case 5:
                    SetCondition(7, 150.0f, 0);
                    break;
                case 8:
                    SetCondition(8, 70.0f, 0);
                    break;
                default:
                    SetCondition(7, 300.0f, 0);
                    break;
            }
            FsmqCutNext();
            return 1;
    }
    return 0;
}

void msw::CChrCoreBasket::SubStateDamageCtorBurn_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(4);
    PlayVoice(0x8E, 0);
    AnimPlay(3, Util_GetDamageMotID(3, 0, 0), 0, 0.0f, 0.0f);
    float fY = GetPosY();
    Util_SetJumpParam(48.0f, fY, fY, 2.5f + fY);
    m_nStatusFlag |= 0x400;
    float fTime;
    if (pWork->m_nFlag == 0x1603) {
        fTime = pWork->m_fDownTime;
    } else {
        fTime = 180.0f;
    }
    SetCondition(4, fTime, 0);
    m_fWeaveSpeed = 0.06981317f;
    m_fWeaveWidth = 0.7853982f;
}

void msw::CChrCoreBasket::SubStateDamageCtorTumble_(STATE_WORK_DAMAGE *pWork)
{
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(4);
    PlayVoice(0x92, 0);
    pWork->m_bBack = 0;
    AnimPlay(3, Util_GetDamageMotID(4, 0, 0), 0, 0.0f, 0.0f);
    switch ((pWork->m_nFlag >> 8) & 0xFF) {
        case 7:
            Eff_CreateAttach(10, 0x584, m_nEffNodeSub2, 0, 0, 3, 0);
            break;
        case 2:
            PlaySePos(0xC56, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            break;
    }
    m_fJumpVelX = 0.0f;
    m_fJumpVelY = 0.0f;
    m_fJumpVelZ = 0.0f;
    m_fJumpAccelY = -0.025f;
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 11, 1, 0, 0);
}

int msw::CChrCoreBasket::SubStateDamageLoopTumble_(STATE_WORK_DAMAGE *pWork)
{
    SetInvincible(30.0f);
    switch (pWork->m_nStep) {
        case 0:
            m_fJumpVelX *= 0.875f;
            m_fJumpVelZ *= 0.875f;
            Util_DoMove(m_fTimeScale * m_fFrameStep);
            if (IsAnimEnd(3)) {
                AnimPlay(3, Util_GetDamageMotID(4, pWork->m_bBack, 3), 1, 4.0f, 0.0f);
                if (pWork->m_fDownTime > 0.0f) {
                    Eff_CreateAttach(6, 0xE, m_nEffNodeSub, 0, 0, 3, 0);
                    m_fDownFrame = pWork->m_fDownTime;
                }
                pWork->m_nStep++;
            }
            break;
        case 1:
            if (!(m_nStatusFlag & 0x80000)) {
                pWork->m_fDownTime -= GetFrameStep();
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

void msw::CChrCoreBasket::CtorStateDizzy()
{
    STATE_WORK_DIZZY *pWork = FsmqGetWork<STATE_WORK_DIZZY>();

    m_nStatusFlag &= ~0x40000000;
    m_UnkEC |= 0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushRank_(1);
    switch ((pWork->m_nFlag >> 8) & 0xFF) {
        case 3:
            AnimPlay(3, 0x21, 1, 8.0f, 0.0f);
            break;
        case 4:
            AnimPlay(3, 0x22, 1, 8.0f, 0.0f);
            pWork->m_fTime = 100.0f;
            break;
        case 5:
            AnimPlay(3, 0x23, 1, 8.0f, 0.0f);
            pWork->m_fTime = 150.0f;
            break;
        case 8:
            AnimPlay(3, 0x23, 1, 8.0f, 0.0f);
            pWork->m_fTime = 70.0f;
            break;
        default:
            AnimPlay(3, 0x23, 1, 8.0f, 0.0f);
            pWork->m_fTime = 300.0f;
            break;
    }
}

void msw::CChrCoreBasket::LoopStateDizzy()
{
    STATE_WORK_DIZZY *pWork = FsmqGetWork<STATE_WORK_DIZZY>();

    if (pWork->m_fTime > 0.0f) {
        // Kind 3 lasts while an arts event plays.
        if ((unsigned char)(pWork->m_nFlag >> 8) == 3 && !(CChrManBasket::GetInstance()->GetFlagAC() & 2)) {
            pWork->m_fTime = 0.0f;
        }
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

void msw::CChrCoreBasket::DtorStateDizzy()
{
    Eff_StopGroup(2);
    SetCondition(0, 0.0f, 0);
}

void msw::CChrCoreBasket::CtorStateFall()
{
    STATE_WORK_FALL *pWork = FsmqGetWork<STATE_WORK_FALL>();

    pWork->m_nFlag = 0;
    pWork->m_nStep = 0;
    m_nStatusFlag |= 0x40000000;
    if (IsHoldBall()) {
        DetachBall();
    }
    SetRiverFallEnable(1);
    CChrManBasket::ClearPlayerIcon(this, 1);
}

void msw::CChrCoreBasket::LoopStateFall()
{
    STATE_WORK_FALL *pWork = FsmqGetWork<STATE_WORK_FALL>();

    if (pWork->m_nStep == 0) {
        // Falling into the water.
        float fStep = m_fTimeScale * m_fFrameStep;
        if (m_fJumpAccelY >= 0.0f) {
            m_fJumpAccelY = -0.025f;
        }
        bcs::FVEC3 move(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 step;
        float fVelAdd = m_fJumpAccelY * fStep;
        bcs::FVEC3Scale(&step, &GetJumpVel(), fStep);
        m_fJumpVelY += fVelAdd;
        step.y += 0.5f * (fVelAdd * fStep);
        bcs::FVEC3Add(&move, &move, &step);
        ClearRevise();
        AddPos(move);
        if (!(pWork->m_nFlag & 1) && GetPos().y < -3.0f) {
            // Under water: the splash, and the team restarts.
            PlaySePos(0xD29, GetPos(), 0, -1, 0.0f, 1.0f, 0.0f);
            SetCondition(0, 0.0f, 0);
            Eff_CreateSimple(0x14C, GetPos(), 3, 0, 0);
            CGameBasket::GetInstance()->SetRiverFallRestart_(GetTeamNo());
            pWork->m_nFlag |= 1;
        }
        if (!(GetPos().y > -7.0f)) {
            SetPosY(-7.0f);
            pWork->m_nStep++;
        }
    }
    if (pWork->m_nFlag & 1) {
        SetEvtHidden(1, 1);
    }
}

void msw::CChrCoreBasket::DtorStateFall()
{
    m_nStatusFlag &= ~0x1C00;
    SetRiverFallEnable(0);
    SetEvtHidden(0, 1);
}

void msw::CChrCoreBasket::CtorStateRebirth()
{
    AnimPlay(3, 1, 1, 0.0f, 0.0f);
    SetPos(bcs::FVEC3(0.0f, 0.0f, 0.0f));
    SetRiverFallEnable(0);
}

void msw::CChrCoreBasket::LoopStateRebirth() {}

void msw::CChrCoreBasket::DtorStateRebirth() {}

float msw::CChrCoreBasket::SubStateParalyzeSetEffect_()
{
    AnimPlay(3, 0x24, 1, 8.0f, 0.0f);
    Eff_CreateAttach(10, 0x59F, 0, 0, 0, 3, 0);
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

void msw::CChrCoreBasket::SubStateParalyzeEndEffect_()
{
    Eff_Stop(10);
    if (m_nComLevel == 9) {
        m_fUnk248 = RandAdd(25.0f, 0.0f);
    }
}
