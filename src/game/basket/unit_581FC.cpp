// msw::CChrCoreBasket: the Canbe* (may the character start an action now?)
// and Methd* (start it) members of the basketball character core.
//
// A Canbe* function answers whether the action may start; the Methd*
// function asks it (through the vtable, so a character may override it)
// and then puts the action's state at the head of the state queue
// (FsmqSetState), filling in the new state's work, or passes the request
// on to the state already running through its work.
//
// The works of the states are defined with the states in other units; the
// structs below map only the words this file reads or writes (names
// guessed, offsets from the code).
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBallManBasket.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CNpcGhost.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CTournament.hpp"
#include "nw4r/math/triangular.h"

namespace {

    // (name and meaning not known) A value the trick move puts in its
    // work; the first constant of the file's pool.
    static const float s_fTrickMoveUnk1C = 6;

    // The start of the character's parameter block (GetCcParam_; the jump
    // parameters CC_DATA::CHR_PARAM::JUMP_PARAM begin there too). Names
    // guessed.
    struct CC_PARAM_HEAD {
        float m_fJumpFrame;    // 0x00: frames of a jump
        float m_fUnk04;        // 0x04
        float m_fJumpHeight;   // 0x08
        float m_fUnk0C;        // 0x0C
        float m_fHeight;       // 0x10
    };

    inline const CC_PARAM_HEAD *GetCcParamHead(const msw::CChrCoreBasket *pChr)
    {
        return static_cast<const CC_PARAM_HEAD *>(pChr->GetCcParam_());
    }

    // STATE_NEUTRAL
    struct WORK_NEUTRAL {
        long m_nStep;          // 0x00
        long m_nIdleLoop;      // 0x04: loops of the stop motion left
        float m_fRotY;         // 0x08
        float m_fFrame;        // 0x0C: the frame the run motion had reached
        int m_bBlockWork;      // 0x10: in the block work
    };

    // STATE_RUN
    struct WORK_RUN {
        long m_nStep;          // 0x00
        long m_nDir;           // 0x04
        long m_nMot;           // 0x08
        // 0x0C: bit 0 the stick is held this frame, bit 1 in the block work
        unsigned long m_nFlag;
        long m_nIdleLoop;      // 0x10
        float m_fStopWait;     // 0x14: frames before the run stops
        float m_fRotYTrg;      // 0x18: the yaw the stick asks for
        float m_fRotY;         // 0x1C
        float m_fSpeedRate;    // 0x20
        unsigned char m_Unk24[0x38 - 0x24];
        float m_fStartFrame;   // 0x38: the frame the run motion starts at
    };

    // STATE_TURN
    struct WORK_TURN {
        long m_nStep;          // 0x00
        long m_nMot;           // 0x04
        unsigned long m_nFlag; // 0x08: bit 0 the stick is held
        unsigned char m_Unk0C[0x14 - 0x0C];
        float m_fRotYEnd;      // 0x14: the yaw the stick asks for at the end
    };

    // STATE_PASS_CATCH
    struct WORK_PASS_CATCH {
        long m_nStep;          // 0x00
        long m_nPassChr;       // 0x04
        unsigned long m_nFlag; // 0x08: bit 2 the stick is held
        unsigned char m_Unk0C[0x20 - 0x0C];
        float m_fRotY;         // 0x20: the yaw the stick asks for
    };

    // STATE_JUMP, STATE_DROP_CUT, STATE_CHARGE_CUT
    struct WORK_JUMP {
        long m_nStep;          // 0x00
        unsigned long m_Unk04; // 0x04
        // 0x08: 0x20 already in the air (a fall); for the drop cut 0x20000
        // the move can no longer be steered.
        unsigned long m_nFlag;
    };

    // STATE_SDB
    struct WORK_SDB {
        long m_nStep;          // 0x00
        unsigned long m_Unk04; // 0x04
        long m_nWait;          // 0x08: frames before another dribble is taken
        // 0x0C: bit 0 a dribble asked for, bit 1 one asked for too early
        unsigned long m_nFlag;
    };

    // STATE_AUTO_DRIBBLE_POSTURE
    struct WORK_AUTO_DPM {
        long m_nStep;          // 0x00
        // 0x04: bit 5 a dribble asked for, bit 6 one asked for too early
        unsigned long m_nFlag;
        float m_fRotY;         // 0x08
        float m_fUnk0C;        // 0x0C
        float m_fUnk10;        // 0x10
        float m_fUnk14;        // 0x14: kept between 0 and 3 frames
        unsigned char m_Unk18[0x20 - 0x18];
        long m_nWait;          // 0x20
        long m_nDir;           // 0x24: one of four directions of the battle
        unsigned char m_Unk28[0x4C - 0x28];
        bcs::FVEC3 m_Pos;      // 0x4C: where the character stood
    };

    // STATE_RDB, STATE_RDB_CHAIN
    struct WORK_RDB {
        long m_nStep;          // 0x00
        long m_nUnk04;         // 0x04
        unsigned long m_Unk08; // 0x08
        // 0x0C: bit 1 the stick is held, bit 2 the turn goes left, bit 3
        // the start of the move
        unsigned long m_nFlag;
        unsigned char m_Unk10[0x1C - 0x10];
        float m_fRotY;         // 0x1C: the yaw the stick asks for
        float m_fStartRotY;    // 0x20
    };

    // STATE_DASH
    struct WORK_DASH {
        long m_nStep;          // 0x00
        long m_nMot;           // 0x04
        unsigned long m_nFlag; // 0x08: bit 0 the stick is held
        float m_fRotYTrg;      // 0x0C: the yaw the stick asks for
    };
}

int msw::CChrCoreBasket::CanbeRun()
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
    if (!(m_nStatusFlag & 0x8000) && (m_nStatusFlag & 1)) {
        return 0;
    }
    long nState = GetState();
    if (nState != STATE_NEUTRAL && nState != STATE_RUN && nState != STATE_TURN && nState != STATE_PASS_CATCH) {
        return 0;
    }
    return 1;
}

// Runs towards fRotY; a turn or a pass catch only takes the direction.
int msw::CChrCoreBasket::MethdRun(float fRotY)
{
    if (!CanbeRun()) {
        return 0;
    }

    long bBlockWork = 0;
    long nIdleLoop = 0;
    float fFrame = 0.0f;
    switch (GetState()) {
        case STATE_NEUTRAL: {
            WORK_NEUTRAL *pNeutral = FsmqGetWork<WORK_NEUTRAL>();
            fFrame = pNeutral->m_fFrame;
            bBlockWork = pNeutral->m_bBlockWork;
            if (AnimGetMotID(3, 0) == 0x77) {
                nIdleLoop = pNeutral->m_nIdleLoop;
            }
        }
        case STATE_BLOCK_CUT: {
            AdjustOffsetPos_(1);
            WORK_RUN *pNew = static_cast<WORK_RUN *>(FsmqSetState(STATE_RUN));
            pNew->m_nFlag = 0;
            if (bBlockWork == 1) {
                pNew->m_nFlag |= 2;
            } else {
                pNew->m_nFlag &= ~2;
            }
            pNew->m_fSpeedRate = 0.0f;
            pNew->m_nStep = 0;
            pNew->m_fRotY = GetRotY();
            pNew->m_nIdleLoop = nIdleLoop;
            break;
        }
        case STATE_RUN:
            break;
        case STATE_TURN: {
            WORK_TURN *pWork = FsmqGetWork<WORK_TURN>();
            pWork->m_fRotYEnd = bcs::ROUND_PI(fRotY);
            pWork->m_nFlag |= 1;
            return 0;
        }
        case STATE_PASS_CATCH: {
            WORK_PASS_CATCH *pWork = FsmqGetWork<WORK_PASS_CATCH>();
            pWork->m_fRotY = fRotY;
            pWork->m_nFlag |= 4;
            return 0;
        }
        default:
            return 0;
    }
    WORK_RUN *pWork = FsmqGetWork<WORK_RUN>();
    pWork->m_fStopWait = 3.0f * m_fTimeScale;
    pWork->m_nFlag |= 1;
    pWork->m_fRotYTrg = bcs::ROUND_PI(fRotY);
    pWork->m_fStartFrame = fFrame;
    return 1;
}

// Which of eight directions (0 ahead, then round by 45 degrees) fRot is.
long msw::CChrCoreBasket::GetDir8ToBlockWork(float fRot)
{
    if (fRot < 0.3926991f) {
        return 4;
    }
    if (fRot < 1.1780972f) {
        return 3;
    }
    if (fRot < 1.9634954f) {
        return 2;
    }
    if (fRot < 2.7488935f) {
        return 1;
    }
    if (fRot < 3.5342917f) {
        return 0;
    }
    if (fRot < 4.3196898f) {
        return 7;
    }
    if (fRot < 5.105088f) {
        return 6;
    }
    if (fRot < 5.8904862f) {
        return 5;
    }
    return 4;
}

long msw::CChrCoreBasket::GetDir4ToBattle(float fRot)
{
    return CChrManBasket::GetInstance()->GetDir4ToBattle_(fRot);
}

int msw::CChrCoreBasket::CanbeJump()
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
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_SIDE_STEP:
            return 1;
        case STATE_BLOCK_CUT:
            return 1;
        default:
            return 0;
    }
}

// The basketball characters do not jump on their own.
int msw::CChrCoreBasket::MethdJump()
{
    return 0;
}

// Falls from a height (a jump already in the air).
int msw::CChrCoreBasket::MethdFall_()
{
    if (GetPosY() <= 0.05f) {
        return 0;
    }
    WORK_JUMP *pWork = static_cast<WORK_JUMP *>(FsmqSetState(STATE_JUMP));
    pWork->m_nFlag = 0x20;
    m_nStatusFlag = (m_nStatusFlag & ~0x800) | 0x1400;
    return 1;
}

int msw::CChrCoreBasket::CanbeSetMoveRevise()
{
    if (IsDisable()) {
        return 0;
    }
    long nState = GetState();
    if (nState != STATE_JUMP && nState != STATE_DROP_CUT && nState != STATE_CHARGE_CUT && nState != STATE_SMASH_CUT) {
        return 0;
    }
    return !(m_UnkEC & 0x08000000);
}

// Steers a jump (or a cut in the air) towards fRotY.
int msw::CChrCoreBasket::MethdSetMoveRevise(float fRotY)
{
    if (!CanbeSetMoveRevise()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_DROP_CUT:
            if (FsmqGetWork<WORK_JUMP>()->m_nFlag & 0x20000) {
                break;
            }
        case STATE_JUMP:
        case STATE_CHARGE_CUT:
            m_fJumpRotY = bcs::ROUND_PI(fRotY);
            return 1;
        case STATE_SMASH_CUT:
            FsmqGetWork<STATE_WORK_SMASH_CUT>()->m_fSteerRotY = fRotY;
            return 1;
    }
    return 0;
}

int msw::CChrCoreBasket::CanbeDribble()
{
    if (IsDisable()) {
        return 0;
    }
    return !(m_nStatusFlag & 0x80);
}

int msw::CChrCoreBasket::CanbeCatchToSDB()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    long bNoDribble = (m_nStatusFlag >> 7) & 1;
    if (bNoDribble) {
        return 0;
    }
    if (GetState() == STATE_CATCH) {
        return bNoDribble == 0;
    }
    return 0;
}

int msw::CChrCoreBasket::MethdCatchToSDB()
{
    if (!CanbeCatchToSDB()) {
        return 0;
    }
    FsmqSetState(STATE_SDB);
    ReattachBall(4, 16.0f, 0);
    return 1;
}

int msw::CChrCoreBasket::CanbeSDB()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SDB:
        case STATE_SDB_CHAIN:
        case STATE_AUTO_DRIBBLE_POSTURE:
            break;
        case STATE_CATCH:
            if (m_nStatusFlag & 0x80) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    return 1;
}

// Asks the dribble running for one more bounce.
int msw::CChrCoreBasket::MethdSDB(float)
{
    if (!CanbeSDB()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SDB: {
            WORK_SDB *pWork = FsmqGetWork<WORK_SDB>();
            if (pWork->m_nWait > 0) {
                pWork->m_nFlag |= 2;
            } else {
                pWork->m_nFlag |= 1;
                pWork->m_nWait = 0;
                SetTap();
            }
            break;
        }
        case STATE_SDB_CHAIN:
            break;
        case STATE_AUTO_DRIBBLE_POSTURE: {
            WORK_AUTO_DPM *pWork = FsmqGetWork<WORK_AUTO_DPM>();
            if (pWork->m_nWait > 0) {
                pWork->m_nFlag |= 0x40;
            } else {
                pWork->m_nFlag |= 0x20;
                pWork->m_nWait = 0;
                SetTap();
            }
            pWork->m_fRotY = -1.0f;
            break;
        }
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeRDB()
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
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_RDB:
        case STATE_RDB_CHAIN:
        case STATE_SDB:
            break;
        case STATE_CATCH:
            if (m_nStatusFlag & 0x80) {
                return 0;
            }
            break;
        case STATE_TURN:
        case STATE_DASH:
            break;
        default:
            return 0;
    }
    return 1;
}

namespace {

    // A running dribble asked to go on towards fRotY: the turn's side is
    // asked again when the stick has moved over.
    inline void UpdateRDBRot(msw::CChrCoreBasket *pChr, WORK_RDB *pWork, float fRotY)
    {
        if (pChr->SubStateRDBIsUpdateRotFlag_(fRotY, pWork->m_fRotY, pWork->m_fStartRotY)) {
            if (pChr->SubStateRDBGetRotFlag_(fRotY, pWork->m_fStartRotY) == 1) {
                pWork->m_nFlag |= 4;
            } else {
                pWork->m_nFlag &= ~4;
            }
        }
    }
}

// Starts a dribble move towards fRotY (or turns the one running).
int msw::CChrCoreBasket::MethdRDB(INPUT_WORK *, float fRotY)
{
    if (!CanbeRDB()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SDB: {
            WORK_RDB *pWork = static_cast<WORK_RDB *>(FsmqSetState(STATE_RDB));
            pWork->m_nFlag = 0;
            if (SubStateRDBGetRotFlag_(fRotY, GetRotY()) == 1) {
                pWork->m_nFlag |= 4;
            } else {
                pWork->m_nFlag &= ~4;
            }
            pWork->m_nUnk04 = 1;
            pWork->m_nFlag |= 0xA;
            pWork->m_fRotY = fRotY;
            pWork->m_fStartRotY = GetRotY();
            break;
        }
        case STATE_RDB: {
            WORK_RDB *pWork = FsmqGetWork<WORK_RDB>();
            UpdateRDBRot(this, pWork, fRotY);
            pWork->m_fRotY = fRotY;
            pWork->m_nFlag = (pWork->m_nFlag | 2) & ~8;
            break;
        }
        case STATE_RDB_CHAIN: {
            WORK_RDB *pWork = FsmqGetWork<WORK_RDB>();
            UpdateRDBRot(this, pWork, fRotY);
            pWork->m_fRotY = fRotY;
            pWork->m_nFlag |= 2;
            break;
        }
        case STATE_CATCH: {
            WORK_RDB *pWork = static_cast<WORK_RDB *>(FsmqSetState(STATE_RDB));
            pWork->m_nUnk04 = 1;
            pWork->m_nFlag = 0;
            if (SubStateRDBGetRotFlag_(fRotY, GetRotY()) == 1) {
                pWork->m_nFlag |= 4;
            } else {
                pWork->m_nFlag &= ~4;
            }
            // (not used)
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.5f, bcs::ROUND_PI(fRotY - GetRotY()));
            SetTap();
            pWork->m_nFlag |= 0xA;
            pWork->m_fRotY = fRotY;
            pWork->m_fStartRotY = GetRotY();
            ReattachBall(4, 8.0f, 0);
            break;
        }
        case STATE_TURN: {
            WORK_TURN *pWork = FsmqGetWork<WORK_TURN>();
            pWork->m_fRotYEnd = fRotY;
            pWork->m_nFlag |= 1;
            break;
        }
        case STATE_AUTO_DRIBBLE_POSTURE:
            return 0;
        case STATE_DASH: {
            WORK_DASH *pWork = FsmqGetWork<WORK_DASH>();
            // A turn of less than 8 degrees keeps the dash's direction.
            float fDiff = bcs::ROUND_PI(fRotY - GetRotY());
            if (fDiff > 3.1415927f) {
                fDiff -= 6.2831855f;
            }
            if (__fabsf(fDiff) <= 0.13962634f) {
                fRotY = GetRotY();
            }
            pWork->m_fRotYTrg = fRotY;
            pWork->m_nFlag |= 1;
            return 0;
        }
        default:
            return 0;
    }
    return 1;
}

