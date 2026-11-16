// msw::CChrCoreVolley's serve state (the toss up, the over-hand, under-hand
// and jump serves and the shot), a file of its own after
// game/volley/unit_3B628.cpp: its literals are read through a pool base,
// the receive part's each with its own lis. The file name is not known.
#include "game/msw/CChrCoreVolley.hpp"

#include <math.h>

#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameGuideMan.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CcEndureSmokeEffect.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

namespace {

    // (guessed helpers) An angle brought into [0, 2 pi), and into
    // (-pi, pi] after that; an angle brought into [-pi, pi].
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

    inline float WrapRadPI(float rad)
    {
        rad = WrapRad(rad);
        if (rad > 3.1415927f) {
            rad -= 6.2831855f;
        }
        return rad;
    }

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

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    // (guessed macro) x brought into [lo, hi]; the bounds and x are read
    // again where they are used, as the original does.
#define CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))

    // The sliding state's work (names guessed).
    struct STATE_WORK_SLIDING {
        long m_nStep;       // 0x00
        unsigned char m_Unk04[0x08 - 0x04];
        float m_fRotY;      // 0x08: the direction of the slide
        float m_fRotYTrg;   // 0x0C: the direction the stick asks for
        float m_fSpeedRate; // 0x10
        int m_bAccel;       // 0x14: the stick is held this frame
        bcs::FVEC3 m_Dir;   // 0x18: the direction a ball is sent in (only x and z read)
    };
}

// ---------------------------------------------------------------------------
// The serve state. In the original this is a file of its own (its literals
// are pooled); see the top of this file.

// (inline helpers of the serve) The ball is hit: the serve guide counts a
// success, and the ball goes off as a serve of kind nType.
static inline void ServeShot(msw::CChrCoreVolley *pChr, msw::CChrCoreVolley::STATE_WORK_SERVE *pWork, long nType)
{
    if ((pWork->m_nFlag & 2) && (pWork->m_nFlag & 0x100) && (pWork->m_nFlag & 0x200) &&
        msw::CGameGuideMan::IsDispEnable()) {
        msw::CGameGuideMan::AddGuideSuccessCount(0x10, pChr->GetPadNo());
    }
    long nShotType = pWork->m_nShotType;
    int bInput = (pWork->m_nFlag >> 1) & 1;
    msw::CBall *pBall = pChr->GetAttachBall();
    pChr->DetachBall();
    if (pBall != 0) {
        if (bInput) {
            nShotType = 1;
            if (pBall->IsVolleyTimingSpike()) {
                nShotType = 2;
            }
        }
        pChr->Util_ServeBall_(pBall, nType, nShotType);
    }
    pWork->m_nFlag |= 1;
}

// One frame of the jump serve's move to its jump point.
static inline void ServeJumpMove(msw::CChrCoreVolley *pChr, msw::CChrCoreVolley::STATE_WORK_SERVE *pWork)
{
    pChr->RotateYParam_(pWork->m_RotLerp);
    if (pWork->m_fMoveFrame < pWork->m_MoveParam.m_fFrameMax) {
        pWork->m_fMoveFrame += pChr->m_fFrameStep * pChr->m_fTimeScale;
        pWork->m_DirMove.m_fRotY = pChr->GetRotY_ToPosXZ(pWork->m_JumpPos);
        pChr->Util_DoDirMove(pWork->m_MoveParam, pWork->m_fMoveFrame, pWork->m_DirMove, 0);
    }
}

