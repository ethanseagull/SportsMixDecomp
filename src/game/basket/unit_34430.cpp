// msw::CChrCoreBasket's dribble states: the standing dribble (SDB) and its
// chain motion, the running dribble (RDB) with its turns and its change of
// hands, the catch into a dribble, the trick move (the spin past a
// defender), the dribble posture of a battle (ADP) and the lost dribble.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CGameGuideManBasket.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CQPanel.hpp"
#include "nw4r/math/triangular.h"

namespace {
    // 70 degrees in radians, set by the file's static initialiser and used
    // by nothing in the file; see CBallManBasket.cpp for the same angle.
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

// The trick move's after-images: one every 6 frames, each fading over 30
// frames (names guessed).
static const float s_IllusionInterval = 6;
static const float s_IllusionFrame = 30;

// The standing dribble's work (names guessed).
struct STATE_WORK_SDB {
    long m_nStep;          // 0x00
    // 0x04: the part of the bounce: 0 the plain bounce, 1 not yet
    // started, 2 to 4 a bounce asked for early, in the middle or late in
    // the motion.
    long m_nType;
    long m_nCount;         // 0x08: bounces still to make
    // 0x0C: bit 0 a dribble asked for, bit 1 one asked for too early
    unsigned long m_nFlag;
};

// The chain motion's work (names guessed): the motion of the change of
// hands and the bounces the standing dribble still had.
struct STATE_WORK_SDB_CHAIN {
    long m_nMot;   // 0x00
    long m_nCount; // 0x04
};

// The running dribble's work (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_RDB {
    long m_nStep;          // 0x00
    long m_nUnk04;         // 0x04
    unsigned long m_Unk08; // 0x08
    // 0x0C: bit 0 the fast run, bit 1 the stick is held, bit 2 the turn
    // goes left, bit 3 the start of the move, bit 6 the ball goes to the
    // left hand, bit 7 to the right hand.
    unsigned long m_nFlag;
    unsigned long m_Unk10; // 0x10
    float m_fFrame;        // 0x14: frames of the run (up to 1000)
    float m_fMoveFrame;    // 0x18
    float m_fRotY;         // 0x1C: the yaw the stick asks for
    float m_fMoveRotY;     // 0x20: the yaw of the run
    float m_fStopWait;     // 0x24: frames before the run stops without the stick
};

// The dribble posture's work (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_AUTO_DRIBBLE_POSTURE {
    long m_nStep;          // 0x00: 0 to 8 (see LoopStateAutoDribblePosture)
    // 0x04: bit 0 the stick is held, bit 1 a dash asked for, bit 2 no
    // battle move until the stick is let go, bit 3 the motion is the one
    // of the stick's direction, bit 5 a bounce asked for, bit 6 one asked
    // for too early, bit 7 the start sound is out.
    unsigned long m_nFlag;
    float m_fRotY;         // 0x08: the yaw the stick asks for
    float m_fStick;        // 0x0C: how far the stick is pushed
    float m_fHoldWait;     // 0x10: frames the stick still counts as held
    float m_fHoldFrame;    // 0x14: frames the stick has been held
    long m_nBoundStep;     // 0x18: the step of the bounce
    long m_nBoundType;     // 0x1C: the kind of bounce (as STATE_WORK_SDB's m_nType)
    long m_nBoundCount;    // 0x20: bounces still to make
    // 0x24-0x48: the battle move, as BATTLEPARAM (game/basket/unit_25D70.cpp)
    // has it, which SubBattleMove_ is given.
    long m_nDir;           // 0x24: the direction of the battle (GetDir4ToBattle)
    float m_fGoalRotY;     // 0x28: the yaw to the goal
    float m_fTurn;         // 0x2C
    float m_fMoveRotY;     // 0x30
    float m_fFrame;        // 0x34
    float m_fStartRotY;    // 0x38
    float m_fFrameNum;     // 0x3C
    float m_fSide;         // 0x40
    float m_fCurve;        // 0x44
    float m_fRate;         // 0x48
    bcs::FVEC3 m_Pos;      // 0x4C: where the character stood
};

// The catch into the dribble's work (names guessed).
struct STATE_WORK_SDB_CATCH {
    long m_nStep;                    // 0x00
    msw::CChrCore::ROT_LERP1 m_Rot;  // 0x04: the turn while the catch plays
    // 0x14: what the catch turns towards: 0 the goal, 1 character
    // m_nPassChr, 2 the yaw m_fPassRotY, 3 straight ahead.
    long m_nPassKind;
    long m_nPassChr;                 // 0x18
    float m_fPassRotY;               // 0x1C
    long m_bPass;                    // 0x20
};

// The lost dribble's work (names guessed).
struct STATE_WORK_DRIBBLE_MISS {
    float m_fRotY;     // 0x00: the yaw the ball was lost towards (MethdDribbleMiss)
    float m_fFrame;    // 0x04: the fraction of a frame not yet moved
    float m_fFrameNum; // 0x08: frames of the stagger back left
    float m_fSpeed;    // 0x0C: the move per frame
    float m_fWait;     // 0x10
};

namespace {

    // (guessed helper) An angle brought into (-pi, pi].
    inline float WrapRadPi(float rad)
    {
        rad = bcs::ROUND_PI(rad);
        if (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        return rad;
    }

    // (guessed helper) x kept between lo and hi; the bounds are only
    // worked out on the branch that needs them.
#define CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))
}

// The frame a dribble motion nMot starts at to carry on from the dribble
// motion playing now (0 when none plays).
float msw::CChrCoreBasket::GetDribbleStartFrame(long nMot)
{
    long nNow = AnimGetMotID(3, 0);
    if ((nNow >= 0x35 && nNow <= 0x38) || (nNow >= 0x3E && nNow <= 0x40) || nNow == 0x42) {
        float fFrame = AnimGetFrame(3);
        float fFrameNum = AnimGetFrameNum(3);
        return fFrame * AnimGetResFrameNum(nMot) / fFrameNum;
    }
    return 0.0f;
}

long msw::CChrCoreBasket::GetMotID_SDB(int bLeft) const
{
    if (bLeft == 1) {
        return -0x42;
    }
    return 0x42;
}

void msw::CChrCoreBasket::CtorStateSDB()
{
    m_UnkEC |= 0x10;
    SetCollBodyPushPower_(3);
    SetMoveMotType(7);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(6), 8.0f);
    SetCollBodyPushRank_(1);
    long nMot = GetMotID_SDB(IsHaveLeft());
    AnimPlay(3, nMot, 1, 10.0f, GetDribbleStartFrame(nMot));
    AnimSetSpeed(3, 1.0f);
}