// The same with the battle form: from a catch in a battle the dribble
// posture starts instead.
int msw::CChrCoreBasket::MethdRDB(INPUT_WORK *pInput, float fRotY, float)
{
    if (!CanbeRDB()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SDB: {
            WORK_RDB *pWork = static_cast<WORK_RDB *>(FsmqSetState(STATE_RDB));
            pWork->m_nFlag = 0;
            if (SubStateRDBGetRotFlag_(fRotY, GetRotY()) == 1) {
                pWork->m_nFlag |= 4;
            } else {
                pWork->m_nFlag &= ~4;
            }
            pWork->m_nUnk04 = 1;
            pWork->m_nFlag |= 0xA;
            pWork->m_fRotY = fRotY;
            pWork->m_fStartRotY = GetRotY();
            break;
        }
        case STATE_RDB: {
            WORK_RDB *pWork = FsmqGetWork<WORK_RDB>();
            UpdateRDBRot(this, pWork, fRotY);
            pWork->m_fRotY = fRotY;
            pWork->m_nFlag = (pWork->m_nFlag | 2) & ~8;
            break;
        }
        case STATE_RDB_CHAIN: {
            WORK_RDB *pWork = FsmqGetWork<WORK_RDB>();
            UpdateRDBRot(this, pWork, fRotY);
            if (pInput->m_nUnk00 != 0) {
                SetTap();
            }
            pWork->m_fRotY = fRotY;
            pWork->m_nFlag |= 2;
            break;
        }
        case STATE_CATCH:
            if (IsBattle()) {
                long nDir = GetDir4ToBattle(bcs::ROUND_PI(fRotY - GetRotY_ToGoal()));
                WORK_AUTO_DPM *pWork = static_cast<WORK_AUTO_DPM *>(FsmqSetState(STATE_AUTO_DRIBBLE_POSTURE));
                pWork->m_nStep = 6;
                pWork->m_fRotY = fRotY;
                pWork->m_Pos = GetPos();
                pWork->m_nDir = nDir;
                pWork->m_nFlag = 0;
                pWork->m_nFlag |= 1;
            } else {
                WORK_RDB *pWork = static_cast<WORK_RDB *>(FsmqSetState(STATE_RDB));
                pWork->m_nUnk04 = 1;
                pWork->m_nFlag = 0;
                if (SubStateRDBGetRotFlag_(fRotY, GetRotY()) == 1) {
                    pWork->m_nFlag |= 4;
                } else {
                    pWork->m_nFlag &= ~4;
                }
                // (not used)
                bcs::FVEC3 dir;
                bcs::BCS_VecZ_RotY(dir, 1.5f, bcs::ROUND_PI(fRotY - GetRotY()));
                SetTap();
                pWork->m_nFlag |= 0xA;
                pWork->m_fRotY = fRotY;
                pWork->m_fStartRotY = GetRotY();
                ReattachBall(4, 8.0f, 0);
            }
            break;
        case STATE_TURN: {
            WORK_TURN *pWork = FsmqGetWork<WORK_TURN>();
            pWork->m_fRotYEnd = fRotY;
            pWork->m_nFlag |= 1;
            break;
        }
        case STATE_AUTO_DRIBBLE_POSTURE:
            return 0;
        case STATE_DASH: {
            WORK_DASH *pWork = FsmqGetWork<WORK_DASH>();
            pWork->m_fRotYTrg = fRotY;
            pWork->m_nFlag |= 1;
            return 0;
        }
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeBlockCut(int bNow)
{
    if (CanbeBlockCut_(bNow) == 0) {
        return 0;
    }
    return !(m_fUnk268 >= 1.0f);
}

int msw::CChrCoreBasket::CanbeBlockCut_(int)
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x40000) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (m_nComLevel == 4) {
        return 0;
    }
    if (m_nStatusFlag & 3) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_BLOCK_WORK:
        case STATE_RUN:
        case STATE_SIDE_STEP:
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeNowBlockCut()
{
    return CanbeBlockCut(1);
}

namespace {

    // STATE_BLOCK_CUT (CChrCoreBasket::STATE_WORK_BLOCK_CUT in
    // game/basket/unit_2FD1C.cpp)
    struct WORK_BLOCK_CUT {
        long m_nUnk00;         // 0x00
        long m_nBallNo;        // 0x04
        unsigned long m_nFlag; // 0x08
        float m_fFrame;        // 0x0C
        float m_fRotY;         // 0x10: the direction asked for
        bcs::FVEC3 m_BallPos;  // 0x14
        // 0x20: a full turn when the cut starts from a side step, else 0
        float m_fTurn;
    };
}

// A smash cut is tried first; then the block cut towards fRotY (negative:
// straight ahead). From the block work the trick move of the ball holder
// may slow the camera.
int msw::CChrCoreBasket::MethdBlockCut(float fRotY)
{
    if (MethdSmashCut(fRotY)) {
        return 0;
    }
    if (!CanbeBlockCut(0)) {
        return 0;
    }
    long nState = GetState();
    int bBlockWork = IsNowBlockWork();
    if (fRotY < 0.0f) {
        fRotY = GetRotY();
    }
    switch (nState) {
        case STATE_BLOCK_CUT:
            FsmqGetWork<WORK_BLOCK_CUT>()->m_fRotY = fRotY;
            break;
        case STATE_BLOCK_WORK: {
            AddPos(GetOffsetPos());
            bcs::FVEC3 zero(0.0f, 0.0f, 0.0f);
            SetOffsetPos(zero);
        }
        default: {
            WORK_BLOCK_CUT *pWork = static_cast<WORK_BLOCK_CUT *>(FsmqSetState(STATE_BLOCK_CUT));
            pWork->m_nUnk00 = 0;
            pWork->m_nFlag = 0;
            pWork->m_fRotY = fRotY;
            pWork->m_BallPos.x = 0.0f;
            pWork->m_BallPos.y = 0.0f;
            pWork->m_BallPos.z = 0.0f;
            float fTurn = 0.0f;
            if (nState == STATE_SIDE_STEP) {
                fTurn = 6.2831855f;
            }
            pWork->m_fTurn = fTurn;
            break;
        }
    }
    if (bBlockWork) {
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && !pChr->IsDisable() && pChr->GetState() == STATE_TRICK_MOVE) {
                CChrManBasket::GetInstance()->CheckCameraSlowTrickMove_(fRotY, this,
                                                                        static_cast<CChrCoreBasket *>(pChr));
                break;
            }
        }
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeChgLeaderState()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_nFlagF4 & 0x100000) {
        return 0;
    }
    if (CGame::g_GameWork.GetGameType() == 2) {
        return 0;
    }
    switch (GetState()) {
        case STATE_PASS_CATCH:
            if (IsAutoChrChangeToPassCatch()) {
                if (FsmqGetWork<WORK_PASS_CATCH>()->m_nFlag & 2) {
                    return 0;
                }
            }
            break;
        case STATE_DEFAULT:
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_JUMP:
        case STATE_DAMAGE:
        case STATE_SIDE_STEP:
        case STATE_ITEM_SHOOT:
        case STATE_BLOCK_CUT:
        case STATE_JUMP_UPPER:
        case STATE_DROP_CUT:
        case STATE_FREEZE:
        case STATE_DIZZY:
        case STATE_SLIDING:
        case STATE_PASS:
        case STATE_AERIAL_PASS:
        case STATE_SDB:
        case STATE_RDB:
        case STATE_TRICK_MOVE:
        case STATE_CATCH:
        case STATE_JUMP_SHOOT:
        case STATE_REBOUND_DUNK:
        case STATE_LAY_UP:
        case STATE_ALLEYOOP:
        case STATE_AIR_TRICK:
        case STATE_DUNK:
        case STATE_BIG_DUNK:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_JUMP_BLOCK:
        case STATE_SWAT_IT:
        case STATE_SMASH_CUT:
        case STATE_DASH:
        case STATE_JUMP_CATCH:
        case STATE_DRIBBLE_MISS:
        case STATE_FALL:
        case STATE_REBIRTH:
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeChgNewLeaderState()
{
    if (IsDisable()) {
        return 0;
    }
    switch (GetState()) {
        default:
            return 0;
        case STATE_DEFAULT:
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_JUMP:
        case STATE_DAMAGE:
        case STATE_SIDE_STEP:
        case STATE_ITEM_SHOOT:
        case STATE_BLOCK_CUT:
        case STATE_JUMP_UPPER:
        case STATE_DROP_CUT:
        case STATE_PASS_CATCH:
        case STATE_SDB:
        case STATE_SDB_CHAIN:
        case STATE_RDB:
        case STATE_RDB_CHAIN:
        case STATE_CATCH:
        case STATE_SDB_CATCH:
        case STATE_JUMP_SHOOT:
        case STATE_REBOUND_DUNK:
        case STATE_LAY_UP:
        case STATE_ALLEYOOP:
        case STATE_AIR_TRICK:
        case STATE_DUNK:
        case STATE_BIG_DUNK:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_JUMP_BLOCK:
        case STATE_SWAT_IT:
        case STATE_SMASH_CUT:
        case STATE_JUMP_CATCH:
        case STATE_DRIBBLE_MISS:
            return 1;
    }
}

// pChr (a teammate) may become the leader of the team.
int msw::CChrCoreBasket::CanbeChgLeader(CChrCore *pChr)
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
    if (!(m_nStatusFlag & 0x100)) {
        return 0;
    }
    if (m_nTeamNo != pChr->GetTeamNo()) {
        return 0;
    }
    if (CanbeChgLeaderState() == 0) {
        return 0;
    }
    return pChr->CanbeChgNewLeaderState() != 0;
}

namespace {

    // (inline in the original) The leader may keep the pad: then only its
    // request to change (bit 28 of the word at 0xEC) is dropped.
    inline int CanbeKeepLeader(msw::CChrCoreBasket *pChr)
    {
        if (pChr->IsDisable()) {
            return 0;
        }
        if (pChr->GetFlagEC() & 0x08000000) {
            return 0;
        }
        if (pChr->GetFlagF4() & 0x100000) {
            return 0;
        }
        if (!(pChr->GetStatusFlag() & 0x100)) {
            return 0;
        }
        if (pChr->GetStatusFlag() & 1) {
            return 0;
        }
        return pChr->CanbeChgLeaderState() != 0;
    }
}

// fRotY not negative keeps the leader; else the pad moves to the teammate
// the manager picks (with bAuto the leader itself may be picked).
int msw::CChrCoreBasket::MethdChgLeader(float fRotY, long nDrv1, long nDrv2, int bAuto)
{
    if (!CGame::IsPlayTypeCanChangeLeader(m_nTeamNo)) {
        return 0;
    }
    CChrCore *pNew;
    if (fRotY >= 0.0f) {
        pNew = this;
    } else if (bAuto) {
        pNew = CChrMan::Util_SearchChgLeader(0, m_nTeamNo, 0);
        if (pNew == this) {
            if (CanbeKeepLeader(this)) {
                m_UnkEC &= ~0x10000000;
            }
            return 0;
        }
    } else {
        pNew = CChrMan::Util_SearchChgLeader(0, m_nTeamNo, 0);
    }
    if (!CanbeChgLeader(pNew)) {
        return 0;
    }
    CChrMan::GetInstance()->SetTeamLeader(m_nTeamNo, pNew->GetPlayerNo(), 0);
    DriverConnect(1, 0, 0, 0, 0);
    pNew->DriverConnect(0, nDrv1, nDrv2, 0, 0);
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(5), 0, 0, 0, 0);
    pNew->DriverSendMessage(static_cast<CChrDrv::MSG_ID>(5), 0, 0, 0, 0);
    m_UnkEC &= ~0x10000000;
    return 1;
}

// Hands the pad to player nChr.
int msw::CChrCoreBasket::MethdChgLeader(long nChr, long nDrv1, long nDrv2)
{
    if (!CGame::IsPlayTypeCanChangeLeader(m_nTeamNo)) {
        return 0;
    }
    CChrCore *pNew = CChrMan::GetChrCore(nChr);
    if (!CanbeChgLeader(pNew)) {
        return 0;
    }
    CChrMan::GetInstance()->SetTeamLeader(m_nTeamNo, nChr, 0);
    DriverConnect(1, 0, 0, 0, 0);
    pNew->DriverConnect(0, nDrv1, nDrv2, 0, 0);
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(5), 0, 0, 0, 0);
    pNew->DriverSendMessage(static_cast<CChrDrv::MSG_ID>(5), 0, 0, 0, 0);
    m_UnkEC &= ~0x10000000;
    OnChrChange();
    return 1;
}

namespace {

    // (as in game/basket/unit_25D70.cpp) rOut = b - a.
    inline void SubVec(bcs::FVEC3 &rOut, const bcs::FVEC3 &b, const bcs::FVEC3 &a)
    {
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &b, &a);
        rOut = diff;
    }
}

// The pad moves to the teammate nearest to the ball (to this character
// when there is no ball), leaving out the human players and, for a while
// after a change, the previous leader.
int msw::CChrCoreBasket::MethdChgLeader()
{
    if (!CGame::IsPlayTypeCanChangeLeader(m_nTeamNo)) {
        return 0;
    }
    if (!CanbeChgLeaderState()) {
        return 0;
    }

    CChrCore *pNew = 0;
    const CChrCore *pPrev;
    if (CChrManBasket::IsPrevLeaderFrame(m_nTeamNo)) {
        pPrev = CChrManBasket::GetPrevLeader(m_nTeamNo);
    } else {
        pPrev = 0;
    }
    // (not used: the distances below are measured from the ball even when
    // there is none, as in the original)
    CBall *pBall = CBallMan::GetBallPtr(0);
    const bcs::FVEC3 &rBasePos = pBall != 0 ? pBall->GetPos() : GetPos();
    float fMinDist = 9999.9f;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == GetTeamNo() && pChr != this && pChr != pPrev &&
            !CChrManBasket::GetInstance()->IsPadTypePlayer_(pChr)) {
            bcs::FVEC3 diff;
            SubVec(diff, pBall->GetPos(), pChr->GetPos());
            float fDist = bcs::BCS_Vec_GetLenXZ(diff);
            if (fDist <= fMinDist) {
                fMinDist = fDist;
                pNew = pChr;
            }
        }
    }

    if (pNew == this) {
        if (CanbeKeepLeader(this)) {
            m_UnkEC &= ~0x10000000;
            CChrManBasket::GetInstance()->DispPlayerIcon_(this);
        }
        return 0;
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
    m_UnkEC &= ~0x10000000;
    OnChrChange();
    return 1;
}

namespace {

    // STATE_DUNK, STATE_REBOUND_DUNK, STATE_ALLEYOOP
    // (CChrCoreBasket::STATE_WORK_DUNK in game/basket/unit_38A7C.cpp)
    struct WORK_DUNK {
        long m_nStep;            // 0x00
        long m_nType;            // 0x04
        long m_nUnk08;           // 0x08
        long m_nLoop;            // 0x0C
        // 0x10: 0x1 may still turn into a lay-up, 0x80 on the way down
        unsigned long m_nFlag;
        float m_fFrame;          // 0x14
        float m_fRestFrame;      // 0x18: frames left to the dunk
        float m_fFrameMax;       // 0x1C
        float m_fSpeedRate;      // 0x20
        float m_fTime;           // 0x24: frames in the state
        unsigned char m_Unk28[0x60 - 0x28];
        bcs::FVEC3 m_GoalPos;    // 0x60: the goal dunked at
    };

    // STATE_LAY_UP (CChrCoreBasket::STATE_WORK_LAYUP)
    struct WORK_LAYUP {
        long m_nStep;            // 0x00
        unsigned long m_nFlag;   // 0x04: 0x1 from the right, 0x2 from the left, 0x4 straight
        unsigned char m_Unk08[0x24 - 0x08];
        bcs::FVEC3 m_Target;     // 0x24
    };
}

int msw::CChrCoreBasket::CanbeCatch()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x80000) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    if (m_nComLevel != 0 && !(m_nStatusFlag & 0x20000)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_REBOUND_DUNK:
        case STATE_ALLEYOOP:
            // checked only on the way down
            if (!(FsmqGetWork<WORK_DUNK>()->m_nFlag & 0x80)) {
                break;
            }
        case STATE_DROP_CUT:
        case STATE_DUNK:
        case STATE_SWAT_IT:
            if (m_nStatusFlag & 0x1400) {
                return 0;
            }
            break;
        case STATE_DAMAGE:
        case STATE_MOT_PLAY:
            return 0;
        case STATE_ITEM_SHOOT:
            if (IsHoldItem()) {
                return 0;
            }
            if (IsHaveStockItem()) {
                return 0;
            }
            break;
        case STATE_JUMP_BLOCK:
            if (AnimGetMotID(3, 0) != 8 && (m_nStatusFlag & 0x1400)) {
                return 0;
            }
            break;
    }
    return 1;
}

namespace {

    // STATE_CATCH
    struct WORK_CATCH {
        long m_nStep;          // 0x00
        unsigned long m_nFlag; // 0x04
        float m_fRotY;         // 0x08
    };
}

// A dribbling character catches the ball again; one catching turns to the
// goal.
int msw::CChrCoreBasket::MethdCatch()
{
    if (!CanbeCatch()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SDB:
        case STATE_RDB:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_SMASH_CUT:
            m_nStatusFlag |= 0x80;
            FsmqClear();
            Util_PushBackCatch(0);
            return 1;
        case STATE_CATCH:
            FsmqGetWork<WORK_CATCH>()->m_nFlag = 0;
            RotateYRate_(GetRotY_ToPosXZ(GetTargetGoalPos()), 0.2f);
            return 1;
        default:
            return 0;
    }
}

namespace {

    // A state that shoots or dunks.
    inline int IsShootState(long nState)
    {
        if ((nState >= msw::CChrCore::STATE_REBOUND_DUNK && nState <= msw::CChrCore::STATE_ALLEYOOP) ||
            nState == msw::CChrCore::STATE_JUMP_SHOOT) {
            return 1;
        }
        return 0;
    }
}

int msw::CChrCoreBasket::CanbeJumpShoot()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x400) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SDB_CATCH:
            if (FsmqGetNextState() == STATE_PASS) {
                return 0;
            }
            if (IsShootState(FsmqGetNextState())) {
                return 0;
            }
        case STATE_SDB:
        case STATE_SDB_CHAIN:
        case STATE_RDB:
        case STATE_RDB_CHAIN:
        case STATE_QUICK_TURN:
        case STATE_TRICK_MOVE:
        case STATE_CATCH:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_DASH:
        case STATE_TURN:
            break;
        case STATE_PASS_CATCH:
            if (GetDist_ToPosXZ(GetGuardGoalPos()) < 2.5f) {
                return 0;
            }
            if (FsmqGetNextState() == STATE_PASS) {
                return 0;
            }
            if (IsShootState(FsmqGetNextState())) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::MethdJumpShoot()
{
    return MethdJumpShoot(-1.0f, 0);
}

