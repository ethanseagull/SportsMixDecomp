// msw::CChrCoreBoss: the Canbe* (may the character start an action now?) and
// Methd* (start it) members of the boss-mode character core.
//
// A Canbe* function answers whether the action may start; the Methd*
// function asks it (through the vtable, so a character may override it)
// and then puts the action's state at the head of the state queue
// (FsmqSetState), filling in the new state's work.
#include "game/msw/CChrCoreBoss.hpp"

#include <stdlib.h>

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItem.hpp"

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

    // The parts of the states' work this file writes or reads (names
    // guessed; the states themselves are in other units).

    // STATE_NEUTRAL
    struct WORK_NEUTRAL {
        long m_nStep;
        long m_nType;  // 0x04: 1, or 2 and 3 (running on), passed to the run
        long m_nPose;  // 0x08: kept by a receive that starts from here
    };

    // STATE_RUN
    struct WORK_RUN {
        long m_nStep;
        long m_nType;            // 0x04
        int m_bStart;            // 0x08
        int m_bTurn;             // 0x0C
        long m_nType2;           // 0x10
        float m_fRotY;           // 0x14: the direction to run in
        float m_fStartRotY;      // 0x18
        float m_fFrame;          // 0x1C
        float m_fSpeedRate;      // 0x20
        unsigned char m_Unk24[0x28 - 0x24];
        long m_nUnk28;           // 0x28
        long m_nUnk2C;           // 0x2C
    };

    // STATE_DAMAGE (CChrCore::STATE_WORK_DAMAGE)
    struct WORK_DAMAGE {
        long m_nStep;
        unsigned char m_Unk04[0x08 - 0x04];
        unsigned long m_nFlag;   // 0x08: the damage flags (type in bits 8-15)
        unsigned char m_Unk0C[0x10 - 0x0C];
        int m_bAir;              // 0x10: hit in the air
        float m_fRotY;           // 0x14
        unsigned char m_Unk18[0x1C - 0x18];
        float m_fDamage;         // 0x1C
        float m_fInvincible;     // 0x20
        float m_fSizeTime;       // 0x24
        long m_nQuake;           // 0x28
        float m_fUnk2C;          // 0x2C
        short m_nUnk30;          // 0x30
        short m_nUnk32;          // 0x32
    };

    // STATE_SIDE_STEP (CChrCoreBoss::STATE_WORK_SIDESTEP, defined with the
    // state in game/boss/unit_636D4.cpp)
    struct WORK_SIDE_STEP {
        unsigned char m_Unk00[0x08];
        float m_fStartRotY;      // 0x08
        float m_fRotY;           // 0x0C
        float m_fSideRotY;       // 0x10
        long m_nUnk14;           // 0x14
    };

    // STATE_MOT_PLAY
    struct WORK_MOT_PLAY {
        long m_nStep;
        unsigned long m_nFlag;   // 0x04
        long m_nMot;             // 0x08: the motion to play
        long m_nNextMot;         // 0x0C: the motion after it
        long m_nVoice;           // 0x10: -1 for none
        long m_nUnk14;           // 0x14
        unsigned char m_Unk18[0x1C - 0x18];
        long m_nUnk1C;           // 0x1C
    };

    // STATE_SIZE_CHANGE
    struct WORK_SIZE_CHANGE {
        long m_nStep;
        long m_nType;            // 0x04: 0 back to normal, 1 shrinking
    };

    // STATE_MAME_CHANGE
    struct WORK_MAME_CHANGE {
        long m_nStep;            // 0x00: 2 while the bean moves about
        unsigned char m_Unk04[0x08 - 0x04];
        float m_fStartRotY;      // 0x08
        float m_fRotY;           // 0x0C
        unsigned char m_Unk10[0x14 - 0x10];
        long m_nUnk14;           // 0x14
    };

    // STATE_SHOOT_STANCE, STATE_JUMP_SHOOT (CChrCoreBoss::STATE_WORK_JUMP_SHOOT
    // in game/boss/unit_636D4.cpp) and STATE_ITEM_SHOOT
    struct WORK_SHOOT {
        long m_nStep;
        float m_fRotY;           // 0x04
    };

    // STATE_PASS
    struct WORK_PASS {
        unsigned char m_Unk00[0x14];
        float m_fRotY;           // 0x14: towards the receiver
    };

    // STATE_AIR_TRICK (CChrCoreBoss::STATE_WORK_AIR_TRICK)
    struct WORK_AIR_TRICK {
        long m_nNo;              // 0x00: 0 for the leader, then 1, 2, ...
        long m_nNum;             // 0x04: characters in the air trick
        float m_fWait;           // 0x08
    };

    // The motion play flags (names guessed).
    enum {
        MOT_PLAY_FLAG_GAME_RESULT = 0x42,
        MOT_PLAY_FLAG_GAME_SET = 0x43,
        MOT_PLAY_FLAG_TURN_OVER = 0x2
    };
}

int msw::CChrCoreBoss::CanbeChgLeaderState()
{
    if (GetState() == STATE_AIR_TRICK) {
        return 0;
    }
    return 1;
}

int msw::CChrCoreBoss::CanbeChgNewLeaderState()
{
    return 1;
}

