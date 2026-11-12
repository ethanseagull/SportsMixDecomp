// msw::CChrCoreBasket's catch states: the catch of a ball coming at the
// character (with the turn towards the goal or an opponent), the catch of a
// pass (running to the point the ball will reach, jumping for a high one)
// and the catch in a jump.
#include "game/msw/CChrCoreBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CNpcGhost.hpp"
#include "nw4r/math/types.h"

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it (see CBallManBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

// The pass catch state's work (names guessed).
struct msw::CChrCoreBasket::STATE_WORK_PASS_CATCH {
    long m_nStep;            // 0x00
    long m_nPassChr;         // 0x04: the index of the passer
    // 0x08: bit 1 the pass is a combination pass (meaning guessed), bit 4
    // set once the character stands at the catch point.
    unsigned long m_nFlag;
    unsigned long m_hBall;   // 0x0C: the task id of the ball
    bcs::FVEC3 m_Pos;        // 0x10: where the ball will be caught
    float m_fJumpWait;       // 0x1C: frames before the jump for a high pass
    float m_fUnk20;          // 0x20
    float m_fWait;           // 0x24: frames to stand before running again
    long m_nLoopCount;       // 0x28: loops of the waiting motion left
    unsigned char m_Unk2C;   // 0x2C
    unsigned char m_bNoJump; // 0x2D
    unsigned char m_bRun;    // 0x2E: the run motion is playing
    unsigned char m_bJump;   // 0x2F: the pass is caught in a jump
};

namespace {

    // The catch state's work (names guessed).
    struct STATE_WORK_CATCH {
        long m_nStep;          // 0x00
        // 0x04: bit 0 the turn of the catch is to the left, bit 2 the
        // opponent is to that side (they pick the motion).
        unsigned long m_nFlag;
        float m_fRotY;         // 0x08: the yaw to turn to (the goal)
    };

    // The jump catch state's work (names guessed).
    struct STATE_WORK_JUMP_CATCH {
        long m_nStep;          // 0x00
        float m_fRotY;         // 0x04: the yaw to turn to
        float m_fUnk08;        // 0x08
        float m_fFrame;        // 0x0C: frames of the rise so far
        float m_fStartRotY;    // 0x10
    };

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

    // (guessed helper) Whether the status flags show a jump.
    inline int IsJump(const msw::CChrCore *pChr)
    {
        return (pChr->GetStatusFlag() & 0x1C00) != 0;
    }

    // In the original bcs::FVEC3 derives from nw4r::math::VEC3 (see
    // CChrCoreBasketNinja.cpp); its operators are reached through a cast.
    typedef const nw4r::math::VEC3 &VEC3_CREF;

    // (guessed helper) The height of the character against the jump block
    // height, clamped to [0, 1].
    inline float Clamp01(float f)
    {
        return f > 1.0f ? 1.0f : (f < 0.0f ? 0.0f : f);
    }
}

void msw::CChrCoreBasket::CtorStateCatch()
{
    m_UnkEC |= 0x10;
    SetCollBodyPushPower_(5);
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(5), 8.0f);
    SetCollBodyPushRank_(1);
    STATE_WORK_CATCH *pWork = FsmqGetWork<STATE_WORK_CATCH>();
    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(3);
    if (!(pWork->m_nFlag & 2)) {
        ReattachBall(5, 0.0f, 0);
    }
    pWork->m_nStep = 0;
}

