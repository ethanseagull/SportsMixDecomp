// msw::CChrCoreDodge's throw from a run or a jump (the jump shoot state,
// whose work is STATE_WORK_ITEM_SHOOT), the stop at the court line (line
// over), the arts event of the special shot (Arts*: the jump, the throw and
// the landing, driven by ArtsProc in the air trick state's work) and the
// spin attack.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_DODGE.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CStageMan.hpp"

// (names guessed) The four constants at the start of the file's .rodata,
// read from memory: one-element arrays (a scalar const would be folded into
// the literal pool behind them).
static const float s_fShootTimeAdd[1] = { 30.0f };   // added to the throw frame (m_fShootTime)
static const float s_fChargeTime[1] = { 19.0f };     // frames of the charged throw's wind-up
static const float s_fSpinFrame[1] = { 25.0f };      // frames of the spin attack
static const float s_fSpinJumpFrame[1] = { 6.0f };   // frames of the spin attack's jump

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // An angle brought into [0, 2 pi).
    inline float WrapRad(float rad)
    {
        while (rad > PI2) {
            rad -= PI2;
        }
        while (rad < 0.0f) {
            rad += PI2;
        }
        return rad;
    }

    // An angle brought into (-pi, pi].
    inline float WrapRadPI(float rad)
    {
        rad = WrapRad(rad);
        if (rad > PI) {
            rad -= PI2;
        }
        return rad;
    }

    // The kinds of throw that come with the hand effect (m_nShootType).
    // A macro, not an inline function: the original tests the condition
    // in place, an inline's int result is kept as a value by our compiler.
#define IsStrongShoot(nType) (((nType) >= 5 && (nType) <= 6) || (nType) == 2)

    // (names guessed) The work of the arts event: the air trick state's
    // work as ArtsProc and the Arts* functions use it.
    struct ARTS_WORK {
        long m_nStep;              // 0x00: 0, 1 (the jump), 2 (the throw), 0x65/0x66 (the landing)
        unsigned char m_Unk04[0x0C - 0x04];
        float m_fFrame;            // 0x0C: frames of the step so far
        float m_fUnk10;            // 0x10
        unsigned char m_Unk14[0x70 - 0x14];
        long m_nNextStep;          // 0x70: the step asked for, negative for none
        float m_fGoalRotY;         // 0x74: the yaw to the target goal
        unsigned char m_Unk78[0x80 - 0x78];
        float m_fInputStart;       // 0x80: from this frame the input is taken
        float m_fInputEnd;         // 0x84: up to this frame
        float m_fBranchFrame;      // 0x88: the step may change from this frame on
        long m_nLockTarget;        // 0x8C: the locked target character, negative for none
        unsigned char m_Unk90[0x94 - 0x90];
        float m_fShotFrame;        // 0x94: the frame of the throw motion the ball leaves at
        unsigned char m_Unk98[0x9C - 0x98];
        unsigned long m_nTrigFlag; // 0x9C: one bit per trigger IsArtsTrig has seen
        int m_bLockDir;            // 0xA0: the lock bit of m_nStatusFlag in the last frame
    };

    // (names guessed) The work of the line over state.
    struct LINE_OVER_WORK {
        long m_nStep;              // 0x00: 0 to 2, plus 0x100 when the character turned round
        unsigned char m_Unk04[0x14 - 0x04];
        float m_fRotY;             // 0x14: the direction to step back in
        msw::CC_DATA::CHR_PARAM::MOVE_PARAM m_MoveParam; // 0x18
        msw::CChrCore::DIR_MOVE m_DirMove;               // 0x28
    };

    // (names guessed) The work of the spin attack state.
    struct SPIN_ATTACK_WORK {
        long m_nStep;              // 0x00
        float m_fTime;             // 0x04: frames since the start
        float m_fRotY;             // 0x08: added to the model's yaw while it spins
        unsigned char m_Unk0C[0x30 - 0x0C];
        msw::CChrCoreDodge::MOT_ACT_CONTEXT m_MotAct; // 0x30
        int m_bAttack;             // 0x48: the attack is on
        float m_fAtkUnk4C;         // 0x4C
        float m_fAtkUnk50;         // 0x50
        float m_fAtkUnk54;         // 0x54
        float m_fAtkUnk58;         // 0x58
        float m_fJumpFrame;        // 0x5C: the jump's frames, -1 for none
    };

    // (name guessed) The neutral state's work: CChrCore's neutral state
    // reads the word at 0xC.
    struct NEUTRAL_WORK {
        unsigned char m_Unk00[0x0C];
        long m_nType;              // 0x0C
    };
}

