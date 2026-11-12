// Written from BossF's game/boss/CChrCoreBossMethod.cpp (CChrCoreBoss has the
// same members) and checked against VolleyF.
// msw::CChrCoreVolley: the Canbe* (may the character start an action now?) and
// Methd* (start it) members of the volley-mode character core.
//
// A Canbe* function answers whether the action may start; the Methd*
// function asks it (through the vtable, so a character may override it)
// and then puts the action's state at the head of the state queue
// (FsmqSetState), filling in the new state's work.
#include "game/msw/CChrCoreVolley.hpp"

#include <stdlib.h>

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CStageMan.hpp"

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
        long m_nType;  // 0x04: 1 and 4, or 2 and 3 (running on), passed to the run
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

    // STATE_JUMP (the block jump; step -1 is a fall)
    struct WORK_JUMP {
        long m_nStep;
        long m_nType;            // 0x04: 1 for a block
        float m_fRotY;           // 0x08: the direction faced
        unsigned char m_Unk0C[0x50 - 0x0C];
        float m_fMoveRotY;       // 0x50: where a steered jump moves to (-1: none)
    };

    // STATE_SERVE
    struct WORK_SERVE {
        unsigned long m_nStep;   // 0x00: 12 for the jump serve; the ball is hit after step 11
        long m_nType;            // 0x04: the kind of serve (2: jump serve)
        unsigned char m_Unk08[0x28 - 0x08];
        float m_fRotY;           // 0x28
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

    // STATE_PARALYZE
    struct WORK_PARALYZE {
        long m_nStep;
        unsigned long m_nFlag;   // 0x04: the damage flags
        float m_fUnk08;          // 0x08
        float m_fTime;           // 0x0C: how long it lasts
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

    // STATE_ITEM_SHOOT
    struct WORK_SHOOT {
        long m_nStep;
        float m_fRotY;           // 0x04
    };

    // The motion play flags (names guessed).
    enum {
        MOT_PLAY_FLAG_GAME_RESULT = 0x42,
        MOT_PLAY_FLAG_GAME_SET = 0x43,
        MOT_PLAY_FLAG_TURN_OVER = 0x2
    };
}

int msw::CChrCoreVolley::CanbeChgLeaderState()
{
    return 1;
}

int msw::CChrCoreVolley::CanbeChgNewLeaderState()
{
    return 1;
}

int msw::CChrCoreVolley::CanbeChgLeader(CChrCore *pChr)
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
int msw::CChrCoreVolley::MethdChgLeader()
{
    if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY) {
        return 0;
    }
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
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == GetTeamNo() && pChr->GetPlayerNo() != GetPlayerNo() && pChr != pLeader) {
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

int msw::CChrCoreVolley::MethdChgLeader(long, long, long)
{
    return 0;
}

// A charge may start while the character's side has the ball (and it is not
// touched twice or held), or while a marker of the other side is shown.
int msw::CChrCoreVolley::CanbeCharge()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (m_UnkEC & 0x24000000) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
        return 0;
    }
    if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY) {
        return 0;
    }
    if ((m_nStatusFlag & 0x10000) || (m_nStatusFlag & 0x8000)) {
        return 0;
    }
    if (m_nStatusFlag & 1) {
        return 0;
    }
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        return 0;
    }
    if (IsOffenseTeam()) {
        if (pBall->GetVolleyTouchNum(m_nTeamNo) > 0) {
            return 0;
        }
        if (pBall->IsAttach()) {
            return 0;
        }
    } else {
        int bFound = 0;
        long nNum = CChrMan::GetChrCoreNum();
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (!pChr->IsDisable() && pChr->GetTeamNo() != GetTeamNo() && pChr->IsVolleyTargetMarkerActive()) {
                bFound = 1;
                break;
            }
        }
        if (!bFound) {
            return 0;
        }
    }
    switch (GetState()) {
        case STATE_DEFAULT:
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreVolley::CanbeRun()
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
    if (!IsVolleyServeMode() && (m_nStatusFlag & 1)) {
        return 0;
    }
    if (!CanbeRunCondition()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
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
int msw::CChrCoreVolley::CanbeRunCondition()
{
    switch (m_nComLevel) {
        case 2:
        case 3:
        case 5:
            return 0;
    }
    return 1;
}

int msw::CChrCoreVolley::MethdRun(float fRotY)
{
    return MethdRun(fRotY, 1.0f);
}

int msw::CChrCoreVolley::MethdRun(float fRotY, float fSpeedRate)
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
                case 4:
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

// A jump (a block) may start near the net, in the attack zone of the
// character's half, while the ball is on the other side.
int msw::CChrCoreVolley::CanbeJump()
{
    // How far past the attack zone, front and back, the jump may start.
    static const float fZMgn = 1.5f;

    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (m_nFlagF4 & 2) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
        return 0;
    }
    if (!CanbeJumpCondition()) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 1) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 8) {
        return 0;
    }
    if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY) {
        return 0;
    }

    CBall *pBall = GetTargetBall_();
    if (pBall != 0) {
        bcs::FVEC3 ballPos = pBall->GetPos();
        if (pBall->IsAttach()) {
            bcs::CScnObj *pMdl = pBall->GetAttachMdlObj();
            if (pMdl != 0) {
                ballPos = pMdl->m_vTrans;
            }
        }
        bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
        if (GetGuardCourtIdx() != 0) {
            if (ballPos.z <= netPos.z) {
                return 0;
            }
        } else if (ballPos.z >= netPos.z) {
            return 0;
        }
    }

    // 0: in the attack zone, 2: off the court half.
    bcs::FVEC3 pos;
    GetCompPos(pos);
    long nZone = 2;
    bcs::FVEC4 rect;
    Util_GetGuardCourtRect_(rect);
    if (pos.x >= rect.x && pos.x <= rect.y && pos.z >= rect.z && pos.z <= rect.w) {
        Util_GetGuardCourtAttackZoneRect_(rect);
        if (pos.x >= rect.x && pos.x <= rect.y && pos.z >= rect.z - fZMgn && pos.z <= fZMgn + rect.w) {
            nZone = 0;
        }
    }
    if (nZone != 0) {
        return 0;
    }

    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
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