void msw::CChrCoreBasket::LoopStateCatch()
{
    STATE_WORK_CATCH *pWork = FsmqGetWork<STATE_WORK_CATCH>();

    if (IsJump(this)) {
        if (Util_DoJumpMove(m_fFrameStep) || GetPos().y < 0.05f) {
            m_nStatusFlag &= ~0x1C00;
            m_nStatusFlag &= ~0x80;
        }
    } else {
        m_nStatusFlag &= ~0x80;
    }

    switch (pWork->m_nStep) {
        case 0: {
            AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) ? -0x45 : 0x45, 1, 8.0f, 0.0f);
            AnimSetSpeed(3, 1.0f);
            pWork->m_nStep = 1;
            break;
        }
        case 2: {
            // Turn to the goal, to the side of the nearest opponent in
            // front.
            pWork->m_fRotY = GetRotY_ToPosXZ(GetTargetGoalPos());
            float fMin = 9999.9f;
            CChrCore *pNear = 0;
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr == 0 || pChr->IsDisable() || pChr->GetTeamNo() == GetTeamNo()) {
                    continue;
                }
                float fDiff = WrapRadPi(GetRotY_ToPosXZ(static_cast<const CChrCore *>(pChr)->GetPos()) -
                                        pWork->m_fRotY);
                bcs::FVEC3 vDiff = reinterpret_cast<VEC3_CREF>(GetPos()) -
                                   reinterpret_cast<VEC3_CREF>(static_cast<const CChrCore *>(pChr)->GetPos());
                float fDist = bcs::BCS_Vec_GetLenXZ(vDiff);
                if (bcs::BCS_Abs(fDiff) <= 1.5707964f && fDist < fMin) {
                    fMin = fDist;
                    pNear = pChr;
                }
            }
            pWork->m_nFlag &= ~5;
            if (m_nStatusFlag & 4) {
                pWork->m_nFlag |= 1;
            } else {
                pWork->m_nFlag |= 4;
            }
            if (pNear != 0) {
                float fDiff = WrapRadPi(GetRotY_ToPosXZ(static_cast<const CChrCore *>(pNear)->GetPos()) -
                                        pWork->m_fRotY);
                if (fDiff > 0.0f) {
                    pWork->m_nFlag |= 4;
                } else {
                    pWork->m_nFlag &= ~4;
                }
            }
            pWork->m_nStep = 3;
        }
            // fall through
        case 3:
        case 4: {
            RotateYRate_(pWork->m_fRotY, 0.2f);
            // The catch motions: for the turn's two steps, to the right or
            // the left, with the opponent on the other side or the same
            // (negative: played mirrored).
            const long nMotTbl[2][2][2] = {
                { { -70, -72 }, { 73, 70 } },
                { { -71, -73 }, { 74, 71 } },
            };
            long nIdx = pWork->m_nStep - 3;
            long nMot = nMotTbl[nIdx][pWork->m_nFlag & 1][(pWork->m_nFlag & 4) >> 2];
            if (nMot != AnimGetMotIDEx(3)) {
                int bLast = nIdx == 1;
                AnimSetSpeed(3, 1.0f);
                AnimPlay(3, nMot, bLast, 8.0f, 0.0f);
                if (bLast) {
                    SetRotY(pWork->m_fRotY);
                    pWork->m_nStep = 5;
                }
            } else if (IsAnimEnd(3)) {
                pWork->m_nStep++;
                FsmqRetry();
            }
            break;
        }
        case 5:
            if (!IsBattle()) {
                pWork->m_nStep = 0;
            }
            break;
    }
}

void msw::CChrCoreBasket::DtorStateCatch()
{
    m_nStatusFlag &= ~0x80;
    m_nBasketFlag &= ~0x80;
}

int msw::CChrCoreBasket::SubStatePassCatchEnd(STATE_WORK_PASS_CATCH *pWork)
{
    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
    if (pBall == 0) {
        return 1;
    }

    if (pBall->IsAttach()) {
        // Over once the passer has stopped throwing.
        long nChr = pWork->m_nPassChr;
        if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum()) {
            CChrCore *pChr = CChrMan::GetChrCore(nChr);
            if (pChr != 0) {
                long nState = pChr->FsmqGetState();
                if (nState == 0x15 || nState == 0x16) {
                    return 0;
                }
                return 1;
            }
        } else if (CGame::g_GameWork.GetStage() == 6) {
            // On the ghost stage the ghost passes too.
            CNpcGhost *pGhost =
                static_cast<CNpcGhost *>(CChrMan::NpcSearch(CGameBasket::GetTeamGhostType(GetTeamNo()), 0));
            if (pGhost != 0 && pGhost->FsmqGetState() == 10) {
                return 0;
            }
            return 1;
        }
    } else if (!pBall->IsPassMode()) {
        return 1;
    }
    return 0;
}