void msw::CChrCoreVolley::CtorStateServe()
{
    STATE_WORK_SERVE *pWork = FsmqGetWork<STATE_WORK_SERVE>();

    pWork->m_pBall = GetAttachBall();
    m_nJumpStep = 3;
    pWork->m_fTargetRotY = GetSpikeTargetPosRot_(0);
    ClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    switch (pWork->m_nType) {
        case 1:
            pWork->m_fTossFrame = GetBallShotTrgFrame_(0x47, GetCcParam_()->m_Unk130);
            pWork->m_fShotFrame = GetBallShotTrgFrame_(0x49, 20.0f);
            break;
        case 2:
            pWork->m_fTossFrame = GetBallShotTrgFrame_(0x4B, GetCcParam_()->m_Unk170);
            pWork->m_fShotFrame = GetBallShotTrgFrame_(0x4C, 4.0f);
            break;
        default:
            pWork->m_fTossFrame = GetBallShotTrgFrame_(0x43, GetCcParam_()->m_Unk104);
            pWork->m_fShotFrame = GetBallShotTrgFrame_(0x45, 20.0f);
            break;
    }
    pWork->m_fShotFrame = Max(1.0f, pWork->m_fShotFrame);
    pWork->m_nShotType = 1;
    if (CGameGuideMan::IsGuideDisp(0x10, m_nPlayerNo)) {
        pWork->m_nFlag |= 0x100;
    }
}