// Conditions 2 to 8 and 10 keep the character from jumping.
int msw::CChrCoreVolley::CanbeJumpCondition() const
{
    switch (m_nComLevel) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 10:
            return 0;
    }
    return 1;
}

// Jumps to block, facing the target ball; a running character keeps some of
// its speed.
int msw::CChrCoreVolley::MethdJump()
{
    if (!CanbeJump()) {
        return 0;
    }

    float fSpeed = 0.0f;
    float fRotY = GetRotY();
    if (GetState() == STATE_RUN) {
        WORK_RUN *pRun = FsmqGetWork<WORK_RUN>();
        fSpeed = 0.5f * (pRun->m_fFrame * (0.016666668f * GetPrmMoveSpeed()));
        fRotY = pRun->m_fRotY;
    }

    FsmqClear();
    AdjustOffsetPos_(1);
    WORK_JUMP *pWork = static_cast<WORK_JUMP *>(FsmqPushWork(STATE_JUMP));
    pWork->m_nStep = 0;
    pWork->m_nType = 1;
    pWork->m_fMoveRotY = -1.0f;
    pWork->m_fRotY = GetBlockLookDir_(GetTargetBall_());
    if (SubStateBlockIsJumpType_()) {
        float fFrame = GetCcParam_()->m_Unk38;
        float fHeight = GetCcParam_()->m_Unk40;
        if (Util_SetJumpParam(fFrame, fHeight, GetCcParam_()->m_Unk3C) > 0.0f && fSpeed > 0.0f) {
            bcs::FVEC3 vel;
            bcs::BCS_VecZ_RotY(vel, fSpeed, fRotY);
            m_fJumpVelX = vel.x;
            m_fJumpVelZ = vel.z;
        }
    }
    return 1;
}