int msw::CChrCoreBoss::CanbeChgLeader(CChrCore *pChr)
{
    if (!CGame::IsPlayTypeCanChangeLeader(m_nTeamNo)) {
        return 0;
    }
    if (pChr == 0) {
        return 0;
    }
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x100000) {
        return 0;
    }
    if (pChr == this) {
        return 0;
    }
    if (m_nTeamNo != pChr->GetTeamNo()) {
        return 0;
    }
    return CanbeChgLeaderState() != 0;
}

// The pad moves to the teammate whose player number is nearest to the one
// two places on (the current leader left out).
int msw::CChrCoreBoss::MethdChgLeader()
{
    if (!CGame::IsPlayTypeCanChangeLeader(m_nTeamNo)) {
        return 0;
    }
    if (!CanbeChgLeaderState()) {
        return 0;
    }

    CChrCore *pNew = 0;
    CChrCore *pLeader = CChrMan::GetTeamLeader(m_nTeamNo);
    long nTarget = (m_nPlayerNo + 2) % CChrMan::GetChrCoreNum();
    long nMinDist = 9999;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && !(pChr->GetStatusFlag() & 0x80000) &&
            pChr->GetTeamNo() == GetTeamNo() && pChr->GetPlayerNo() != GetPlayerNo() && pChr != pLeader) {
            long nDist = abs(pChr->GetPlayerNo() - nTarget);
            if (nMinDist >= nDist) {
                nMinDist = nDist;
                pNew = pChr;
            }
        }
    }

    if (!CanbeChgLeader(pNew)) {
        return 0;
    }
    CChrMan::GetInstance()->SetTeamLeader(m_nTeamNo, pNew->GetPlayerNo(), 0);
    DriverConnect(1, 0, 0, 0, 0);
    pNew->DriverConnect(0, 0, 0, 0, 0);
    pNew->OnFlagF4(0x100000);
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(5), 0, 0, 0, 0);
    pNew->DriverSendMessage(static_cast<CChrDrv::MSG_ID>(5), 0, 0, 0, 0);
    if (pNew->IsPadCtrl()) {
        long nSe;
        if (CGame::g_GameWork.m_nUnk00 != 0) {
            nSe = 0xC18;
            if (pNew->GetTeamNo() != 0) {
                nSe = 0xC17;
            }
        } else {
            nSe = 0xC17;
            if (pNew->GetTeamNo() != 0) {
                nSe = 0xC18;
            }
        }
        if (nSe >= 0) {
            PlaySePos(nSe, 0, 0, -1, 0.0f, 1.0f, 0.0f);
        }
    }
    CGame::SetCockpitFaceIcon();
    return 1;
}

int msw::CChrCoreBoss::MethdChgLeader(long, long, long)
{
    return 0;
}

int msw::CChrCoreBoss::CanbeShootStance()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (!CanbeReceiveCondition()) {
        return 0;
    }
    if (m_nStatusFlag & 3) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_SHOOT_STANCE:
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBoss::MethdShootStance(float fRotY, int)
{
    if (!CanbeShootStance()) {
        return 0;
    }

    // CanbeShootStance allows no other state, so pWork is always set (the
    // original has no default case either).
    WORK_SHOOT *pWork;
    switch (GetState()) {
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                // Still in the stance: only the direction changes.
                pWork = FsmqGetWork<WORK_SHOOT>();
                break;
            }
            pWork = static_cast<WORK_SHOOT *>(FsmqSetState(STATE_SHOOT_STANCE));
            break;
        case STATE_NEUTRAL:
        case STATE_RUN:
            AdjustOffsetPos_(1);
            pWork = static_cast<WORK_SHOOT *>(FsmqSetState(STATE_SHOOT_STANCE));
            break;
    }
    if (fRotY < 0.0f) {
        fRotY = GetRotY();
    }
    pWork->m_fRotY = fRotY;
    return 1;
}

int msw::CChrCoreBoss::CanbeSideStep(float, float)
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 2) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (m_nStatusFlag & 3) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
            break;
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    return 1;
}

// fStickRotY: the direction of the stick; fRotY: the direction faced,
// negative for the present one.
int msw::CChrCoreBoss::MethdSideStep(float fStickRotY, float fRotY)
{
    if (!CanbeSideStep(fStickRotY, fRotY)) {
        return 0;
    }
    if (fRotY < 0.0f) {
        fRotY = GetRotY();
    }
    // (not used) The stick's direction relative to the one faced.
    float fRelRotY = WrapRad(fRotY - fStickRotY);
    float fSideRotY = WrapRad(1.5707964f + fRotY);
    SetRotY(fRotY);

    WORK_SIDE_STEP *pWork = static_cast<WORK_SIDE_STEP *>(FsmqSetState(STATE_SIDE_STEP));
    pWork->m_fStartRotY = GetRotY();
    pWork->m_fRotY = WrapRad(fRotY);
    pWork->m_fSideRotY = WrapRad(fSideRotY);
    pWork->m_nUnk14 = 0;
    return 1;
}