namespace {

    // STATE_JUMP_SHOOT
    struct WORK_JUMP_SHOOT {
        long m_nStep;          // 0x00
        unsigned char m_Unk04[0x0C - 0x04];
        unsigned long m_nFlag; // 0x0C: bit 4 shoot the stocked item
        unsigned char m_Unk10[0x18 - 0x10];
        float m_fRotY;         // 0x18: the yaw asked for, negative for none
        float m_fFrame;        // 0x1C: frames since the jump began
    };
}

// The shot is queued after a catch, a turn or a chain still running.
int msw::CChrCoreBasket::MethdJumpShoot(float fRotY, int bItem)
{
    if (!CanbeJumpShoot()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SDB:
        case STATE_RDB:
        case STATE_TRICK_MOVE:
        case STATE_CATCH:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_DASH:
        case STATE_TURN:
            FsmqClear();
            break;
        case STATE_PASS_CATCH:
        case STATE_SDB_CHAIN:
        case STATE_RDB_CHAIN:
        case STATE_QUICK_TURN:
        case STATE_SDB_CATCH:
            FsmqCutQueue();
            break;
    }
    WORK_JUMP_SHOOT *pWork = static_cast<WORK_JUMP_SHOOT *>(FsmqPushWork(STATE_JUMP_SHOOT));
    pWork->m_nStep = 0;
    pWork->m_nFlag = 0;
    pWork->m_fRotY = fRotY;
    if (bItem && CanbeUseItem()) {
        pWork->m_nFlag |= 0x10;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeLayUp()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x10000) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_DUNK:
            return (m_nStatusFlag >> 10) & 1;
        case STATE_REBOUND_DUNK:
        case STATE_ALLEYOOP:
            return 0;
        default:
            return 0;
    }
}

// A dunk on its way turns into a lay-up at the same goal; on stage 5 the
// lay-up goes at most fDist towards the goal.
int msw::CChrCoreBasket::MethdLayUp()
{
    static const float fDist = 6.0f;

    if (!CanbeLayUp()) {
        return 0;
    }
    AnimStop(3);
    bcs::FVEC3 goal = GetTargetGoalPos();
    if (GetState() == STATE_DUNK) {
        goal = FsmqGetWork<WORK_DUNK>()->m_GoalPos;
        Util_NodePatchWarpToTransNode();
        WORK_LAYUP *pWork = static_cast<WORK_LAYUP *>(FsmqSetState(STATE_LAY_UP));
        pWork->m_nFlag = 0;
        if (CGame::g_GameWork.GetStage() == 5) {
            pWork->m_Target = GetTargetGoalPos();
            bcs::FVEC3 dir;
            SubVec(dir, pWork->m_Target, GetPos());
            if (bcs::BCS_Vec_GetLenXZ(dir) > fDist) {
                bcs::FVEC3 xz(dir.x, 0.0f, dir.z);
                bcs::BCS_Vec_Normalize(dir, xz);
                pWork->m_Target.x = fDist * dir.x + GetPos().x;
                pWork->m_Target.z = fDist * dir.z + GetPos().z;
            }
        } else {
            pWork->m_Target = goal;
        }
        m_nBasketFlag &= ~0x8000;
        pWork->m_nStep = 0;
        return 1;
    }
    return 0;
}

// nSide: 0 from the left, 1 from the right, else straight.
int msw::CChrCoreBasket::MethdLayUp(int nSide)
{
    if (MethdLayUp()) {
        WORK_LAYUP *pWork = FsmqGetWork<WORK_LAYUP>();
        switch (nSide) {
            case 0:
                pWork->m_nFlag |= 2;
                break;
            case 1:
                pWork->m_nFlag |= 1;
                break;
            default:
                pWork->m_nFlag |= 4;
                break;
        }
        return 1;
    }
    return 0;
}

int msw::CChrCoreBasket::CanbeRebound()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 2) {
        return 0;
    }
    if (m_nStatusFlag & 1) {
        return 0;
    }
    if (m_nStatusFlag & 2) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 8) {
        return 1;
    }
    if (CGame::g_GameWork.GetGameType() != 2) {
        CBall *pBall = CBallMan::GetBallPtr(0);
        if (!CBallManBasket::GetInstance()->CatchEnable_(pBall)) {
            return 0;
        }
    }
    return 1;
}

// A dunk of the rebound: the ball comes off the ring of the goal the team
// attacks, within 3 of it and above it.
int msw::CChrCoreBasket::CanbeReboundDunk()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x8000) {
        return 0;
    }
    if (GetFlag(1)) {
        return 0;
    }
    if (GetFlag(0x8000) || GetFlag(0x10000)) {
        return 0;
    }
    if (CGame::g_GameWork.m_nUserTeam != m_nTeamNo) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    if (IsHoldItem()) {
        return 0;
    }
    if (!CanbeDunkJump()) {
        return 0;
    }
    if (CGame::g_GameWork.GetGameType() == 2) {
        return 0;
    }
    CBall *pBall = CBallMan::GetBallPtr(0);
    if (pBall == 0) {
        return 0;
    }
    if (pBall->IsAttach()) {
        return 0;
    }
    const bcs::FVEC3 &rGoal = GetTargetGoalPos();
    if (pBall->GetPos().y < rGoal.y) {
        return 0;
    }
    bcs::FVEC3 diff;
    SubVec(diff, rGoal, pBall->GetPos());
    if (bcs::BCS_Vec_GetLen(diff) >= 3.0f) {
        return 0;
    }
    m_nUnk17B = 1;
    if (GetState() != STATE_RUN) {
        return 0;
    }
    return DunkRotCheck_() != 0;
}

int msw::CChrCoreBasket::MethdReboundDunk()
{
    if (!CanbeReboundDunk()) {
        return 0;
    }
    FsmqSetState(STATE_REBOUND_DUNK);
    return 1;
}

namespace {

    // (inline in the original) The characters whose special shot is a
    // dunk (0, and 0x13 to 0x18).
    inline int IsArtsDunkChr(const msw::CChrCoreBasket *pChr)
    {
        long nChr = pChr->GetChrTypeID();
        if ((nChr < 0x13 || nChr > 0x18) && nChr != 0) {
            return 0;
        }
        return 1;
    }

    // (inline in the original) The air trick is at a dunk step (2, 3, 6 or
    // 7) without a blocker yet.
    inline int IsAirTrickDunkBlockEnable(msw::CChrCoreBasket *pChr)
    {
        if (pChr->GetState() != msw::CChrCore::STATE_AIR_TRICK) {
            return 0;
        }
        msw::CChrCoreBasket::STATE_WORK_AIR_TRICK *pWork =
            pChr->FsmqGetWork<msw::CChrCoreBasket::STATE_WORK_AIR_TRICK>();
        long nStep = pWork->m_nStep;
        if ((nStep >= 2 && nStep <= 3) || (nStep >= 6 && nStep <= 7)) {
            if (pWork->m_pBlocker != 0) {
                return 0;
            }
            return IsArtsDunkChr(pChr);
        }
        return 0;
    }
}

// pChr (of the other team, in front, towards the goal) answers the dunk of
// the air trick with a block: the dunk slows down to fDownRate percent.
int msw::CChrCoreBasket::MethdAirTrickDunkToEndure(CChrCore *pChr)
{
    static const float fDownRate = 60.0f;

    if (!IsAirTrickDunkBlockEnable(this)) {
        return 0;
    }
    if (pChr == 0) {
        return 0;
    }
    if (pChr->GetTeamNo() == m_nTeamNo) {
        return 0;
    }
    float fGoal = GetRotY_ToGoal();
    float fDiff = bcs::ROUND_PI(GetRotY_ToPosXZ(pChr->GetPos()) - fGoal);
    if (fDiff > 3.1415927f) {
        fDiff -= 6.2831855f;
    }
    if (__fabsf(fDiff) > 1.0471976f) {
        return 0;
    }
    if (!pChr->MethdSpecialBattle()) {
        return 0;
    }
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    pWork->m_pBlocker = static_cast<CChrCoreBasket *>(pChr);
    if (!IsArtsDunkChr(this)) {
        return 0;
    }
    pWork->m_fSpeed = fDownRate / 100.0f;
    AnimSetSpeed(3, fDownRate / 100.0f);
    pWork->m_nFlag |= 0x20;
    return 1;
}

int msw::CChrCoreBasket::CanbeAirTrick()
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
    if (CGame::g_GameWork.m_nUnk180 != 0) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (CGame::g_GameWork.m_nUnk40 & 8) {
        return 0;
    }
    if (!CGameBasket::GetInstance()->IsNowMainLoopEnable_()) {
        return 0;
    }
    if (CGameBasket::GetInstance()->IsRiverFallRestart_()) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    if (CGame::g_GameWork.GetGameType() == 2) {
        return 0;
    }
    if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
        return 0;
    }
    if (CGame::GetTeamArtsGauge(m_nTeamNo) < 1.0f) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SDB:
        case STATE_SDB_CHAIN:
        case STATE_RDB:
        case STATE_RDB_CHAIN:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_CATCH:
            break;
        default:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::MethdAirTrick()
{
    if (!CanbeAirTrick()) {
        return 0;
    }
    FsmqSetState(STATE_AIR_TRICK);
    bcs::FVEC3 pos = GetPos();
    pos.y = 0.0f;
    SetPos(pos);
    CGameBasket::GetInstance()->SetOffenseTeam_(m_nTeamNo);
    CGame::SetLooseBallMode(0);
    m_nStatusFlag &= ~4;
    PlaySe(0xC06, 1, -1, 0.0f, 1.0f, 0.0f);
    return 1;
}

// The player slows the fall of the air trick: fRate is added to the
// motion's speed (-1 stops it).
int msw::CChrCoreBasket::MethdAirTrickDownSpeed(float fRate)
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    if (fRate == -1.0f) {
        pWork->m_nFlag |= 0x40;
        AnimSetSpeed(3, 0.0f);
    } else {
        pWork->m_fSpeed += fRate;
        if (pWork->m_fSpeed < 0.0f) {
            pWork->m_fSpeed = 0.0f;
        }
        AnimSetSpeed(3, pWork->m_fSpeed);
    }
    return 1;
}

// A dunk is allowed: not in a tournament mission that forbids it, and for
// the characters that dunk only from close (the byte at 0x17B above 1)
// only inside the three-point line.
int msw::CChrCoreBasket::CanbeDunkJump() const
{
    if (CTournament::IsMissionDunkShootDisable(0)) {
        return 0;
    }
    if (GetUnk17B() > 1 && !IsTargetGoal3PointArea(GetPos(), 0.7f + GetCourt3PointSize())) {
        return 0;
    }
    return 1;
}

// On stage 5 a dunk from further than fDist from the goal misses.
int msw::CChrCoreBasket::CanbeDunkMissDist() const
{
    static const float fDist = 9.0f;

    if (IsDisable()) {
        return 1;
    }
    if (CGame::g_GameWork.GetStage() != 5) {
        return 0;
    }
    bcs::FVEC3 diff;
    SubVec(diff, GetPos(), GetTargetGoalPos());
    return fDist < bcs::BCS_Vec_GetLenXZ(diff);
}

int msw::CChrCoreBasket::CanbeDunk()
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
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (m_nStatusFlag & 2) {
        return 0;
    }
    if (!IsHoldBall()) {
        return 0;
    }
    if (!CanbeDunkJump()) {
        return 0;
    }
    m_nUnk17B = 1;
    switch (GetState()) {
        case STATE_RDB:
        case STATE_RDB_CHAIN:
        case STATE_RUN:
        case STATE_TRICK_MOVE:
        case STATE_DASH:
            break;
        case STATE_AUTO_DRIBBLE_POSTURE:
            if (!(FsmqGetWork<WORK_AUTO_DPM>()->m_nFlag & 0x10)) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    return DunkRotCheck_() != 0;
}

int msw::CChrCoreBasket::MethdDunk()
{
    if (!CanbeDunk()) {
        return 0;
    }
    FsmqSetState(STATE_DUNK);
    return 1;
}

namespace {

    // The states a pass may start from after a catch (no pass or shot is
    // queued after it).
    inline int IsPassQueueFree(msw::CChrCoreBasket *pChr)
    {
        if (pChr->FsmqGetNextState() == msw::CChrCore::STATE_PASS) {
            return 0;
        }
        if (IsShootState(pChr->FsmqGetNextState())) {
            return 0;
        }
        return 1;
    }
}

// pChr: the receiver (null for a pass in a direction).
int msw::CChrCoreBasket::CanbePass(CChrCore *pChr)
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x20) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_JUMP_SHOOT:
        case STATE_DUNK:
            return 0;
        case STATE_PASS_CATCH:
        case STATE_SDB_CATCH:
            if (FsmqGetNextState() == STATE_PASS) {
                return 0;
            }
            if (IsShootState(FsmqGetNextState())) {
                return 0;
            }
        case STATE_SDB:
        case STATE_SDB_CHAIN:
        case STATE_RDB:
        case STATE_RDB_CHAIN:
        case STATE_QUICK_TURN:
        case STATE_TRICK_MOVE:
        case STATE_TRICK_DANCE:
        case STATE_CATCH:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_DASH:
        case STATE_TURN:
            if (pChr == 0) {
                return 1;
            }
            if (this == pChr) {
                return 0;
            }
            if (m_nTeamNo != pChr->GetTeamNo()) {
                return 0;
            }
            return pChr->CanbePassCatchState() != 0;
        default:
            return 0;
    }
}

namespace {

    // (inline in the original) A pass from a catch in a battle is a battle
    // pass; from the dribble posture the ball changes hands for it.
    inline int StartPass(msw::CChrCoreBasket *pChr, msw::CChrCore *pTo, msw::CNpcBase *pNpc, float fRotY,
                         long bTarget, const bcs::FVEC3 *pPos, int nUnk, long nType)
    {
        switch (pChr->GetState()) {
            case msw::CChrCore::STATE_CATCH:
                if (pChr->IsBattle()) {
                    nType = 4;
                }
                break;
            case msw::CChrCore::STATE_AUTO_DRIBBLE_POSTURE:
                if (pChr->FsmqGetWork<WORK_AUTO_DPM>()->m_nDir == 1) {
                    pChr->ResetFlag(4);
                } else {
                    pChr->OnStatusFlag(4);
                }
                break;
        }
        return pChr->MethdPass_(pTo, pNpc, fRotY, bTarget, pPos, nUnk, nType);
    }
}

// A pass towards fRotY: the manager picks the receiver (or the ghost).
// With the special gauge at half or more the special pass starts instead.
int msw::CChrCoreBasket::MethdPass(float fRotY, int bLong)
{
    if (m_fUnk264 >= 0.5f && CanbePass(0)) {
        FsmqSetState(STATE_ARTS_PASS);
        return 1;
    }
    if (!CanbePass(0)) {
        return 0;
    }
    CChrCore *pTo = 0;
    long nType = -1;
    CNpcBase *pNpc = 0;
    bcs::FVEC3 pos;
    CChrManBasket::GetInstance()->Util_SearchPassTo_(&pTo, &pNpc, m_nPlayerNo, fRotY, pos);
    if (bLong) {
        if (m_nFlagF4 & 0x40) {
            return 0;
        }
        nType = 3;
    }
    switch (GetState()) {
        case STATE_CATCH:
            if (IsBattle()) {
                nType = 4;
            }
            break;
        case STATE_AUTO_DRIBBLE_POSTURE:
            if (FsmqGetWork<WORK_AUTO_DPM>()->m_nDir == 1) {
                m_nStatusFlag &= ~4;
            } else {
                m_nStatusFlag |= 4;
            }
            break;
    }
    return MethdPass_(pTo, pNpc, fRotY, 1, &pos, 1, nType);
}

// A pass to pTo; in the air an aerial pass.
int msw::CChrCoreBasket::MethdPass(CChrCore *pTo, int bLong)
{
    if (m_nStatusFlag & 0x1400) {
        if (MethdAerialPass_(pTo, 0, GetRotY_ToPosXZ(pTo->GetPos()), 1, &pTo->GetPos(), 1)) {
            return 1;
        }
    }
    if (m_fUnk264 >= 0.5f && CanbePass(0)) {
        FsmqSetState(STATE_ARTS_PASS);
        return 1;
    }
    float fCamRotY = m_fCamRotY;
    long nType = -1;
    float fRotY = bcs::ROUND_PI(GetRotY_ToPosXZ(pTo->GetPos()) + fCamRotY);
    bcs::FVEC3 pos;
    if (pTo != 0) {
        pos = pTo->GetPos();
    }
    if (bLong) {
        if (m_nFlagF4 & 0x40) {
            return 0;
        }
        nType = 3;
    }
    return StartPass(this, pTo, 0, fRotY, 1, &pos, 1, nType);
}

int msw::CChrCoreBasket::CanbeJumpPass()
{
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    // not already in the air
    return ((m_nStatusFlag >> 10) & 1) ^ 1;
}

namespace {

    // STATE_JUMP_PASS (CChrCoreBasket::STATE_WORK_JUMP_PASS)
    struct WORK_JUMP_PASS {
        long m_nStep;               // 0x00
        float m_fShotFrame;         // 0x04
        msw::CChrCore *m_pTo;       // 0x08: the receiver
    };
}