void msw::CChrCoreVolley::LoopStateServe()
{
    STATE_WORK_SERVE *pWork = FsmqGetWork<STATE_WORK_SERVE>();
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            // The toss of an over or under hand serve.
            SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
            SetCollBodyPushPower_(5);
            m_nStatusFlag &= ~0x1F000000;
            SetMoveMotType(4);
            if (pWork->m_nType == 1) {
                AnimPlay(3, 0x47, 0, 8.0f, 0.0f);
                SetTargetMarkerEnable_(1, GetPos(), 0);
            } else {
                AnimPlay(3, 0x43, 0, 8.0f, 0.0f);
                SetTargetMarkerEnable_(0, GetPos(), 0);
            }
            CalcRotParam_(pWork->m_RotLerp, pWork->m_fTargetRotY, pWork->m_fTossFrame);
            pWork->m_nStep = 1;
            // fall through
        case 1:
            RotateYParam_(pWork->m_RotLerp);
            if ((m_nStatusFlag & 1) && AnimGetFrame(3) >= pWork->m_fTossFrame) {
                SubStateServeBallTossUp_(pWork->m_nType, pWork->m_TargetPos);
                pWork->m_nStep = 2;
            }
            break;
        case 2:
            // The ball is up: hit it when the button is pressed.
            if (!(m_nStatusFlag & 1)) {
                CBall *pBall = pWork->m_pBall;
                if (pBall != 0 && pBall->GetVelocity().y < 0.0f) {
                    bcs::FVEC3 ofs = m_ServeBallPosOver;
                    if (pWork->m_nType == 1) {
                        ofs = m_ServeBallPosUnder;
                    }
                    ofs.y -= pWork->m_pBall->GetRadius();
                    SetBallCatchEnableRoot_(0.8f, 2.0f * pWork->m_pBall->GetRadius(), ofs);
                }
            }
            if (DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8A), 0, 0, 0, 0)) {
                pWork->m_nFlag |= 2;
                pWork->m_nCourse = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8B), 0, 0, 0, 0);
                if (pWork->m_nType == 1) {
                    SubStateServeStartUnderHand_(pWork);
                } else {
                    SubStateServeStartOverHand_(pWork);
                }
            }
            if (CGame::g_GameWork.GetGameType() == 1 && (CGame::g_GameWork.m_nFlag18 & 1)) {
                bEnd = 1;
            }
            break;
        case 3: {
            if (!(m_nStatusFlag & 1)) {
                CBall *pBall = pWork->m_pBall;
                if (pBall != 0 && pBall->GetVelocity().y < 0.0f) {
                    bcs::FVEC3 ofs(0.0f, -1.0f * pBall->GetRadius(), 0.0f);
                    SetBallCatchEnableVNode_(0.8f, 2.0f * pWork->m_pBall->GetRadius(), ofs, 4);
                }
            }
            long nCourse = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8B), 0, 0, 0, 0);
            if (nCourse == -1) {
                pWork->m_fWait -= m_fFrameStep;
                if (pWork->m_fWait <= 0.0f) {
                    nCourse = 4;
                }
            }
            if (nCourse != -1) {
                switch (nCourse) {
                    case 1:
                    case 2:
                    case 4:
                        SubStateServeStartOverHand_(pWork);
                        break;
                    case 3:
                        SubStateServeStartUnderHand_(pWork);
                        break;
                }
            }
            if (CGame::g_GameWork.GetGameType() == 1 && (CGame::g_GameWork.m_nFlag18 & 1)) {
                bEnd = 1;
            }
            break;
        }
        case 4:
            if (!(m_nStatusFlag & 1)) {
                CBall *pBall = pWork->m_pBall;
                bcs::FVEC3 ofs(0.0f, -2.0f * pBall->GetRadius(), 0.0f);
                SetBallCatchEnableVNode_(0.8f, 2.0f * pWork->m_pBall->GetRadius(), ofs, 4);
                break;
            }
            // fall through
        case 5:
            SetBallCatchDisable_();
            if (pWork->m_nType == 1) {
                if (CGame::IsVolleyServeClockOver()) {
                    SubStateServeStartUnderHand_(pWork);
                } else {
                    AnimPlay(3, 0x48, 0, 8.0f, 0.0f);
                    pWork->m_fWait = 8.0f;
                    SetTargetMarkerDisable_();
                    pWork->m_nStep = 17;
                }
                break;
            }
            AnimPlay(3, 0x44, 0, 8.0f, 0.0f);
            pWork->m_nStep = 6;
            pWork->m_fWait = 8.0f;
            // fall through
        case 6: {
            long nInput = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8A), 0, 0, 0, 0);
            int bClockOver = 0;
            if (nInput == 0) {
                bClockOver = CGame::IsVolleyServeClockOver();
            }
            if (nInput != 0 || bClockOver) {
                if (!bClockOver) {
                    pWork->m_nFlag |= 2;
                    pWork->m_nCourse = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8B), 0, 0, 0, 0);
                }
                if (pWork->m_nType == 1) {
                    SubStateServeStartUnderHand_(pWork);
                } else {
                    SubStateServeStartOverHand_(pWork);
                }
            } else {
                pWork->m_fWait -= m_fFrameStep * m_fTimeScale;
                if (pWork->m_fWait <= 0.0f) {
                    SetTargetMarkerDisable_();
                    CBall *pBall = GetAttachBall();
                    if (pBall != 0) {
                        bcs::FVEC3 pos = pBall->GetPos();
                        PlaySePos(0xC24, &pos, 0, -1, 0.0f, 1.0f, 0.0f);
                    }
                    pWork->m_nStep = 18;
                }
            }
            break;
        }
        case 7: {
            long nCourse = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8B), 0, 0, 0, 0);
            if (nCourse == -1) {
                pWork->m_fWait -= m_fFrameStep;
                if (pWork->m_fWait <= 0.0f) {
                    nCourse = 4;
                }
            }
            if (nCourse != -1) {
                // (a course of 4, the time being up, starts nothing here)
                switch (nCourse) {
                    case 0:
                    case 1:
                    case 2:
                        SubStateServeStartOverHand_(pWork);
                        break;
                    case 3:
                        SubStateServeStartUnderHand_(pWork);
                        break;
                }
            }
            break;
        }
        case 8:
            // The over hand hit.
            if (pWork->m_nCourse == -1) {
                pWork->m_nCourse = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8B), 0, 0, 0, 0);
            }
            Util_DoJumpMoveEX(0);
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                if (m_nStatusFlag & 1) {
                    ServeShot(this, pWork, 0);
                }
                if (IsAnimEnd(3) || AnimGetFrame(3) >= 4.0f + pWork->m_fShotFrame) {
                    SetBallCatchDisable_();
                    pWork->m_nStep = 9;
                }
            }
            if (!(pWork->m_nFlag & 1) && !(m_nStatusFlag & 1)) {
                float fFrame = AnimGetFrame(3);
                if (fFrame >= pWork->m_fShotFrame - 4.0f && fFrame < 4.0f + pWork->m_fShotFrame) {
                    bcs::FVEC3 ofs(0.0f, -0.2f, 0.0f);
                    SetBallCatchEnableVNode_(0.8f, ofs, 4);
                }
            }
            break;
        case 9:
            if (Util_DoJumpMoveEX(0) >= 2 && IsAnimEnd(3)) {
                PlayLandSe(0);
                AnimPlay(3, 0x46, 0, 2.0f, 0.0f);
                pWork->m_nStep = 18;
            }
            break;
        case 10:
            // The under hand hit.
            if (pWork->m_nCourse == -1) {
                pWork->m_nCourse = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8B), 0, 0, 0, 0);
            }
            Util_DoJumpMoveEX(0);
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
                if (m_nStatusFlag & 1) {
                    ServeShot(this, pWork, 1);
                }
                if (IsAnimEnd(3) || AnimGetFrame(3) >= 4.0f + pWork->m_fShotFrame) {
                    SetBallCatchDisable_();
                    pWork->m_nStep = 11;
                }
            }
            if (!(pWork->m_nFlag & 1) && !(m_nStatusFlag & 1)) {
                float fFrame = AnimGetFrame(3);
                if (fFrame >= pWork->m_fShotFrame - 4.0f && fFrame < 4.0f + pWork->m_fShotFrame) {
                    bcs::FVEC3 ofs(m_ServeHitPosUnder.x, 0.3f, m_ServeHitPosUnder.z);
                    SetBallCatchEnableRoot_(0.8f, m_ServeHitPosUnder.y, ofs);
                }
            }
            break;
        case 11:
            if (Util_DoJumpMoveEX(0) >= 2 && IsAnimEnd(3)) {
                PlayLandSe(0);
                AnimPlay(3, 0x4A, 0, 2.0f, 0.0f);
                pWork->m_nStep = 18;
            }
            break;
        case 12:
            // The jump serve: the run up and the toss.
            SetJumpServeMode_(1);
            SubStateServeStartJump_(pWork);
            PlaySePos(-1, (const bcs::FVEC3 *)0, 0, -1, 0.0f, 1.0f, 0.0f);
            // fall through
        case 13:
            if (Util_DoJumpMoveEX(0) == 1) {
                ServeJumpMove(this, pWork);
            }
            if ((m_nStatusFlag & 1) && (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fTossFrame)) {
                SubStateServeBallTossUp_(2, pWork->m_TargetPos);
                pWork->m_nStep = 14;
            }
            break;
        case 14: {
            // The ball is up: the jump starts with the button (or by itself
            // when the time is up).
            int bTimeUp = 0;
            if (!(m_nStatusFlag & 1)) {
                CBall *pBall = GetTargetBall_();
                if (pBall != 0 && pBall->GetVelocity().y < 0.0f) {
                    bcs::FVEC3 ofs(0.0f, -2.0f * pWork->m_pBall->GetRadius(), 0.0f);
                    SetBallCatchEnableVNode_(0.8f, 2.0f * pWork->m_pBall->GetRadius(), ofs, 4);
                }
            }
            if (pWork->m_fWait > 0.0f) {
                pWork->m_fWait -= m_fFrameStep * m_fTimeScale;
                if (pWork->m_fWait <= 0.0f) {
                    bTimeUp = 1;
                }
            }
            long nInput = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x8A), 0, 0, 0, 0);
            if (nInput != 0 || (m_nStatusFlag & 1) || bTimeUp) {
                if (nInput != 0) {
                    pWork->m_nFlag |= 2;
                } else {
                    pWork->m_nShotType = 0;
                }
                SubStateServeStartJumpShot_(pWork);
            }
            int nJump = Util_DoJumpMoveEX(0);
            if (nJump == 1) {
                ServeJumpMove(this, pWork);
            }
            if (nJump >= 2) {
                SetServeMode_(0);
                PlayLandSe(0);
                Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
                float fBlend = GetCcParam_()->m_Unk1B0;
                AnimPlay(3, Util_GetLandMotID(-1), 0, fBlend, 0.0f);
                pWork->m_nStep = 18;
            }
            break;
        }
        case 15:
            // The jump serve's hit.
            if (Util_DoJumpMoveEX(0) == 1) {
                ServeJumpMove(this, pWork);
            }
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame - 1.0f) {
                if (m_nStatusFlag & 1) {
                    float fFrame = GetCcParam_()->m_Unk19C;
                    float fDist = GetCcParam_()->m_Unk1A0;
                    float fJumpFrame = GetCcParam_()->m_Unk1A4;
                    float fJumpHeight = GetCcParam_()->m_Unk1A8;
                    float fJumpFrom = GetCcParam_()->m_Unk1AC;
                    pWork->m_MoveParam.m_nType = 11;
                    pWork->m_MoveParam.m_fFrame = 0.0f;
                    pWork->m_MoveParam.m_fFrameMax = fFrame;
                    pWork->m_MoveParam.m_fDist = fDist;
                    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                    pWork->m_fMoveFrame = 0.0f;
                    // Fall back from the hit, away from the target.
                    float fRotY = GetRotY();
                    if (GetVolleyTargetMarkerPos(pWork->m_JumpPos, 0)) {
                        fRotY = GetRotY_ToPosXZ(pWork->m_JumpPos);
                        CalcRotParam_(pWork->m_RotLerp, fRotY, fFrame);
                    }
                    bcs::BCS_VecZ_RotY(pWork->m_JumpPos, -2.0f * fDist, fRotY);
                    const bcs::FVEC3 &rPos = GetPos();
                    pWork->m_JumpPos.x += rPos.x;
                    pWork->m_JumpPos.z += rPos.z;
                    Util_SetJumpParam(fJumpFrame, fJumpFrom, fJumpHeight);
                    ServeShot(this, pWork, 2);
                    SetServeMode_(0);
                    SetBallCatchDisable_();
                    pWork->m_nStep = 16;
                }
                float fEnd = 1.0f + pWork->m_fShotFrame;
                if (IsAnimEnd(3) || AnimGetFrame(3) >= fEnd) {
                    SetServeMode_(0);
                    SetBallCatchDisable_();
                    pWork->m_nStep = 16;
                }
            }
            if (!(pWork->m_nFlag & 1) && !(m_nStatusFlag & 1)) {
                float fFrame = AnimGetFrame(3);
                float fMin = pWork->m_fShotFrame - 3.0f;
                float fMax = 1.0f + pWork->m_fShotFrame;
                if (fFrame >= fMin && fFrame < fMax) {
                    bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
                    SetBallCatchEnableVNode_(0.8f, ofs, 4);
                }
            }
            break;
        case 16: {
            int nJump = Util_DoJumpMoveEX(0);
            if (nJump == 1) {
                ServeJumpMove(this, pWork);
            }
            if (nJump >= 2) {
                PlayLandSe(0);
                Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
                AnimPlay(3, 0x4D, 0, GetCcParam_()->m_Unk1B0, 0.0f);
                pWork->m_nStep = 18;
            }
            break;
        }
        case 17:
            // No serve: the ball is dropped.
            if (pWork->m_fWait > 0.0f) {
                pWork->m_fWait -= m_fFrameStep * m_fTimeScale;
                if (pWork->m_fWait <= 0.0f) {
                    CBall *pBall = GetAttachBall();
                    if (pBall != 0) {
                        bcs::FVEC3 pos = pBall->GetPos();
                        PlaySePos(0xC24, &pos, 0, -1, 0.0f, 1.0f, 0.0f);
                    }
                    pWork->m_nStep = 18;
                }
            }
            // fall through
        case 18:
            if (Util_DoJumpMoveEX(0) >= 2 && IsAnimEnd(3)) {
                bEnd = 1;
                m_nStatusFlag &= ~0x800;
            }
            break;
        default:
            bEnd = 1;
            break;
    }

    if (CGameGuideMan::IsGuideDisp(0x11, m_nPlayerNo)) {
        pWork->m_nFlag |= 0x200;
    }
    if (bEnd) {
        FsmqNext();
        FsmqRetry();
    }
    // Unread: the original's pool has 0.25f after this function's literals
    // (unused local, idioms "Unread constant in place").
    float fUnused = 0.25f;
}