void msw::CChrCoreBasket::LoopStateSDB()
{
    STATE_WORK_SDB *pWork = FsmqGetWork<STATE_WORK_SDB>();
    float fFrame;

    if ((pWork->m_nFlag & 1) && pWork->m_nType != 1) {
        // Another bounce asked for: how far the motion had got.
        pWork->m_nFlag &= ~1;
        fFrame = AnimGetFrame(3);
        pWork->m_nStep = 0;
        if (fFrame < 28.0f) {
            pWork->m_nType = 2;
        } else if (fFrame < 42.0f) {
            pWork->m_nType = 3;
        } else {
            pWork->m_nType = 4;
        }
        // Next to a ? panel on the side of the free hand: the ball changes
        // hands first.
        CQPanel *pPanel = CGimmickMan::QPanelGetNear(GetPos());
        if (pPanel != 0) {
            float fDiff = WrapRadPi(GetRotY_ToPosXZ(pPanel->GetPos()) - GetRotY());
            if ((fDiff < 0.0f && (m_nStatusFlag & 4)) || (fDiff > 0.0f && !(m_nStatusFlag & 4))) {
                STATE_WORK_SDB_CHAIN *pChain = static_cast<STATE_WORK_SDB_CHAIN *>(FsmqPushWork(STATE_SDB_CHAIN));
                pChain->m_nMot = (m_nStatusFlag & 4) ? -0x39 : 0x39;
                pChain->m_nCount = pWork->m_nCount;
                FsmqPush(STATE_SDB);
                FsmqDoChain();
                return;
            }
        }
    } else {
        fFrame = AnimGetFrame(3);
    }

    switch (pWork->m_nType) {
        case 2:
            switch (pWork->m_nStep) {
                case 0:
                    AnimChgSpeed(3, 3.75f, 0.0f);
                    pWork->m_nStep++;
                    break;
                case 1:
                    if (fFrame >= 28.0f) {
                        if (pWork->m_nCount > 1) {
                            pWork->m_nCount--;
                            pWork->m_nFlag |= 1;
                            SetTap();
                        } else if (pWork->m_nCount > 0) {
                            AnimChgSpeed(3, 3.0f, 0.5f * (56.0f - fFrame));
                            pWork->m_nType = 0;
                        } else {
                            AnimSetSpeed(3, 3.0f);
                            AnimChgSpeed(3, 1.0f, 0.5f * (56.0f - fFrame));
                            pWork->m_nType = 0;
                        }
                    }
                    break;
            }
            break;
        case 3:
            switch (pWork->m_nStep) {
                case 0:
                    AnimChgSpeed(3, 3.75f, 0.0f);
                    pWork->m_nStep++;
                    break;
                case 1:
                    if (fFrame >= 42.0f) {
                        AnimSkipInter(3, 56.0f - fFrame, 0.0f);
                        pWork->m_nType = 2;
                        pWork->m_nStep = 1;
                    }
                    break;
            }
            break;
        case 4:
            if (pWork->m_nStep == 0) {
                AnimSetSpeed(3, 3.75f);
                AnimSetFrame(3, 56.0f - fFrame);
                pWork->m_nType = 2;
                pWork->m_nStep = 1;
            }
            break;
        case 0:
            if (pWork->m_nCount > 0) {
                if (IsAnimLoop(3)) {
                    if (pWork->m_nFlag & 2) {
                        pWork->m_nFlag &= ~2;
                        pWork->m_nCount = 0;
                    } else {
                        pWork->m_nCount--;
                    }
                    pWork->m_nFlag |= 1;
                    SetTap();
                }
            } else {
                unsigned long nFlag = pWork->m_nFlag;
                if (nFlag & 2) {
                    // The bounce asked for too early is taken now.
                    pWork->m_nCount = 0;
                    pWork->m_nFlag = (nFlag & ~2) | 1;
                    SetTap();
                }
            }
            break;
    }
}

void msw::CChrCoreBasket::DtorStateSDB()
{
}

void msw::CChrCoreBasket::CtorStateSDB_Chain()
{
    STATE_WORK_SDB_CHAIN *pWork = FsmqGetWork<STATE_WORK_SDB_CHAIN>();

    if (pWork->m_nMot == -0x39 || pWork->m_nMot == 0x39) {
        AnimPlayEx(3, pWork->m_nMot, 0, 6.0f, 0.0f, 8);
    }
}

void msw::CChrCoreBasket::LoopStateSDB_Chain()
{
    STATE_WORK_SDB_CHAIN *pWork = FsmqGetWork<STATE_WORK_SDB_CHAIN>();

    if (IsAnimEnd(3)) {
        // The ball is in the other hand now.
        switch (pWork->m_nMot) {
            case -0x39:
                m_nStatusFlag &= ~4;
                break;
            case 0x39:
                m_nStatusFlag |= 4;
                break;
        }
        long nCount = pWork->m_nCount;
        FsmqDoChain();
        if (GetState() == STATE_SDB) {
            FsmqGetWork<STATE_WORK_SDB>()->m_nCount = nCount;
        }
    }
}

void msw::CChrCoreBasket::DtorStateSDB_Chain()
{
}

void msw::CChrCoreBasket::CtorStateRDB()
{
    m_UnkEC |= 0x10;
    SetCollBodyPushPower_(1);
    SetMoveMotType(8);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(6), 8.0f);
    SetCollBodyPushRank_(1);
    STATE_WORK_RDB *pWork = FsmqGetWork<STATE_WORK_RDB>();
    long nMot = 0x40;
    pWork->m_fFrame = 0.0f;
    pWork->m_fMoveFrame = 0.0f;
    pWork->m_fStopWait = 0.0f;
    if (IsHaveLeft() == 1) {
        nMot = -0x40;
    }
    AnimPlay(3, nMot, 1, 8.0f, GetDribbleStartFrame(nMot));
}

namespace {

    // (guessed helper) Whether the running dribble asks for a change of
    // hands (bits 6 and 7).
    inline unsigned long IsRDBChangeHand(const msw::CChrCoreBasket::STATE_WORK_RDB *pWork)
    {
        return (pWork->m_nFlag & 0xC0) != 0;
    }
}

void msw::CChrCoreBasket::LoopStateRDB()
{
    STATE_WORK_RDB *pWork = FsmqGetWork<STATE_WORK_RDB>();

    pWork->m_fFrame += GetWholeFrameRate();
    if (pWork->m_fFrame > 1000.0) {
        pWork->m_fFrame = 1000.0f;
    }

    if (!(pWork->m_nFlag & 2)) {
        // The stick let go: the run stops after a few frames.
        pWork->m_fStopWait -= GetFrameStep();
        if (pWork->m_fStopWait < 0.0f) {
            if (FsmqGetNextState() == 0) {
                FsmqPush(STATE_SDB);
            }
            FsmqDoChain();
            return;
        }
    } else {
        pWork->m_fStopWait = 4.0f;
    }

    switch (AnimGetMotID(2, 0)) {
        case -0x40:
        case 0x40:
            CheckRDBChangeDribble_();
            if (IsRDBChangeHand(pWork) == 1) {
                float fFrame = AnimGetFrame(2);
                if (fFrame <= 3.0f || fFrame >= 0.9f * AnimGetFrameNum(2)) {
                    // The ball changes hands.
                    if (((pWork->m_nFlag >> 7) & 1) == 1) {
                        AnimPlay(3, 0x41, 0, 8.0f, 0.0f);
                    } else {
                        AnimPlay(3, -0x41, 0, 8.0f, 0.0f);
                    }
                }
            }
            break;
        case -0x41:
        case 0x41:
            if (IsAnimEnd(2) == 1) {
                if (((pWork->m_nFlag >> 7) & 1) == 1) {
                    m_nStatusFlag |= 4;
                } else {
                    m_nStatusFlag &= ~4;
                }
                AnimPlay(3, IsHaveLeft() == 1 ? -0x40 : 0x40, 1, 0.0f, 0.0f);
                pWork->m_nFlag &= ~0xC0;
            }
            break;
    }

    if (!IsBattle()) {
        pWork->m_nFlag &= ~1;
    } else if (pWork->m_fFrame >= 10.0f) {
        // In a battle a sharp turn of the stick goes into the dribble
        // posture.
        float fDiff = WrapRadPi(pWork->m_fMoveRotY - GetRotY());
        if (__fabsf(fDiff) > 0.017453292f * GetCcParam2_()->m_nDashTurnDeg) {
            STATE_WORK_AUTO_DRIBBLE_POSTURE *pNext =
                static_cast<STATE_WORK_AUTO_DRIBBLE_POSTURE *>(FsmqPushWork(STATE_AUTO_DRIBBLE_POSTURE));
            pNext->m_nFlag = 0;
            pNext->m_fHoldWait = 0.0f;
            pNext->m_fHoldFrame = 0.0f;
            pNext->m_nStep = 0;
            FsmqDoChain();
            return;
        }
    }

    // (the speed is asked for and not used: SubStateRDBMove_ asks again)
    if (!(pWork->m_nFlag & 1)) {
        GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(1));
    } else {
        GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(2));
    }
    pWork->m_fMoveRotY = SubStateRDBMove_(pWork);
    AnimSetSpeed(3, GetConditionStarMotSpeed());
    pWork->m_nFlag &= ~2;
}