// A pass in a jump to player nChr.
int msw::CChrCoreBasket::MethdJumpPass(long nChr)
{
    if (!CanbeJumpPass()) {
        return 0;
    }
    long nState = GetState();
    if (nState != STATE_RDB && nState != STATE_SDB && nState != STATE_CATCH && nState != STATE_AUTO_DRIBBLE_POSTURE &&
        nState != STATE_TURN && nState != STATE_DASH) {
        return 0;
    }
    WORK_JUMP_PASS *pWork = static_cast<WORK_JUMP_PASS *>(FsmqSetState(STATE_JUMP_PASS));
    pWork->m_pTo = CChrMan::GetChrCore(nChr);
    return 1;
}

int msw::CChrCoreBasket::CanbeDash() const
{
    if (m_nBasketFlag & 0x80000) {
        return 0;
    }
    long nState = GetState();
    if (nState != STATE_SDB && nState != STATE_RDB && nState != STATE_TURN && nState != STATE_AUTO_DRIBBLE_POSTURE &&
        nState != STATE_CATCH && nState != STATE_DASH) {
        return 0;
    }
    return 1;
}

namespace {

    // STATE_DASH (CChrCoreBasket::STATE_WORK_DASH in game/basket/unit_25D70.cpp)
    struct WORK_DASH_START {
        long m_nStep;          // 0x00
        long m_nMot;           // 0x04: the dribble motion of the dash
        // 0x08: bit 0 the stick is held, bit 1 a turn ends the dash, bit 3
        // the start effect, bit 5 a dash of the block work
        unsigned long m_nFlag;
        float m_fRotYTrg;      // 0x0C
        float m_fRotY;         // 0x10
        float m_fUnk14;        // 0x14
        float m_fStartFrame;   // 0x18: the frame the dribble motion starts at
        float m_fFrame;        // 0x1C
        float m_fFrameNum;     // 0x20
    };

    // STATE_TURN (the parts a turn from the dribble posture sets)
    struct WORK_TURN_START {
        long m_nStep;          // 0x00
        long m_nMot;           // 0x04
        unsigned long m_nFlag; // 0x08
        float m_fFrame;        // 0x0C
        float m_fFrameNum;     // 0x10
        float m_fRotYEnd;      // 0x14
        float m_fMoveRotY;     // 0x18
        float m_fRotY;         // 0x1C
        float m_fUnk20;        // 0x20
    };
}

// The dash: from the dribble posture a turn of more than the parameter's
// angle first turns round.
int msw::CChrCoreBasket::MethdDash()
{
    if (!CanbeDash()) {
        return 0;
    }
    unsigned long nFlag;
    float fRotY;
    float fStartFrame;
    long nMot;
    float fFrame;
    switch (GetState()) {
        case STATE_RDB:
            fRotY = FsmqGetWork<WORK_RDB>()->m_fRotY;
            nFlag = 0xB;
            fStartFrame = AnimGetFrame(3);
            nMot = -0x3E;
            if (!(m_nStatusFlag & 4)) {
                nMot = 0x3E;
            }
            fFrame = 0.0f;
            break;
        case STATE_TURN: {
            WORK_TURN *pTurn = FsmqGetWork<WORK_TURN>();
            nFlag = 3;
            if (pTurn->m_nFlag & 0x10) {
                nFlag |= 0x20;
            }
            nMot = -0x3E;
            fRotY = pTurn->m_fRotYEnd;
            fStartFrame = 10.0f;
            if (!(m_nStatusFlag & 4)) {
                nMot = 0x3E;
            }
            fFrame = 0.0f;
            break;
        }
        case STATE_AUTO_DRIBBLE_POSTURE: {
            WORK_AUTO_DPM *pDpm = FsmqGetWork<WORK_AUTO_DPM>();
            nFlag = 0xB;
            fRotY = pDpm->m_fRotY;
            nMot = -0x3E;
            fStartFrame = 0.0f;
            if (!(m_nStatusFlag & 4)) {
                nMot = 0x3E;
            }
            fFrame = 0.0f;
            const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
            float fDiff = bcs::ROUND_PI(fRotY - GetRotY());
            if (fDiff > 3.1415927f) {
                fDiff -= 6.2831855f;
            }
            if (__fabsf(fDiff) > bcs::DEG_TO_RAD(pParam->m_nDashTurnDeg)) {
                WORK_TURN_START *pTurn = static_cast<WORK_TURN_START *>(FsmqPushWork(STATE_TURN));
                if (m_nStatusFlag & 4) {
                    pTurn->m_nMot = -0x3F;
                    m_nStatusFlag |= 4;
                } else {
                    pTurn->m_nMot = 0x3F;
                    m_nStatusFlag &= ~4;
                }
                pTurn->m_nFlag = 0;
                pTurn->m_nFlag |= 0x33;
                pTurn->m_fMoveRotY = GetRotY();
                pTurn->m_fRotY = fRotY;
                pTurn->m_fRotYEnd = fRotY;
                pTurn->m_fUnk20 = 0.0f;
                FsmqDoChain();
                return 1;
            }
            break;
        }
        case STATE_CATCH:
            nFlag = 0xB;
            fRotY = GetRotY();
            fStartFrame = 10.0f;
            nMot = -0x3E;
            if (!(m_nStatusFlag & 4)) {
                nMot = 0x3E;
            }
            fFrame = 0.0f;
            break;
        case STATE_DASH:
            // a dash asked for again only starts its count again
            GetCcParam2_();
            FsmqGetWork<WORK_DASH_START>()->m_fFrame = 0.0f;
            return 0;
        default:
            return 0;
    }
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    WORK_DASH_START *pWork = static_cast<WORK_DASH_START *>(FsmqPushWork(STATE_DASH));
    pWork->m_nFlag = nFlag;
    pWork->m_fRotYTrg = fRotY;
    pWork->m_fStartFrame = fStartFrame;
    pWork->m_nMot = nMot;
    pWork->m_fFrame = fFrame;
    pWork->m_fFrameNum = pParam->m_nDashFrame;
    if (IsBattle()) {
        pWork->m_nFlag |= 0x20;
    }
    AnimSetSpeed(3, 1.0f);
    FsmqDoChain();
    return 1;
}

int msw::CChrCoreBasket::CanbeJumpCatch()
{
    if (!CanbeRebound()) {
        return 0;
    }
    if (GetState() == STATE_JUMP_BLOCK) {
        if (CGame::g_GameWork.GetGameType() != 2) {
            // only from the jump block's motions that reach for the ball
            switch (AnimGetMotID(3, 0)) {
                case 8:
                case 9:
                case 4:
                case 6:
                    break;
                case 5:
                    if (GetChrTypeID() != 8 && GetChrTypeID() != 0x10) {
                        return 0;
                    }
                    break;
                default:
                    return 0;
            }
        }
        return 1;
    }
    return 0;
}

namespace {

    // STATE_JUMP_BLOCK (CChrCoreBasket::STATE_WORK_JUMP_BLOCK in
    // game/basket/unit_2BBEC.cpp)
    struct WORK_JUMP_BLOCK {
        long m_nStep;              // 0x00
        long m_nUnk04;             // 0x04
        // 0x08: 0x8 at the start, 0x400000 the jump to the upper floor
        // asked for too early
        unsigned long m_nFlag;
        long m_nUnk0C;             // 0x0C
        float m_fRotY;             // 0x10: the yaw to turn to (negative: none)
        float m_fFrame;            // 0x14: frames in the air
        float m_fRotYAsked;        // 0x18: the yaw asked for (negative: none)
        float m_fDunkBattleFrame;  // 0x1C
        long m_nUnk20;             // 0x20
        float m_fFrameAll;         // 0x24: frames since the state began
        unsigned char m_Unk28[0x40 - 0x28];
        float m_fReboundRotY;      // 0x40: the yaw to the other team
        void *m_pDunker;           // 0x44
        unsigned long m_hBall;     // 0x48: task id of the ball jumped at
    };

    // STATE_JUMP_CATCH
    struct WORK_JUMP_CATCH {
        long m_nStep;          // 0x00
        float m_fRotY;         // 0x04: the yaw to turn to
        float m_fUnk08;        // 0x08
        float m_fFrame;        // 0x0C
    };
}

int msw::CChrCoreBasket::MethdJumpCatch(float fRate)
{
    if (!CanbeJumpCatch()) {
        return 0;
    }
    if (CGame::g_GameWork.GetGameType() != 2 && AnimGetMotID(3, 0) != 8) {
        return 0;
    }
    // (CanbeJumpCatch allows only the jump block, so the yaw is always set)
    float fRotY;
    if (GetState() == STATE_JUMP_BLOCK) {
        fRotY = FsmqGetWork<WORK_JUMP_BLOCK>()->m_fReboundRotY;
    }
    WORK_JUMP_CATCH *pWork = static_cast<WORK_JUMP_CATCH *>(FsmqSetState(STATE_JUMP_CATCH));
    pWork->m_nStep = 0;
    pWork->m_fFrame = 0.0f;
    pWork->m_fUnk08 = fRate;
    pWork->m_fRotY = fRotY;
    return 1;
}

namespace {

    // STATE_TRICK_MOVE
    struct WORK_TRICK_MOVE {
        long m_nStep;          // 0x00
        unsigned char m_Unk04[0x14 - 0x04];
        float m_fFrame;        // 0x14: frames of the move so far
    };
}

// A trick move held for 10 frames or more may lose the ball.
int msw::CChrCoreBasket::CanbeDribbleMiss()
{
    if (!IsHoldBall()) {
        return 0;
    }
    if (GetState() == STATE_TRICK_MOVE) {
        return !(10.0f > FsmqGetWork<WORK_TRICK_MOVE>()->m_fFrame);
    }
    return 0;
}

int msw::CChrCoreBasket::MethdDribbleMiss(float fRotY)
{
    if (!CanbeDribbleMiss()) {
        return 0;
    }
    if (GetState() == STATE_DRIBBLE_MISS) {
        return 0;
    }
    float *pWork = static_cast<float *>(FsmqSetState(STATE_DRIBBLE_MISS));
    *pWork = fRotY;
    return 1;
}

int msw::CChrCoreBasket::CanbePassIdx(long nChr)
{
    return CanbePass(CChrMan::GetChrCore(nChr));
}

int msw::CChrCoreBasket::MethdPassIdx(long nChr)
{
    CChrCore *pTo = CChrMan::GetChrCore(nChr);
    if (pTo != 0) {
        return MethdPass_(pTo, 0, GetRotY_ToChr(pTo), 0, 0, 1, -1);
    }
    return 0;
}

// For the pad only: an aerial pass, else a pass, towards fRotY, marked as
// a combination.
int msw::CChrCoreBasket::MethdCombinationPass(float fRotY)
{
    if (!IsPadCtrl()) {
        return 0;
    }
    if (MethdAerialPass(fRotY, 0)) {
        m_nBasketFlag |= 0x100000;
        return 1;
    }
    if (MethdPass(fRotY, 0)) {
        m_nBasketFlag |= 0x100000;
        return 1;
    }
    return 0;
}

namespace {

    // STATE_SDB_CATCH (the pass asked for while the catch plays)
    struct WORK_SDB_CATCH {
        unsigned char m_Unk00[0x14];
        long m_nPassKind;      // 0x14: 1 to a character, 2 in a direction
        long m_nPassChr;       // 0x18
        float m_fPassRotY;     // 0x1C
        long m_bPass;          // 0x20
    };

    // STATE_PASS (CChrCoreBasket::STATE_WORK_PASS in game/basket/unit_3BB84.cpp)
    struct WORK_PASS {
        long m_nStep;              // 0x00
        long m_nUnk04;             // 0x04: 1 aims at m_Target
        long m_nPassType;          // 0x08
        long m_nUnk0C;             // 0x0C
        msw::CChrCore *m_pTo;      // 0x10
        msw::CNpcBase *m_pGhost;   // 0x14
        float m_fRotY;             // 0x18
        float m_fShotFrame;        // 0x1C
        float m_fDirRotY;          // 0x20
        float m_fSplineRate;       // 0x24
        int m_bBattle;             // 0x28
        int m_nUnk2C;              // 0x2C
        bcs::FVEC3 m_Target;       // 0x30
    };
}

// Starts the pass to pTo (or the ghost pNpc, or towards fRotY), aiming at
// pPos when given. nType (when bTarget is 1 and nType not negative) is
// the kind of pass.
int msw::CChrCoreBasket::MethdPass_(CChrCore *pTo, CNpcBase *pNpc, float fRotY, long bTarget, const bcs::FVEC3 *pPos,
                                     int nUnk, long nType)
{
    float fPassRotY = 0.0f;
    long nPassChr = 0;
    long nPassType = 0;
    if (!CanbePass(pTo)) {
        return 0;
    }
    long nKind;
    if (pTo != 0) {
        nKind = 1;
        nPassChr = pTo->GetPlayerNo();
    } else {
        nKind = 2;
        fPassRotY = fRotY;
    }
    int bBattle = 0;
    switch (GetState()) {
        case STATE_SDB:
        case STATE_RDB:
            FsmqClear();
            nPassType = 2;
            break;
        case STATE_SDB_CHAIN:
        case STATE_RDB_CHAIN:
        case STATE_QUICK_TURN:
            FsmqCutQueue();
            nPassType = 2;
            break;
        case STATE_SDB_CATCH: {
            WORK_SDB_CATCH *pCatch = FsmqGetWork<WORK_SDB_CATCH>();
            pCatch->m_nPassKind = nKind;
            pCatch->m_nPassChr = nPassChr;
            pCatch->m_fPassRotY = fPassRotY;
            pCatch->m_bPass = 1;
            FsmqClear();
            nPassType = 2;
            break;
        }
        case STATE_CATCH:
            bBattle = IsBattle();
            FsmqClear();
            break;
        case STATE_PASS_CATCH:
            FsmqCutQueue();
            break;
        case STATE_JUMP_SHOOT:
        case STATE_DUNK:
            CGame::SendMessage(0x32, 0, m_nPlayerNo, 0);
        default:
            nPassType = 0;
            FsmqClear();
            break;
    }
    if (bTarget == 1 && nType >= 0) {
        nPassType = nType;
    }
    WORK_PASS *pWork = static_cast<WORK_PASS *>(FsmqPushWork(STATE_PASS));
    pWork->m_nStep = 0;
    pWork->m_pTo = pTo;
    pWork->m_pGhost = pNpc;
    pWork->m_nUnk04 = bTarget;
    pWork->m_nPassType = nPassType;
    pWork->m_fRotY = fRotY;
    pWork->m_fDirRotY = fRotY;
    pWork->m_bBattle = bBattle;
    pWork->m_nUnk2C = nUnk;
    pWork->m_nUnk0C = 0;
    if (pPos != 0) {
        pWork->m_Target = *pPos;
    }
    return 1;
}

// pChr: the receiver (null for a pass in a direction).
int msw::CChrCoreBasket::CanbeAerialPass(CChrCore *pChr)
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 0x20) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    if (m_nStatusFlag & 0x800) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (CGame::g_GameWork.GetGameType() == 2) {
        return 0;
    }
    switch (GetState()) {
        case STATE_JUMP_SHOOT: {
            WORK_JUMP_SHOOT *pWork = FsmqGetWork<WORK_JUMP_SHOOT>();
            if (pWork->m_nFlag & 4) {
                return 0;
            }
            if (pWork->m_fFrame < 10.0f) {
                return 0;
            }
            break;
        }
        case STATE_PASS_CATCH:
        case STATE_CATCH:
        case STATE_AERIAL_SHOOT:
        case STATE_JUMP_CATCH:
            if (!(m_nStatusFlag & 0x400)) {
                return 0;
            }
            break;
        case STATE_LAY_UP:
            return 0;
        case STATE_TRICK_DANCE:
            if (!(GetWaistBaseHeight() >= 2.0f)) {
                return 0;
            }
            break;
        case STATE_REBOUND_DUNK:
        case STATE_ALLEYOOP:
            return 0;
        case STATE_AIR_TRICK:
            return 0;
        case STATE_DUNK:
            if (FsmqGetWork<WORK_DUNK>()->m_fTime < 10.0f) {
                return 0;
            }
            break;
        default:
            return 0;
    }
    if (pChr == 0) {
        return 1;
    }
    if (this == pChr) {
        return 0;
    }
    if (m_nTeamNo != pChr->GetTeamNo()) {
        return 0;
    }
    return pChr->CanbePassCatchState() != 0;
}

int msw::CChrCoreBasket::MethdAerialPass(float fRotY, int)
{
    CChrCore *pTo = 0;
    CNpcBase *pNpc = 0;
    bcs::FVEC3 pos;
    CChrManBasket::GetInstance()->Util_SearchPassTo_(&pTo, &pNpc, m_nPlayerNo, fRotY, pos);
    return MethdAerialPass_(pTo, pNpc, fRotY, 1, &pos, 1);
}

int msw::CChrCoreBasket::CanbeAerialPassIdx(long nChr)
{
    return CanbeAerialPass(CChrMan::GetChrCore(nChr));
}

int msw::CChrCoreBasket::MethdAerialPassIdx(long nChr)
{
    CChrCore *pTo = CChrMan::GetChrCore(nChr);
    // (not used)
    GetRotY_ToChr(pTo);
    return MethdAerialPass_(pTo, 0, GetRotY(), 0, 0, 0);
}