int msw::CChrCoreBoss::CanbeRun()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (!IsBossServeMode() && (m_nStatusFlag & 1)) {
        return 0;
    }
    if (!CanbeRunCondition()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
            break;
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                return 0;
            }
            break;
        case STATE_RECEIVE:
            // A receive of kind 4 may be left.
            if (FsmqGetWork<STATE_WORK_RECEIVE>()->m_nKind == 4) {
                break;
            }
        default:
            return 0;
    }
    return 1;
}

// Conditions 2, 3 and 5 keep the character from running.
int msw::CChrCoreBoss::CanbeRunCondition()
{
    switch (m_nComLevel) {
        case 2:
        case 3:
        case 5:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBoss::MethdRun(float fRotY)
{
    return MethdRun(fRotY, 1.0f);
}

int msw::CChrCoreBoss::MethdRun(float fRotY, float fSpeedRate)
{
    if (!CanbeRun()) {
        return 0;
    }

    long nType = 0;
    long nType2 = 0;
    WORK_RUN *pWork;
    switch (GetState()) {
        case STATE_NEUTRAL:
            switch (FsmqGetWork<WORK_NEUTRAL>()->m_nType) {
                case 1:
                    nType = 1;
                    break;
                case 2:
                case 3:
                    nType = 1;
                    nType2 = 1;
                    break;
            }
        case STATE_RECEIVE:
            AdjustOffsetPos_(1);
            pWork = static_cast<WORK_RUN *>(FsmqSetState(STATE_RUN));
            pWork->m_nType = nType;
            pWork->m_nType2 = nType2;
            pWork->m_fFrame = 0.0f;
            pWork->m_nStep = 0;
            pWork->m_fStartRotY = GetRotY();
            pWork->m_nUnk28 = -1;
            pWork->m_nUnk2C = -1;
            break;
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                return 0;
            }
            pWork = static_cast<WORK_RUN *>(FsmqSetState(STATE_RUN));
            pWork->m_nType = 0;
            pWork->m_nType2 = 0;
            pWork->m_fFrame = 0.0f;
            pWork->m_nStep = 0;
            pWork->m_fStartRotY = GetRotY();
            pWork->m_nUnk28 = -1;
            pWork->m_nUnk2C = -1;
            break;
        case STATE_RUN:
            pWork = FsmqGetWork<WORK_RUN>();
            break;
        default:
            return 0;
    }
    pWork->m_bStart = 1;
    pWork->m_fRotY = WrapRad(fRotY);
    pWork->m_fSpeedRate = fSpeedRate;
    pWork->m_bTurn = 1;
    return 1;
}

int msw::CChrCoreBoss::CanbeSetMoveRevise()
{
    if (IsDisable()) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
        return 0;
    }
    if (GetState() != STATE_JUMP) {
        return 0;
    }
    return !(m_UnkEC & 0x0A000000);
}

// Steers a jump towards fRotY.
int msw::CChrCoreBoss::MethdSetMoveRevise(float fRotY)
{
    if (!CanbeSetMoveRevise()) {
        return 0;
    }
    if (GetState() == STATE_JUMP) {
        m_fJumpRotY = WrapRad(fRotY);
        return 1;
    }
    return 0;
}

int msw::CChrCoreBoss::MethdDamage(unsigned long nFlag, float fRotY)
{
    return MethdDamage(nFlag, fRotY, 0.0f, 0.0f, -1, 0.0f, 0, 0);
}