void msw::CChrCoreVolley::DtorStateServe()
{
    SetTargetMarkerDisable_();
    SetBallCatchDisable_();
    m_nStatusFlag &= ~0x800;
}

void msw::CChrCoreVolley::SubStateServeStartOverHand_(STATE_WORK_SERVE *pWork)
{
    // The hit motion starts so that the ball is met at the hit point; a
    // ball tossed too high makes the character jump to it.
    float fStart = 0.0f;
    float fBlend = 4.0f;
    float fHang = 8.0f;
    if (m_nStatusFlag & 1) {
        fStart = pWork->m_fShotFrame - fBlend;
    } else if (pWork->m_pBall != 0) {
        bcs::FVEC3 ballPos;
        pWork->m_pBall->GetPosAfterFrame(ballPos, pWork->m_fShotFrame);
        float fOver = ballPos.y - (m_ServeHitPosOver.y + GetPosY());
        if (fOver >= 0.01f) {
            float fFrame = 2.0f * pWork->m_fShotFrame - fHang;
            Util_SetJumpParam(fFrame, GetPosY(), GetPosY(), 0.05f + fOver);
            SetMoveMotType(0);
            m_fJumpGravity = -0.0125f;
            m_fJumpUnk228 = fHang;
        } else {
            float fLate = pWork->m_fShotFrame - pWork->m_pBall->GetTouchDownFrame(m_ServeHitPosOver.y + GetPosY());
            fStart = pWork->m_fShotFrame - fBlend;
            fStart = CLAMP(fLate, 0.0f, fStart);
        }
    }
    AnimPlay(3, 0x45, 0, fBlend, fStart);
    if (!(m_nStatusFlag & 1)) {
        if (fStart >= pWork->m_fShotFrame - 4.0f && fStart < 4.0f + pWork->m_fShotFrame) {
            bcs::FVEC3 ofs(m_ServeHitPosOver.x, 0.0f, m_ServeHitPosOver.z);
            SetBallCatchEnableRoot_(0.8f, m_ServeHitPosOver.y, ofs);
        } else {
            SetBallCatchDisable_();
        }
    }
    pWork->m_nStep = 8;
}