void msw::CChrCoreBasket::CtorStatePassCatch()
{
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(1);
    STATE_WORK_PASS_CATCH *pWork = FsmqGetWork<STATE_WORK_PASS_CATCH>();
    pWork->m_fWait = 0.0f;
    if (pWork->m_nStep == 0) {
        m_UnkEC |= 0x10;
        m_nStatusFlag &= ~0x1F000000;
        SetMoveMotType(0);
        pWork->m_bRun = 0;
    }
}

void msw::CChrCoreBasket::LoopStatePassCatch()
{
    // (name from the symbol) Not read: the compiler folds it into the
    // literal 1.0f of the jump (a guess at where the original used it).
    static const float JUMP_CATCH_HEIGHT_BASE = 1.0f;

    STATE_WORK_PASS_CATCH *pWork = FsmqGetWork<STATE_WORK_PASS_CATCH>();
    int bFirst = 0;
    int bEnd = SubStatePassCatchEnd(pWork);

    switch (pWork->m_nStep) {
        case 0:
            if (CheckPassCatchEnd_()) {
                m_nStatusFlag &= ~0x1F000000;
                FsmqDoChain();
                break;
            }
            bFirst = 1;
            pWork->m_nStep = 1;
            // fall through
        case 1: {
            int bJump = 0;
            if (!pWork->m_bNoJump && pWork->m_bJump) {
                if (pWork->m_fJumpWait > 0.0f) {
                    pWork->m_fJumpWait -= m_fTimeScale * m_fFrameStep;
                }
                if (pWork->m_fJumpWait <= 0.0f) {
                    bJump = 1;
                }
            }

            if (!bJump) {
                if (CheckPassCatchEnd_() || bEnd) {
                    // The catch is over: on to the next state.
                    long nChr = pWork->m_nPassChr;
                    int bValid = 0;
                    if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum()) {
                        bValid = 1;
                    }
                    CChrCore *pPasser = bValid ? CChrMan::GetChrCore(nChr) : 0;
                    int bCombination = (pWork->m_nFlag >> 1) & 1;
                    FsmqDoChain();
                    int bPad = 0;
                    if (pPasser != 0 && pPasser->DriverGetType() == 1) {
                        bPad = 1;
                    }
                    if (FsmqGetState() == 0x20 && bCombination && bPad) {
                        // (the leader is looked up and not used: stripped code)
                        CChrMan::GetTeamLeader(GetTeamNo());
                    }
                    return;
                }

                int bMotEnd = 0;
                long nMot = AnimGetMotID(3, 0);
                if (nMot == 0x77 || nMot == 0x78) {
                    if (IsAnimLoop(3)) {
                        if (--pWork->m_nLoopCount <= 0) {
                            pWork->m_nLoopCount = 0;
                            bMotEnd = 1;
                        }
                    }
                } else {
                    bMotEnd = 1;
                }

                pWork->m_fWait -= m_fFrameStep;
                if (pWork->m_fWait < 0.0f) {
                    pWork->m_fWait = 0.0f;
                }

                if (IsInnerXZ(pWork->m_Pos, 0.1f)) {
                    // At the catch point: wait, facing the ball.
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
                    if (pBall != 0) {
                        RotateYToPosRate_(pBall->GetPos(), 0.125f);
                    }
                    if ((pWork->m_bRun || bFirst) && bMotEnd) {
                        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
                        SetMoveMotType(0);
                        AnimPlay(3, Util_GetIdleMotID(), 1, 8.0f, 0.0f);
                        pWork->m_bRun = 0;
                    }
                    if (AnimGetMotID(3, 0) == 0x78) {
                        long nWaitMot = m_pMdl->IsAnmFlipPlay(3, 0) ? -0x77 : 0x77;
                        AnimPlay(3, nWaitMot, 1, 4.0f, AnimGetFrame(3));
                    }
                    if (!(pWork->m_nFlag & 0x10)) {
                        pWork->m_fWait = 15.0f;
                        pWork->m_nFlag |= 0x10;
                    }
                } else if (pWork->m_fWait <= 0.0f) {
                    // Run to the catch point, looking at the ball when it
                    // is near.
                    CBall *pBall = CBallMan::GetBallPtr(0);
                    if (pBall != 0) {
                        bcs::FVEC3 vDiff = reinterpret_cast<VEC3_CREF>(pBall->GetPos()) -
                                           reinterpret_cast<VEC3_CREF>(GetPos());
                        if (bcs::BCS_Vec_GetLenXZ(vDiff) > 2.5f) {
                            RotateYToPosRate_(pWork->m_Pos, 0.125f);
                        } else {
                            float fRot = GetRotY_ToPosXZ(pWork->m_Pos);
                            float fDiff = WrapRadPi(GetRotY_ToPosXZ(pBall->GetPos()) - fRot);
                            float fMax = 1.5707964f;
                            if (bcs::BCS_Abs(fDiff) > fMax) {
                                if (fDiff < 0.0) {
                                    fMax = -fMax;
                                }
                                fDiff = fMax;
                            }
                            RotateYRate_(fDiff + fRot, 0.2f);
                        }
                    }
                    if ((!pWork->m_bRun || bFirst) && bMotEnd) {
                        SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(2), 8.0f);
                        SetMoveMotType(2);
                        AnimPlay(3, Util_GetRunMotID(), 1, 8.0f, 0.0f);
                        pWork->m_bRun = 1;
                    }
                    if (AnimGetMotID(3, 0) == 0x77) {
                        long nRunMot = m_pMdl->IsAnmFlipPlay(3, 0) ? -0x78 : 0x78;
                        AnimPlay(3, nRunMot, 1, 4.0f, AnimGetFrame(3));
                    }
                    Util_DoHomingMoveXZ(pWork->m_Pos,
                                        m_fFrameStep * (GetMoveSpeedKind(static_cast<MOVE_SPEED_KIND>(0)) * m_fTimeScale));
                    pWork->m_nFlag &= ~0x10;
                } else {
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
                    if (pBall != 0) {
                        RotateYToPosRate_(pBall->GetPos(), 0.125f);
                    }
                }
                break;
            }

            // A high pass: jump for it.
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            SetCollBodyPushRank_(4);
            m_UnkEC &= ~0x10;
            Util_SetJumpParam(*static_cast<const CC_DATA::CHR_PARAM::JUMP_PARAM *>(GetCcParam_()),
                              JUMP_CATCH_HEIGHT_BASE);
            long nJumpMot = 8;
            if (m_nStatusFlag & 3) {
                nJumpMot = 8; // (the same motion either way)
            }
            AnimPlay(3, nJumpMot, 0, 4.0f, 0.0f);
            m_nStatusFlag |= 0x400;
            SetMoveMotType(0);
            pWork->m_nStep = 2;
        }
            // fall through
        case 2:
            if (Util_DoJumpMove(1.0f) == 1) {
                SetCollBodyPushRank_(1);
                m_UnkEC |= 0x10;
                AnimPlay(3, Util_GetLandMotID(-1), 0, -4.0f, 0.0f);
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                pWork->m_nStep = 3;
            }
            if ((m_nStatusFlag & 0x400) && !(m_nStatusFlag & 1)) {
                CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(pWork->m_hBall));
                if (pBall != 0) {
                    RotateYToPosRate_(pBall->GetPos(), 0.125f);
                }
            } else if (m_nStatusFlag & 1) {
                RotateYToPosRate_(GetTargetGoalPos(), 0.125f);
            }
            break;
        default:
            Util_DoJumpMoveLand(1.0f);
            if (IsAnimEnd(3)) {
                long nNext = FsmqGetNextState();
                m_nStatusFlag &= ~0xC00;
                if (nNext == 0 && (m_nStatusFlag & 1)) {
                    Util_PushBackCatch(1);
                }
                FsmqNext();
                return;
            }
            break;
    }

    if (m_nStatusFlag & 0x400) {
        SetCollBodyPushPower_(1);
    } else {
        SetCollBodyPushPower_(5);
    }
}