// A pass in the air; from a shot or a dunk the character hangs on for a
// short jump.
int msw::CChrCoreBasket::MethdAerialPass_(CChrCore *pTo, CNpcBase *pNpc, float fRotY, long bTarget,
                                           const bcs::FVEC3 *pPos, int nUnk)
{
    if (!CanbeAerialPass(pTo)) {
        return 0;
    }
    int bBattle = 0;
    bcs::FVEC3 vel(GetJumpVel());
    long nState = GetState();
    if (nState == STATE_JUMP_SHOOT || nState == STATE_DUNK || nState == STATE_REBOUND_DUNK ||
        nState == STATE_ALLEYOOP) {
        CGame::SendMessage(0x32, 0, m_nPlayerNo, 0);
    }
    switch (GetState()) {
        case STATE_JUMP_SHOOT:
        case STATE_AERIAL_SHOOT:
        case STATE_PASS_CATCH:
        case STATE_LAY_UP:
            Util_SetJumpParam(*static_cast<const CC_DATA::CHR_PARAM::JUMP_PARAM *>(GetCcParam_()), GetPosY());
            m_fJumpAccelY = -0.025f;
            m_fJumpUnk224 += 10.0f;
            m_fJumpVelY = 0.0f;
            break;
        case STATE_AIR_TRICK:
        case STATE_DUNK:
            AnimStop(3);
            Util_NodePatchWarpToTransNode();
            m_nStatusFlag = (m_nStatusFlag & ~0x800) | 0x400;
            AnimPlay(3, 8, 1, 0.0f, 0.0f);
            Util_SetJumpParam(*static_cast<const CC_DATA::CHR_PARAM::JUMP_PARAM *>(GetCcParam_()), GetPosY());
            bBattle = 1;
            m_fJumpAccelY = -0.025f;
            m_fJumpUnk224 += 10.0f;
            m_fJumpVelY = 0.0f;
            break;
    }
    m_nBasketFlag &= ~0x8000;
    WORK_PASS *pWork = static_cast<WORK_PASS *>(FsmqSetState(STATE_AERIAL_PASS));
    pWork->m_nStep = 0;
    pWork->m_pTo = pTo;
    pWork->m_pGhost = pNpc;
    pWork->m_nUnk04 = bTarget;
    pWork->m_nPassType = 0;
    pWork->m_fRotY = fRotY;
    pWork->m_fDirRotY = fRotY;
    pWork->m_bBattle = bBattle;
    pWork->m_nUnk2C = nUnk;
    if (pPos != 0) {
        pWork->m_Target = *pPos;
    }
    m_fJumpVelX = vel.x;
    m_fJumpVelZ = vel.z;
    return 1;
}

// The catch of a pass ends when the play stops, the ball is caught or the
// team no longer attacks.
int msw::CChrCoreBasket::CheckPassCatchEnd_()
{
    if (CGame::g_GameWork.m_nFlag18 & 1) {
        return 1;
    }
    if (m_nStatusFlag & 1) {
        return 1;
    }
    return CGame::g_GameWork.m_nUserTeam != m_nTeamNo;
}

int msw::CChrCoreBasket::CanbePassCatchState()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    long nState = GetState();
    if ((nState < STATE_DEFAULT || nState > STATE_NEUTRAL) && nState != STATE_RUN && nState != STATE_ALLEYOOP &&
        nState != STATE_DUNK) {
        return 0;
    }
    return 1;
}

namespace {

    // STATE_PASS_CATCH (CChrCoreBasket::STATE_WORK_PASS_CATCH in
    // game/basket/unit_2E514.cpp)
    struct WORK_PASS_CATCH_START {
        long m_nStep;            // 0x00
        long m_nPassChr;         // 0x04: the passer, -1 for none
        // 0x08: bit 2 the character was running
        unsigned long m_nFlag;
        unsigned long m_hBall;   // 0x0C
        bcs::FVEC3 m_Pos;        // 0x10: where the ball will be caught
        float m_fJumpWait;       // 0x1C: frames before the jump for a high pass
        float m_fUnk20;          // 0x20
        float m_fWait;           // 0x24
        long m_nLoopCount;       // 0x28
        unsigned char m_nType;   // 0x2C
        unsigned char m_bNoJump; // 0x2D
        unsigned char m_bRun;    // 0x2E
        unsigned char m_bJump;   // 0x2F
    };
}

// The character waits for pBall at rPos (nType 1; else where it stands).
int msw::CChrCoreBasket::MethdSetPassCatchState(CBall *pBall, int bNoJump, const bcs::FVEC3 &rPos, long nType)
{
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000) || m_nComLevel == 4) {
        return 0;
    }
    int bRun = 0;
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_DEFAULT:
            m_fJumpVelX = 0.0f;
            m_fJumpVelY = 0.0f;
            m_fJumpVelZ = 0.0f;
            break;
        case STATE_RUN:
            bRun = 1;
            break;
        default:
            return 0;
    }
    WORK_PASS_CATCH_START *pWork = static_cast<WORK_PASS_CATCH_START *>(FsmqSetState(STATE_PASS_CATCH));
    pWork->m_nStep = 0;
    pWork->m_hBall = pBall->GetId();
    pWork->m_fJumpWait = 9999.9f;
    pWork->m_bNoJump = bNoJump;
    pWork->m_bJump = 0;
    pWork->m_nType = nType;
    pWork->m_nPassChr = -1;
    CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(0);
    if (pHold != 0) {
        pWork->m_nPassChr = pHold->GetPlayerNo();
    }
    if (nType == 1) {
        pWork->m_Pos = rPos;
    } else {
        GetCompPos(pWork->m_Pos);
    }
    pWork->m_nFlag = 0;
    if (bRun == 1) {
        pWork->m_nFlag |= 4;
    }
    return 1;
}

// A high pass: the catch jumps after fFrame frames, less the frames of the
// jump itself, to catch at rPos.
int msw::CChrCoreBasket::MethdSetPassCatchJumpWaitFrame(const bcs::FVEC3 &rPos, float fFrame)
{
    if (GetState() == STATE_PASS_CATCH) {
        WORK_PASS_CATCH_START *pWork = FsmqGetWork<WORK_PASS_CATCH_START>();
        pWork->m_Pos = rPos;
        if (fFrame < 0.0f) {
            fFrame = 0.0f;
        }
        pWork->m_fJumpWait = fFrame;
        pWork->m_fJumpWait -= GetCcParamHead(this)->m_fJumpFrame;
        pWork->m_bJump = 1;
        return 1;
    }
    return 0;
}

int msw::CChrCoreBasket::CanbeAlleyoop()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (IsHoldItem()) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
        return 0;
    }
    if (!CanbeDunkJump()) {
        return 0;
    }
    m_nUnk17B = 1;
    if (GetState() != STATE_RUN) {
        return 0;
    }
    return DunkRotCheck_() != 0;
}

int msw::CChrCoreBasket::MethdAlleyoop()
{
    if (!CanbeAlleyoop()) {
        return 0;
    }
    FsmqSetState(STATE_ALLEYOOP);
    return 1;
}

int msw::CChrCoreBasket::CanbeItemShoot()
{
    if (IsDisable()) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (m_nComLevel == 4) {
        return 0;
    }
    if (!IsHaveStockItem()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_RUN:
        case STATE_JUMP:
        case STATE_NEUTRAL:
        case STATE_JUMP_BLOCK:
        case STATE_ITEM_SHOOT:
            break;
        default:
            return 0;
    }
    return 1;
}

// The score of a target of the item shot (contents not known; as defined
// in game/basket/unit_25D70.cpp).
struct msw::CChrCoreBasket::SCORE_PARAM {
    long m_anUnk00[3];
    long m_anUnk0C[3];

    SCORE_PARAM()
    {
        for (int i = 0; i < 3; i++) {
            m_anUnk00[i] = 0;
            m_anUnk0C[i] = 0;
        }
    }
};

namespace {

    // STATE_ITEM_SHOOT
    struct WORK_ITEM_SHOOT {
        long m_nStep;          // 0x00: 1 thrown in a jump
        float m_fRotY;         // 0x04: the direction to throw in
    };

    // (as in game/basket/unit_25D70.cpp)
    inline float Clamp(float x, float fMin, float fMax)
    {
        return x > fMax ? fMax : (x < fMin ? fMin : x);
    }
}

// The item is thrown at the target the item shot picks, ahead of where it
// will be; against a target in the air the thrower jumps.
int msw::CChrCoreBasket::MethdItemShoot(float fRotY)
{
    static const float fMaxDist = 5.0f;

    if (!CanbeItemShoot()) {
        return 0;
    }
    CHR_CORE_BASKET::JUMP_BLOCK_PARAM jump;
    GetJumpBlockParam_(1, jump);
    if (GetState() == STATE_ITEM_SHOOT) {
        WORK_ITEM_SHOOT *pWork = FsmqGetWork<WORK_ITEM_SHOOT>();
        if (pWork->m_nStep == 0) {
            pWork->m_fRotY = fRotY;
        }
    } else {
        WORK_ITEM_SHOOT *pWork = static_cast<WORK_ITEM_SHOOT *>(FsmqSetState(STATE_ITEM_SHOOT));
        SCORE_PARAM param;
        CChrCoreBasket *pTarget = GetTargetItemShootCore(param);
        int bJump = 0;
        if (pTarget != 0) {
            int bAir = 0;
            if (GetState() != STATE_JUMP_BLOCK) {
                switch (pTarget->GetState()) {
                    case STATE_DUNK:
                    case STATE_REBOUND_DUNK:
                    case STATE_ALLEYOOP:
                        bAir = 1;
                        break;
                    case STATE_ITEM_SHOOT:
                    case STATE_JUMP_BLOCK:
                    case STATE_JUMP_SHOOT:
                        bAir = (pTarget->GetStatusFlag() & 0x400) != 0;
                        break;
                }
            }
            float fFrame = AnimSearchTrgFrame(GetMotID_ItemShoot(GetAttachItemMotType(), bAir), 2, -1.0f) - 5.0f;
            if (fFrame < 0.0f) {
                fFrame = 6.0f;
            }
            bcs::FVEC3 diff;
            SubVec(diff, GetPos(), pTarget->GetPos());
            float fRate = Clamp(bcs::BCS_Vec_GetLen(diff) / fMaxDist, 0.0f, 1.0f);
            float fAhead = 18.0f * fRate;
            bcs::FVEC3 pos;
            pTarget->GetFuturePos(pos, fFrame + fAhead);
            float fTargetRotY = GetRotY_ToPosXZ(pos);
            if (bAir) {
                float fHeight = GetCcParamHead(this)->m_fJumpHeight;
                Util_SetJumpParam(jump.m_nUnk04, jump.m_nUnk00, 2.5f + GetCcParamHead(this)->m_fHeight + fHeight);
                m_fJumpVelX = 0.0f;
                m_fJumpVelZ = 0.0f;
                pWork->m_fRotY = bcs::ROUND_PI(fTargetRotY);
                bJump = 1;
                pWork->m_nStep = 1;
            }
        }
        if (!bJump) {
            pWork->m_fRotY = GetRotY();
            pWork->m_nStep = 0;
        }
    }
    return 1;
}

int msw::CChrCoreBasket::MethdDamage(unsigned long nFlag, float fRotY)
{
    return MethdDamage(nFlag, fRotY, -1.0f, 60.0f, -1, 0.0f, 0, 0);
}

namespace {

    // STATE_DAMAGE (CChrCore::STATE_WORK_DAMAGE)
    struct WORK_DAMAGE {
        long m_nStep;            // 0x00
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

    // STATE_MAME_CHANGE
    struct WORK_MAME_CHANGE {
        long m_nStep;            // 0x00: 2 while the bean moves about
        unsigned char m_Unk04[0x08 - 0x04];
        float m_fStartRotY;      // 0x08
        float m_fRotY;           // 0x0C
        unsigned char m_Unk10[0x14 - 0x10];
        long m_nUnk14;           // 0x14
    };

    // STATE_MOT_PLAY (CChrCoreBasket's, in game/basket/unit_25D70.cpp)
    struct WORK_MOT_PLAY {
        long m_nStep;            // 0x00
        unsigned long m_nFlag;   // 0x04
        long m_nMot;             // 0x08: the motion to play
        long m_nNextMot;         // 0x0C: the motion after it
        long m_nVoice;           // 0x10: -1 for none
        long m_nSe;              // 0x14: -1 for none
    };

    // STATE_PARALYZE
    struct WORK_PARALYZE {
        long m_nStep;            // 0x00
        unsigned long m_nFlag;   // 0x04
        float m_fUnk08;          // 0x08
        float m_fTime;           // 0x0C
    };
}

// nFlag: the kind of damage in the low byte (0xD paralyses, 0xE puts the
// paralysis state's own handling to work), the type in the next (7 turns
// the character into a bean, 0xB stuns it too); 0x10000 drops coins,
// 0x40000 shrinks the character for fSizeTime frames.
int msw::CChrCoreBasket::MethdDamage(unsigned long nFlag, float fRotY, float fDamage, float fSizeTime, long nQuake,
                                     float fUnk, long nUnk1, long nUnk2)
{
    static const float fScale = 0.25f;

    unsigned long nType = (nFlag >> 8) & 0xFF;
    switch (nType) {
        case 7: {
            float fBeanRotY = GetRotY();
            if (GetState() == STATE_RUN) {
                fBeanRotY = FsmqGetWork<WORK_RUN>()->m_fRotYTrg;
            }
            switch (GetState()) {
                case STATE_MAME_CHANGE:
                    FsmqGetWork<WORK_MAME_CHANGE>()->m_nUnk14 = 1;
                    return 0;
                case STATE_DEFAULT:
                case STATE_JUMP:
                case STATE_JUMP_BLOCK:
                    return 0;
                default:
                    if (!(m_nStatusFlag & 0x1400)) {
                        WORK_MAME_CHANGE *pWork = static_cast<WORK_MAME_CHANGE *>(FsmqSetState(STATE_MAME_CHANGE));
                        pWork->m_fStartRotY = fBeanRotY;
                        pWork->m_fRotY = fBeanRotY;
                        pWork->m_nUnk14 = 1;
                        return 1;
                    }
                    return 0;
            }
        }
        case 4:
            if (GetState() == STATE_DAMAGE) {
                if (((FsmqGetWork<WORK_DAMAGE>()->m_nFlag >> 8) & 0xFF) == 4) {
                    return 0;
                }
            }
            if (AnimGetMotID(3, 0) == 0x22) {
                return 0;
            }
        default:
            break;
    }

    if (IsHoldBall()) {
        switch (GetState()) {
            case STATE_DUNK:
                AddCountWorkShootNum(3);
                break;
            case STATE_LAY_UP:
                AddCountWorkShootNum(6);
                break;
        }
    }
    float fInvincible = 0.0f;
    int bAir = 0;
    if (fSizeTime >= fInvincible) {
        fInvincible = fSizeTime;
    }
    switch (nFlag & 0xFF) {
        case 0xD: {
            long nMot = AnimGetMotID(3, 0);
            if (nMot < 0x79 || nMot > 0x7A) {
                Util_NodePatchWarpToTransNode();
            }
            break;
        }
        case 0xE:
            if (AnimGetMotID(3, 0) == 0x24) {
                break;
            }
        default:
            Util_NodePatchWarpToTransNode();
            break;
    }
    // The motions that face backwards turn the character round.
    long nMot = AnimGetMotID(3, 0);
    if ((nMot >= 0x53 && nMot <= 0x54) || (nMot >= 0x57 && nMot <= 0x58) || (nMot >= 0x5A && nMot <= 0x5B) ||
        nMot == 0x5E) {
        float fBackRotY = bcs::ROUND_PI(3.1415927f + GetRotY());
        Util_NodePatchRotY(fBackRotY);
        SetRotY(fBackRotY);
    }
    Util_DunkEnd(1);
    m_nStatusFlag &= ~0x800;
    if (m_nStatusFlag & 0x1400) {
        bAir = 1;
    }
    if (IsHoldBall()) {
        CBall *pBall = GetAttachBall();
        DetachBall();
        CGame::SetLooseBallMode(1);
        float fReflectRotY = 3.1415927f + fRotY;
        float fSpeed;
        float fHeight;
        if (nFlag == 0x20000 || nFlag == 0x60E) {
            fSpeed = 0.04f;
            fHeight = 0.13f;
        } else {
            fSpeed = 0.1f;
            fHeight = 0.05f;
        }
        if (pBall != 0) {
            static_cast<CBallBasket *>(pBall)->SetChrReflectMode(m_nPlayerNo, fReflectRotY, fSpeed, fHeight);
        }
        if (!(m_nBasketFlag & 0x10000)) {
            Eff_CreateSimple(0xC, pBall->GetPos(), 3, 0, 0);
        }
        AnimStop(3);
    }
    if (IsHoldItem()) {
        bcs::FVEC3 vel;
        bcs::BCS_VecZ_RotY(vel, 0.2f, GetRotY());
        vel.y = 0.1f;
        DropItem(vel, 0, 1);
    }
    if ((nFlag & 0x10000) && Util_DropCoin(fUnk, nUnk1, nUnk2)) {
        m_UnkEC |= 0x20;
    }
    if (nFlag & 0x40000) {
        float fTime = 600.0f;
        if (fSizeTime > 0.0f) {
            fTime = fSizeTime;
        }
        if (!MethdSizeChange(1, fTime) && (m_nStatusFlag & 0x10000)) {
            SetBodyScaleState(2, fTime);
        }
        if ((nFlag & 0xFF) != 0xD) {
            fSizeTime = 0.0f;
        }
    }
    if (nQuake >= 0) {
        CGame::StartCameraQuake(nQuake, m_nPlayerNo, -1);
    }
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0xA), nFlag, 0, 0, 0);

    switch (nFlag & 0xFF) {
        case 0xE:
            SubStateParalyzeStateSet_(nFlag, fSizeTime);
            m_fDamageSizeTime = fSizeTime;
            m_nDamageQuake = nQuake;
            m_fDamageUnk1F8 = fUnk;
            m_nDamageUnk1FC = nUnk1;
            m_nDamageUnk1FE = nUnk2;
            if (m_fJumpVelY > 0.0f) {
                m_fJumpVelY = 0.0f;
            }
            m_fJumpVelZ = 0.0f;
            m_fJumpVelX = 0.0f;
            return 1;
        case 0xD:
            if (m_nStatusFlag & 0x10000) {
                // shrunk: only the stun
                PlaySePos(0xC45, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                if (m_fSizeChangeTime < fSizeTime) {
                    SetBodyScaleState(2, fSizeTime);
                }
                if (nType == 0xB) {
                    PlayVoice(0x8A, 0);
                    SetCondition(0xA, fSizeTime, 0);
                }
            } else {
                // A motion playing (or queued next) plays again after the
                // paralysis.
                int bKeep = 0;
                WORK_MOT_PLAY *pMot = 0;
                unsigned long nMotFlag;
                long nMotId;
                long nNextMot;
                long nVoice;
                long nSe;
                if (GetState() == STATE_MOT_PLAY) {
                    pMot = FsmqGetWork<WORK_MOT_PLAY>();
                } else if (FsmqGetNextState() == STATE_MOT_PLAY) {
                    pMot = static_cast<WORK_MOT_PLAY *>(FsmqGetNextWork_());
                }
                if (pMot != 0) {
                    nMotFlag = pMot->m_nFlag;
                    bKeep = 1;
                    nMotId = pMot->m_nMot;
                    nNextMot = pMot->m_nNextMot;
                    nVoice = pMot->m_nVoice;
                    nSe = pMot->m_nSe;
                }
                WORK_PARALYZE *pWork = static_cast<WORK_PARALYZE *>(FsmqSetState(STATE_PARALYZE));
                pWork->m_nFlag = nFlag;
                pWork->m_fUnk08 = 0.45f;
                pWork->m_fTime = fSizeTime;
                if (bKeep) {
                    FsmqCutQueue();
                    if (m_nStatusFlag & 0x1400) {
                        WORK_JUMP *pJump = static_cast<WORK_JUMP *>(FsmqPushWork(STATE_JUMP));
                        pJump->m_nFlag = 0x20;
                        m_nStatusFlag = (m_nStatusFlag | 0x1400) & ~0x800;
                    }
                    WORK_MOT_PLAY *pNext = static_cast<WORK_MOT_PLAY *>(FsmqPushWork(STATE_MOT_PLAY));
                    pNext->m_nFlag = nMotFlag;
                    pNext->m_nMot = nMotId;
                    pNext->m_nNextMot = nNextMot;
                    pNext->m_nVoice = nVoice;
                    pNext->m_nSe = nSe;
                } else if (m_nStatusFlag & 0x2000) {
                    // knocked down after the paralysis
                    FsmqCutQueue();
                    WORK_DAMAGE *pDown = static_cast<WORK_DAMAGE *>(FsmqPushWork(STATE_DAMAGE));
                    pDown->m_nFlag = 0;
                    pDown->m_bAir = 1;
                    pDown->m_fRotY = GetRotY_ToGuardGoal();
                    pDown->m_fInvincible = 0.0f;
                    pDown->m_nStep = 0;
                    pDown->m_fDamage = 0.0f;
                    pDown->m_fSizeTime = 0.0f;
                    pDown->m_nQuake = -1;
                    pDown->m_fUnk2C = 0.0f;
                    pDown->m_nUnk30 = 0;
                    pDown->m_nUnk32 = 0;
                    m_fDamageSizeTime = 0.0f;
                    m_nDamageQuake = pDown->m_nQuake;
                    m_fDamageUnk1F8 = pDown->m_fUnk2C;
                    m_nDamageUnk1FC = pDown->m_nUnk30;
                    m_nDamageUnk1FE = pDown->m_nUnk32;
                }
            }
            return 1;
        default: {
            SetInvincible(30.0f);
            if ((nFlag & 0xFF) == 4) {
                // pushed along the way it was moving
                m_fJumpVelX = 0.0f;
                m_fJumpVelY = 0.0f;
                m_fJumpVelZ = 0.0f;
                float fMoveRotY = GetNowMoveDir();
                if (fMoveRotY >= 0.0f) {
                    bcs::BCS_VecZ_RotY(GetJumpVel(), 1.0f, fMoveRotY);
                    m_fJumpVelX *= fScale;
                    m_fJumpVelZ *= fScale;
                }
            }
            WORK_DAMAGE *pWork = static_cast<WORK_DAMAGE *>(FsmqSetState(STATE_DAMAGE));
            pWork->m_nFlag = nFlag;
            pWork->m_bAir = bAir;
            pWork->m_fRotY = fRotY;
            pWork->m_fInvincible = fInvincible;
            pWork->m_nStep = 0;
            pWork->m_fDamage = fDamage;
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
            return 1;
        }
    }
}