void msw::CChrCoreVolley::SubStateServeStartUnderHand_(STATE_WORK_SERVE *pWork)
{
    // As the over hand serve.
    float fStart = 0.0f;
    float fBlend = 4.0f;
    float fHang = 14.0f;
    if (m_nStatusFlag & 1) {
        fStart = pWork->m_fShotFrame - fBlend;
    } else if (pWork->m_pBall != 0) {
        bcs::FVEC3 ballPos;
        pWork->m_pBall->GetPosAfterFrame(ballPos, pWork->m_fShotFrame);
        float fOver = ballPos.y - (m_ServeHitPosUnder.y + GetPosY());
        if (fOver >= 0.01f) {
            float fFrame = (fBlend - 1.0f) + (2.0f * pWork->m_fShotFrame - fHang);
            Util_SetJumpParam(fFrame, GetPosY(), GetPosY(), 0.05f + fOver);
            SetMoveMotType(0);
            m_fJumpGravity = -0.0125f;
            m_fJumpUnk228 = fHang;
        } else {
            float fLate = pWork->m_fShotFrame - pWork->m_pBall->GetTouchDownFrame(m_ServeHitPosUnder.y + GetPosY());
            fStart = pWork->m_fShotFrame - fBlend;
            fStart = CLAMP(fLate, 0.0f, fStart);
        }
    }
    AnimPlay(3, 0x49, 0, fBlend, fStart);
    if (!(m_nStatusFlag & 1)) {
        if (fStart >= pWork->m_fShotFrame - 4.0f && fStart < 4.0f + pWork->m_fShotFrame) {
            bcs::FVEC3 ofs(m_ServeHitPosUnder.x, 0.3f, m_ServeHitPosUnder.z);
            SetBallCatchEnableRoot_(0.8f, m_ServeHitPosUnder.y, ofs);
        } else {
            SetBallCatchDisable_();
        }
    }
    pWork->m_nStep = 10;
}