// Falls from a height (a jump state of step -1).
int msw::CChrCoreVolley::MethdFall_()
{
    if (GetPosY() <= 0.05f) {
        return 0;
    }
    WORK_JUMP *pWork = static_cast<WORK_JUMP *>(FsmqSetState(STATE_JUMP));
    pWork->m_nStep = -1;
    m_nStatusFlag = (m_nStatusFlag & ~0x800) | 0x1400;
    return 1;
}

int msw::CChrCoreVolley::CanbeCombination() const
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (IsVolleyCombinationMode()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_DEFAULT:
        case STATE_NEUTRAL:
        case STATE_RUN:
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreVolley::MethdCombination()
{
    if (!CanbeCombination()) {
        return 0;
    }
    if (IsVolleyCombinationMode()) {
        return 0;
    }
    m_bCombination = 1;
    return 1;
}

int msw::CChrCoreVolley::CanbeSetMoveRevise()
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
int msw::CChrCoreVolley::MethdSetMoveRevise(float fRotY)
{
    if (!CanbeSetMoveRevise()) {
        return 0;
    }
    if (GetState() == STATE_JUMP) {
        FsmqGetWork<WORK_JUMP>()->m_fMoveRotY = WrapRad(fRotY);
        m_fJumpRotY = WrapRad(fRotY);
        return 1;
    }
    return 0;
}

int msw::CChrCoreVolley::MethdDamage(unsigned long nFlag, float fRotY)
{
    return MethdDamage(nFlag, fRotY, 0.0f, 0.0f, -1, 0.0f, 0, 0);
}

// nFlag: the kind of damage in the low byte (2: a condition, 0xD: a
// paralysis, or a size change while the character is small, 0xE: handed to
// SubStateParalyzeStateSet_), the type in the next (7 turns the character
// into a bean, 5 is ignored high in the air); fDamage is kept, fTime is how
// long the paralysis or condition lasts; nQuake is a camera quake (negative
// for none); bit 0x10000 makes the character drop coins.
int msw::CChrCoreVolley::MethdDamage(unsigned long nFlag, float fRotY, float fDamage, float fTime, long nQuake,
                                     float fUnk, long nUnk1, long nUnk2)
{
    unsigned long nType = (nFlag >> 8) & 0xFF;
    switch (nType) {
        case 7: {
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
        case 5:
            if ((m_nStatusFlag & 0x400) && GetTransNodeHeight() > 0.05f) {
                return 0;
            }
            break;
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
    if ((nFlag & 0x10000) && Util_DropCoin(fUnk, nUnk1, nUnk2)) {
        m_UnkEC |= 0x20;
    }
    if (nQuake >= 0) {
        CGame::StartCameraQuake(nQuake, m_nPlayerNo, -1);
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0xA), nFlag, 0, 0, 0);

    switch (nFlag & 0xFF) {
        case 0xE:
            SubStateParalyzeStateSet_(nFlag, fTime);
            m_fDamageSizeTime = fTime;
            m_nDamageQuake = nQuake;
            m_fDamageUnk1F8 = fUnk;
            m_nDamageUnk1FC = nUnk1;
            m_nDamageUnk1FE = nUnk2;
            return 1;
        case 0xD:
            if (m_nStatusFlag & 0x10000) {
                // Small: no paralysis, the small size only lasts longer.
                PlaySePos(0xC45, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                if (m_fSizeChangeTime < fTime) {
                    SetBodyScaleState(2, fTime);
                }
                if (nType == 0xB) {
                    PlayVoice(0x8A, 0);
                    SetCondition(10, fTime, 0);
                }
            } else {
                WORK_PARALYZE *pWork = static_cast<WORK_PARALYZE *>(FsmqSetState(STATE_PARALYZE));
                pWork->m_nFlag = nFlag;
                pWork->m_fUnk08 = 0.45f;
                pWork->m_fTime = fTime;
            }
            return 1;
        case 2: {
            // A condition: when it is already on, it only starts again.
            float fFrame = 60.0f;
            long nCondition = 7;
            switch (nType) {
                case 3:
                    fFrame = 9999.0f;
                    nCondition = 5;
                    break;
                case 4:
                    fFrame = 180.0f;
                    nCondition = 6;
                    break;
                case 5:
                    fFrame = 260.0f;
                    nCondition = 7;
                    break;
                case 6:
                    fFrame = 300.0f;
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
            break;
        }
        default:
            SetInvincible(30.0f);
            fInvincible = 30.0f;
            break;
    }

    WORK_DAMAGE *pWork = static_cast<WORK_DAMAGE *>(FsmqSetState(STATE_DAMAGE));
    pWork->m_nFlag = nFlag;
    pWork->m_bAir = bAir;
    pWork->m_fRotY = fRotY;
    pWork->m_fDamage = fDamage;
    pWork->m_fInvincible = fInvincible;
    pWork->m_nStep = 0;
    pWork->m_fSizeTime = fTime;
    pWork->m_nQuake = nQuake;
    pWork->m_fUnk2C = fUnk;
    pWork->m_nUnk30 = nUnk1;
    pWork->m_nUnk32 = nUnk2;
    m_fDamageSizeTime = fTime;
    m_nDamageQuake = nQuake;
    m_fDamageUnk1F8 = fUnk;
    m_nDamageUnk1FC = nUnk1;
    m_nDamageUnk1FE = nUnk2;
    return 1;
}

// Sliding is how a bean (STATE_MAME_CHANGE) moves.
int msw::CChrCoreVolley::CanbeSliding()
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

int msw::CChrCoreVolley::MethdSliding(float fRotY)
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
int msw::CChrCoreVolley::CanbeSizeChange(long nType)
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
            break;
    }
    return 1;
}

int msw::CChrCoreVolley::MethdSizeChange(long nType, float fTime)
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
            m_fSizeChangeTime = fTime;
            m_fScaleFrom = m_fScaleBase;
            m_fScaleTo = 0.45f;
            m_fScaleRate = 1.0f;
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
                case STATE_RECEIVE:
                case STATE_SPIKE:
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

int msw::CChrCoreVolley::CanbeItemShoot()
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
    if (CGame::g_GameWork.m_nFlag18 & 8) {
        // Only while the target ball is shot.
        CBall *pBall = GetTargetBall_();
        if (pBall != 0 && !pBall->IsShootMode()) {
            return 0;
        }
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

int msw::CChrCoreVolley::MethdItemShoot(float fRotY)
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

int msw::CChrCoreVolley::CanbeChgStarState()
{
    if (IsDisable()) {
        return 0;
    }
    return !(m_nStatusFlag & 0x8000);
}

int msw::CChrCoreVolley::MethdChgStarState()
{
    if (!CanbeChgStarState()) {
        return 0;
    }
    SetCondition(1, 600.0f, 0);
    return 1;
}

// The reaction to a turn over: a motion for the team that gets the ball
// (nTeam) and another for the one that loses it.
int msw::CChrCoreVolley::MethdTurnOver(long nTeam)
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
        pWork->m_nMot = 0x6E;
        pWork->m_nNextMot = 0x6F;
        pWork->m_nVoice = 0x47;
        pWork->m_nUnk14 = -1;
        pWork->m_nUnk1C = 4;
    } else {
        pWork->m_nMot = 0x70;
        pWork->m_nNextMot = 0x71;
        pWork->m_nVoice = 0x49;
        pWork->m_nUnk14 = -1;
        pWork->m_nUnk1C = 3;
    }
    return 1;
}

// The time is up: the motion of the winning (nTeam) or the losing side,
// after the catch or serve now running, at once otherwise.
int msw::CChrCoreVolley::MethdTimeOver(long nTeam)
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
        case STATE_SERVE:
            // A serve not yet hit is dropped.
            if (FsmqGetWork<WORK_SERVE>()->m_nStep <= 11) {
                FsmqClear();
            }
            break;
        case STATE_DEFAULT:
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
            FsmqClear();
            break;
    }
    WORK_MOT_PLAY *pWork = static_cast<WORK_MOT_PLAY *>(FsmqPushWork(STATE_MOT_PLAY));
    pWork->m_nFlag = 0;
    pWork->m_nFlag |= MOT_PLAY_FLAG_TURN_OVER;
    if (nTeam == m_nTeamNo) {
        pWork->m_nMot = 0x6E;
        pWork->m_nNextMot = 0x6F;
        pWork->m_nVoice = 0x48;
        pWork->m_nUnk14 = -1;
    } else {
        pWork->m_nMot = 0x70;
        pWork->m_nNextMot = 0x71;
        pWork->m_nVoice = 0x49;
        pWork->m_nUnk14 = -1;
    }
    return 1;
}