void msw::CChrCoreDodge::CtorStateJumpShoot()
{
    const CHR_CORE_DODGE::CC_PARAM_DODGE *pParam = GetCcParam_();

    SetCollBodyPushType_(static_cast<BODY_COLL_SIZE_SET_TYPE>(0), 8.0f);
    m_UnkEC |= 0x10;
    if (m_nStatusFlag & 0x400) {
        SetCollBodyPushPower_(1);
        SetCollBodyPushRank_(4);
    } else {
        SetCollBodyPushPower_(5);
        SetCollBodyPushRank_(1);
    }

    STATE_WORK_ITEM_SHOOT *pWork = FsmqGetWork<STATE_WORK_ITEM_SHOOT>();
    long nMot = 0x42;
    pWork->m_fShotFrame = pParam->m_Unk34;
    float fSpeed = 1.0f;
    pWork->m_nMotType = pWork->m_nShootType;
    if (pWork->m_nShootType == 0 && !(m_nStatusFlag & 0x400) && IsInner1on1AreaAnyChr()) {
        pWork->m_nMotType = 2;
    }
    switch (pWork->m_nMotType) {
        case 3:
            if (m_nStatusFlag & 0x400) {
                nMot = 0x4B;
                pWork->m_fShotFrame = pParam->m_Unk3C;
            } else {
                nMot = 0x46;
            }
            fSpeed = pParam->m_Unk4F4;
            break;
        case 2:
        case 5:
        case 6:
            if (m_nStatusFlag & 0x400) {
                pWork->m_fShotFrame = pParam->m_Unk3C;
                nMot = 0x4C;
                pWork->m_nMotType = 0;
            } else if (pWork->m_nShootType == 0) {
                pWork->m_fShotFrame = pParam->m_Unk38;
                nMot = 0x43;
                fSpeed = 1.35f;
                pWork->m_nMotType = 2;
            } else {
                fSpeed = pParam->m_Unk4A4;
                nMot = 0x26;
                pWork->m_fShotFrame = pParam->m_Unk38;
                pWork->m_nMotType = 2;
            }
            break;
        default:
            if (m_nStatusFlag & 0x400) {
                nMot = 0x4A;
                pWork->m_fShotFrame = pParam->m_Unk3C;
            }
            break;
    }

    float fTrgFrame = AnimSearchTrgFrame(nMot, 2, -1.0f);
    if (fTrgFrame >= 0.0f) {
        pWork->m_fShotFrame = fTrgFrame;
    }
    m_nStatusFlag &= ~0x1F000000;
    AnimPlay(3, nMot, 0, 8.0f, 0.0f);
    AnimSetSpeed(3, fSpeed);
    if (!(m_nStatusFlag & 0x400) && IsStrongShoot(pWork->m_nShootType)) {
        Eff_CreateAttach_Node(0xD, pParam->m_Unk4B0, pParam->m_Unk4B4);
    }

    pWork->m_nStep = 0;
    pWork->m_fStartRotY = GetRotY();
    pWork->m_fRotY = WrapRad(pWork->m_fRotY);
    pWork->m_bJump = (m_nStatusFlag >> 10) & 1;

    int bCharge = 0;
    if (pWork->m_nMotType == 2 && pWork->m_nShootType == 0) {
        bCharge = 1;
    }
    float fFrameMax;
    if (bCharge) {
        fFrameMax = s_fChargeTime[0];
    } else {
        long nMotType = pWork->m_nMotType;
        if (nMotType == 2) {
            fFrameMax = -1.0f;
        } else if (nMotType == 5) {
            fFrameMax = -1.0f;
        } else if (nMotType == 6) {
            fFrameMax = -1.0f;
        } else {
            fFrameMax = pParam->m_Unk440;
        }
    }
    pWork->m_fFrameMax = fFrameMax;
    pWork->m_fStopFrame = pParam->m_Unk40;
    SetMoveMotType(0);
    if (pWork->m_nShootType == 3) {
        CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(0x48), m_nPlayerNo, 0, 0, 0);
    }
    if (pWork->m_nFlag & 2) {
        pWork->m_nStep = 2;
        pWork->m_bJump = 1;
    }
    OnShootSubStepChange(pWork);
}

void msw::CChrCoreDodge::OnShootSubStepChange(const STATE_WORK_ITEM_SHOOT *)
{
}