void msw::CChrCoreBasket::DtorStateRDB()
{
    Util_DetachEffect(static_cast<ATTACH_EFFECT_TYPE>(0));
}

// Asks for a change of hands: away from an enemy within 5 m, else towards
// the side the stick turns to.
void msw::CChrCoreBasket::CheckRDBChangeDribble_()
{
    STATE_WORK_RDB *pWork = FsmqGetWork<STATE_WORK_RDB>();

    const CChrCore *pEnemy = CChrMan::Util_SearchNearPosEnemy(m_nPlayerNo);
    if (pEnemy != 0) {
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &pEnemy->GetPos(), &GetPos());
        bcs::FVEC3 toEnemy = diff;
        if (bcs::BCS_Vec_GetLenXZ(toEnemy) <= 5.0f) {
            float fDiff = WrapRadPi(GetRotY_ToPosXZ(pEnemy->GetPos()) - GetRotY());
            int bLeft = IsHaveLeft();
            if (bLeft == 1 && fDiff > 0.0f) {
                pWork->m_nFlag = (pWork->m_nFlag & ~0x80) | 0x40;
            } else if (bLeft == 0 && fDiff < 0.0f) {
                pWork->m_nFlag = (pWork->m_nFlag | 0x80) & ~0x40;
            } else {
                pWork->m_nFlag &= ~0xC0;
            }
            return;
        }
    }

    float fDiff = WrapRadPi(pWork->m_fRotY - GetRotY());
    int bLeft = IsHaveLeft();
    if (bLeft == 1 && fDiff > 0.13962634f) {
        pWork->m_nFlag = (pWork->m_nFlag & ~0x80) | 0x40;
    } else if (bLeft == 0 && fDiff < -0.13962634f) {
        pWork->m_nFlag = (pWork->m_nFlag | 0x80) & ~0x40;
    } else {
        pWork->m_nFlag &= ~0xC0;
    }
}

// One frame of the running dribble's move: turns towards the stick by
// CC_PARAM_BASKET2's turn rate and runs on. Returns the yaw asked for.
float msw::CChrCoreBasket::SubStateRDBMove_(STATE_WORK_RDB *pWork)
{
    float fSpeed;
    if ((pWork->m_nFlag & 1) == 0) {
        fSpeed = GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(1));
    } else {
        fSpeed = GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(2));
    }
    float fRate = GetCcParam2_()->m_fUnk020;
    pWork->m_fMoveFrame += GetWholeFrameRate();

    float fDiff = bcs::ROUND_PI(pWork->m_fRotY - GetRotY());
    float fStep = GetWholeFrameRate();
    if (fDiff < 3.1415927f) {
        float fTurn = fDiff * fRate;
        fDiff = CLAMP(fTurn, 0.0f, fDiff);
    } else {
        float fRest = 6.2831855f - fDiff;
        float fTurn = -fRate * fRest;
        fDiff = CLAMP(fTurn, -1.0f * fRest, 0.0f);
    }
    fDiff += GetRotY();
    SetRotY(fDiff);

    bcs::FVEC3 move;
    bcs::BCS_VecZ_RotY(move, fSpeed * fStep, pWork->m_fRotY);
    AddPosXZ(move.x, move.z);
    m_fJumpUnk220 += fStep;
    pWork->m_fMoveFrame -= fStep;
    return pWork->m_fRotY;
}

namespace unit_34430_cpp {

    // uncalled stand-in: the original has SubStateRDBMove_(float, float,
    // int, float) here, which nothing calls and the link strips; its local
    // static fRate (0.125) and a literal 0.125 stay in .rodata.
    float UnusedRDBMoveRate(float fFrame)
    {
        static const float fRate = 0.125f;
        return fFrame * fRate;
    }
}

// 1 when the dribble towards fRotY turns left of fStartRotY, 0 when it
// turns right; near a goal the turn is bent away from the end line.
long msw::CChrCoreBasket::SubStateRDBGetRotFlag_(float fRotY, float fStartRotY)
{
    // static-order swap: the original has these in the reverse order.
    static const float CC_RDB_ROT_PLUS = 60.0f;
    static const float CC_RDB_ROT_DIR_DIS = 14.6f;
    static const float CC_RDB_ROT_DIR_Z_END = -16.6f;
    static const float CC_RDB_ROT_DIR_Z_START = -2.0f;
    static const float STAGE_GOAL_Z = -14.6f;

    float fDiff = bcs::ROUND_PI(fRotY - fStartRotY);
    if (!CGame::IsExistGoalRing()) {
        return fDiff >= 3.1415927f;
    }

    float fPlus = 0.0f;
    const bcs::FVEC3 &rPos = GetPos();
    if (GetTargetGoalPos().z < 0.0f) {
        if (rPos.z < CC_RDB_ROT_DIR_Z_START) {
            if (rPos.z < CC_RDB_ROT_DIR_Z_END) {
                fPlus = bcs::DEG_TO_RAD(CC_RDB_ROT_PLUS);
            } else {
                fPlus = -(rPos.z - CC_RDB_ROT_DIR_Z_START) * bcs::DEG_TO_RAD(CC_RDB_ROT_PLUS) / CC_RDB_ROT_DIR_DIS;
            }
        }
    } else {
        if (rPos.z > -CC_RDB_ROT_DIR_Z_START) {
            if (rPos.z > -CC_RDB_ROT_DIR_Z_END) {
                fPlus = bcs::DEG_TO_RAD(CC_RDB_ROT_PLUS);
            } else {
                fPlus = (CC_RDB_ROT_DIR_Z_START + rPos.z) * bcs::DEG_TO_RAD(CC_RDB_ROT_PLUS) / CC_RDB_ROT_DIR_DIS;
            }
        }
    }

    long nFlag;
    if (GetTargetGoalPos().z < 0.0f) {
        if (rPos.x >= 0.0f) {
            nFlag = 0;
            if (fDiff > 3.1415927f + fPlus) {
                nFlag = 1;
            }
            return nFlag;
        }
        nFlag = 1;
        if (fDiff < 3.1415927f - fPlus) {
            nFlag = 0;
        }
        return nFlag;
    }
    if (rPos.x < 0.0f) {
        nFlag = 0;
        if (fDiff > 3.1415927f + fPlus) {
            nFlag = 1;
        }
        return nFlag;
    }
    nFlag = 1;
    if (fDiff < 3.1415927f - fPlus) {
        nFlag = 0;
    }
    return nFlag;
}