// Sliding is how a bean (STATE_MAME_CHANGE) moves.
int msw::CChrCoreBasket::CanbeSliding()
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

int msw::CChrCoreBasket::MethdSliding(float fRotY)
{
    if (!CanbeSliding()) {
        return 0;
    }
    if (GetState() == STATE_MAME_CHANGE) {
        WORK_MAME_CHANGE *pWork = FsmqGetWork<WORK_MAME_CHANGE>();
        if (pWork->m_nStep == 2) {
            pWork->m_fRotY = bcs::ROUND_PI(fRotY);
        }
        return 1;
    }
    return 0;
}

namespace {

    // STATE_SIZE_CHANGE
    struct WORK_SIZE_CHANGE {
        long m_nStep;            // 0x00
        long m_nType;            // 0x04: 1 shrinking, 0 back to normal
    };
}

// nType: 0 back to the normal size, 1 or 2 shrink.
int msw::CChrCoreBasket::CanbeSizeChange(long nType)
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
                case STATE_FREEZE:
                    return 0;
            }
            break;
        case 1:
        case 2:
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

int msw::CChrCoreBasket::MethdSizeChange(long nType, float fTime)
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
        case 2:
            m_nStatusFlag &= ~0x1F000000;
            SetMoveMotType(0);
            DropBall();
            PlaySePos(0xC45, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            SetBodyScaleState(2, 420.0f);
            m_fSizeChangeTime = fTime;
            m_fScaleFrom = m_fScaleBase;
            m_fScaleTo = 0.45f;
            m_fScaleRate = 1.0f;
            // The states in the air or with an action of their own go on;
            // the others stop and the character stands on the floor.
            switch (GetState()) {
                case STATE_JUMP:
                case STATE_ITEM_SHOOT:
                case STATE_DROP_CUT:
                case STATE_AERIAL_PASS:
                case STATE_PASS_CATCH:
                case STATE_JUMP_SHOOT:
                case STATE_REBOUND_DUNK:
                case STATE_LAY_UP:
                case STATE_ALLEYOOP:
                case STATE_DUNK:
                case STATE_JUMP_BLOCK:
                case STATE_SWAT_IT:
                case STATE_JUMP_CATCH:
                case STATE_FALL:
                    break;
                case STATE_RUN:
                    if (m_nStatusFlag & 3) {
                        m_nStatusFlag &= ~0xC00;
                        FsmqClear();
                        SetPosY(0.0f);
                    }
                    break;
                case STATE_AERIAL_SHOOT:
                default:
                    m_nStatusFlag &= ~0xC00;
                    FsmqClear();
                    SetPosY(0.0f);
                    break;
            }
            DropAttachItem();
            break;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeChgStarState()
{
    if (IsDisable()) {
        return 0;
    }
    return !(m_nStatusFlag & 0x8000);
}

int msw::CChrCoreBasket::MethdChgStarState()
{
    if (!CanbeChgStarState()) {
        return 0;
    }
    SetCondition(1, GetStarConditionFrame(), 0);
    return 1;
}

// The reaction to a turn over: the motion of the team that gets the ball
// (nTeam) or of the one that loses it, after the action now running.
int msw::CChrCoreBasket::MethdTurnOver(long nTeam)
{
    m_UnkEC = (m_UnkEC | 0xE0) & ~0x7F8000;
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SLIDING:
            // a bean in the air falls first
            if (GetPos().y > 0.0f) {
                FsmqCutQueue();
                WORK_JUMP *pJump = static_cast<WORK_JUMP *>(FsmqPushWork(STATE_JUMP));
                pJump->m_nFlag = 0x20;
            } else {
                FsmqClear();
            }
            break;
        default:
            FsmqCutQueue();
            break;
        case STATE_DEFAULT:
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_SDB:
        case STATE_SDB_CHAIN:
        case STATE_RDB:
        case STATE_RDB_CHAIN:
        case STATE_QUICK_TURN:
        case STATE_CATCH:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_TURN:
            FsmqClear();
            break;
        case STATE_PARALYZE:
            if (m_nStatusFlag & 0x1400) {
                FsmqClear();
                WORK_JUMP *pJump = static_cast<WORK_JUMP *>(FsmqSetState(STATE_JUMP));
                m_nStatusFlag = (m_nStatusFlag & ~0x800) | 0x1400;
                pJump->m_nFlag = 0x20;
            } else {
                FsmqCutQueue();
            }
            break;
        case STATE_JUMP:
        case STATE_MAME_CHANGE:
        case STATE_PASS_CATCH:
        case STATE_SDB_CATCH:
        case STATE_REBOUND_DUNK:
        case STATE_ALLEYOOP:
        case STATE_AIR_TRICK:
        case STATE_DUNK:
        case STATE_FALL:
            FsmqCutQueue();
            break;
        case STATE_MOT_PLAY:
            return 0;
    }
    if (m_nComLevel == 9 || m_nComLevel == 4) {
        SetCondition(0, 0.0f, 0);
    }
    WORK_MOT_PLAY *pWork = static_cast<WORK_MOT_PLAY *>(FsmqPushWork(STATE_MOT_PLAY));
    pWork->m_nFlag = 0;
    pWork->m_nFlag |= 2;
    if (nTeam == m_nTeamNo) {
        pWork->m_nMot = 0x79;
        pWork->m_nNextMot = 0x7A;
        pWork->m_nVoice = 0x28;
        pWork->m_nSe = -1;
    } else {
        pWork->m_nMot = 0x7B;
        pWork->m_nNextMot = 0x7C;
        pWork->m_nVoice = 0x29;
        pWork->m_nSe = -1;
    }
    return 1;
}

// The time is up: the same as a turn over.
int msw::CChrCoreBasket::MethdTimeOver(long nTeam)
{
    m_UnkEC = (m_UnkEC | 0xE0) & ~0x7F8000;
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SLIDING:
            // a bean in the air falls first
            if (GetPos().y > 0.0f) {
                FsmqCutQueue();
                WORK_JUMP *pJump = static_cast<WORK_JUMP *>(FsmqPushWork(STATE_JUMP));
                pJump->m_nFlag = 0x20;
            } else {
                FsmqClear();
            }
            break;
        default:
            FsmqCutQueue();
            break;
        case STATE_DEFAULT:
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_SDB:
        case STATE_SDB_CHAIN:
        case STATE_RDB:
        case STATE_RDB_CHAIN:
        case STATE_QUICK_TURN:
        case STATE_CATCH:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_TURN:
            FsmqClear();
            break;
        case STATE_PARALYZE:
            if (m_nStatusFlag & 0x1400) {
                FsmqClear();
                WORK_JUMP *pJump = static_cast<WORK_JUMP *>(FsmqSetState(STATE_JUMP));
                m_nStatusFlag = (m_nStatusFlag & ~0x800) | 0x1400;
                pJump->m_nFlag = 0x20;
            } else {
                FsmqCutQueue();
            }
            break;
        case STATE_JUMP:
        case STATE_MAME_CHANGE:
        case STATE_PASS_CATCH:
        case STATE_SDB_CATCH:
        case STATE_REBOUND_DUNK:
        case STATE_ALLEYOOP:
        case STATE_AIR_TRICK:
        case STATE_DUNK:
        case STATE_FALL:
            FsmqCutQueue();
            break;
        case STATE_MOT_PLAY:
            return 0;
    }
    if (m_nComLevel == 9 || m_nComLevel == 4) {
        SetCondition(0, 0.0f, 0);
    }
    WORK_MOT_PLAY *pWork = static_cast<WORK_MOT_PLAY *>(FsmqPushWork(STATE_MOT_PLAY));
    pWork->m_nFlag = 0;
    pWork->m_nFlag |= 2;
    if (nTeam == m_nTeamNo) {
        pWork->m_nMot = 0x79;
        pWork->m_nNextMot = 0x7A;
        pWork->m_nVoice = 0x28;
        pWork->m_nSe = -1;
    } else {
        pWork->m_nMot = 0x7B;
        pWork->m_nNextMot = 0x7C;
        pWork->m_nVoice = 0x29;
        pWork->m_nSe = -1;
    }
    return 1;
}

// The end of the game: everything stops and the character stands.
int msw::CChrCoreBasket::MethdGameSet()
{
    m_UnkEC = (m_UnkEC | 0xE0) & ~0x7F8000;
    long nState = GetState();
    if (nState >= STATE_FALL && nState <= STATE_REBIRTH) {
        return 0;
    }
    ResetBodyColor_();
    ResetBodyScale_();
    if (m_nStatusFlag & 0x100) {
        m_nStatusFlag = 0x100;
    } else {
        m_nStatusFlag = 0;
    }
    if (m_pChrData->m_fUnk20 > 0.0f) {
        m_nStatusFlag |= 0x200;
    }
    m_pPassCheckChr = 0;
    SetBodyScaleState(0, 420.0f);
    SetSpecialState(0);
    SetInvincible(0.0f);
    SetNoPickupCount(0.0f);
    m_fUnk284 = 0.0f;
    WORK_MOT_PLAY *pWork = static_cast<WORK_MOT_PLAY *>(FsmqSetState(STATE_MOT_PLAY));
    pWork->m_nFlag = 0;
    pWork->m_nFlag |= 0x43;
    pWork->m_nMot = Util_GetIdleMotID();
    pWork->m_nNextMot = Util_GetIdleMotID();
    pWork->m_nVoice = -1;
    pWork->m_nSe = -1;
    return 1;
}

// The result: the motion of the winners (bWin) or of the losers.
int msw::CChrCoreBasket::MethdGameResult(int bWin)
{
    m_UnkEC = (m_UnkEC | 0xE0) & ~0x7F8000;
    long nState = GetState();
    if (nState >= STATE_FALL && nState <= STATE_REBIRTH) {
        return 0;
    }
    ResetBodyColor_();
    ResetBodyScale_();
    if (m_nStatusFlag & 0x100) {
        m_nStatusFlag = 0x100;
    } else {
        m_nStatusFlag = 0;
    }
    if (m_pChrData->m_fUnk20 > 0.0f) {
        m_nStatusFlag |= 0x200;
    }
    m_pPassCheckChr = 0;
    SetBodyScaleState(0, 420.0f);
    SetSpecialState(0);
    SetInvincible(0.0f);
    SetNoPickupCount(0.0f);
    m_fUnk284 = 0.0f;
    WORK_MOT_PLAY *pWork = static_cast<WORK_MOT_PLAY *>(FsmqSetState(STATE_MOT_PLAY));
    pWork->m_nFlag = 0;
    pWork->m_nFlag |= 0x42;
    if (bWin) {
        pWork->m_nMot = 0x79;
        pWork->m_nNextMot = 0x7A;
        pWork->m_nVoice = 0x28;
        pWork->m_nSe = -1;
    } else {
        pWork->m_nMot = 0x7B;
        pWork->m_nNextMot = 0x7C;
        pWork->m_nVoice = 0x29;
        pWork->m_nSe = -1;
    }
    return 1;
}

// The yaw from the goal the team defends (moved 2 towards the court) to
// the character.
float msw::CChrCoreBasket::CalcBWLookDir_() const
{
    bcs::FVEC3 goal = CStageMan::GetInstance()->GetGoalPos(GetGuardGoalIdx());
    if (goal.z < 0.0f) {
        goal.z -= 2.0f;
    } else {
        goal.z += 2.0f;
    }
    bcs::FVEC3 pos;
    GetCompPos(pos);
    pos.x -= goal.x;
    pos.z -= goal.z;
    return bcs::ROUND_PI(0.024543693f * nw4r::math::Atan2FIdx(pos.x, pos.z));
}

// A dunk is only started facing the goal: within 60 degrees of it, and
// not from behind the board.
int msw::CChrCoreBasket::DunkRotCheck_()
{
    bcs::FVEC3 goal;
    GetTargetGoalPos(goal);
    float fDiff;
    if (GetState() == STATE_AUTO_DRIBBLE_POSTURE) {
        WORK_AUTO_DPM *pWork = FsmqGetWork<WORK_AUTO_DPM>();
        if (pWork->m_fRotY <= -1.0f) {
            return 0;
        }
        fDiff = bcs::ROUND_PI(GetRotY_ToPosXZ(goal) - pWork->m_fRotY);
    } else {
        fDiff = GetRotY_Diff_ToPosXZ(goal);
    }
    if (fDiff > 3.1415927f) {
        fDiff = __fabsf(fDiff - 6.2831855f);
    }
    if (__fabsf(fDiff) > 1.0471976f) {
        return 0;
    }
    bcs::FVEC3 goalDir;
    bcs::BCS_VecOneZ_RotY(goalDir, GetTargetGoalYRot());
    bcs::FVEC3 fromGoal;
    bcs::BCS_VecOneZ_RotY(fromGoal, GetRotY_FromGoal());
    return !(goalDir.x * fromGoal.x + goalDir.z * fromGoal.z < -0.61086524f);
}