void msw::CChrCoreDodge::LoopStateJumpShoot()
{
    STATE_WORK_ITEM_SHOOT *pWork = FsmqGetWork<STATE_WORK_ITEM_SHOOT>();
    const CHR_CORE_DODGE::CC_PARAM_DODGE *pParam = GetCcParam_();

    if (pWork->m_fTimer > 0.0f) {
        pWork->m_fTimer -= m_fTimeScale * m_fFrameStep;
    }

    // Up to the throw: in the air the character falls, on the ground the
    // run slows down.
    if (pWork->m_nStep <= 3) {
        if (m_nStatusFlag & 0x400) {
            if (Util_DoJumpMove(m_fFrameStep) == 1) {
                SetCollBodyPushPower_(5);
                SetCollBodyPushRank_(1);
                Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
                PlayLandSe(1);
                if (pWork->m_nShootType == 3 && IsLineOver_(1)) {
                    FsmqDoChain();
                    return;
                }
            }
        } else {
            float fAccel = m_fTimeScale * (m_fFrameStep * (CalcMoveSpeedRate_(1.0f, 0) * -pParam->m_Unk444));
            float fSpeed = pWork->m_fRunSpeed + fAccel;
            pWork->m_fRunSpeed = fSpeed;
            float fMin = 0.0f;
            if (fSpeed > fMin) {
                fMin = fSpeed;
            }
            pWork->m_fRunSpeed = fMin;
            MoveRunXZ_(pWork->m_fRunRotY, GetPrmMoveSpeed(), pWork->m_fRunSpeed);
        }
    }

    switch (pWork->m_nStep) {
        case 0:
            if (m_nStatusFlag & 1) {
                float fFrame = AnimGetFrame(3);
                if ((fFrame >= pWork->m_fShotFrame || IsAnimEnd(3)) && pWork->m_nShootType != 3) {
                    // The throw.
                    if (m_nComLevel == 1) {
                        GetAttachBall()->AttachItemEffect(4);
                    } else if (pWork->m_bItem) {
                        MethdUseItem();
                    }
                    if (!(m_nStatusFlag & 0x400)) {
                        bcs::FVEC3 dir;
                        bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_fRotY);
                        Util_ShootBall(pWork->m_fRotY, 0, pWork->m_nShootType);
                        long nVoice = 0x52;
                        long nSe = 0xC9A;
                        m_fShootTime = pWork->m_fShotFrame + s_fShootTimeAdd[0];
                        if (pWork->m_bItem) {
                            nVoice = 0x61;
                        } else if (pWork->m_nShootType == 2) {
                            nVoice = 0x53;
                            nSe = -1;
                            WSE_Play(0, 0xF, 0, -1);
                        } else if (pWork->m_nShootType == 5 || pWork->m_nShootType == 6) {
                            nVoice = 0x53;
                            nSe = -1;
                            WSE_Play(0, 0xF, 0, -1);
                        }
                        if (nSe >= 0) {
                            PlaySePos(nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        }
                        PlayVoice(nVoice, 0);
                    } else {
                        // In the air the ball goes to the locked target, or
                        // to the nearest one in front.
                        int bTarget = 0;
                        float fRotY = GetRotY();
                        CChrCore *pTarget = GetLockTargetChrCore();
                        if (pTarget == 0) {
                            pTarget = Util_GetNearInnerChrExclusive(m_nTeamNo, pWork->m_fRotY, -1.0f);
                            float fDiff = WrapRadPI(GetRotY_Diff_ToChrXZ(pTarget));
                            if (__fabsf(fDiff) > 0.34906584f) {
                                pTarget = 0;
                            }
                        }
                        if (pTarget != 0) {
                            float fDiff = WrapRadPI(GetRotY_ToChr(pTarget) - fRotY);
                            if (__fabsf(fDiff) < PI) {
                                bTarget = 1;
                            }
                        }
                        if (bTarget) {
                            Util_JumpShootBall(pTarget->GetPlayerNo(), pWork->m_nShootType == 2 ? 0xB : 2);
                            pWork->m_fRotY = GetRotY_ToChr(pTarget);
                        } else {
                            Util_JumpShootBall(-1, pWork->m_nShootType == 2 ? 0xB : 2);
                        }
                        long nVoice = 0x52;
                        long nSe = 0xC9A;
                        m_fShootTime = pWork->m_fShotFrame + s_fShootTimeAdd[0];
                        if (pWork->m_nShootType == 2) {
                            nVoice = 0x53;
                            nSe = 0xC9B;
                        }
                        PlaySePos(nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        PlayVoice(nVoice, 0);
                    }
                } else if (fFrame >= pWork->m_fShotFrame && pWork->m_nShootType == 3) {
                    if (!(pWork->m_nFlag & 1)) {
                        PlayVoice(0x54, 0);
                        PlaySePos(0xC9C, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        pWork->m_nFlag |= 1;
                    }
                } else {
                    // Before the throw: turn to the target.
                    CChrCore *pTarget = GetLockTargetChrCore();
                    if (pWork->m_bJump && pTarget != 0) {
                        RotateYRate_(GetRotY_ToChr(pTarget), 0.15f);
                    } else if (pTarget != 0) {
                        RotateYRate_(GetRotY_ToChr(pTarget), 0.15f);
                    } else {
                        RotateYRate_(pWork->m_fRotY, 0.15f);
                    }
                }
            } else if (!pWork->m_bJump) {
                float fStopFrame = pWork->m_fStopFrame;
                if (AnimGetFrame(3) >= fStopFrame && pWork->m_nMotType != 2) {
                    AnimSetFrame(3, fStopFrame);
                    AnimSetSpeed(3, 0.0f);
                    pWork->m_nStep = 2;
                    OnShootSubStepChange(pWork);
                }
            }
            if (IsAnimEnd(3)) {
                if (pWork->m_nMotType == 2 && pWork->m_nShootType == 0) {
                    AnimPlay(3, 0x44, 0, 0.0f, 0.0f);
                    AnimSetSpeed(3, 0.7f);
                    pWork->m_nStep = 1;
                    OnShootSubStepChange(pWork);
                } else if (pWork->m_nMotType == 2) {
                    AnimPlay(3, 0x27, 0, 0.0f, 0.0f);
                    AnimSetSpeed(3, pParam->m_Unk4A8);
                    pWork->m_nStep = 1;
                    if (!(m_nStatusFlag & 0x400) && IsStrongShoot(pWork->m_nShootType)) {
                        Eff_CreateAttach_Node(0xD, pParam->m_Unk4B8, pParam->m_Unk4BC);
                    }
                    OnShootSubStepChange(pWork);
                } else {
                    pWork->m_nStep = 2;
                    OnShootSubStepChange(pWork);
                }
            }
            // fall through
        case 1:
            if (pWork->m_nStep == 1) {
                if (pWork->m_nMotType == 2 && pWork->m_nShootType == 0) {
                    if (IsAnimEnd(3)) {
                        AnimPlay(3, 0x45, 0, 0.0f, 0.0f);
                        float fFrameNum = AnimGetFrameNum(3);
                        if (fFrameNum > 0.0f && s_fChargeTime[0] > 0.0f) {
                            AnimSetSpeed(3, 0.6f * fFrameNum / s_fChargeTime[0]);
                        }
                        pWork->m_nStep++;
                        OnShootSubStepChange(pWork);
                    }
                } else if (pWork->m_nMotType == 2 || pWork->m_nMotType == 5 || pWork->m_nMotType == 6) {
                    if (IsAnimEnd(3)) {
                        AnimPlay(3, 0x28, 0, 0.0f, 0.0f);
                        AnimSetSpeed(3, pParam->m_Unk4AC);
                        if (!(m_nStatusFlag & 0x400) && IsStrongShoot(pWork->m_nShootType)) {
                            Eff_CreateAttach_Node(0xD, pParam->m_Unk4C0, pParam->m_Unk4C4);
                        }
                        pWork->m_nStep++;
                        OnShootSubStepChange(pWork);
                    }
                } else {
                    pWork->m_nStep++;
                    OnShootSubStepChange(pWork);
                }
            }
            // fall through
        case 2:
            if (!(m_nStatusFlag & 0x400)
                && (pWork->m_nStep >= 2 || (pWork->m_bJump && !(m_nStatusFlag & 1)))) {
                if (pWork->m_bJump) {
                    if (IsDodgeInfield() && !(m_nStatusFlag & 1)) {
                        // Landed after a throw from the jump: slide on
                        // along the jump.
                        AnimPlay(3, 0x51, 0, 4.0f, 0.0f);
                        float fRotY = GetRotY();
                        if (PSVECMag(reinterpret_cast<const Vec *>(&GetJumpVel())) > 0.01f) {
                            bcs::FVEC3 front(0.0f, 0.0f, 1.0f);
                            bcs::FVEC3 dir = GetJumpVel();
                            dir.y = 0.0f;
                            bcs::BCS_Vec_Normalize(dir, dir);
                            fRotY = bcs::BCS_Vec_GetRot(front, dir);
                            bcs::FVEC3 cross;
                            PSVECCrossProduct(reinterpret_cast<const Vec *>(&front),
                                              reinterpret_cast<const Vec *>(&dir), reinterpret_cast<Vec *>(&cross));
                            if (cross.y < 0.0f) {
                                fRotY = WrapRad(-fRotY);
                            }
                            SetRotY(fRotY);
                        }
                        float fResFrame = AnimGetResFrameNum(0x51);
                        float fMotFrame = pParam->m_Unk47C;
                        float fStopFrame = pParam->m_Unk480;
                        AnimSetSpeed(3, fMotFrame > 0.0f ? fResFrame / fMotFrame : 1.0f);
                        float fFrameMax = pParam->m_Unk478;
                        pWork->m_fFrameMax = fFrameMax;
                        pWork->m_fStopFrame = fFrameMax - fStopFrame;
                        if (pWork->m_fStopFrame < 0.0f) {
                            pWork->m_fStopFrame = 0.0f;
                        }
                        pWork->m_MoveParam.m_fFrame = 0.0f;
                        pWork->m_MoveParam.m_fFrameMax = pWork->m_fFrameMax;
                        pWork->m_MoveParam.m_fDist = pParam->m_Unk474;
                        pWork->m_MoveParam.m_nType = 2;
                        Util_SetDirMoveParam(pWork->m_DirMove, fRotY);
                        pWork->m_nStep = 6;
                        OnShootSubStepChange(pWork);
                    } else {
                        AnimPlay(3, Util_GetLandMotID(-1), 0, 4.0f, 0.0f);
                        pWork->m_nStep = 3;
                        OnShootSubStepChange(pWork);
                    }
                    PlayFtSe_(3, 0);
                } else if (pWork->m_nShootType != 3) {
                    pWork->m_nStep = 3;
                    OnShootSubStepChange(pWork);
                } else {
                    pWork->m_nStep = 4;
                    OnShootSubStepChange(pWork);
                }
            }
            if (pWork->m_nStep <= 2) {
                break;
            }
            // fall through
        case 3:
            if (!pWork->m_bJump) {
                if (pWork->m_nStep < 3) {
                    break;
                }
                if (pWork->m_fFrameMax >= 0.0f) {
                    pWork->m_fFrame = m_fTimeScale * m_fFrameStep + pWork->m_fFrame;
                    if (pWork->m_fFrame < pWork->m_fFrameMax) {
                        break;
                    }
                    AnimSetSpeed(3, 1.0f);
                }
            } else if (!IsAnimEnd(3)) {
                break;
            }
            pWork->m_nStep++;
            OnShootSubStepChange(pWork);
            // fall through
        case 4:
            if (!IsAnimEnd(3)) {
                break;
            }
            if (pWork->m_bJump) {
                FsmqDoChain();
                break;
            }
            pWork->m_nStep = 5;
            pWork->m_fFrame = 0.0f;
            OnShootSubStepChange(pWork);
            if (CGameDodge::IsEnemyZoneLimited() == 1 && IsLineOver_(1) && !(m_nStatusFlag & 0x400)) {
                CBall *pBall = GetAttachBall();
                if (pBall != 0) {
                    m_nFlagF4 |= 0x80000;
                    pBall->SetChrReflectMode(m_nPlayerNo, 0.1f);
                }
                DetachBall();
            }
            // fall through
        case 5:
            if (m_nStatusFlag & 1) {
                NEUTRAL_WORK *pNeutral = static_cast<NEUTRAL_WORK *>(FsmqSetState(STATE_NEUTRAL));
                pNeutral->m_nType = 2;
                FsmqRetry();
            } else {
                FsmqDoChain();
            }
            break;
        case 6:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x52, 1, 0.0f, 0.0f);
                pWork->m_nStep++;
                OnShootSubStepChange(pWork);
            }
            // fall through
        case 7:
            if (pWork->m_nStep == 7 && pWork->m_fFrame >= pWork->m_fStopFrame) {
                AnimPlay(3, 0x53, 0, 0.0f, 0.0f);
                float fFrames = pWork->m_fFrameMax - pWork->m_fStopFrame;
                float fResFrame = AnimGetResFrameNum(0x53);
                AnimSetSpeed(3, pParam->m_Unk480 > 0.0f ? fResFrame / fFrames : 1.0f);
                pWork->m_nStep++;
                OnShootSubStepChange(pWork);
            }
            // fall through
        case 8:
            pWork->m_fFrame = m_fTimeScale * m_fFrameStep + pWork->m_fFrame;
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fFrame, pWork->m_DirMove, 1);
            if (pWork->m_fFrame >= pWork->m_fFrameMax) {
                FsmqDoChain();
            }
            break;
    }
}