// Whether the side of the dribble's turn is to be asked again: the stick
// has come within 45 degrees of the start yaw, or has left 45 degrees of
// its last yaw.
int msw::CChrCoreBasket::SubStateRDBIsUpdateRotFlag_(float fRotY, float fOldRotY, float fStartRotY)
{
    static const float CC_RDB_ROT_UPDATE = 45.0f;
    static const float CC_RDB_ROT_NEAR = 45.0f;

    float fDiff = bcs::ROUND_PI(fRotY - fStartRotY);
    if (fDiff <= bcs::DEG_TO_RAD(CC_RDB_ROT_UPDATE) || fDiff >= 6.2831855f - bcs::DEG_TO_RAD(CC_RDB_ROT_UPDATE)) {
        return 1;
    }
    fDiff = bcs::ROUND_PI(fRotY - fOldRotY);
    if (fDiff <= bcs::DEG_TO_RAD(CC_RDB_ROT_NEAR) || fDiff >= 6.2831855f - bcs::DEG_TO_RAD(CC_RDB_ROT_NEAR)) {
        return 0;
    }
    return 1;
}

void msw::CChrCoreBasket::CtorStateSDB_Catch()
{
    m_UnkEC |= 0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    SetCollBodyPushRank_(1);
    STATE_WORK_SDB_CATCH *pWork = FsmqGetWork<STATE_WORK_SDB_CATCH>();
    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(3);
    if (CGame::IsExistGoalRing()) {
        m_nStatusFlag |= 0x80;
    }
    m_nStatusFlag &= ~0x40;

    // The point the catch turns towards.
    bcs::FVEC3 target;
    switch (pWork->m_nPassKind) {
        case 0:
            if (CGame::IsExistGoalRing()) {
                GetTargetGoalPos(target);
                break;
            }
            // (no goal: straight ahead)
        case 3:
            bcs::BCS_VecOneZ_RotY(target, GetRotY());
            bcs::FVEC3Add(&target, &target, &GetPos());
            break;
        case 1:
            CChrMan::GetChrCore(pWork->m_nPassChr)->GetCompPos(target);
            break;
        case 2:
            bcs::BCS_VecOneZ_RotY(target, bcs::ROUND_PI(pWork->m_fPassRotY));
            bcs::FVEC3Add(&target, &target, &GetPos());
            break;
    }
    CalcSdbCatchRotLerpParam_(target, pWork->m_Rot);
    m_nStatusFlag &= ~0xC;
    ReattachBall(5, 8.0f, 0);
}

void msw::CChrCoreBasket::LoopStateSDB_Catch()
{
    STATE_WORK_SDB_CATCH *pWork = FsmqGetWork<STATE_WORK_SDB_CATCH>();

    int bEnd = 0;
    if (RotateYParam_(pWork->m_Rot) && !IsAnimInter(3)) {
        bEnd = 1;
    }
    if (bEnd) {
        if (GetAttachBall() != 0) {
            PlaySeCatch_();
        }
        if (FsmqGetNextState() == 0) {
            Util_PushBackCatch(1);
        }
        FsmqNext();
    }
}

void msw::CChrCoreBasket::DtorStateSDB_Catch()
{
}

namespace {

    // (guessed helper) The yaw of the direction from rFrom to rTo in the
    // XZ plane (rDir: the direction, without its y).
    inline float GetRotYXZ(bcs::FVEC3 &rDir, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        rDir.x = rTo.x - rFrom.x;
        rDir.z = rTo.z - rFrom.z;
        return bcs::ROUND_PI(nw4r::math::Atan2Rad(rDir.x, rDir.z));
    }
}

// (a macro: the original's literal order needs the operand written twice)
#define SQUARE(x) ((x) * (x))

// The turn of the catch into the dribble: towards rTarget, the way round
// that does not swing the ball through the body, over more frames the
// further it goes.
void msw::CChrCoreBasket::CalcSdbCatchRotLerpParam_(const bcs::FVEC3 &rTarget, CChrCore::ROT_LERP1 &rLerp)
{
    const CBall *pBall = GetAttachBall();

    bcs::FVEC3 dir;
    float fRotY = GetRotYXZ(dir, GetPos(), rTarget);
    rLerp.m_afUnk[0] = fRotY;
    float fBallRotY = GetRotYXZ(dir, GetPos(), pBall->GetPos());
    float fDiff = bcs::ROUND_PI(GetRotY() - fRotY);
    float fBallDiff = bcs::ROUND_PI(fBallRotY - fRotY);

    // Which way round (1: the yaw goes down).
    int bRight;
    if (!(m_nStatusFlag & 4)) {
        if (fDiff >= 3.5430183f) {
            if (fBallDiff >= 0.40142572f && fBallDiff <= 2.7401669f) {
                bRight = 1;
            } else {
                bRight = 0;
            }
        } else if (fDiff <= 2.3561945f) {
            bRight = 1;
        } else if (fBallDiff >= 2.7576201f && fBallDiff <= 3.5255651f) {
            bRight = 0;
        } else {
            bRight = 1;
        }
    } else {
        if (fDiff <= 2.7401669f) {
            if (fBallDiff >= 3.5430183f && fBallDiff <= 5.8817596f) {
                bRight = 0;
            } else {
                bRight = 1;
            }
        } else if (fDiff >= 3.9269908f) {
            bRight = 0;
        } else if (fBallDiff >= 2.7576201f && fBallDiff <= 3.5255651f) {
            bRight = 1;
        } else {
            bRight = 0;
        }
    }
    fDiff = bRight ? fDiff : 6.2831855f - fDiff;

    // 6 to 14 frames, the most for half a turn.
    float fFrame = 6.0f + (-8.0f * SQUARE(0.15915494f * (fDiff - 6.2831855f)) + 8.0f);
    rLerp.m_afUnk[2] = fFrame;
    if (!(m_nStatusFlag & 4)) {
        if (fBallDiff >= 2.7576201f && fBallDiff <= 3.9269908f) {
            float fAdd = 4.0f * (-fBallDiff + 2.7576201f + 1.1693705f);
            fAdd *= 0.85516095f;
            rLerp.m_afUnk[2] = fFrame + fAdd;
        }
    } else if (fBallDiff <= 3.5255651f && fBallDiff >= 2.3561945f) {
        float fAdd = 4.0f * (fBallDiff - 2.3561945f) * 0.85516095f;
        rLerp.m_afUnk[2] = fFrame + fAdd;
    }
    rLerp.m_afUnk[2] *= 0.55f;
    if (rLerp.m_afUnk[2] < 1.0f) {
        rLerp.m_afUnk[2] = 1.0f;
    }
    float fStep = fDiff / rLerp.m_afUnk[2];
    if (bRight) {
        fStep = -fStep;
    }
    rLerp.m_afUnk[1] = bcs::ROUND_PI(fStep);
    rLerp.m_afUnk[3] = 0.0f;
}

void msw::CChrCoreBasket::CtorStateTrickMove()
{
    STATE_WORK_TRICK_MOVE *pWork = FsmqGetWork<STATE_WORK_TRICK_MOVE>();

    m_UnkEC |= 0x10;
    SetMoveMotType(7);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(6), 8.0f);
    SetCollBodyPushPower_(1);
    SetCollBodyPushRank_(0);
    m_pCollBody->ResetTargetFlag(1, 0x10);
    CGameBasket::GetInstance()->StopEffect_(0.0f, 0.0f, 0.0f);
    PlayTrickMoveEffect(pWork->m_nFlag & 1);
    PlaySePos(0xC7A, GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
    if (GetChrId() != 14) {
        // After-images (not for character 14).
        pWork->m_nFlag |= 8;
    }
    pWork->m_nSlowBlockCutBit = 0;
    pWork->m_nSlowRunBit = 0;
}