// nFlag: the kind of damage in the low byte (2: a condition, 0xD keeps the
// size change), the type in the next (7: turns the character into a bean);
// fDamage is taken off the life, fSizeTime is how long a size change from
// bit 0x40000 lasts; nQuake is a camera quake (negative for none).
int msw::CChrCoreBoss::MethdDamage(unsigned long nFlag, float fRotY, float fDamage, float fSizeTime, long nQuake,
                                   float fUnk, long nUnk1, long nUnk2)
{
    unsigned long nType = (nFlag >> 8) & 0xFF;
    if (nType == 7) {
        float fBeanRotY = GetRotY();
        if (GetState() == STATE_RUN) {
            fBeanRotY = FsmqGetWork<WORK_RUN>()->m_fRotY;
        }
        switch (GetState()) {
            case STATE_MAME_CHANGE:
                FsmqGetWork<WORK_MAME_CHANGE>()->m_nUnk14 = 1;
                break;
            case STATE_DEFAULT:
            case STATE_NEUTRAL:
            case STATE_JUMP:
                return 0;
            case STATE_RECEIVE:
                if (FsmqGetWork<STATE_WORK_RECEIVE>()->m_nKind == 1) {
                    break;
                }
            default:
                if (m_bCanMame && !(m_nStatusFlag & 0x1400)) {
                    WORK_MAME_CHANGE *pWork = static_cast<WORK_MAME_CHANGE *>(FsmqSetState(STATE_MAME_CHANGE));
                    pWork->m_fStartRotY = fBeanRotY;
                    pWork->m_fRotY = fBeanRotY;
                    pWork->m_nUnk14 = 1;
                    return 1;
                }
                break;
        }
        return 0;
    }

    int bAir = 0;
    m_fDamage = fDamage;
    float fInvincible = 60.0f;
    m_nStatusFlag &= ~0x800;
    if (m_nStatusFlag & 0x400) {
        bAir = 1;
    }
    switch (GetState()) {
        case STATE_REBOUND_DUNK:
        case STATE_LAY_UP:
        case STATE_ALLEYOOP:
        case STATE_AIR_TRICK:
        case STATE_PASS:
        case STATE_AERIAL_PASS:
        case STATE_ARTS_PASS:
        case STATE_SPIKE:
            Util_NodePatchWarpToTransNode();
            if (GetPosY() > 0.0f) {
                bAir = 1;
                m_nStatusFlag |= 0x400;
            } else if (GetPosY() < 0.0f) {
                SetPosY(0.0f);
            }
            break;
    }
    if (m_nStatusFlag & 2) {
        bcs::FVEC3 vel;
        BCS_VecZ_RotY(vel, 0.2f, GetRotY());
        vel.y = 0.1f;
        DropItem(vel, 1, 0);
    }
    if (m_nStatusFlag & 1) {
        DropBall_(GetRotY());
    }
    if (nQuake >= 0) {
        CGame::StartCameraQuake(nQuake, m_nPlayerNo, -1);
    }
    if (nFlag & 0x40000) {
        if (!MethdSizeChange(1, fSizeTime) && (m_nStatusFlag & 0x10000) && m_fSizeChangeTime < fSizeTime) {
            SetBodyScaleState(2, fSizeTime);
        }
        if ((nFlag & 0xFF) != 0xD) {
            fSizeTime = 0.0f;
        }
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0xA), nFlag, 0, 0, 0);

    if ((nFlag & 0xFF) == 2) {
        // A condition: when it is already on, it only starts again.
        float fFrame = 60.0f;
        long nCondition = 7;
        switch (nType) {
            case 3:
                fFrame = 9999.0f;
                nCondition = 5;
                break;
            case 4:
                nCondition = 6;
                break;
            case 5:
                nCondition = 7;
                break;
            case 6:
                nCondition = 9;
                break;
            case 12:
                nCondition = 7;
                fFrame = 60.0f * bcs::BCS_RandRange(3.0f, 5.0f);
                break;
        }
        if (nCondition == m_nComLevel) {
            SetCondition(nCondition, fFrame, 0);
            return 1;
        }
    } else {
        SetInvincible(30.0f);
        fInvincible = 30.0f;
    }

    WORK_DAMAGE *pWork = static_cast<WORK_DAMAGE *>(FsmqSetState(STATE_DAMAGE));
    pWork->m_nFlag = nFlag;
    pWork->m_bAir = bAir;
    pWork->m_fRotY = fRotY;
    pWork->m_fDamage = fDamage;
    pWork->m_fInvincible = fInvincible;
    pWork->m_nStep = 0;
    pWork->m_fSizeTime = fSizeTime;
    pWork->m_nQuake = nQuake;
    pWork->m_fUnk2C = fUnk;
    pWork->m_nUnk30 = nUnk1;
    pWork->m_nUnk32 = nUnk2;
    m_fDamageSizeTime = fSizeTime;
    m_nDamageQuake = nQuake;
    m_fDamageUnk1F8 = fUnk;
    m_nDamageUnk1FC = nUnk1;
    m_nDamageUnk1FE = nUnk2;
    m_fHP -= fDamage;
    if (m_fHP <= 0.0f) {
        m_fHP = 0.0f;
        m_fDownTime = 1200.0f;
        m_nStatusFlag |= 0x80000;
    }
    return 1;
}

// Sliding is how a bean (STATE_MAME_CHANGE) moves.
int msw::CChrCoreBoss::CanbeSliding()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (m_nComLevel == 1) {
        return 0;
    }
    if (GetState() == STATE_MAME_CHANGE && FsmqGetWork<WORK_MAME_CHANGE>()->m_nStep < 3) {
        return 1;
    }
    return 0;
}

int msw::CChrCoreBoss::MethdSliding(float fRotY)
{
    if (!CanbeSliding()) {
        return 0;
    }
    if (GetState() == STATE_MAME_CHANGE) {
        WORK_MAME_CHANGE *pWork = FsmqGetWork<WORK_MAME_CHANGE>();
        if (pWork->m_nStep == 2) {
            pWork->m_fRotY = WrapRad(fRotY);
        }
        return 1;
    }
    return 0;
}

// nType: 0 back to the normal size, 1 shrink.
int msw::CChrCoreBoss::CanbeSizeChange(long nType)
{
    if (IsDisable()) {
        return 0;
    }

    // The change running now (-1 for none).
    long nNow = -1;
    if (GetState() == STATE_SIZE_CHANGE) {
        nNow = FsmqGetWork<WORK_SIZE_CHANGE>()->m_nType;
    } else if (m_fScaleRate > 0.0f) {
        nNow = m_fScaleTo < 1.0f;
    }

    switch (nType) {
        case 0:
            if (nNow == nType) {
                return 0;
            }
            if (!(m_nStatusFlag & 0x8000) && !(m_nStatusFlag & 0x10000)) {
                return 0;
            }
            switch (GetState()) {
                case STATE_ITEM_BALL_ATTACK:
                case STATE_DAMAGE:
                case STATE_FREEZE:
                    return 0;
            }
            break;
        case 1:
            if (m_nStatusFlag & 0x8000) {
                return 0;
            }
            if (m_nStatusFlag & 0x20000) {
                return 0;
            }
            if ((m_nStatusFlag & 0x10000) && m_fSizeChangeTime > 0.0f) {
                return 0;
            }
            if (GetState() == STATE_FREEZE) {
                return 0;
            }
            break;
    }
    return 1;
}