void msw::CChrCoreDodge::DtorStateJumpShoot()
{
    m_nFlagF4 &= ~0x80000;
    ArtsOptDestroy();
    Eff_Stop(0xD);
}

// The character stepped over the court line: he stops, turned towards the
// court (turning round first when he faced away), and steps back.
void msw::CChrCoreDodge::CtorStateLineOver()
{
    LINE_OVER_WORK *pWork = FsmqGetWork<LINE_OVER_WORK>();

    if (m_nStatusFlag & 1) {
        CBall *pBall = GetAttachBall();
        if (pBall != 0) {
            pBall->SetChrReflectMode(m_nPlayerNo, 0.1f);
        }
        DetachBall();
        DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x53), 4, 0, 0, 0);
    }
    m_nFlagF4 |= 0x80000;

    float fDiff = WrapRadPI(pWork->m_fRotY - GetRotY());
    if (__fabsf(fDiff) < 1.5707964f) {
        pWork->m_nStep = 0;
        AnimPlay(3, 0x4F, 0, 4.0f, 0.0f);
        SetRotY(pWork->m_fRotY);
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = AnimGetFrameNum(3);
        pWork->m_MoveParam.m_fDist = 1.0f;
        pWork->m_MoveParam.m_nType = 2;
        Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fRotY);
    } else {
        // Facing away: turn round first.
        pWork->m_nStep = 0x100;
        AnimPlay(3, 0x25, 0, 4.0f, 0.0f);
        SetRotY(WrapRad(PI + pWork->m_fRotY));
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = AnimGetFrameNum(3);
        pWork->m_MoveParam.m_fDist = 1.0f;
        pWork->m_MoveParam.m_nType = 2;
        Util_SetDirMoveParam(pWork->m_DirMove, pWork->m_fRotY);
    }
    PlayVoice(0x4D, 0);
}