void msw::CChrCoreBasket::LoopStateTrickMove()
{
    static const float fLocalTargetFrame = 1.0f;

    STATE_WORK_TRICK_MOVE *pWork = FsmqGetWork<STATE_WORK_TRICK_MOVE>();
    float fStep = GetWholeFrameRate();

    pWork->m_fFrame += fStep;
    pWork->m_fRemainderFrame -= fStep;
    if (pWork->m_fRemainderFrame <= 0.0f) {
        pWork->m_nStep = 3;
    }

    switch (pWork->m_nStep) {
        case 0:
        case 1: {
            // The spin: on along the yaw it ends at.
            // (the yaw to the goal is asked for and not used)
            GetRotY_ToPosXZ(GetTargetGoalPos());
            bcs::FVEC3 move;
            bcs::BCS_VecZ_RotY(move, 1.0f, pWork->m_fRotY);
            float fSpeed = pWork->m_fSpeed * fStep;
            move.x *= fSpeed;
            move.z *= fSpeed;
            AddPos(move);
            SetRotY(bcs::ROUND_PI(pWork->m_fRotStep * fStep + GetRotY()));
            break;
        }
        case 2: {
            // The turn round the defender: done once the point a little
            // ahead faces the goal on the side the spin goes to.
            bcs::FVEC3 pos = GetPos();
            float fLook = fLocalTargetFrame * 1.2f;
            pWork->m_fLookFrame = fLook;
            bcs::FVEC3 target(pWork->m_Dir.x * fLook + pos.x, 0.0f, pWork->m_Dir.z * fLook + pos.z);
            SetPos(target);
            float fGoalRotY = GetRotY_ToPosXZ(GetTargetGoalPos());
            SetPos(pos);
            float fDiff = WrapRadPi(GetRotY_FromPosXZ(target) - fGoalRotY);
            int bOther = pWork->m_nFlag & 1;
            if (__fabsf(fDiff) <= 1.5707964f && ((fDiff > 0.0f && bOther) || (fDiff < 0.0f && !bOther))) {
                pWork->m_fRotY = GetRotY_ToPosXZ(GetTargetGoalPos());
                FsmqRetry();
                pWork->m_nStep = 1;
                return;
            }

            float fTurn = fStep * pWork->m_fTurnSpeed;
            bcs::FVEC3 diff;
            bcs::FVEC3Sub(&diff, &GetPos(), &target);
            bcs::FVEC3 rel = diff;
            bcs::FVEC3 turned;
            if (pWork->m_nFlag & 1) {
                bcs::BCS_Vec_RotY(turned, rel, -fTurn);
            } else {
                bcs::BCS_Vec_RotY(turned, rel, fTurn);
            }
            bcs::FVEC3 moveDiff;
            bcs::FVEC3Sub(&moveDiff, &turned, &rel);
            bcs::FVEC3 move = moveDiff;
            AddPos(move);
            SetRotY(bcs::ROUND_PI(pWork->m_fRotStep * fStep + GetRotY()));

            bcs::FVEC3 dirDiff;
            bcs::FVEC3Sub(&dirDiff, &target, &GetPos());
            bcs::FVEC3 dir = dirDiff;
            pWork->m_Dir = dir;
            bcs::BCS_Vec_Normalize(pWork->m_Dir, pWork->m_Dir);
            break;
        }
        case 3: {
            // The end: the ball stays in the hand of the side spun to.
            long bLeft = !!(GetStatusFlag() & 4);
            if (static_cast<long>(pWork->m_nFlag & 1) != bLeft) {
                if (bLeft) {
                    m_nStatusFlag &= ~4;
                } else {
                    m_nStatusFlag |= 4;
                }
            }
            CGameGuideManBasket::AddSuccessCount(this, 7);
            Util_PushBackOnBallerBattleEnd_(IsBattle(), 1, GetRotY());
            FsmqDoChain();
            return;
        }
    }

    if (pWork->m_nFlag & 8) {
        // An after-image every s_IllusionInterval frames.
        while (fStep >= 1.0f) {
            fStep -= 1.0f;
            pWork->m_fIllusionWait -= 1.0f;
            if (pWork->m_fIllusionWait <= 0.0f) {
                pWork->m_fIllusionWait += s_IllusionInterval;
                CreateIllusion(0.5f, 1.0f / s_IllusionFrame);
            }
        }
        pWork->m_fIllusionWait -= fStep;
    }
}

void msw::CChrCoreBasket::DtorStateTrickMove()
{
    Eff_Stop(10);
    m_pCollBody->SetTargetFlag(1, 0x10);
    m_nStatusFlag &= ~0x20000000;
    m_fTrickMoveWait = 0.0f;
}

void msw::CChrCoreBasket::CtorStateAutoDribblePosture()
{
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(4);
    SetCollBodyPushPower_(6);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(6), 8.0f);
    m_fBlockWorkWait = 20.0f;
}

// One frame of the dribble of the posture, as the standing dribble's:
// the motion has two bounces (frames 14 and 42 of 56). Returns 1 once no
// more bounces are asked for.
int msw::CChrCoreBasket::SubStateADPCmnDribble_(STATE_WORK_AUTO_DRIBBLE_POSTURE *pWork)
{
    // static-order swap: the original has these in the reverse order.
    static const float ADP_LAST_MID_SPEED = 1.5f;
    static const float ADP_HIGH_SPEED = 1.875f;
    static const float ADP_MID_SPEED = 1.5f;
    static const float ADP_DEF_SPEED = 1.0f;
    static const float ADP_BALL_BOUND_FRM = 14.0f;
    static const float ADP_FRAME = 56.0f;

    float fFrame;
    if ((pWork->m_nFlag & 0x20) && pWork->m_nBoundType != 1) {
        pWork->m_nFlag &= ~0x20;
        fFrame = AnimGetFrame(3);
        pWork->m_nBoundStep = 0;
        if (fFrame < ADP_BALL_BOUND_FRM || (fFrame > 28.0f && fFrame < 42.0f)) {
            pWork->m_nBoundType = 2;
        } else if (fFrame < 21.0f || (fFrame > 42.0f && fFrame < 49.0f)) {
            pWork->m_nBoundType = 3;
        } else {
            pWork->m_nBoundType = 4;
        }
    } else {
        fFrame = AnimGetFrame(3);
    }

    switch (pWork->m_nBoundType) {
        case 2:
            switch (pWork->m_nBoundStep) {
                case 0:
                    AnimChgSpeed(3, ADP_HIGH_SPEED, 0.0f);
                    pWork->m_nBoundStep++;
                    break;
                case 1:
                    if ((fFrame >= ADP_BALL_BOUND_FRM && fFrame < 28.0f) || fFrame >= 42.0f) {
                        if (pWork->m_nBoundCount > 1) {
                            pWork->m_nBoundCount--;
                            pWork->m_nFlag |= 0x20;
                            SetTap();
                        } else if (pWork->m_nBoundCount > 0) {
                            AnimChgSpeed(3, ADP_LAST_MID_SPEED, 0.5f * (ADP_FRAME - fFrame));
                            pWork->m_nBoundType = 0;
                        } else {
                            AnimSetSpeed(3, ADP_MID_SPEED);
                            AnimChgSpeed(3, ADP_DEF_SPEED, 0.5f * (ADP_FRAME - fFrame));
                            pWork->m_nBoundType = 0;
                        }
                    }
                    break;
            }
            break;
        case 3:
            switch (pWork->m_nBoundStep) {
                case 0:
                    AnimChgSpeed(3, ADP_HIGH_SPEED, 0.0f);
                    pWork->m_nBoundStep++;
                    break;
                case 1:
                    if (fFrame >= 21.0f && fFrame <= 28.0f) {
                        AnimSkipInter(3, 28.0f + (28.0f - fFrame), 0.0f);
                        pWork->m_nBoundType = 2;
                        pWork->m_nBoundStep = 1;
                    } else if (fFrame >= 49.0f && fFrame <= ADP_FRAME) {
                        AnimSkipInter(3, ADP_FRAME - fFrame, 0.0f);
                        pWork->m_nBoundType = 2;
                        pWork->m_nBoundStep = 1;
                    }
                    break;
            }
            break;
        case 4:
            if (pWork->m_nBoundStep == 0) {
                AnimSetSpeed(3, ADP_HIGH_SPEED);
                AnimSetFrame(3, fFrame < 28.0f ? 28.0f + (28.0f - fFrame) : ADP_FRAME - fFrame);
                pWork->m_nBoundType = 2;
                pWork->m_nBoundStep = 1;
            }
            break;
        case 0:
            if (pWork->m_nBoundCount > 0) {
                if (IsAnimLoop(3)) {
                    if (pWork->m_nFlag & 0x40) {
                        pWork->m_nFlag &= ~0x40;
                        pWork->m_nBoundCount = 0;
                    } else {
                        pWork->m_nBoundCount--;
                    }
                    pWork->m_nFlag |= 0x20;
                    SetTap();
                }
            } else if (pWork->m_nFlag & 0x40) {
                pWork->m_nBoundCount = 0;
                pWork->m_nFlag &= ~0x40;
                pWork->m_nFlag |= 0x20;
                SetTap();
            } else {
                return 1;
            }
            break;
    }
    return 0;
}