void msw::CChrCoreBasket::DtorStatePassCatch()
{
}

void msw::CChrCoreBasket::CtorStateJumpCatch()
{
    AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) == 0 ? 10 : -10, 0, 4.0f, 0.0f);
    AnimSetSpeed(3, 1.0f);
    m_nStatusFlag |= 0x400;

    // The rise: slower the higher the character already is.
    CHR_CORE_BASKET::JUMP_BLOCK_PARAM param;
    GetJumpBlockParam_(3, param);
    float fRange = 0.3f;
    m_fJumpVelY = (0.5f - Clamp01(GetPos().y / param.m_fHeight) * fRange) / 17.0f;

    STATE_WORK_JUMP_CATCH *pWork = FsmqGetWork<STATE_WORK_JUMP_CATCH>();
    pWork->m_fStartRotY = GetRotY();
}

void msw::CChrCoreBasket::LoopStateJumpCatch()
{
    STATE_WORK_JUMP_CATCH *pWork = FsmqGetWork<STATE_WORK_JUMP_CATCH>();
    float fStep = GetWholeFrameRate();
    // (read and not used: stripped code)
    GetCcParam2_();

    switch (pWork->m_nStep) {
        case 0: {
            // Rise for 17 frames.
            bcs::FVEC3 pos = GetPos();
            pos.y += m_fJumpVelY * fStep;
            SetPos(pos);
            RotateYRate_(pWork->m_fRotY, 0.2f);
            pWork->m_fFrame += fStep;
            if (pWork->m_fFrame >= 17.0f) {
                pWork->m_nStep = 1;
            }
            break;
        }
        case 1: {
            // Then fall from the height reached.
            CHR_CORE_BASKET::JUMP_BLOCK_PARAM param;
            GetJumpBlockParam_(3, param);
            float fRange = 10.0f;
            float fFrame = fRange * Clamp01(GetPos().y / param.m_fHeight) + 15.0f;
            // (the horizontal speed of the jump is kept)
            bcs::FVEC3 vel = GetJumpVel();
            const bcs::FVEC3 &rPos = GetPos();
            Util_SetJumpParam(fFrame, GetPos().y, 0.0f, rPos.y);
            m_fJumpVelX = vel.x;
            m_fJumpVelZ = vel.z;
            pWork->m_nStep = 2;
        }
            // fall through
        case 2:
            m_fJumpVelX *= 0.98f;
            m_fJumpVelZ *= 0.98f;
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (Util_DoJumpMove(fStep)) {
                m_UnkEC |= 0x10;
                m_nStatusFlag = (m_nStatusFlag & ~0x400) | 0x800;
                AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) == 0 ? 11 : -11, 0, 4.0f, 0.0f);
                PlayLandSe(0);
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                pWork->m_nStep = 4;
            }
            break;
        case 3:
            m_fJumpVelX *= 0.98f;
            m_fJumpVelZ *= 0.98f;
            if (Util_DoJumpMove(fStep)) {
                m_UnkEC |= 0x10;
                m_nStatusFlag = (m_nStatusFlag & ~0x400) | 0x800;
                AnimPlay(3, m_pMdl->IsAnmFlipPlay(3, 0) == 0 ? 11 : -11, 0, 4.0f, 0.0f);
                PlayLandSe(0);
                Util_SetEffect_(static_cast<SET_EFFECT_TYPE_BASKET>(0));
                pWork->m_nStep = 4;
            }
            break;
        case 4:
            if (IsAnimEnd(3) == 1) {
                FsmqDoChain();
            }
            break;
    }
}

void msw::CChrCoreBasket::DtorStateJumpCatch()
{
    m_nStatusFlag &= ~0x1C00;
}