int msw::CChrCoreBoss::MethdSizeChange(long nType, float fTime)
{
    if (!CanbeSizeChange(nType)) {
        return 0;
    }
    switch (nType) {
        case 0:
            m_fScaleFrom = m_fScaleBase;
            m_fScaleTo = 1.0f;
            m_fScaleRate = 1.0f;
            break;
        case 1:
            m_nStatusFlag &= ~0x1F000000;
            SetMoveMotType(0);
            DropBall_(GetRotY());
            PlaySePos(0xC45, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            SetBodyScaleState(2, 420.0f);
            m_fScaleFrom = m_fScaleBase;
            m_fScaleTo = 0.45f;
            m_fScaleRate = 1.0f;
            m_fSizeChangeTime = fTime;
            switch (GetState()) {
                case STATE_RUN:
                    if (GetStatusFlag() & 3) {
                        m_nStatusFlag &= ~0xC00;
                        FsmqClear();
                        SetPosY(0.0f);
                    }
                    break;
                case STATE_JUMP:
                case STATE_PASS_CATCH:
                    if (m_nStatusFlag & 0xC00) {
                        break;
                    }
                default:
                    m_nStatusFlag &= ~0xC00;
                    FsmqClear();
                    SetPosY(0.0f);
                    break;
            }
            if (m_nStatusFlag & 2) {
                CItem *pItem = GetAttachItem();
                DetachItem(0);
                if (pItem != 0) {
                    bcs::FVEC3 vel;
                    BCS_VecZ_RotY(vel, 0.025f, GetRotY());
                    pItem->DoDrop(vel, 0, 0);
                }
            }
            break;
    }
    return 1;
}

int msw::CChrCoreBoss::CanbeShoot()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            break;
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBoss::MethdShoot(float fRotY)
{
    if (!CanbeShoot()) {
        return 0;
    }
    WORK_SHOOT *pWork = static_cast<WORK_SHOOT *>(FsmqSetState(STATE_JUMP_SHOOT));
    if (fRotY < 0.0f) {
        pWork->m_fRotY = GetRotY();
    } else {
        pWork->m_fRotY = WrapRad(fRotY);
    }
    return 1;
}

int msw::CChrCoreBoss::CanbeItemShoot()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (!IsHaveStockItem()) {
        return 0;
    }
    if (!CanbeReceiveCondition()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            break;
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBoss::MethdItemShoot(float fRotY)
{
    if (!CanbeItemShoot()) {
        return 0;
    }
    WORK_SHOOT *pWork = static_cast<WORK_SHOOT *>(FsmqSetState(STATE_ITEM_SHOOT));
    if (fRotY < 0.0f) {
        pWork->m_fRotY = GetRotY();
    } else {
        pWork->m_fRotY = WrapRad(fRotY);
    }
    return 1;
}

int msw::CChrCoreBoss::CanbeChgStarState()
{
    if (IsDisable()) {
        return 0;
    }
    return !(m_nStatusFlag & 0x8000);
}

int msw::CChrCoreBoss::MethdChgStarState()
{
    if (!CanbeChgStarState()) {
        return 0;
    }
    SetCondition(1, 600.0f, 0);
    return 1;
}

// The reaction to a turn over: a motion for the team that gets the ball
// (nTeam) and another for the one that loses it.
int msw::CChrCoreBoss::MethdTurnOver(long nTeam)
{
    m_UnkEC |= 0xE0;
    m_UnkEC &= ~0x7F8000;
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }

    WORK_MOT_PLAY *pWork;
    switch (GetState()) {
        case STATE_DEFAULT:
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            if (m_nStatusFlag & 1) {
                return 0;
            }
            pWork = static_cast<WORK_MOT_PLAY *>(FsmqSetState(STATE_MOT_PLAY));
            break;
        default:
            // After the action now running.
            pWork = static_cast<WORK_MOT_PLAY *>(FsmqPushState(STATE_MOT_PLAY));
            break;
    }
    pWork->m_nFlag = 0;
    pWork->m_nFlag |= MOT_PLAY_FLAG_TURN_OVER;
    if (nTeam == m_nTeamNo) {
        pWork->m_nMot = 0x8F;
        pWork->m_nNextMot = 0x90;
        pWork->m_nVoice = 0x47;
        pWork->m_nUnk14 = -1;
        pWork->m_nUnk1C = 4;
    } else {
        pWork->m_nMot = 0x91;
        pWork->m_nNextMot = 0x92;
        pWork->m_nVoice = -1;
        pWork->m_nUnk14 = -1;
        pWork->m_nUnk1C = 3;
    }
    return 1;
}

int msw::CChrCoreBoss::MethdTimeOver(long)
{
    m_UnkEC |= 0xE0;
    m_UnkEC &= ~0x7F8000;
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }

    switch (GetState()) {
        case STATE_PASS_CATCH:
        case STATE_SDB_CATCH:
            // The action ends first.
            FsmqCutQueue();
            break;
        case STATE_DEFAULT:
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            FsmqClear();
            break;
    }
    WORK_MOT_PLAY *pWork = static_cast<WORK_MOT_PLAY *>(FsmqPushState(STATE_MOT_PLAY));
    pWork->m_nFlag = 0;
    pWork->m_nFlag |= MOT_PLAY_FLAG_TURN_OVER;
    pWork->m_nMot = 0x8B;
    pWork->m_nNextMot = 0x75;
    pWork->m_nVoice = 0x49;
    pWork->m_nUnk14 = -1;
    SetCondition(3, 0.0f, 0);
    return 1;
}