namespace {

    // (guessed helper) Starts the battle move of the posture in direction
    // nDir from where the character stands.
    inline void StartADPBattleMove(msw::CChrCoreBasket *pChr,
                                   msw::CChrCoreBasket::STATE_WORK_AUTO_DRIBBLE_POSTURE *pWork, long nDir)
    {
        pWork->m_Pos = pChr->GetPos();
        pWork->m_nDir = nDir;
        pWork->m_nStep = 2;
        pChr->FsmqRetry();
    }
}

namespace {

    // (guessed helper) 1 while the ball is in the left hand, as IsHaveLeft.
    inline int IsBallInLeft(const msw::CChrCore *pChr)
    {
        return (pChr->GetStatusFlag() & 4) ? 1 : 0;
    }

    // (guessed helper) The block work is over (CtorStateAutoDribblePosture
    // sets 20 frames).
    inline int IsBlockWorkEnd(const msw::CChrCoreBasket *pChr)
    {
        return 0.0f == pChr->m_fBlockWorkWait;
    }
}

// The dribble posture of a battle: steps 0 and 1 the bounce facing the
// goal, 2 to 5 the battle move to a side (2 and 4 start it, 3 and 5 run
// it), 6 and 8 the dribble that follows the stick, 7 the change of hands.
void msw::CChrCoreBasket::LoopStateAutoDribblePosture()
{
    static const float fInter = 10.0f;
    static const float fKeyValueRang = 0.85f;

    STATE_WORK_AUTO_DRIBBLE_POSTURE *pWork = FsmqGetWork<STATE_WORK_AUTO_DRIBBLE_POSTURE>();
    int bInput = pWork->m_nFlag & 1;
    int bBattle = IsBattle();
    UpDate1on1ActMinFrame();

    if (!bBattle && IsBlockWorkEnd(this)) {
        // The battle is over.
        Util_PushBackOnBallerBattleEnd_(bBattle, bInput, pWork->m_fRotY);
        FsmqDoChain();
        return;
    }

    if (!bInput) {
        pWork->m_fHoldFrame = 0.0f;
        pWork->m_fHoldWait -= GetWholeFrameRate();
        if (pWork->m_fHoldWait <= 0.0f) {
            pWork->m_fHoldWait = 0.0f;
            // Back to the bounce once a move has got half way.
            if (static_cast<unsigned long>(pWork->m_nStep) > 1) {
                if (pWork->m_nStep == 3 || pWork->m_nStep == 5 || pWork->m_nStep == 7) {
                    if (AnimGetFrame(3) >= 0.5f * AnimGetFrameNum(3)) {
                        if (pWork->m_nDir == 1) {
                            m_nStatusFlag &= ~4;
                        } else {
                            m_nStatusFlag |= 4;
                        }
                        pWork->m_nStep = 0;
                    }
                } else {
                    pWork->m_nStep = 0;
                }
            }
        }
        SetCollBodyPushRank_(1);
    } else {
        SetCollBodyPushRank_(0);
    }

    float fHalfPi = 1.5707964f;
    switch (pWork->m_nStep) {
        case 0: {
            if (!(pWork->m_nFlag & 0x80)) {
                PlaySePos(0xC76, static_cast<const bcs::FVEC3 *>(0), 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nFlag |= 0x80;
            }
            long nMot = -0x35;
            if (!(m_nStatusFlag & 4)) {
                nMot = 0x35;
            }
            AnimSetSpeed(3, 1.0f);
            AnimPlay(3, nMot, 1, fInter, 0.0f);
            pWork->m_fGoalRotY = GetRotY_ToGoal();
            pWork->m_fHoldFrame = 0.0f;
            pWork->m_nFlag &= ~4;
            pWork->m_Pos = GetPos();
            pWork->m_nStep = 1;
            pWork->m_nBoundStep = 0;
            pWork->m_nBoundType = 0;
            pWork->m_nBoundCount = 0;
        }
            // fall through
        case 1:
            SetCollBodyPushRank_(5);
            SetCollBodyPushPower_(6);
            RotateYRate_(pWork->m_fGoalRotY, 0.2f);
            if ((pWork->m_nFlag & 0x20) && pWork->m_nBoundType != 1) {
                // Next to a ? panel on the side of the free hand: the ball
                // changes hands.
                CQPanel *pPanel = CGimmickMan::QPanelGetNear(GetPos());
                if (pPanel != 0) {
                    float fDiff = WrapRadPi(GetRotY_ToPosXZ(pPanel->GetPos()) - GetRotY());
                    if ((fDiff < 0.0f && IsBallInLeft(this)) || (fDiff > 0.0f && !IsBallInLeft(this))) {
                        long nMot = -0x39;
                        if (!IsBallInLeft(this)) {
                            nMot = 0x39;
                        }
                        if (IsBallInLeft(this)) {
                            m_nStatusFlag &= ~4;
                        } else {
                            m_nStatusFlag |= 4;
                        }
                        AnimPlayEx(3, nMot, 0, 5.0f, 0.0f, 8);
                        pWork->m_nDir = IsHaveLeft() == 1 ? 3 : 1;
                        pWork->m_nStep = 7;
                        pWork->m_fGoalRotY = GetRotY_ToGoal();
                        FsmqRetry();
                        return;
                    }
                }
            }
            SubStateADPCmnDribble_(pWork);
            if (bInput == 1 && pWork->m_fHoldFrame >= 3.0f) {
                long nDir = GetDir4ToBattle(bcs::ROUND_PI(pWork->m_fRotY - pWork->m_fGoalRotY));
                pWork->m_nDir = nDir;
                if (pWork->m_fStick >= 0.85f && (nDir == 3 || nDir == 1)) {
                    pWork->m_nStep = 2;
                } else {
                    pWork->m_nStep = 6;
                }
            }
            break;

        case 2:
        case 4:
            // The start of a battle move.
            if (!CanbeDash()) {
                pWork->m_nFlag = (pWork->m_nFlag & ~2) | 4;
                pWork->m_nStep = 6;
                FsmqRetry();
                return;
            }
            pWork->m_nFlag &= ~4;
            if (pWork->m_nStep == 2) {
                // A side step facing the goal.
                long nMot;
                if (pWork->m_nDir == 1) {
                    pWork->m_fTurn = bcs::ROUND_PI(-fHalfPi);
                    pWork->m_fMoveRotY = bcs::ROUND_PI(pWork->m_fGoalRotY - fHalfPi);
                    nMot = 0x3A;
                    if (IsHaveLeft() == 1) {
                        nMot = -0x3C;
                    }
                } else {
                    pWork->m_fTurn = bcs::ROUND_PI(fHalfPi);
                    pWork->m_fMoveRotY = bcs::ROUND_PI(pWork->m_fGoalRotY + fHalfPi);
                    nMot = 0x3C;
                    if (IsHaveLeft() == 1) {
                        nMot = -0x3A;
                    }
                }
                if (pWork->m_fTurn > 3.1415927f) {
                    pWork->m_fTurn -= 6.2831855f;
                }
                AnimPlay(3, nMot, 0, 4.0f, 0.0f);
                pWork->m_fStartRotY = pWork->m_fGoalRotY;
                pWork->m_fFrameNum = AnimGetFrameNum(3);
                pWork->m_fSide = 0.8f;
                pWork->m_nStep = 3;
            } else {
                // A turn back to where the move started.
                long nMot;
                if (pWork->m_nDir == 3) {
                    nMot = 0x3B;
                    pWork->m_fTurn = bcs::ROUND_PI(pWork->m_fGoalRotY + fHalfPi - GetRotY());
                } else {
                    nMot = 0x3D;
                    pWork->m_fTurn = bcs::ROUND_PI(pWork->m_fGoalRotY + -fHalfPi - GetRotY());
                }
                if (pWork->m_fTurn > 3.1415927f) {
                    pWork->m_fTurn -= 6.2831855f;
                }
                AnimPlayEx(3, nMot, 0, fInter, 0.0f, 8);
                pWork->m_fStartRotY = GetRotY();
                pWork->m_fFrameNum = fInter + AnimGetFrameNum(3);
                bcs::FVEC3 diff;
                bcs::FVEC3Sub(&diff, &pWork->m_Pos, &GetPos());
                bcs::FVEC3 back = diff;
                pWork->m_fSide = 0.8f + bcs::BCS_Vec_GetLenXZ(back);
                pWork->m_fMoveRotY = bcs::ROUND_PI(pWork->m_fMoveRotY + 3.1415927f);
                pWork->m_nStep = 5;
            }
            pWork->m_fRate = 0.0f;
            pWork->m_fCurve = 1.0f;
            pWork->m_fFrame = 0.0f;
            AnimSetSpeed(3, 1.2f);
            PlayTurnSe(0);
            // fall through
        case 3:
        case 5: {
            SetCollBodyPushRank_(5);
            SetCollBodyPushPower_(6);
            float fFrameAdd = GetWholeFrameRate() * m_pAnm->GetRate();
            pWork->m_fFrame += fFrameAdd;
            // (the work's battle move has BATTLEPARAM's layout)
            if (SubBattleMove_(*reinterpret_cast<BATTLEPARAM *>(&pWork->m_nDir))) {
                pWork->m_nFlag |= 2;
                break;
            }
            if (pWork->m_fFrame >= 0.8f * pWork->m_fFrameNum) {
                if (pWork->m_nDir == 1) {
                    m_nStatusFlag &= ~4;
                } else {
                    m_nStatusFlag |= 4;
                }
                long nDir = GetDir4ToBattle(bcs::ROUND_PI(pWork->m_fRotY - pWork->m_fGoalRotY));
                if (pWork->m_nDir != nDir && (nDir == 3 || nDir == 1)) {
                    pWork->m_nDir = nDir;
                    pWork->m_nStep = 4;
                    FsmqRetry();
                    return;
                }
                if (nDir == 0 || nDir == 2) {
                    pWork->m_nDir = nDir;
                    pWork->m_nStep = 6;
                }
            }
            break;
        }

        case 7:
            // The change of hands.
            SetCollBodyPushRank_(5);
            SetCollBodyPushPower_(6);
            RotateYRate_(pWork->m_fGoalRotY, 0.2f);
            if (!IsAnimEnd(3)) {
                if (bInput == 1 && AnimGetFrame(3) >= 0.6f * AnimGetFrameNum(3) && !(pWork->m_nFlag & 4)) {
                    long nDir = GetDir4ToBattle(bcs::ROUND_PI(pWork->m_fRotY - pWork->m_fGoalRotY));
                    if (pWork->m_fStick >= 0.85f && (nDir == 1 || nDir == 3)) {
                        StartADPBattleMove(this, pWork, nDir);
                        return;
                    }
                }
                break;
            }
            // fall through
        case 6:
            AnimSetSpeed(3, 1.0f);
            pWork->m_fHoldFrame = 0.0f;
            pWork->m_fGoalRotY = GetRotY_ToGoal();
            pWork->m_nStep = 8;
            if (!(pWork->m_nFlag & 0x80)) {
                PlaySePos(0xC76, static_cast<const bcs::FVEC3 *>(0), 0, -1, 0.0f, 1.0f, 0.0f);
                pWork->m_nFlag |= 0x80;
            }
            // fall through
        case 8:
            // The dribble that follows the stick.
            SetCollBodyPushPower_(1);
            SetCollBodyPushRank_(1);
            RotateYRate_(pWork->m_fGoalRotY, 0.2f);
            if (bInput && pWork->m_fHoldFrame >= 3.0f) {
                // The motion of each direction, ball in the right and in
                // the left hand.
                const long anMot[4][2] = {
                    { 0x36, -0x36 },
                    { 0x38, 0x38 },
                    { 0x37, -0x37 },
                    { -0x38, -0x38 },
                };
                long nDir = GetDir4ToBattle(bcs::ROUND_PI(pWork->m_fRotY - pWork->m_fGoalRotY));
                if (AnimGetMotIDEx(3) != anMot[nDir][IsBallInLeft(this)]) {
                    int bBlend = 0;
                    long nNow = AnimGetMotIDEx(3);
                    switch (nNow) {
                        case -0x35:
                        case 0x35:
                            break;
                        case -0x37:
                        case -0x36:
                        case 0x36:
                        case 0x37:
                            if (pWork->m_nFlag & 8) {
                                pWork->m_nFlag &= ~4;
                            }
                            if (!(pWork->m_nFlag & 4) && pWork->m_fStick >= 0.85f && (nDir == 1 || nDir == 3)) {
                                StartADPBattleMove(this, pWork, nDir);
                                return;
                            }
                            pWork->m_nFlag &= ~4;
                            break;
                        case -0x39:
                        case -0x38:
                        case 0x38:
                        case 0x39:
                            if (pWork->m_nFlag & 8) {
                                pWork->m_nFlag &= ~4;
                            }
                            if (!(pWork->m_nFlag & 4) && pWork->m_fStick >= 0.85f && (nDir == 1 || nDir == 3) &&
                                nDir != pWork->m_nDir) {
                                StartADPBattleMove(this, pWork, nDir);
                                return;
                            }
                            if (nNow == -0x39 || nNow == 0x39) {
                                pWork->m_nFlag |= 8;
                            }
                            break;
                        default:
                            // Coming from another motion: the hand follows
                            // the side asked for.
                            AnimGetFrame(2);
                            AnimGetFrameNum(2);
                            long nPrev = AnimGetMotIDEx(2);
                            if (nPrev == 0x41 || nPrev == -0x41) {
                                if (nDir == 1) {
                                    m_nStatusFlag |= 4;
                                } else if (nDir == 3) {
                                    m_nStatusFlag &= ~4;
                                }
                            } else if (nPrev == 0x47 || nPrev == -0x47 || nPrev == 0x49 || nPrev == -0x49) {
                                if (nDir == 1) {
                                    m_nStatusFlag |= 4;
                                } else if (nDir == 3) {
                                    m_nStatusFlag &= ~4;
                                }
                            }
                            bBlend = 1;
                            break;
                    }

                    if ((nDir == 1 && IsBallInLeft(this) == 1) || (nDir == 3 && IsBallInLeft(this) == 0)) {
                        // The ball is in the hand of the side asked for:
                        // it changes hands.
                        if (!bBlend) {
                            long nMot;
                            if (nDir == 1) {
                                nMot = -0x39;
                                m_nStatusFlag &= ~4;
                            } else {
                                nMot = 0x39;
                                m_nStatusFlag |= 4;
                            }
                            AnimPlayEx(3, nMot, 0, 5.0f, 0.0f, 8);
                            pWork->m_nDir = nDir;
                            pWork->m_nStep = 7;
                            PlaySePos(0xC77, static_cast<const bcs::FVEC3 *>(0), 0, -1, 0.0f, 1.0f, 0.0f);
                            FsmqRetry();
                            return;
                        }
                        long nMot = -0x38;
                        if (IsBallInLeft(this)) {
                            nMot = 0x38;
                        }
                        AnimPlayEx(3, nMot, 1, 7.0f, AnimGetResFrameNum(nMot) * 0.5f, 8);
                        AnimSetSpeed(3, GetConditionStarMotSpeed());
                        if (nDir == 1) {
                            m_nStatusFlag &= ~4;
                        } else {
                            m_nStatusFlag |= 4;
                        }
                    } else {
                        AnimPlay(3, anMot[nDir][IsBallInLeft(this)], 1, 8.0f, 0.0f);
                        AnimSetSpeed(3, GetConditionStarMotSpeed());
                    }
                } else {
                    pWork->m_nFlag |= 8;
                    if ((nDir == 1 || nDir == 3) && !(pWork->m_nFlag & 4) && pWork->m_fStick >= 0.85f) {
                        StartADPBattleMove(this, pWork, nDir);
                        return;
                    }
                }

                // The move in the stick's direction, relative to the goal.
                pWork->m_fGoalRotY = GetRotY_ToGoal();
                RotateYRate_(pWork->m_fGoalRotY, 0.2f);
                float fStep = GetWholeFrameRate();
                float fSpeed = GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(5));
                bcs::FVEC3 move;
                bcs::BCS_VecZ_RotY(move, fSpeed * fStep, bcs::ROUND_PI(pWork->m_fRotY - pWork->m_fGoalRotY));
                bcs::BCS_Vec_RotY(move, move, pWork->m_fGoalRotY);
                // (worked out and not used)
                bcs::FVEC3 to;
                bcs::FVEC3Add(&to, &GetPos(), &move);
                AddPos(move);
                pWork->m_Pos = GetPos();
                pWork->m_nDir = nDir;
            }
            break;
    }

    if (pWork->m_nFlag & 2) {
        // A dash asked for: towards the stick, turned by 20 degrees at
        // most.
        if (CanbeDash()) {
            float fMax = 0.34906584f;
            float fDiff = WrapRadPi(pWork->m_fRotY - GetRotY());
            if (__fabsf(fDiff) >= fMax) {
                if (fDiff < 0.0f) {
                    fDiff = -fMax;
                } else {
                    fDiff = fMax;
                }
            }
            SetRotY(bcs::ROUND_PI(fDiff + GetRotY()));
            if (pWork->m_nDir == 1) {
                m_nStatusFlag &= ~4;
            } else {
                m_nStatusFlag |= 4;
            }
            if (MethdDash()) {
                return;
            }
        }
        pWork->m_nFlag = (pWork->m_nFlag & ~2) | 4;
        pWork->m_nStep = 6;
    }
    pWork->m_nFlag &= ~1;
}