int msw::CChrCoreVolley::MethdGameSet()
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

int msw::CChrCoreVolley::MethdGameResult(int bWin)
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
        pWork->m_nMot = 0x6E;
        pWork->m_nNextMot = 0x6F;
        pWork->m_nVoice = 0x48;
        pWork->m_nUnk14 = -1;
    } else {
        pWork->m_nMot = 0x70;
        pWork->m_nNextMot = 0x71;
        pWork->m_nVoice = 0x49;
        pWork->m_nUnk14 = -1;
    }
    return 1;
}

// A serve may start while the character holds the ball and stands or runs.
int msw::CChrCoreVolley::CanbeServe(long)
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
    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
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
            break;
        default:
            return 0;
    }
    return 1;
}

// nType: the kind of serve; 2 is the jump serve, which starts at step 12
// facing the present direction.
int msw::CChrCoreVolley::MethdServe(long nType)
{
    if (!CanbeServe(nType)) {
        return 0;
    }
    float fRotY = -1.0f;
    int bJump = 0;
    if (nType == 2) {
        bJump = 1;
        if (fRotY < 0.0f) {
            fRotY = GetRotY();
        }
    }
    WORK_SERVE *pWork = static_cast<WORK_SERVE *>(FsmqSetState(STATE_SERVE));
    pWork->m_nStep = 0;
    pWork->m_nType = nType;
    if (bJump) {
        pWork->m_nStep = 12;
        pWork->m_fRotY = fRotY;
    }
    return 1;
}