int msw::CChrCoreBasket::Canbe1on1()
{
    if (CGame::g_GameWork.GetGameType() == 2) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    if (m_nStatusFlag & 0x20000) {
        return 0;
    }
    switch (GetState()) {
        case STATE_PASS:
        case STATE_SDB:
        case STATE_RDB:
        case STATE_TRICK_MOVE:
        case STATE_CATCH:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_DASH:
        case STATE_DRIBBLE_MISS:
            return 1;
        default:
            return 0;
    }
}

// The ball holder faces a defender one on one (bBlock: the defender
// blocks); a dribble turns into the dribble posture.
int msw::CChrCoreBasket::Methd1on1(int bBlock)
{
    if (!Canbe1on1()) {
        return 0;
    }
    switch (GetState()) {
        case STATE_CATCH: {
            unsigned long *pStep = FsmqGetWork<unsigned long>();
            if (*pStep <= 1) {
                *pStep = 2;
            }
        }
        case STATE_TRICK_MOVE:
        case STATE_DASH:
        case STATE_DRIBBLE_MISS:
            break;
        case STATE_SDB: {
            WORK_AUTO_DPM *pWork = static_cast<WORK_AUTO_DPM *>(FsmqSetState(STATE_AUTO_DRIBBLE_POSTURE));
            pWork->m_nStep = 0;
            pWork->m_nFlag = 0;
            break;
        }
        case STATE_RDB: {
            // (not used)
            AnimGetFrame(3);
            AnimGetFrameNum(3);
            WORK_RDB *pRdb = FsmqGetWork<WORK_RDB>();
            if ((pRdb->m_nFlag & 1) != 1) {
                float fRotY = pRdb->m_fRotY;
                long nDir = GetDir4ToBattle(bcs::ROUND_PI(fRotY - GetRotY_ToGoal()));
                WORK_AUTO_DPM *pWork = static_cast<WORK_AUTO_DPM *>(FsmqSetState(STATE_AUTO_DRIBBLE_POSTURE));
                pWork->m_nStep = 6;
                pWork->m_fRotY = fRotY;
                pWork->m_Pos = GetPos();
                pWork->m_nDir = nDir;
                pWork->m_nFlag = 0;
                pWork->m_nFlag |= 4;
            }
            break;
        }
        case STATE_AUTO_DRIBBLE_POSTURE: {
            WORK_AUTO_DPM *pWork = FsmqGetWork<WORK_AUTO_DPM>();
            if (bBlock == 1) {
                pWork->m_nFlag |= 0x10;
            } else {
                pWork->m_nFlag &= ~0x10;
            }
            break;
        }
        case STATE_PASS:
            // (not used)
            FsmqGetWork<WORK_PASS>();
            break;
        default:
            return 0;
    }
    m_nBasketFlag |= 2;
    return 1;
}

int msw::CChrCoreBasket::CanbeAutoDPM()
{
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    long nState = GetState();
    if (nState == STATE_SDB || nState == STATE_RDB || nState == STATE_AUTO_DRIBBLE_POSTURE) {
        return 1;
    }
    return 0;
}

// The dribble posture turns to fRotY.
int msw::CChrCoreBasket::MethdAutoDPM(float fRotY, float fRate)
{
    if (!CanbeAutoDPM()) {
        return 0;
    }
    if (GetState() == STATE_AUTO_DRIBBLE_POSTURE) {
        WORK_AUTO_DPM *pWork = FsmqGetWork<WORK_AUTO_DPM>();
        pWork->m_fRotY = fRotY;
        pWork->m_fUnk0C = fRate;
        pWork->m_fUnk10 = 3.0f;
        pWork->m_fUnk14 += GetWholeFrameRate();
        pWork->m_fUnk14 = pWork->m_fUnk14 > 3.0f ? 3.0f : (pWork->m_fUnk14 < 0.0f ? 0.0f : pWork->m_fUnk14);
        pWork->m_nFlag |= 1;
    } else {
        return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeBlockWorkLose()
{
    return 0;
}

int msw::CChrCoreBasket::MethdBlockWorkLose()
{
    return CanbeBlockWorkLose() != 0;
}

int msw::CChrCoreBasket::CanbeJumpUpper() const
{
    if (IsDisable()) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (m_nComLevel == 4) {
        return 0;
    }
    if (!(m_nStatusFlag & 0x400)) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 8) {
        return 0;
    }
    if (GetState() != STATE_JUMP_BLOCK) {
        return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeNowJumpUpper() const
{
    if (IsDisable()) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    if ((m_nStatusFlag & 0x8000) || (m_nStatusFlag & 0x10000)) {
        return 0;
    }
    if (!(m_nStatusFlag & 0x400)) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 8) {
        return 0;
    }
    if (m_nComLevel == 4) {
        return 0;
    }
    if (GetState() == STATE_JUMP_BLOCK) {
        return !(FsmqGetWork<WORK_JUMP_BLOCK>()->m_fFrameAll < 10.0f);
    }
    return 0;
}

// From a jump block: the jump to the upper floor; asked for too early it
// is kept for later.
int msw::CChrCoreBasket::MethdJumpUpper()
{
    if (!CanbeNowJumpUpper()) {
        if (GetState() == STATE_JUMP_BLOCK) {
            FsmqGetWork<WORK_JUMP_BLOCK>()->m_nFlag |= 0x400000;
        }
        return 0;
    }
    if (GetState() == STATE_JUMP_BLOCK) {
        FsmqSetState(STATE_DROP_CUT);
    } else {
        return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeSideStep(float, float)
{
    if (IsDisable()) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    return IsNowBlockWork() != 0;
}

namespace {

    // STATE_SIDE_STEP
    struct WORK_SIDE_STEP {
        long m_nStep;            // 0x00
        unsigned char m_Unk04[0x08 - 0x04];
        unsigned long m_nFlag;   // 0x08
        unsigned char m_Unk0C[0x10 - 0x0C];
        float m_fRotY;           // 0x10: the stick's direction
        float m_fUnk14;          // 0x14
        float m_fUnk18;          // 0x18
        float m_fUnk1C;          // 0x1C
        float m_fUnk20;          // 0x20
    };
}

int msw::CChrCoreBasket::MethdSideStep(float fStickRotY, float fRotY)
{
    if (!CanbeSideStep(fStickRotY, fRotY)) {
        return 0;
    }
    if (GetState() == STATE_SIDE_STEP) {
        return 0;
    }
    WORK_SIDE_STEP *pWork = static_cast<WORK_SIDE_STEP *>(FsmqSetState(STATE_SIDE_STEP));
    pWork->m_nStep = 0;
    pWork->m_nFlag = 0;
    pWork->m_fRotY = fStickRotY;
    pWork->m_fUnk14 = 0.0f;
    pWork->m_fUnk18 = 0.0f;
    pWork->m_fUnk1C = 0.0f;
    pWork->m_fUnk20 = 0.0f;
    return 1;
}

int msw::CChrCoreBasket::CanbeJumpBlock()
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
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }
    if (IsHoldItem()) {
        return 0;
    }
    // At the jump ball only the team's jumper jumps.
    if ((CGame::g_GameWork.m_nFlag18 & 8) &&
        CGameBasket::GetInstance()->GetNowJumpBallerChrIdx_(m_nTeamNo) != m_nPlayerNo) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_BLOCK_WORK:
        case STATE_RUN:
        case STATE_SIDE_STEP:
        case STATE_MAME_CHANGE:
            return 1;
        case STATE_BLOCK_CUT:
            return 1;
        default:
            return 0;
    }
}

namespace {

    // (inline in the original) pBall is a ball the jump block may go for:
    // a free ball that may be caught, a shot of nobody's, or a ball of the
    // other team that is shot or carried up in a dunk or jump shot.
    inline int IsJumpBlockBall(msw::CChrCoreBasket *pChr, msw::CBall *pBall)
    {
        msw::CChrCore *pOwner = 0;
        if (!pBall->IsShootMode()) {
            if (!msw::CBallManBasket::GetInstance()->CatchEnable_(pBall)) {
                pOwner = msw::CChrMan::Util_SearchHoldBallChr(pBall->GetBallNo());
                if (pOwner == 0) {
                    return 0;
                }
            }
        } else if (!pBall->IsNowRebound()) {
            long nChr = pBall->GetShotChrIdx();
            if (nChr >= 0 && nChr < msw::CChrMan::GetChrCoreNum()) {
                pOwner = msw::CChrMan::GetChrCore(nChr);
            }
        }
        if (pOwner == 0) {
            return 1;
        }
        if (pOwner->GetTeamNo() == pChr->GetTeamNo()) {
            return 0;
        }
        if (pBall->IsShootMode()) {
            return 1;
        }
        switch (pOwner->GetState()) {
            case msw::CChrCore::STATE_AIR_TRICK:
                return pOwner->IsAirTrickDunkMode() != 0;
            case msw::CChrCore::STATE_DUNK:
            case msw::CChrCore::STATE_REBOUND_DUNK:
            case msw::CChrCore::STATE_ALLEYOOP:
            case msw::CChrCore::STATE_JUMP_SHOOT:
                return 1;
        }
        return 0;
    }
}

// The jump block towards fRotY (negative: the way the character faces).
// It jumps at the ball in reach (within the parameter's distance and
// angle): nKind 1 at a shot, 2 at a dunk, 3 at a free ball, 4 at the
// ninja's special shot (him or one of his copies).
int msw::CChrCoreBasket::MethdJumpBlock(float fRotY)
{
    static const long MAX = 3;

    if (!CanbeJumpBlock()) {
        return 0;
    }
    if (m_nComLevel == 4 && fRotY < 0.0f) {
        fRotY = GetRotY();
    }
    float fRange = GetJumpBlockRevR();
    float fRangeRot = bcs::DEG_TO_RAD(GetJumpBlockRevRangRad());
    if (GetState() == STATE_MAME_CHANGE) {
        fRotY = FsmqGetWork<WORK_MAME_CHANGE>()->m_fStartRotY;
    }
    // (not used)
    GetRotY();
    AdjustOffsetPos_(1);
    WORK_JUMP_BLOCK *pWork = static_cast<WORK_JUMP_BLOCK *>(FsmqSetState(STATE_JUMP_BLOCK));
    pWork->m_nStep = 0;
    pWork->m_nFlag = 8;
    long nKind = 0;
    CBall *pTarget = 0;
    pWork->m_fRotY = -1.0f;
    pWork->m_fFrame = 0.0f;
    pWork->m_hBall = 0;

    if ((CChrManBasket::GetInstance()->GetFlagAC() & 2) && CChrManBasket::GetInstance()->GetArtsChr() != 0 &&
        CChrManBasket::GetInstance()->GetArtsChr()->GetChrTypeID() == 0xE) {
        nKind = 4;
    } else {
        float fBaseRotY = -1.0f != fRotY ? fRotY : GetRotY();
        for (long i = 0; i < CBallMan::GetBallNum(); i++) {
            CBall *pBall = CBallMan::GetBallPtr(i);
            if (pBall == 0 || !pBall->IsActive() || pBall->IsFlag2()) {
                continue;
            }
            if (CGame::g_GameWork.GetGameType() == 2 && pBall->IsBaExplode()) {
                continue;
            }
            if (!IsJumpBlockBall(this, pBall)) {
                continue;
            }
            bcs::FVEC3 diff;
            SubVec(diff, pBall->GetPos(), GetPos());
            float fDist = bcs::BCS_Vec_GetLenXZ(diff);
            if (!(fDist > fRange)) {
                float fDiff = bcs::ROUND_PI(GetRotY_ToPosXZ(pBall->GetPos()) - fBaseRotY);
                if (fDiff > 3.1415927f) {
                    fDiff -= 6.2831855f;
                }
                float fAbs = __fabsf(fDiff);
                if (!(fAbs > fRangeRot)) {
                    fRangeRot = fAbs;
                    pTarget = pBall;
                    fRange = fDist;
                }
            }
        }
    }

    if (pTarget != 0 && nKind != 4) {
        CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(pTarget->GetBallNo());
        if (pHold != 0) {
            if (pHold->GetTeamNo() != m_nTeamNo) {
                switch (pHold->GetState()) {
                    case STATE_AIR_TRICK:
                        if (pHold->IsAirTrickDunkMode()) {
                            nKind = 2;
                        }
                        break;
                    case STATE_DUNK:
                        nKind = 2;
                        break;
                    case STATE_JUMP_SHOOT:
                        nKind = 1;
                        break;
                }
            }
        } else if (pTarget->IsShootMode()) {
            nKind = 1;
        } else if (CBallManBasket::GetInstance()->CatchEnable_(pTarget)) {
            nKind = 3;
        }
    }

    CHR_CORE_BASKET::JUMP_BLOCK_PARAM jump;
    GetJumpBlockParam_(nKind, jump);
    Util_SetJumpParam(jump.m_nUnk04, jump.m_nUnk00, jump.m_fHeight);
    float fMaxDist = jump.m_fUnk0C * jump.m_nUnk04;
    float fDist = fMaxDist;
    switch (nKind) {
        case 0:
            pTarget = 0;
            break;
        case 1: {
            const CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(pTarget->GetBallNo());
            if (pHold != 0) {
                if (pHold->GetState() == STATE_JUMP_SHOOT) {
                    // on the line from the goal to the shooter, 85% of the
                    // way out
                    const bcs::FVEC3 &rGoal = pHold->GetTargetGoalPos();
                    bcs::FVEC3 aim;
                    SubVec(aim, pHold->GetPos(), rGoal);
                    float fLen = 0.85f * bcs::BCS_Vec_GetLenXZ(aim);
                    bcs::FVEC3 xz(aim.x, 0.0f, aim.z);
                    bcs::BCS_Vec_Normalize(aim, xz);
                    aim.x = aim.x * fLen + rGoal.x;
                    aim.z = aim.z * fLen + rGoal.z;
                    bcs::FVEC3 diff;
                    SubVec(diff, aim, GetPos());
                    fDist = bcs::BCS_Vec_GetLenXZ(diff) * 0.5f;
                    fRotY = GetRotY_ToPosXZ(aim);
                }
            } else {
                bcs::FVEC3 pos;
                pTarget->GetPosAfterFrame(pos, jump.m_nUnk04);
                bcs::FVEC3 diff;
                SubVec(diff, pos, GetPos());
                fDist = bcs::BCS_Vec_GetLenXZ(diff);
                fRotY = GetRotY_ToPosXZ(pos);
            }
            break;
        }
        case 2: {
            CChrCore *pHave = CChrMan::Util_SearchHaveBallChr(pTarget->GetBallNo());
            if (pHave != 0) {
                bcs::FVEC3 pos;
                static_cast<CChrCoreBasket *>(pHave)->GetFuturePos(pos, jump.m_nUnk04);
                bcs::FVEC3 diff;
                SubVec(diff, pos, GetPos());
                fDist = bcs::BCS_Vec_GetLenXZ(diff);
                fRotY = GetRotY_ToPosXZ(pos);
            }
            break;
        }
        case 3: {
            bcs::FVEC3 pos;
            pTarget->GetPosAfterFrame(pos, jump.m_nUnk04);
            bcs::FVEC3 diff;
            SubVec(diff, pos, GetPos());
            fDist = bcs::BCS_Vec_GetLenXZ(diff);
            fRotY = GetRotY_ToPosXZ(pos);
            break;
        }
        case 4:
            if (CChrManBasket::GetInstance()->GetArtsChr() != 0) {
                const CChrCoreBasketNinja *pNinja =
                    static_cast<const CChrCoreBasketNinja *>(CChrManBasket::GetInstance()->GetArtsChr());
                if (pNinja != 0) {
                    // the ninja or the copy nearest to the direction
                    const bcs::FVEC3 *apPos[MAX];
                    apPos[0] = &pNinja->GetPos();
                    apPos[1] = pNinja->GetClonePos(0);
                    apPos[2] = pNinja->GetClonePos(1);
                    float fBaseRotY = fRotY;
                    if (fRotY < 0.0f) {
                        fBaseRotY = GetRotY();
                    }
                    float fMinRot = 9999.9f;
                    long nSel = -1;
                    for (long i = 0; i < MAX; i++) {
                        if (apPos[i] != 0) {
                            bcs::FVEC3 pos = *apPos[i];
                            bcs::FVEC3 diff;
                            SubVec(diff, pos, GetPos());
                            if (!(bcs::BCS_Vec_GetLenXZ(diff) > GetJumpBlockRevR())) {
                                float fDiff = bcs::ROUND_PI(GetRotY_ToPosXZ(pos) - fBaseRotY);
                                if (fDiff > 3.1415927f) {
                                    fDiff -= 6.2831855f;
                                }
                                if (!(__fabsf(fDiff) > __fabsf(fMinRot))) {
                                    fMinRot = fDiff;
                                    nSel = i;
                                }
                            }
                        }
                    }
                    if (nSel != -1) {
                        const bcs::FVEC3 *pPos = apPos[nSel];
                        bcs::FVEC3 diff;
                        SubVec(diff, *pPos, GetPos());
                        fDist = bcs::BCS_Vec_GetLenXZ(diff);
                        fRotY = GetRotY_ToPosXZ(*pPos);
                    }
                }
            }
            break;
    }

    if (fRotY >= 0.0f) {
        if (fDist > fMaxDist) {
            fDist = fMaxDist;
        }
        float fSpeed = fDist / jump.m_nUnk04;
        bcs::FVEC3 dir;
        bcs::BCS_VecZ_RotY(dir, 1.0f, fRotY);
        m_fJumpVelX = dir.x * fSpeed;
        m_fJumpVelZ = dir.z * fSpeed;
        if (pTarget != 0) {
            pWork->m_hBall = pTarget->GetId();
        }
    } else {
        m_fJumpVelX = 0.0f;
        m_fJumpVelZ = 0.0f;
    }
    pWork->m_fRotYAsked = fRotY;
    pWork->m_fRotY = fRotY;
    return 1;
}

int msw::CChrCoreBasket::CanbeSwatIt() const
{
    if (IsDisable()) {
        return 0;
    }
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }
    if (m_nStatusFlag & 0x10000) {
        return 0;
    }
    if (m_nComLevel == 4) {
        return 0;
    }
    // not while attacking
    if (CGame::g_GameWork.GetUserTeam() == m_nTeamNo) {
        return 0;
    }
    if (GetState() == STATE_JUMP_BLOCK) {
        if (FsmqGetWork<WORK_JUMP_BLOCK>()->m_fFrameAll < 10.0f) {
            return 0;
        }
        if ((m_nStatusFlag & 0x1000) && GetPos().y < 0.5f) {
            return 0;
        }
        return 1;
    }
    return 0;
}