void msw::CChrCoreBasket::DtorStateAutoDribblePosture()
{
}

void msw::CChrCoreBasket::CtorStateDribbleMiss()
{
    STATE_WORK_DRIBBLE_MISS *pWork = FsmqGetWork<STATE_WORK_DRIBBLE_MISS>();

    pWork->m_fFrame = 0.0f;
    pWork->m_fFrameNum = AnimGetResFrameNum(0x51);
    pWork->m_fSpeed = 0.6f / pWork->m_fFrameNum;
    pWork->m_fWait = 0.0f;
    SetCollBodyPushRank_(1);
    DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 10, 1, 0, 0);
}

void msw::CChrCoreBasket::LoopStateDribbleMiss()
{
    STATE_WORK_DRIBBLE_MISS *pWork = FsmqGetWork<STATE_WORK_DRIBBLE_MISS>();

    RotateYRate_(pWork->m_fRotY, 0.2f);
    float fStep = m_fFrameStep;
    pWork->m_fFrame += fStep;

    // Staggers back, away from where the ball went, 0.6 in all.
    bcs::FVEC3 move;
    bcs::BCS_VecZ_RotY(move, 1.0f, bcs::ROUND_PI(3.1415927f + pWork->m_fRotY));
    move.x *= pWork->m_fSpeed;
    move.z *= pWork->m_fSpeed;
    while (pWork->m_fFrame >= 1.0f) {
        pWork->m_fFrame -= 1.0f;
        pWork->m_fFrameNum -= 1.0f;
        if (pWork->m_fFrameNum > 0.0f) {
            AddPos(move);
        } else {
            pWork->m_fFrameNum = 0.0f;
        }
    }

    if (AnimGetMotID(3, 0) == 0x51) {
        if (IsAnimEnd(3)) {
            pWork->m_fWait -= fStep;
            if (pWork->m_fWait <= 0.0f) {
                if (m_pMdl->IsAnmFlipPlay(3, 0)) {
                    m_nStatusFlag |= 4;
                } else {
                    m_nStatusFlag &= ~4;
                }
                AnimSetSpeed(3, 1.0f);
                Util_PushBackOnBallerBattleEnd_(IsBattle(), 0, GetRotY());
                FsmqDoChain();
            }
        }
    } else {
        long nMot = -0x51;
        if (m_pMdl->IsAnmFlipPlay(3, 0)) {
            nMot = 0x51;
        }
        AnimPlay(3, nMot, 0, 4.0f, 0.0f);
        AnimSetSpeed(3, 0.42f);
    }
}

void msw::CChrCoreBasket::DtorStateDribbleMiss()
{
}