void msw::CChrCoreVolley::SubStateServeStartJump_(STATE_WORK_SERVE *pWork)
{
    // The run up and jump of the jump serve, facing the target marker's
    // default position.
    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    SetCollBodyPushRank_(4);
    SetCollBodyPushPower_(5);
    m_UnkEC &= ~0x10;
    m_nStatusFlag &= ~0x1F000000;
    SetMoveMotType(4);
    float fHang = GetCcParam_()->m_Unk15C;
    float fDist = GetCcParam_()->m_Unk160;
    float fFrame = GetCcParam_()->m_Unk164;
    float fHeight = GetCcParam_()->m_Unk168;
    float fUnk = GetCcParam_()->m_Unk16C;
    float fTop = fHeight + GetCcParam_()->m_Unk44;
    AnimPlay(3, 0x4B, 0, 8.0f, 0.0f);
    SetMoveMotType(0);
    Util_SetJumpParam(2.0f * fFrame, GetPosY(), GetPosY(), fTop);
    m_fJumpGravity = -0.0125f;
    m_fJumpUnk224 = fUnk;
    m_fJumpUnk228 = fHang;
    pWork->m_MoveParam.m_nType = 2;
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = fFrame;
    pWork->m_MoveParam.m_fDist = fDist;
    pWork->m_fMoveFrame = 0.0f;
    pWork->m_JumpPos = GetPos();
    pWork->m_JumpPos.y = fTop;
    pWork->m_fWait = 4.0f + (fUnk + (fHang + fFrame));
    // (one work vector: the move, then the way to the target, then the
    // hit point's offset)
    bcs::FVEC3 vec;
    bcs::BCS_VecZ_RotY(vec, fDist, pWork->m_fRotY);
    pWork->m_JumpPos.x += vec.x;
    pWork->m_JumpPos.z += vec.z;

    // Not into a wall.
    bcs::FVEC3 pos(pWork->m_JumpPos.x, pWork->m_JumpPos.y, pWork->m_JumpPos.z);
    bcs::FVEC3 hit;
    bcs::FVEC3 normal;
    float fTime;
    if (CCollMan::DetectMapHit(hit, normal, fTime, pos, pos, 0.4f, 1, (unsigned char)GetGuardCourtIdx() | 0x2000000)) {
        pos.x = hit.x;
        pos.y = pWork->m_JumpPos.y;
        pos.z = hit.z;
    }

    vec = pos;
    GetTargetMarkerDefaultPos_(2, pos, vec, 1);
    vec.x -= pos.x;
    vec.z -= pos.z;
    pWork->m_fTargetRotY = nw4r::math::Atan2Rad(vec.x, vec.z);
    pWork->m_fTargetRotY = WrapRad(pWork->m_fTargetRotY);
    CalcRotParam_(pWork->m_RotLerp, pWork->m_fTargetRotY, Max(0.0f, (fFrame - pWork->m_fTossFrame) * 0.75f));
    bcs::BCS_Vec_RotY(vec, m_ServeHitPosJump, pWork->m_fTargetRotY);
    bcs::FVEC3Add(&pWork->m_TargetPos, &pos, &vec);
    pWork->m_nStep = 13;
    SetTargetMarkerEnable_(2, pWork->m_TargetPos, 0);
}