void msw::CChrCoreDodge::LoopStateLineOver()
{
    LINE_OVER_WORK *pWork = FsmqGetWork<LINE_OVER_WORK>();

    switch (pWork->m_nStep & ~0x100) {
        case 0:
            if (IsAnimEnd(3)) {
                pWork->m_nStep++;
                if (pWork->m_nStep & 0x100) {
                    FsmqDoChain();
                    break;
                }
                AnimPlay(3, 0x50, 0, 0.0f, 0.0f);
            } else {
                Util_DoDirMove(pWork->m_MoveParam, AnimGetFrame(3), pWork->m_DirMove, 1);
                break;
            }
            // fall through
        case 1:
        case 2:
            if (IsAnimEnd(3)) {
                if (pWork->m_nStep & 0x100) {
                    switch (pWork->m_nStep & ~0x100) {
                        case 1:
                            AnimPlay(3, 0x60, 0, 0.0f, 0.0f);
                            pWork->m_nStep++;
                            break;
                        case 2:
                            FsmqDoChain();
                            break;
                        default:
                            pWork->m_nStep++;
                            break;
                    }
                } else {
                    FsmqDoChain();
                }
            }
            break;
    }
}

void msw::CChrCoreDodge::DtorStateLineOver()
{
    m_nFlagF4 &= ~0x80000;
}