namespace {

    // STATE_SWAT_IT (CChrCoreBasket::STATE_WORK_SWAT_IT)
    struct WORK_SWAT_IT {
        long m_nStep;            // 0x00
        float m_fRotY;           // 0x04
        unsigned long m_nFlag;   // 0x08
    };
}

int msw::CChrCoreBasket::MethdSwatIt()
{
    if (!CanbeSwatIt()) {
        return 0;
    }
    WORK_SWAT_IT *pWork = static_cast<WORK_SWAT_IT *>(FsmqSetState(STATE_SWAT_IT));
    pWork->m_nStep = 0;
    pWork->m_nFlag = 0;
    return 1;
}

int msw::CChrCoreBasket::CanbeSmashCut()
{
    if (IsDisable()) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    if (!IsSmashCutEnable()) {
        return 0;
    }
    // not while attacking
    if (CGame::g_GameWork.GetUserTeam() == m_nTeamNo) {
        return 0;
    }
    long nState = GetState();
    if (nState != STATE_NEUTRAL && nState != STATE_RUN) {
        return 0;
    }
    return 1;
}

// The smash cut towards fRotY (a cut already running only turns).
int msw::CChrCoreBasket::MethdSmashCut(float fRotY)
{
    if (!CanbeSmashCut()) {
        return 0;
    }
    STATE_WORK_SMASH_CUT *pWork;
    if (GetState() == STATE_SMASH_CUT) {
        pWork = FsmqGetWork<STATE_WORK_SMASH_CUT>();
    } else {
        pWork = static_cast<STATE_WORK_SMASH_CUT *>(FsmqSetState(STATE_SMASH_CUT));
    }
    if (pWork == 0) {
        return 0;
    }
    pWork->m_fSteerRotY = fRotY;
    return 1;
}

// The computer's smash cut: at the ball holder.
int msw::CChrCoreBasket::MethdAutoSmashCut()
{
    bool bCan = CanbeSmashCut();
    if (!bCan) {
        return 0;
    }
    float fRotY = GetRotY();
    const CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(0);
    if (pHold != 0) {
        fRotY = GetRotY_ToPosXZ(pHold->GetPos());
    }
    return MethdSmashCut(fRotY);
}

int msw::CChrCoreBasket::CanbeSpecialBattle()
{
    if (IsDisable()) {
        return 0;
    }
    if (!(CChrManBasket::GetInstance()->GetFlagAC() & 2)) {
        return 0;
    }
    if (m_nComLevel == 1) {
        return 0;
    }
    if (GetState() == STATE_DROP_CUT) {
        if (FsmqGetWork<STATE_WORK_JUMP_UPPER>()->m_nFlag & 0x100000) {
            return 0;
        }
        return AnimGetMotID(3, 0) == 0x6B;
    }
    return 0;
}

// In the jump to the upper floor: the battle in the air with the special
// shot of the other team.
int msw::CChrCoreBasket::MethdSpecialBattle()
{
    if (!CanbeSpecialBattle()) {
        return 0;
    }
    if (GetState() == STATE_DROP_CUT) {
        STATE_WORK_JUMP_UPPER *pWork = FsmqGetWork<STATE_WORK_JUMP_UPPER>();
        if (pWork->m_nFlag & 0x100000) {
            return 0;
        }
        long nMot = AnimGetMotID(3, 0);
        if (nMot < 0x6A || nMot > 0x6B) {
            return 0;
        }
        pWork->m_nFlag |= 0x300000;
        CChrCoreBasket *pArts = CChrManBasket::GetInstance()->GetArtsChr();
        if (pArts->GetComLevelWork() == 1) {
            return 0;
        }
        if (pArts->IsHoldBall()) {
            pWork->m_fBattleFrame = pArts->GetStaddleLimitTime_();
        } else {
            pWork->m_fBattleFrame = 0.0f;
        }
        Eff_CreateAttach(0xD, 0x4D8, m_nEffNodeSpike, 0, 0, 3, 0);
        Eff_SetRotateY(0xD, GetRotY_FromGuardGoal());
        SetEndure(1);
        CGameBasket::GetInstance()->PlayExceptionalCameraSlow_(3);
        long nQuake = 0x12;
        if (pArts->GetChrTypeID() == 0) {
            nQuake = 0xB;
        }
        CGameBasket::GetInstance()->CameraQuake_(nQuake, m_nPlayerNo);
        SE_Play(7, 0xC09, 0, -1, 0.0f, 1.0f, 0.0f);
        CGameBasket::GetInstance()->SetCheckSpEndure_(1);
        SetAttach_(pArts->GetMdl()->GetHandle(), 0);
        pWork->m_nBattleStep = 0;
        pWork->m_nFlag &= ~0xC000;
    } else {
        return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeTrickMove()
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
    if (m_nStatusFlag & 0x8000) {
        return 0;
    }
    if (!(m_nStatusFlag & 1)) {
        return 0;
    }
    switch (GetState()) {
        case STATE_SDB:
        case STATE_RDB:
        case STATE_AUTO_DRIBBLE_POSTURE:
        case STATE_CATCH:
            if (!IsBattle() && m_fTrickMoveWait <= 0.0f) {
                return 0;
            }
            break;
        case STATE_DASH:
            if (!IsBattle() && m_fTrickMoveWait <= 0.0f) {
                if (!(FsmqGetWork<WORK_DASH>()->m_nFlag & 0x20)) {
                    return 0;
                }
            }
            break;
        case STATE_TURN:
            if (!IsBattle() && m_fTrickMoveWait <= 0.0f) {
                if (!(FsmqGetWork<WORK_TURN>()->m_nFlag & 0x10)) {
                    return 0;
                }
            }
            break;
        default:
            return 0;
    }
    return 1;
}

namespace {

    // STATE_TRICK_MOVE (the parts the start sets)
    struct WORK_TRICK_MOVE_START {
        long m_nStep;          // 0x00
        unsigned char m_Unk04[0x0C - 0x04];
        unsigned long m_nFlag; // 0x0C: bit 0 the spin goes the other way
        float m_fRotY;         // 0x10: the yaw the spin ends at
        unsigned char m_Unk14[0x18 - 0x14];
        float m_fFrameNum;     // 0x18: frames of the spin
        float m_fUnk1C;        // 0x1C
        float m_fRotStep;      // 0x20: the turn per frame
        float m_fUnk24;        // 0x24
        float m_fUnk28;        // 0x28
    };
}

// The spin move round the defender (fDir 1: the other way round).
int msw::CChrCoreBasket::MethdTrickMove(float fDir)
{
    if (!CanbeTrickMove()) {
        return 0;
    }
    WORK_TRICK_MOVE_START *pWork = static_cast<WORK_TRICK_MOVE_START *>(FsmqSetState(STATE_TRICK_MOVE));
    pWork->m_nFlag = 0;
    if (1.0f == fDir) {
        pWork->m_nFlag |= 1;
    }
    const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = GetCcParam2_();
    pWork->m_fUnk24 = pParam->m_fTrickMoveUnk54;
    pWork->m_fUnk28 = pParam->m_fTrickMoveUnk58;
    pWork->m_fRotY = bcs::ROUND_PI(GetRotY_ToPosXZ(GetTargetGoalPos()));
    SetRotY(pWork->m_fRotY);
    RotateYRate_(pWork->m_fRotY, 0.2f);
    long nMot;
    if (pWork->m_nFlag & 1) {
        if (m_nStatusFlag & 4) {
            nMot = -0x44;
        } else {
            nMot = 0x43;
        }
    } else {
        nMot = 0x44;
        if (m_nStatusFlag & 4) {
            nMot = -0x43;
        }
    }
    AnimPlay(3, nMot, 0, 0.0f, 0.0f);
    AnimSetSpeed(3, 1.0f);
    pWork->m_fFrameNum = AnimGetFrameNum(3);
    AnimSetSpeed(3, pWork->m_fFrameNum / pParam->m_nTrickMoveFrame);
    pWork->m_fFrameNum = pParam->m_nTrickMoveFrame;
    if ((nMot >= -0x44 && nMot <= -0x43) || (nMot >= 0x43 && nMot <= 0x44)) {
        if (pWork->m_nFlag & 1) {
            pWork->m_fRotStep = -6.2831855f / pWork->m_fFrameNum;
        } else {
            pWork->m_fRotStep = 6.2831855f / pWork->m_fFrameNum;
        }
    }
    float fTurn = bcs::DEG_TO_RAD(pParam->m_nTrickMoveRotDeg);
    if (pWork->m_nFlag & 1) {
        pWork->m_fRotY = bcs::ROUND_PI(pWork->m_fRotY + fTurn);
    } else {
        pWork->m_fRotY = bcs::ROUND_PI(pWork->m_fRotY - fTurn);
    }
    pWork->m_fUnk1C = s_fTrickMoveUnk1C;
    pWork->m_nStep = 1;
    return 1;
}

int msw::CChrCoreBasket::CanbeCombination() const
{
    if (CChrManBasket::IsBadConditon(this)) {
        return 0;
    }
    if (m_UnkEC & 0x08000000) {
        return 0;
    }
    if (m_nFlagF4 & 1) {
        return 0;
    }
    if (m_nBasketFlag & 0x800) {
        return 0;
    }
    // only while attacking
    if (CGame::g_GameWork.GetUserTeam() != m_nTeamNo) {
        return 0;
    }
    if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
        return 0;
    }
    switch (GetState()) {
        case STATE_AIR_TRICK:
            break;
        case STATE_DAMAGE:
        case STATE_JUMP_SHOOT:
        case STATE_REBOUND_DUNK:
        case STATE_LAY_UP:
        case STATE_ALLEYOOP:
        case STATE_DUNK:
        case STATE_JUMP_BLOCK:
        case STATE_JUMP_CATCH:
        case STATE_DRIBBLE_MISS:
            return 0;
    }
    return 1;
}

int msw::CChrCoreBasket::MethdCombination()
{
    if (!CanbeCombination()) {
        return 0;
    }
    if (m_nBasketFlag & 0x800) {
        return 1;
    }
    // (not used)
    bcs::FVEC3 ofs(0.0f, 1.5f, 0.0f);
    SetStateCombinationPlay(1);
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x32), 0, 0, 0, 0);
    PlayVoice(0x1D, 0);
    return 1;
}

// Over the water (bit 0x40000 of the basket flags) the character does not
// land on floor of attribute 8.
int msw::CChrCoreBasket::CanbeLanding() const
{
    if (m_nBasketFlag & 0x40000) {
        CStageMan *pStage = CStageMan::GetInstance();
        if (static_cast<unsigned long>(pStage->GetFloorAttribute(GetPos(), static_cast<CStageMan::FLOW_TYPE>(0))) == 8) {
            return 0;
        }
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeFall() const
{
    return 1;
}

// Falls into the water after the state now running; a motion queued next
// plays after the fall.
int msw::CChrCoreBasket::MethdFall()
{
    if (!CanbeFall()) {
        return 0;
    }
    int bKeep = 0;
    unsigned long nMotFlag;
    long nMotId;
    long nNextMot;
    long nVoice;
    long nSe;
    if (FsmqGetNextState() == STATE_MOT_PLAY) {
        WORK_MOT_PLAY *pMot = static_cast<WORK_MOT_PLAY *>(FsmqGetNextWork_());
        if (pMot != 0) {
            nMotFlag = pMot->m_nFlag;
            bKeep = 1;
            nMotId = pMot->m_nMot;
            nNextMot = pMot->m_nNextMot;
            nVoice = pMot->m_nVoice;
            nSe = pMot->m_nSe;
        }
    }
    FsmqCutQueue();
    FsmqCutQueue();
    FsmqPushWork(STATE_FALL);
    if (bKeep) {
        WORK_MOT_PLAY *pNext = static_cast<WORK_MOT_PLAY *>(FsmqPushWork(STATE_MOT_PLAY));
        pNext->m_nFlag = nMotFlag;
        pNext->m_nMot = nMotId;
        pNext->m_nNextMot = nNextMot;
        pNext->m_nVoice = nVoice;
        pNext->m_nSe = nSe;
    }
    return 1;
}

int msw::CChrCoreBasket::CanbeRebirth() const
{
    return 1;
}

// Back on the court after a fall.
int msw::CChrCoreBasket::MethdRebirth()
{
    if (!CanbeRebirth()) {
        return 0;
    }
    AnimPlay(3, 1, 1, 0.0f, 0.0f);
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
    SetPos(pos);
    SetRiverFallEnable(0);
    FsmqSetState(STATE_REBIRTH);
    return 1;
}

long msw::CChrCoreBasket::GetChangeStateDisableInput() const
{
    return 10;
}

// The character may ask for the pass: its team (or, on the ghost stage,
// the ghost) holds the ball.
int msw::CChrCoreBasket::CanbePassRequest()
{
    if (CChrManBasket::IsBadConditon(this)) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    if (IsNowDefense()) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 1) {
        return 0;
    }
    if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
        return 0;
    }
    switch (GetState()) {
        case STATE_NEUTRAL:
        case STATE_RUN:
        case STATE_DUNK:
        case STATE_ALLEYOOP:
        case STATE_REBOUND_DUNK: {
            CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(0);
            if (pHold != 0) {
                if (pHold->GetTeamNo() != GetTeamNo()) {
                    return 0;
                }
            } else if (CGame::g_GameWork.GetStage() == 6) {
                CNpcBase *pGhost = static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetGhost_(GetTeamNo());
                if (pGhost == 0) {
                    return 0;
                }
                if (!pGhost->IsHoldBall()) {
                    return 0;
                }
            } else {
                return 0;
            }
            return 1;
        }
    }
    return 0;
}

int msw::CNpcGhost::IsHoldBall() const
{
    return (m_nGhostFlag >> 1) & 1;
}

int msw::CChrCoreBasket::MethdPassRequest()
{
    if (!CanbePassRequest()) {
        return 0;
    }
    CChrManBasket::GetInstance()->Util_SendPassRequest_(0, this);
    MethdPassSign();
    return 1;
}

int msw::CChrCoreBasket::CanbePassSign()
{
    if (IsDisable()) {
        return 0;
    }
    if (IsHoldBall()) {
        return 0;
    }
    if (IsNowDefense()) {
        return 0;
    }
    if (IsHoldItem()) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 1) {
        return 0;
    }
    if (IsPassRequestMotPlay()) {
        return 0;
    }
    if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
        return 0;
    }
    long nState = GetState();
    if (nState == STATE_NEUTRAL || nState == STATE_RUN) {
        CChrCore *pHold = CChrMan::Util_SearchHoldBallChr(0);
        if (pHold != 0) {
            if (pHold->GetTeamNo() != m_nTeamNo) {
                return 0;
            }
        } else if (CGame::g_GameWork.GetStage() == 6) {
            CNpcBase *pGhost = static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetGhost_(m_nTeamNo);
            if (pGhost == 0) {
                return 0;
            }
            if (!pGhost->IsHoldBall()) {
                return 0;
            }
        } else {
            return 0;
        }
        return 1;
    }
    return 0;
}

// The motion asking for the pass, turned to the ball holder, and a call.
int msw::CChrCoreBasket::MethdPassSign()
{
    static const float fRequestFrame = 72.0f;

    if (!CanbePassSign()) {
        return 0;
    }
    CChrCore *pHave = CChrMan::Util_SearchHaveBallChr(0);
    switch (GetState()) {
        case STATE_NEUTRAL: {
            WORK_NEUTRAL *pWork = FsmqGetWork<WORK_NEUTRAL>();
            pWork->m_fRotY = GetRotY();
            AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x77 : 0x77, 1, 4.0f, 0.0f);
            pWork->m_nIdleLoop = GetPassRequestMotLoopNum();
            if (pHave != 0) {
                pWork->m_fRotY = GetRotY_ToPosXZ(pHave->GetPos());
            }
            break;
        }
        case STATE_RUN: {
            long nMot = 0x78;
            if (pHave != 0) {
                float fDiff = bcs::ROUND_PI(GetRotY_ToPosXZ(pHave->GetPos()) - GetRotY());
                if (fDiff > 3.1415927f) {
                    fDiff -= 6.2831855f;
                }
                if (fDiff < 0.0f) {
                    nMot = -0x78;
                }
            }
            AnimPlay(3, nMot, 1, 4.0f, 0.0f);
            FsmqGetWork<WORK_RUN>()->m_nIdleLoop = GetPassRequestMotLoopNum();
            break;
        }
        default:
            return 0;
    }
    long nRand = bcs::BCS_RandMax(0xFFFL);
    if (!IsPlayVoice()) {
        PlayVoiceBase(0x1C, 0, nRand);
    }
    return 1;
}