void msw::CChrCoreVolley::SubStateServeStartJumpShot_(STATE_WORK_SERVE *pWork)
{
    // The hit of the jump serve: when the ball is above the hit point, the
    // jump goes to meet it there.
    if (!(m_nStatusFlag & 1) && pWork->m_pBall != 0) {
        bcs::FVEC3 ballPos;
        pWork->m_pBall->GetPosAfterFrame(ballPos, pWork->m_fShotFrame);
        float fOver = ballPos.y - (m_ServeHitPosJump.y + GetPosY());
        if (fOver >= 0.01f) {
            Util_SetJumpParam(pWork->m_fShotFrame, 10.0f, 0.05f + fOver);
            SetMoveMotType(0);
            m_fJumpGravity = -0.0125f;
            pWork->m_JumpPos = ballPos;
            bcs::FVEC3 ofs;
            bcs::BCS_Vec_RotY(ofs, m_ServeHitPosJump, pWork->m_fTargetRotY);
            bcs::FVEC3Sub(&pWork->m_JumpPos, &pWork->m_JumpPos, &ofs);
            pWork->m_MoveParam.m_nType = 2;
            pWork->m_MoveParam.m_fFrame = 0.0f;
            pWork->m_MoveParam.m_fFrameMax = pWork->m_fShotFrame;
            const bcs::FVEC3 &rPos = GetPos();
            bcs::FVEC3 to(pWork->m_JumpPos.x, 0.0f, pWork->m_JumpPos.z);
            bcs::FVEC3 from(rPos.x, 0.0f, rPos.z);
            pWork->m_MoveParam.m_fDist = nw4r::math::FSqrt(bcs::FVEC3DistSq(&to, &from));
            pWork->m_fMoveFrame = 0.0f;
            Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
            CalcRotParam_(pWork->m_RotLerp, pWork->m_fTargetRotY, pWork->m_fShotFrame);
        }
    }
    AnimPlay(3, 0x4C, 0, 0.0f, 0.0f);
    SetBallCatchDisable_();
    pWork->m_nStep = 15;
}