int msw::CChrCoreBoss::MethdGameSet()
{
    m_UnkEC |= 0xE0;
    m_UnkEC &= ~0x7F8000;
    ResetBodyColor_();
    ResetBodyScale_();
    if (m_nStatusFlag & 0x100) {
        m_nStatusFlag = 0x100;
    } else {
        m_nStatusFlag = 0;
    }
    m_pPassCheckChr = 0;
    SetBodyScaleState(0, 420.0f);
    SetSpecialState(0);
    SetInvincible(0.0f);
    SetNoPickupCount(0.0f);
    m_fUnk284 = 0.0f;

    WORK_MOT_PLAY *pWork = static_cast<WORK_MOT_PLAY *>(FsmqSetState(STATE_MOT_PLAY));
    pWork->m_nFlag = 0;
    pWork->m_nFlag |= MOT_PLAY_FLAG_GAME_SET;
    pWork->m_nMot = Util_GetIdleMotID();
    pWork->m_nNextMot = Util_GetIdleMotID();
    pWork->m_nVoice = -1;
    pWork->m_nUnk14 = -1;
    return 1;
}

int msw::CChrCoreBoss::MethdGameResult(int bWin)
{
    m_UnkEC |= 0xE0;
    m_UnkEC &= ~0x7F8000;
    ResetBodyColor_();
    ResetBodyScale_();
    if (m_nStatusFlag & 0x100) {
        m_nStatusFlag = 0x100;
    } else {
        m_nStatusFlag = 0;
    }
    m_pPassCheckChr = 0;
    SetBodyScaleState(0, 420.0f);
    SetSpecialState(0);
    SetInvincible(0.0f);
    SetNoPickupCount(0.0f);
    SetServeMode_(0);
    m_fUnk284 = 0.0f;

    WORK_MOT_PLAY *pWork = static_cast<WORK_MOT_PLAY *>(FsmqSetState(STATE_MOT_PLAY));
    pWork->m_nFlag = 0;
    pWork->m_nFlag |= MOT_PLAY_FLAG_GAME_RESULT;
    if (bWin) {
        pWork->m_nMot = 0x8F;
        pWork->m_nNextMot = 0x90;
        pWork->m_nVoice = 0x48;
        pWork->m_nUnk14 = -1;
    } else {
        pWork->m_nMot = 0x91;
        pWork->m_nNextMot = 0x92;
        pWork->m_nVoice = 0x49;
        pWork->m_nUnk14 = -1;
    }
    return 1;
}

// bAny: also when the ball is not one this character may touch now.
int msw::CChrCoreBoss::CanbeReceive(int bAny)
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (m_nStatusFlag & 0x1400) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (!CanbeReceiveCondition()) {
        return 0;
    }
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        return 0;
    }
    if (!pBall->CanbeVsBossReceive()) {
        return 0;
    }

    long nBallNo = 0;
    if (pBall != 0) {
        nBallNo = pBall->GetBallNo();
        if (pBall->CanbeVolleyPickup(m_nPlayerNo)) {
            return 0;
        }
        if (pBall->IsFlag1()) {
            return 0;
        }
        // Not the ball this character has just shot.
        if (pBall->GetShotTeamIdx() == GetTeamNo() && pBall->GetShotChrIdx() == GetPlayerNo()) {
            return 0;
        }
    }
    if (!bAny && !IsBossRightBallTouch(nBallNo)) {
        return 0;
    }

    switch (GetState()) {
        case STATE_NEUTRAL:
            break;
        case STATE_RUN:
        case STATE_TURN:
            if (!IsBossRightBallTouch(nBallNo)) {
                return 0;
            }
            if (SubStateReceiveChoiceProcType_(0) == 4) {
                return 0;
            }
            break;
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                return 0;
            }
            break;
        case STATE_RECEIVE:
            if (FsmqGetWork<STATE_WORK_RECEIVE>()->m_nKind == 4) {
                break;
            }
        default:
            return 0;
    }
    return 1;
}