void msw::CChrCoreDodge::ArtsFadeOut() const
{
}

void msw::CChrCoreDodge::DoPlayArtsEvent()
{
    AnimPlay(3, 0x79, 0, 0.0f, 0.0f);
    Eff_CreateSimple(0xF, GetPos(), GetRotY(), 3, 0, 0);
    PlaySe(0xC06, 1, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreDodge::ArtsFadeIn() const
{
    CGame::BgSetFadeIn(0.0f, 30.0f);
    CGame::SendMessage(0xE, 0, 0, 0);
}

void msw::CChrCoreDodge::ArtsFadeReset() const
{
    CStageMan::FadeIn();
}

int msw::CChrCoreDodge::IsArtsInputEnable() const
{
    ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
    if (FsmqGetState() == STATE_AIR_TRICK && pWork->m_nNextStep < 0
        && pWork->m_fFrame >= pWork->m_fInputStart && pWork->m_fFrame <= pWork->m_fInputEnd) {
        return 1;
    }
    return 0;
}

int msw::CChrCoreDodge::IsArtsBranchEnable() const
{
    ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
    if (FsmqGetState() == STATE_AIR_TRICK && pWork->m_nNextStep >= 0 && pWork->m_fFrame >= pWork->m_fBranchFrame) {
        return 1;
    }
    return 0;
}

int msw::CChrCoreDodge::ArtsInputCheck()
{
    int bInput = 0;
    if (IsArtsInputEnable()) {
        bInput = DriverSendMessage(static_cast<CChrDrv::MSG_ID>(0x21), 0, 0, 0, 0);
    }
    return bInput;
}

void msw::CChrCoreDodge::ArtsNextSubStep(int bForce)
{
    ArtsNextSubStep(bForce, FsmqGetWork<ARTS_WORK>()->m_nStep + 1);
}

void msw::CChrCoreDodge::ArtsNextSubStep(int, long nStep)
{
    ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
    if (CanbeArtsNextSubStep(nStep)) {
        pWork->m_nNextStep = nStep;
    }
}

void msw::CChrCoreDodge::DoArtsNextSubStep()
{
    if (IsArtsBranchEnable()) {
        ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
        ArtsProc(static_cast<ARTS_EVENT>(2));
        pWork->m_nStep = pWork->m_nNextStep;
        pWork->m_nNextStep = -1;
        pWork->m_fFrame = 0.0f;
        pWork->m_fUnk10 = 0.0f;
        pWork->m_fInputStart = -1.0f;
        pWork->m_fInputEnd = -1.0f;
        pWork->m_fBranchFrame = -1.0f;
        ArtsProc(static_cast<ARTS_EVENT>(0));
    }
}

void msw::CChrCoreDodge::ArtsSignEffectKick()
{
    unsigned long hEffect = CEffectCtrl::CreateAttachEffect(0xF, m_pMdl, 0, 0, 1, 1, 1, 3, 0, 0, 0);
    if (hEffect != 0) {
        CEffectCtrl::GetInstance()->FindList(hEffect);
    }
}

int msw::CChrCoreDodge::CanbeArtsNextSubStep(long) const
{
    return 1;
}

void msw::CChrCoreDodge::ArtsParamInit()
{
    ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
    pWork->m_fGoalRotY = GetRotY_ToPosXZ(GetTargetGoalPos());
    long nTarget = GetLockTargetChrIdx();
    pWork->m_nLockTarget = nTarget;
    pWork->m_bLockDir = nTarget >= 0;
}

void msw::CChrCoreDodge::ArtsPreProc()
{
    ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
    int bLock = (m_nStatusFlag >> 4) & 1;
    if (bLock) {
        UpdateLockTargetDir_();
    } else if (pWork->m_bLockDir != bLock) {
        UnlockTargetDir_();
    }
    pWork->m_bLockDir = (m_nStatusFlag >> 4) & 1;
}

// The arts event's steps: 0 waits for the event to start, 1 jumps, 2
// throws, 0x65 and 0x66 land.
void msw::CChrCoreDodge::ArtsProc(ARTS_EVENT event)
{
    ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
    const CHR_CORE_DODGE::CC_PARAM_DODGE *pParam = GetCcParam_();

    switch (pWork->m_nStep) {
        case 0:
            switch (event) {
                case 0:
                    CGame::TrgArtsEvent(m_nPlayerNo);
                    CGame::SendMessage(0xD, 0, 0, 0);
                    break;
                case 1:
                    if (!CGame::IsArtsEventMode() && !CGame::IsReqArtsEvent()) {
                        ArtsNextSubStep(0);
                    }
                    break;
            }
            break;
        case 1:
            switch (event) {
                case 0: {
                    AnimPlay(3, 0x7A, 0, 4.0f, 0.0f);
                    float fFrames = AnimGetFrameNum(3);
                    Util_SetJumpParam(fFrames, 30.0f, 4.0f);
                    pWork->m_fInputStart = fFrames;
                    pWork->m_fInputEnd = fFrames;
                    pWork->m_fBranchFrame = fFrames;
                    // A little push away from the goal.
                    bcs::FVEC3 dir;
                    bcs::FVEC3Sub(&dir, &GetTargetGoalPos(), &GetPos());
                    dir.y = 0.0f;
                    bcs::BCS_Vec_Normalize(dir, dir);
                    bcs::FVEC3Scale(&dir, &dir, -0.03f);
                    bcs::FVEC3Add(&GetJumpVel(), &GetJumpVel(), &dir);
                    Util_AttachEffect(static_cast<ATTACH_EFFECT_TYPE>(6), 4);
                    ArtsFadeOut();
                    break;
                }
                case 1: {
                    if (IsAnimEnd(3)) {
                        AnimPlay(3, 0x85, 0, 0.0f, 0.0f);
                    }
                    int bNext = ArtsInputCheck();
                    if (IsArtsInputEnable() && GetPos().y <= 3.0f) {
                        bNext = 1;
                    }
                    if (bNext) {
                        ArtsNextSubStep(1);
                    }
                    ArtsDoMove();
                    break;
                }
            }
            break;
        case 2:
            switch (event) {
                case 0: {
                    ClearRevise();
                    AnimPlay(3, 0x7F, 0, 4.0f, 0.0f);
                    Util_SetJumpParam(18.0f, 34.0f, 2.2f);
                    pWork->m_fInputEnd = -1.0f;
                    pWork->m_fBranchFrame = -1.0f;
                    bcs::FVEC3 dir;
                    bcs::FVEC3Sub(&dir, &GetTargetGoalPos(), &GetPos());
                    dir.y = 0.0f;
                    bcs::BCS_Vec_Normalize(dir, dir);
                    bcs::FVEC3Scale(&dir, &dir, -0.03f);
                    bcs::FVEC3Add(&GetJumpVel(), &GetJumpVel(), &dir);
                    pWork->m_fShotFrame = AnimSearchTrgFrame(0x7F, 2, -1.0f);
                    break;
                }
                case 1:
                    if (IsAnimEnd(3)) {
                        AnimPlay(3, 0x85, 0, 0.0f, 0.0f);
                    }
                    if ((m_nStatusFlag & 1) && AnimGetFrame(3) >= pWork->m_fShotFrame) {
                        Util_JumpShootBall(GetLockTargetChrIdx(), 9);
                        StopSeSlot(3);
                    }
                    ArtsDoMove();
                    break;
            }
            break;
        case 0x65:
            if (event == 1) {
                Util_DoJumpDodge(m_fFrameStep * m_fTimeScale, 0);
                if (IsAnimEnd(3)) {
                    AnimPlay(3, 0x50, 0, 4.0f, 0.0f);
                    pWork->m_nStep = 0x66;
                }
            }
            break;
        case 0x66:
            if (event == 1) {
                Util_DoJumpMoveLand(m_fFrameStep * m_fTimeScale);
                if (IsAnimEnd(3)) {
                    FsmqDoChain();
                }
            }
            break;
    }
}

void msw::CChrCoreDodge::ArtsDoMove()
{
    if (FsmqGetState() == STATE_AIR_TRICK) {
        ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
        int nRet = Util_DoJumpMove(m_fFrameStep * m_fTimeScale);
        if (pWork->m_nLockTarget < 0) {
            RotateYRate_(pWork->m_fGoalRotY, 0.2f);
        } else {
            CChrCore *pTarget = CChrMan::GetChrCore(pWork->m_nLockTarget);
            if (pTarget != 0) {
                RotateYRate_(GetRotY_ToChr(pTarget), 0.2f);
            }
        }
        if (nRet == 1) {
            m_UnkEC |= 0x10;
            SetCollBodyPushPower_(5);
            Util_SetEffect(static_cast<SET_EFFECT_TYPE>(0));
            PlayLandSe(1);
        }
        if (nRet != 0) {
            ArtsOnLanding();
        }
    }
}

void msw::CChrCoreDodge::ArtsOnLanding()
{
    if (FsmqGetState() == STATE_AIR_TRICK) {
        ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
        if (m_nStatusFlag & 1) {
            if (m_nUnk17B == 1 && CGameDodge::IsEnemyZoneLimited()) {
                AnimPlay(3, 0x4F, 0, 4.0f, 0.0f);
                pWork->m_nStep = 0x65;
                return;
            }
            AnimPlay(3, 0x4E, 0, 4.0f, 0.0f);
            pWork->m_nStep = 0x66;
            return;
        }
        AnimPlay(3, 0x84, 0, 4.0f, 0.0f);
        ArtsOptAnimPlay(0x84, 0, 4.0f, 0.0f);
        pWork->m_nStep = 0x66;
    }
}

int msw::CChrCoreDodge::IsArtsTrig(long nTrig, float fFrame, float fTrgFrame)
{
    if (FsmqGetState() != STATE_AIR_TRICK) {
        return 0;
    }
    ARTS_WORK *pWork = FsmqGetWork<ARTS_WORK>();
    unsigned long nBit = 1 << nTrig;
    int bTrig = 0;
    if (!(pWork->m_nTrigFlag & nBit) && fFrame <= fTrgFrame) {
        pWork->m_nTrigFlag |= nBit;
        bTrig = 1;
    }
    return bTrig;
}

// The spin attack: the model spins three times while the character hops.
void msw::CChrCoreDodge::CtorStateSpinAttack()
{
    SPIN_ATTACK_WORK *pWork = FsmqGetWork<SPIN_ATTACK_WORK>();
    GetCcParam_();

    MOT_ACT_PARAM param;
    param.m_nUse = 1;
    param.m_fBlend = 0.0f;
    param.m_fFrame = s_fSpinFrame[0];
    param.m_bLoop = 1;
    param.m_fSpeed = 1.0f;
    param.m_nRotAxis = 1;
    param.m_fRotNum = 3.0f;
    param.m_nEffect = -1;
    param.m_nEffNode = -1;
    MotActSet(pWork->m_MotAct, 0x8D, param, 1.0f, 0.0f);

    float fJumpFrame = s_fSpinJumpFrame[0];
    float fHang = (param.m_fFrame - 1.0f) - 2.0f * fJumpFrame;
    if (fHang > 1.0f) {
        pWork->m_fJumpFrame = Util_SetJumpParam(fJumpFrame, fHang, 0.5f);
    } else {
        pWork->m_fJumpFrame = -1.0f;
    }

    long nEff;
    switch (m_nChrId) {
        case 16:
            nEff = -1;
            break;
        case 5:
        case 7:
        case 10:
            nEff = 0x487;
            break;
        default:
            nEff = 0x486;
            break;
    }
    if (nEff >= 0) {
        Eff_CreateAttach(0xF, nEff, m_nEffNodeBlock, 0, 0, 3, 0);
    }
    PlaySePos(0xC1C, 0, 0, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreDodge::LoopStateSpinAttack()
{
    SPIN_ATTACK_WORK *pWork = FsmqGetWork<SPIN_ATTACK_WORK>();
    GetCcParam_();
    MOT_ACT_CONTEXT &rMotAct = pWork->m_MotAct;

    switch (pWork->m_nStep) {
        case 0: {
            int bEnd = MotActUpdate(rMotAct, m_fFrameStep);
            if (pWork->m_fJumpFrame > 0.0f) {
                Util_DoJumpMove(m_fFrameStep);
            }
            bcs::CMdlObj *pMdl = m_pMdl;
            float fFrameMax = rMotAct.m_fFrameMax;
            if (pMdl != 0 && fFrameMax > 0.0f) {
                float fRate = rMotAct.m_fFrame / fFrameMax;
                fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
                float fRot = PI2 * (fRate * rMotAct.m_fRotNum);
                switch (rMotAct.m_nRotAxis) {
                    case 1:
                        pMdl->SetRotY(pWork->m_fRotY + fRot);
                        break;
                    case 2:
                        pMdl->SetRotX(fRot);
                        break;
                    case 3:
                        pMdl->SetRotZ(fRot);
                        break;
                }
            }
            pWork->m_fAtkUnk4C = 1.2f;
            pWork->m_fAtkUnk54 = 1.2f;
            pWork->m_fAtkUnk50 = 2.4f;
            pWork->m_fAtkUnk58 = 0.0f;
            pWork->m_bAttack = 1;
            pWork->m_fTime += m_fFrameStep;
            if (bEnd) {
                pWork->m_nStep++;
                AnimPlay(3, 0x22, 0, 0.0f, 0.0f);
            }
            break;
        }
        case 1:
            pWork->m_bAttack = 0;
            if (IsAnimEnd(3)) {
                FsmqDoChain();
            }
            break;
        default:
            FsmqDoChain();
            break;
    }
}

void msw::CChrCoreDodge::DtorStateSpinAttack()
{
    m_nStatusFlag &= ~0x40000000;
    Eff_Stop(0xF);
}