void msw::CChrCoreVolley::SubStateServeBallTossUp_(long nType, const bcs::FVEC3 &rPos)
{
    // The ball leaves the hand for the toss; its model's scale (to two
    // places) goes with it.
    CBall *pBall = GetAttachBall();
    float fScale = 1.0f;
    if (pBall != 0) {
        bcs::CScnObj *pMdl = pBall->GetAttachMdlObj();
        if (pMdl != 0) {
            bcs::FMTX34 mtx;
            pMdl->GetNodeWorldMtx(pBall->GetMdlNode(), mtx);
            bcs::FVEC3 scale;
            scale.x = PSVECMag((const Vec *)mtx.m[0]);
            scale.y = PSVECMag((const Vec *)mtx.m[1]);
            scale.z = PSVECMag((const Vec *)mtx.m[2]);
            fScale = 0.01f * (float)ceil(100.0f * ((scale.z + (scale.x + scale.y)) / 3.0f));
        }
    }
    DetachBall();
    if (pBall != 0) {
        long nVoice = 0x2A;
        BALL::VOLLEY::SERVE_TOSS_ARG arg;
        arg.m_nType = nType;
        arg.m_nPlayerNo = m_nPlayerNo;
        arg.m_nTeamNo = m_nTeamNo;
        arg.m_fScale = fScale;
        arg.m_nOutline = PlayGetOutline_(0);
        switch (nType) {
            case 2:
                arg.m_Pos = rPos;
                arg.m_fTopY = rPos.y + GetCcParam_()->m_Unk174;
                arg.m_fCatchY = rPos.y;
                arg.m_fUnk20 = GetCcParam_()->m_Unk178;
                arg.m_fUnk28 = GetCcParam_()->m_Unk17C;
                arg.m_fUnk2C = GetCcParam_()->m_Unk17C + GetCcParam_()->m_Unk180;
                nVoice = 0x2E;
                arg.m_pMdl = 0;
                break;
            case 1:
                arg.m_Pos = pBall->GetPos();
                arg.m_fTopY = GetCcParam_()->m_Unk134;
                arg.m_fCatchY = m_ServeBallPosUnder.y + pBall->GetRadius();
                arg.m_fUnk20 = GetCcParam_()->m_Unk138;
                arg.m_fUnk28 = GetCcParam_()->m_Unk13C;
                arg.m_fUnk2C = GetCcParam_()->m_Unk13C + GetCcParam_()->m_Unk140;
                arg.m_pMdl = m_pMdl;
                break;
            default:
                arg.m_Pos = pBall->GetPos();
                arg.m_fTopY = GetCcParam_()->m_Unk108;
                arg.m_fCatchY = m_ServeBallPosOver.y + pBall->GetRadius();
                arg.m_fUnk20 = GetCcParam_()->m_Unk10C;
                arg.m_fUnk28 = GetCcParam_()->m_Unk110;
                arg.m_fUnk2C = GetCcParam_()->m_Unk110 + GetCcParam_()->m_Unk114;
                arg.m_pMdl = m_pMdl;
                break;
        }
        pBall->SetVolleyServeTossMode(arg);
        if (nVoice >= 0) {
            PlayVoice(nVoice, 0);
        }
    }
}