// Conditions 2 to 6, 8 and 10 keep the character from receiving.
int msw::CChrCoreBoss::CanbeReceiveCondition() const
{
    switch (m_nComLevel) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 10:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBoss::CanbeDiveReceive()
{
    if (!CanbeReceive(0)) {
        return 0;
    }
    long nType = SubStateReceiveChoiceProcType_(0);
    return nType == 1;
}

int msw::CChrCoreBoss::MethdReceive(int bKeep, int bAny)
{
    return MethdReceive(bKeep, bAny, 12.566371f);
}

// fRotY: the direction to send the ball in; above 2 pi for none (the
// character's parameters may ask for that too), negative for "back".
int msw::CChrCoreBoss::MethdReceive(int bKeep, int bAny, float fRotY)
{
    if (!CanbeReceive(bAny)) {
        return 0;
    }

    int bPose = 0;
    long nPose;
    if (GetState() == STATE_NEUTRAL) {
        WORK_NEUTRAL *pNeutral = FsmqGetWork<WORK_NEUTRAL>();
        if (pNeutral->m_nType != 0) {
            bPose = 1;
            nPose = pNeutral->m_nPose;
        }
    }

    STATE_WORK_RECEIVE *pWork = static_cast<STATE_WORK_RECEIVE *>(FsmqSetState(STATE_RECEIVE));
    pWork->m_fTargetRotY = -1.0f;
    pWork->m_nReceiveType = -1;
    pWork->m_nFlickType = -1;
    pWork->m_fUnk40 = m_fUnk268;
    if (bKeep) {
        pWork->m_nFlag |= 2;
    }
    if (bPose) {
        pWork->m_nPose = nPose;
    }
    if (fRotY > 6.2831855f || GetCcParam_()->m_Unk8C != 0) {
        pWork->m_nFlag |= 0x2000;
        pWork->m_fTargetRotY = -1.0f;
    } else if (fRotY < 0.0f) {
        pWork->m_nFlag |= 0x1000;
        pWork->m_fTargetRotY = -1.0f;
    } else {
        pWork->m_fTargetRotY = WrapRad(fRotY);
    }
    return 1;
}

msw::CChrCoreBoss::STATE_WORK_RECEIVE *msw::CChrCoreBoss::SetStateReceive_(CBall *pBall, long nKind, long nPose)
{
    STATE_WORK_RECEIVE *pWork = static_cast<STATE_WORK_RECEIVE *>(FsmqSetState(STATE_RECEIVE));
    pWork->m_nKind = nKind;
    pWork->m_nPose = nPose;
    pWork->m_Dir = pBall->GetVelocity();
    pWork->m_fUnk38 = pWork->m_fUnk3C = 0.0f;
    pWork->m_fUnk40 = m_fUnk268;
    pWork->m_nFlag |= 0x30;
    SetReceiveEndureWork_(pBall, pWork);
    return pWork;
}

void msw::CChrCoreBoss::SetReceiveEndureWork_(CBall *pBall, STATE_WORK_RECEIVE *pWork)
{
    if (pBall == 0 || pWork == 0) {
        return;
    }
    pWork->m_fUnk50 = 0.0f;
    pWork->m_nReceiveType = -1;
    pWork->m_nFlickType = -1;
    if (pBall->IsArtsShootMode()) {
        pWork->m_fUnk40 = 0.0f;
        pWork->m_nReceiveType = 0x45;
        pWork->m_nFlickType = -1;
        pWork->m_nFlag |= 0x100;
    }
}

int msw::CChrCoreBoss::CanbeSpike(int bAny) const
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (m_nStatusFlag & 0x1400) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (!CanbeReceiveCondition()) {
        return 0;
    }
    if (m_nStatusFlag & 1) {
        return 0;
    }
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        return 0;
    }
    if (pBall->CanbeVolleyPickup(m_nPlayerNo)) {
        return 0;
    }
    if (pBall->IsFlag1()) {
        return 0;
    }
    if (pBall->IsFlag2()) {
        bAny = 1;
    }

    if (pBall->IsAttach()) {
        // (has no effect) Looks for the teammate holding the ball.
        long nTeam = m_nTeamNo;
        CChrCore *pHolder = 0;
        long nNum = CChrMan::GetChrCoreNum();
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && nTeam == pChr->GetTeamNo() && (pChr->GetStatusFlag() & 1) &&
                pBall->GetId() == pChr->GetBallHandle()) {
                pHolder = pChr;
                break;
            }
        }
    } else if (!bAny) {
        // The ball must come down high enough to be hit.
        float fHeight = 3.0f;
        float fBaseHeight = fHeight;
        bcs::FVEC3 ofs = m_SpikeOfs;
        float fFrame = 25.0f;
        if (IsChargeBall_(pBall)) {
            ofs = m_SpikeOfsCharge;
            fHeight = 3.2f;
            fFrame = 40.0f;
        }
        fHeight += GetCcParam_()->m_Unk44;
        CBall *pTarget = GetTargetBall_();
        if (pTarget != 0) {
            bcs::FVEC3 pos;
            pTarget->GetPosAfterFrame(pos, fFrame);
            if (pos.y - ofs.y < fHeight - fBaseHeight) {
                return 0;
            }
        }
    }

    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            break;
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                return 0;
            }
            break;
        case STATE_RECEIVE:
            if (FsmqGetWork<STATE_WORK_RECEIVE>()->m_nKind == 4) {
                break;
            }
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBoss::MethdSpike(int bAny)
{
    if (!CanbeSpike(bAny)) {
        return 0;
    }
    FsmqSetState(STATE_SPIKE);
    return 1;
}