// bAny: also when the ball is not one this character may touch now.
int msw::CChrCoreVolley::CanbeReceive(int bAny)
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
    long nBallNo = 0;
    CBall *pBall = GetTargetBall_();
    if (pBall != 0) {
        nBallNo = pBall->GetBallNo();
        if (pBall->CanbeVolleyPickup(m_nPlayerNo)) {
            return 0;
        }
        if (pBall->IsFlag1()) {
            return 0;
        }
        // Not the ball this character has just shot (unless the count is
        // running).
        if (pBall->GetShotTeamIdx() == GetTeamNo() && pBall->GetShotChrIdx() == GetPlayerNo() &&
            m_afCountWork[0] <= 0.0f) {
            return 0;
        }
    }
    if (!bAny && !IsVolleyRightBallTouch(nBallNo)) {
        return 0;
    }

    switch (GetState()) {
        case STATE_NEUTRAL:
            break;
        case STATE_RUN:
        case STATE_TURN:
            if (!IsVolleyRightBallTouch(nBallNo)) {
                return 0;
            }
            if (SubStateReceiveChoiceProcType_(0) == 4) {
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
int msw::CChrCoreVolley::CanbeReceiveCondition() const
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

int msw::CChrCoreVolley::CanbeDiveReceive()
{
    if (!CanbeReceive(0)) {
        return 0;
    }
    long nType = SubStateReceiveChoiceProcType_(0);
    return nType == 1;
}

int msw::CChrCoreVolley::MethdReceive(int bKeep, int bAny)
{
    return MethdReceive(bKeep, bAny, 12.566371f);
}

// fRotY: the direction to send the ball in; above 2 pi for none (the
// character's parameters may ask for that too), negative for "back".
int msw::CChrCoreVolley::MethdReceive(int bKeep, int bAny, float fRotY)
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
    if (fRotY > 6.2831855f || GetCcParam_()->m_Unk100 != 0) {
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

msw::CChrCoreVolley::STATE_WORK_RECEIVE *msw::CChrCoreVolley::SetStateReceive_(CBall *pBall, long nKind, long nPose)
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

void msw::CChrCoreVolley::SetReceiveEndureWork_(CBall *pBall, STATE_WORK_RECEIVE *pWork)
{
    if (pBall == 0 || pWork == 0) {
        return;
    }
    pWork->m_fEndure50 = 0.0f;
    pWork->m_nReceiveType = -1;
    pWork->m_nFlickType = -1;
    if (pBall->IsArtsShootMode()) {
        if (m_nComLevel != 1) {
            pWork->m_fUnk40 = 0.0f;
        }
        pWork->m_nReceiveType = 0x45;
        pWork->m_nFlickType = -1;
        pWork->m_nFlag |= 0x100;
        BALL::VOLLEY::ENDURE_PRM prm;
        int bPrm = pBall->GetVolleyEndureParam(prm);
        if (m_nComLevel != 1 && bPrm) {
            pWork->m_fEndure50 = prm.m_fUnk00;
            pWork->m_fEndure48 = prm.m_fUnk04;
            pWork->m_fEndure4C = prm.m_fUnk08;
            pWork->m_nFlag |= 4;
        }
    }
}

int msw::CChrCoreVolley::CanbeSpike(int bAny) const
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
    if (CGame::g_GameWork.m_nFlag18 & 8) {
        return 0;
    }
    if (m_afCountWork[0] > 0.0f) {
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

    if (IsOffenseTeam()) {
        if (pBall->IsAttach()) {
            // (the original names IsVolleyArtsAttach without calling it,
            // so the test is always true: an attached ball is never
            // spiked unless bAny)
            if (!bAny && &CBall::IsVolleyArtsAttach) {
                return 0;
            }
            if (pBall->GetVolleyTouchNum(m_nTeamNo) >= 2) {
                return 0;
            }
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
        } else if (pBall->GetTouchNum() > 0) {
            if (pBall->GetShotChrIdx() != m_nPlayerNo) {
                if (!bAny) {
                    // The ball must come down high enough to be hit.
                    bcs::FVEC3 ofs = m_SpikeOfs;
                    float fHeight = 2.9f;
                    float fFrame = 32.0f;
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
                        if (pos.y - ofs.y < fHeight - 1.0f) {
                            return 0;
                        }
                    }
                }
            } else if (pBall->GetTouchNum() >= 2) {
                return 0;
            }
        }
    } else if (!bAny && pBall->IsVolleyArtsAttach()) {
        return 0;
    }

    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_TURN:
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

int msw::CChrCoreVolley::MethdSpike(int bAny)
{
    if (!CanbeSpike(bAny)) {
        return 0;
    }
    STATE_WORK_SPIKE *pWork = static_cast<STATE_WORK_SPIKE *>(FsmqSetState(STATE_SPIKE));
    if (m_afCountWork[1] > 0.0f) {
        pWork->m_nFlag |= 0xC;
    }
    // The attacker of a combination, at its second step.
    if (m_nCombinationRole == 2 && m_nCombinationStep == 1) {
        pWork->m_nFlag |= 0x800;
    }
    return 1;
}

// The air trick (the special shot) may start when the team's gauge is full,
// on a ball on the character's side of the net that the character's team
// has touched fewer than three times.
int msw::CChrCoreVolley::CanbeAirTrick()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
        return 0;
    }
    if (m_UnkEC & 0x0A000000) {
        return 0;
    }
    if (CGame::g_GameWork.m_nUnk40 & 8) {
        return 0;
    }
    if (CGame::g_GameWork.m_nUnk180 != 0) {
        return 0;
    }
    if (CGame::IsReqArtsEvent()) {
        return 0;
    }
    if (CGame::IsArtsEventMode()) {
        return 0;
    }
    if ((m_nStatusFlag & 0x10000) || (m_nStatusFlag & 0x8000)) {
        return 0;
    }
    if (CGame::GetTeamArtsGauge(m_nTeamNo) < 1.0f) {
        return 0;
    }
    if (!CanbeReceiveCondition()) {
        return 0;
    }
    if (m_nStatusFlag & 1) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 8) {
        return 0;
    }
    CBall *pBall = GetTargetBall_();
    if (pBall == 0) {
        return 0;
    }
    if (pBall->IsFlag1()) {
        return 0;
    }
    if (pBall->IsArtsShootMode()) {
        return 0;
    }
    if (pBall->IsAttach()) {
        return 0;
    }
    if (pBall->IsVolleyThroughUnderNet()) {
        return 0;
    }
    if (CGame::g_GameWork.m_nUserTeam != m_nTeamNo && pBall->GetShotTeamIdx() != m_nTeamNo) {
        return 0;
    }
    if (pBall->GetVolleyTouchNum(m_nTeamNo) >= 3) {
        return 0;
    }

    // Not on a ball over the other half.
    bcs::FVEC3 ballPos = pBall->GetPos();
    bcs::FVEC3 netPos = CStageMan::GetInstance()->GetNetPos();
    bcs::FVEC3 dir;
    if (GetGuardCourtIdx() != 0) {
        dir.Set(0.0f, 0.0f, 1.0f);
    } else {
        dir.Set(0.0f, 0.0f, -1.0f);
    }
    bcs::FVEC3 diff;
    bcs::FVEC3Sub(&diff, &ballPos, &netPos);
    if (bcs::FVEC3Dot(&diff, &dir) > 0.0f) {
        return 0;
    }

    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_JUMP:
        case STATE_TURN:
        case STATE_SPIKE:
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

// The leader's air trick; each teammate joins in with MethdAirTrickSub.
int msw::CChrCoreVolley::MethdAirTrick()
{
    if (!CanbeAirTrick()) {
        return 0;
    }
    STATE_WORK_AIR_TRICK *pWork = static_cast<STATE_WORK_AIR_TRICK *>(FsmqSetState(STATE_AIR_TRICK));
    pWork->m_nFlag = 0;
    return 1;
}

int msw::CChrCoreVolley::CanbeUseItem_(CBall *pBall)
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
    // The team has an item in stock.
    return CGame::GetTeamItem(m_nTeamNo, 0) >= 0;
}

int msw::CChrCoreVolley::MethdEventDoll()
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

// The character may land anywhere but on floor of attribute 8 (when it
// checks the floor at all).
int msw::CChrCoreVolley::CanbeLanding() const
{
    if (m_bCheckLanding == 0) {
        return 1;
    }
    CStageMan *pStage = CStageMan::GetInstance();
    return pStage->GetFloorAttribute(GetPos(), static_cast<CStageMan::FLOW_TYPE>(0)) != 8;
}

int msw::CChrCoreVolley::CanbeFall() const
{
    return 1;
}

// Falls after the state now running.
int msw::CChrCoreVolley::MethdFall()
{
    if (!CanbeFall()) {
        return 0;
    }
    FsmqCutQueue();
    FsmqPush(STATE_FALL);
    return 1;
}

int msw::CChrCoreVolley::CanbeRebirth() const
{
    return 1;
}

int msw::CChrCoreVolley::MethdRebirth()
{
    if (!CanbeRebirth()) {
        return 0;
    }
    FsmqChangeState(STATE_REBIRTH);
    return 1;
}