int msw::CChrCoreBoss::CanbeUseItem_(CBall *pBall)
{
    if (pBall == 0) {
        return 0;
    }
    if (IsDisable()) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
        return 0;
    }
    if (CGame::IsReqArtsEvent()) {
        return 0;
    }
    if (CGame::IsArtsEventMode()) {
        return 0;
    }
    return GetStockItem() != 0;
}

int msw::CChrCoreBoss::CanbeDunk()
{
    // A dunk is possible from up to this far from the point it aims at.
    static const float fDistMax = 6.0f;

    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x4000) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (m_nStatusFlag & 2) {
        return 0;
    }
    if (!IsHoldBall()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            break;
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    bcs::FVEC3 pos;
    GetDunkTargetPosRot_(&pos);
    return !(GetDist_ToPosXZSq(pos) >= fDistMax * fDistMax);
}

int msw::CChrCoreBoss::MethdDunk()
{
    if (!CanbeDunk()) {
        return 0;
    }
    FsmqSetState(STATE_DUNK);
    return 1;
}

// The air trick needs a full arts gauge, the ball (held, or one that may be
// taken in the air) and every teammate ready to join in.
int msw::CChrCoreBoss::CanbeAirTrick()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x4000) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (m_nStatusFlag & 2) {
        return 0;
    }
    if (!IsHoldBall()) {
        CBall *pBall = GetTargetBall_();
        if (pBall == 0) {
            return 0;
        }
        if (!pBall->CanbeVsBossAirTrick()) {
            return 0;
        }
    }
    if (CGame::GetTeamArtsGauge(m_nTeamNo) < 1.0f) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            break;
        case STATE_SHOOT_STANCE:
            if (!SubStateShootStanceCanbeCancel_()) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    long nNum = CChrMan::GetChrCoreNum();
    for (long i = 0; i < nNum; i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo() &&
            pChr->GetTeamNo() == GetTeamNo() && !pChr->CanbeAirTrickSub()) {
            return 0;
        }
    }
    return 1;
}

// The leader's air trick; each teammate joins in with MethdAirTrickSub.
int msw::CChrCoreBoss::MethdAirTrick()
{
    if (!CanbeAirTrick()) {
        return 0;
    }

    long nSub = 0;
    long nNum = 1;
    long nChrNum = CChrMan::GetChrCoreNum();
    for (long i = 0; i < nChrNum; i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo() &&
            pChr->GetTeamNo() == GetTeamNo() && pChr->CanbeAirTrickSub()) {
            nNum++;
        }
    }
    for (long i = 0; i < nChrNum; i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetPlayerNo() != GetPlayerNo() &&
            pChr->GetTeamNo() == GetTeamNo()) {
            pChr->MethdAirTrickSub(nSub, nNum);
            nSub++;
        }
    }

    WORK_AIR_TRICK *pWork = static_cast<WORK_AIR_TRICK *>(FsmqSetState(STATE_AIR_TRICK));
    pWork->m_fWait = 180.0f;
    pWork->m_nNo = 0;
    pWork->m_nNum = nNum;
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x8000;
    return 1;
}

int msw::CChrCoreBoss::CanbeAirTrickSub()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x4000) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (m_nStatusFlag & 2) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
        case STATE_SHOOT_STANCE:
            break;
        default:
            return 0;
    }
    return 1;
}

// nSub: this character's place among the teammates joining in (from 0);
// nNum: the characters in the air trick. Each one jumps 120 frames after
// the one before.
int msw::CChrCoreBoss::MethdAirTrickSub(long nSub, long nNum)
{
    if (!CanbeAirTrickSub()) {
        return 0;
    }
    WORK_AIR_TRICK *pWork = static_cast<WORK_AIR_TRICK *>(FsmqSetState(STATE_AIR_TRICK));
    long nNo = nSub + 1;
    pWork->m_nNo = nNo;
    pWork->m_nNum = nNum;
    pWork->m_fWait = 180.0f + 120.0f * nNo;
    m_nStatusFlag |= 0x40000000;
    m_UnkEC &= ~0x8000;
    return 1;
}

int msw::CChrCoreBoss::CanbePass(CChrCore *)
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (m_nStatusFlag & 0x1400) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBoss::MethdPass(CChrCore *pTo, int)
{
    if (!CanbePass(pTo)) {
        return 0;
    }
    WORK_PASS *pWork = static_cast<WORK_PASS *>(FsmqSetState(STATE_PASS));
    if (pTo == 0) {
        pWork->m_fRotY = GetRotY();
    } else {
        pWork->m_fRotY = GetRotY_ToChr(pTo);
    }
    return 1;
}

int msw::CChrCoreBoss::MethdEventDoll()
{
    if (!CanbeEventDoll()) {
        return 0;
    }
    CBall *pBall = GetAttachBall();
    Reset();
    if (pBall != 0) {
        AttachBall(pBall->GetBallNo(), 5, 0.0f, 0);
    }
    FsmqSetState(STATE_EVENT_DOLL);
    return 1;
}
