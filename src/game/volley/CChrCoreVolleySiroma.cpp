// msw::CChrCoreVolleySiroma (the White Mage) in the volleyball game.
//
// Her block jump puts a wall of light up over the net, her diving receive
// catches the ball with a spell, and her air trick draws a magic circle
// (CC15_VOLLEY::CcSiromaMagicCircle) on the target marker. The circle
// follows the marker until the ball is shot, then shrinks onto the point the
// ball goes to and vanishes with an effect.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/EF/CResource.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameTask.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/math/arithmetic.h"

#include <math.h>

namespace {

    // The two circles of the air trick: their models, and the start and
    // loop motions of each.
    const char *s_apMdlName[2] = {"c15_sp_mg_square00", "c15_sp_mg_square01"};
    const char *s_apAnmName[2][2] = {
        {"c15_sp_mg_square00_start", "c15_sp_mg_square00_loop"},
        {"c15_sp_mg_square01_start", "c15_sp_mg_square01_loop"},
    };

    // (guessed) The effect the circle vanishes with, and the depth sort
    // type of its models (negative: left as it is).
    struct EFFECT_INFO {
        short nEffect;
        short nZSortType;
    };
    const EFFECT_INFO s_DisappearEffect = {0x609, -1};

    // (guessed) The original reads s_DisappearEffect from .rodata in
    // CtorStateDisappear; ours folds the two values, which would drop the
    // table. This dead reference keeps it in place. See notes/decisions.md,
    // "Unreferenced data kept by dead references".
    inline void UnusedReferences()
    {
        (void)&s_DisappearEffect;
    }

    // (guessed helpers) An angle brought into [0, 2 pi), the same into
    // (-pi, pi], and degrees to radians.
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

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    inline float Min(float fValue, float fMax)
    {
        return fValue > fMax ? fMax : fValue;
    }

    inline float Max(float fValue, float fMin)
    {
        return fMin < fValue ? fValue : fMin;
    }

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    // The distance between two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    // (guessed) Where a ball is and where it goes. Only the position is
    // read; the original copies both.
    struct BALL_MOVE {
        BALL_MOVE(const msw::CBall *pBall) : pos(pBall->GetPos()), vel(pBall->GetVelocity()) {}

        bcs::FVEC3 pos;
        bcs::FVEC3 vel;
    };

    // Queues a scene object for deletion and forgets it.
    template <class T>
    inline void DeleteScnObj(T *&pObj)
    {
        if (pObj) {
            bcs::CScnObj::AppendDelayDeleteObject(pObj);
        }
        pObj = 0;
    }
}

namespace msw {
    namespace CC15_VOLLEY {

        // One of the two circles: a model with its animations, playing the
        // start motion once and then the loop. 0x60 bytes. (member names
        // guessed)
        class CcSiromaMagicCircleObj : public CGameTask {
            public:
                CcSiromaMagicCircleObj(bcs::CTask *pParent, bcs::CScnGroup *pGroup, long nType,
                                       nw4r::g3d::ResFile file);
                virtual ~CcSiromaMagicCircleObj();
                virtual void Main();

                bcs::CMdlObj *GetMdl() const { return m_pMdl; }

            private:
                void Create_(bcs::CScnGroup *pGroup, nw4r::g3d::ResFile file);

                nw4r::g3d::ResFile m_File;   // 0x34
                long m_nType;                // 0x38, index in s_apAnmName
                bcs::CMdlObj *m_pMdl;        // 0x3C
                bcs::CAnmMotion *m_pAnmChr;  // 0x40
                bcs::CAnmMatClr *m_pAnmClr;  // 0x44
                bcs::CAnmTexSrt *m_pAnmTexSrt; // 0x48
                bcs::CAnmTexPat *m_pAnmTexPat; // 0x4C
                long m_nChrStep;             // 0x50: 0 start motion, 1 loop, 2 done
                long m_nClrStep;             // 0x54, the same for the colour animation
                long m_nTexSrtStep;          // 0x58 (not used)
                long m_nTexPatStep;          // 0x5C (not used)
        };

        // The air trick's magic circle. 0xB8 bytes. (member names guessed)
        class CcSiromaMagicCircle : public CGameTask, public bcs::CFsm<CcSiromaMagicCircle> {
            public:
                enum {
                    STATE_SLEEP,
                    STATE_APPEAR,
                    STATE_LOOP,      // follows the target marker
                    STATE_REDUCTION, // shrinks onto the target
                    STATE_DISAPPEAR,
                    STATE_NUM
                };

                CcSiromaMagicCircle(bcs::CTask *pParent, CChrCore *pChr, const bcs::FVEC3 &rPos, unsigned long hBall);
                virtual ~CcSiromaMagicCircle();
                virtual void Main();

                // Shrinks onto rPos.
                void StartReduction(const bcs::FVEC3 &rPos)
                {
                    m_TargetPos = rPos;
                    ChangeState(STATE_REDUCTION);
                }

            private:
                // Not zero once the ball has gone its way (no longer held,
                // pulled in or shot by the trick).
                int IsBallReleased_()
                {
                    if (m_hBall == 0) {
                        return 1;
                    }
                    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_hBall));
                    if (pBall == 0) {
                        return 1;
                    }
                    if (pBall->IsVolleyArtsAttach()) {
                        return 0;
                    }
                    if (pBall->IsAttach()) {
                        return 0;
                    }
                    return !pBall->IsArtsShootMode();
                }

                void CtorStateSleep();
                void LoopStateSleep();
                void DtorStateSleep();
                void CtorStateAppear();
                void LoopStateAppear();
                void DtorStateAppear();
                void CtorStateLoop();
                void LoopStateLoop();
                void DtorStateLoop();
                void CtorStateReduction();
                void LoopStateReduction();
                void DtorStateReduction();
                void CtorStateDisappear();
                void LoopStateDisappear();
                void DtorStateDisappear();

                long m_nStep;                // 0x7C
                bcs::FVEC3 m_Pos;            // 0x80
                bcs::FVEC3 m_TargetPos;      // 0x8C
                long m_nPlayerNo;            // 0x98, the White Mage's
                unsigned long m_hBall;       // 0x9C
                float m_fFrame;              // 0xA0
                float m_fWait;               // 0xA4
                bcs::CScnGroup *m_pGroup;    // 0xA8
                CcSiromaMagicCircleObj *m_apObj[2]; // 0xAC
                unsigned long m_hEffect;     // 0xB4
        };
    }
}

msw::CChrCoreVolleySiroma::CChrCoreVolleySiroma()
{
    m_hMagic = 0;
}

msw::CChrCoreVolleySiroma::~CChrCoreVolleySiroma()
{
}

inline void msw::CChrCoreVolleySiroma::ReleaseMagic_()
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hMagic);
    if (pTask && !pTask->IsDelete()) {
        pTask->Delete();
    }
    m_hMagic = 0;
}

void msw::CChrCoreVolleySiroma::Reset_()
{
    ReleaseMagic_();
    CChrCoreVolley::Reset_();
}

// The block: a jump with its own motion.
void msw::CChrCoreVolleySiroma::SubStateBlockJumpActStart_(STATE_WORK_BLOCK_JUMP *pWork)
{
    pWork->m_fActStartFrame = 0.0f;
    pWork->m_fActTrgFrame = GetBallShotTrgFrame_(0x3E, 0.0f);
    SE_PlayWrapAct(0xB, 0, -1);
    AnimPlay(3, 0x3E, 0, 4.0f, 0.0f);
}

// Not zero once the block motion is over. At its trigger frame the wall
// comes up over the net where the ball will cross it (at least 2 high,
// inside the court).
int msw::CChrCoreVolleySiroma::SubStateBlockJumpIsActEnd_(STATE_WORK_BLOCK_JUMP *pWork)
{
    if (pWork->m_fRotY >= 0.0f) {
        RotateYRate_(pWork->m_fRotY, 0.2f);
    }

    float fFrame = AnimGetFrame(3);
    if (fFrame >= pWork->m_fActTrgFrame + GetCcParam_()->m_Unk78) {
        if (pWork->m_nFlag & 2) {
            pWork->m_nFlag &= ~2;
        }
    } else if (fFrame >= pWork->m_fActTrgFrame && !(pWork->m_nFlag & 8) && !(pWork->m_nFlag & 2)) {
        pWork->m_nFlag |= 2;
        GetTransNodePos(pWork->m_EffOfs);
        pWork->m_EffOfs.y = 3.0f;
        bcs::FVEC3 netPos(CStageMan::GetInstance()->GetNetPos());
        pWork->m_EffOfs.z = netPos.z;

        CBall *pBall = GetTargetBall_();
        if (pBall) {
            float fMinY = 2.0f;
            pWork->m_EffOfs.x = pBall->GetPos().x;
            pWork->m_EffOfs.y = pBall->GetPos().y;
            pWork->m_EffOfs.y = Max(pWork->m_EffOfs.y, fMinY);
            if (pBall->IsShootMode() && !pBall->IsFlag1()) {
                BALL_MOVE move(pBall);
                float fLandFrame = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
                bcs::FVEC3 landPos;
                pBall->GetPosAfterFrame(landPos, fLandFrame);
                bcs::FVEC3 point(pWork->m_EffOfs);
                bcs::FVEC3 normal(0.0f, 0.0f, 1.0f);
                if (GetTargetCourtIdx()) {
                    normal.z = -1.0f;
                } else {
                    normal.z = 1.0f;
                }
                bcs::FVEC3 hitPos;
                float fTime =
                    CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(hitPos, normal, point, move.pos, landPos);
                if (fTime > 0.0f && fTime < 1.0f) {
                    pWork->m_EffOfs.x = hitPos.x;
                    pBall->GetPosAfterFrame(landPos, fLandFrame * fTime);
                    pWork->m_EffOfs.y = landPos.y;
                    pWork->m_EffOfs.y = Max(pWork->m_EffOfs.y, fMinY);
                }
            }
        }

        bcs::FVEC4 rect;
        Util_GetGuardCourtRect_(rect);
        pWork->m_EffOfs.x = Clamp(pWork->m_EffOfs.x, rect.x, rect.y);

        bcs::FVEC3 pos;
        bcs::BCS_VecZ_RotY(pos, GetCcParam_()->m_Unk84, GetBlockTargetRot_());
        bcs::FVEC3Add(&pos, &pos, &pWork->m_EffOfs);
        Eff_CreateSimple(0x12, 0x6A5, pos, GetBlockTargetRot_(), 3, 0, 0);
    }
    return IsAnimEnd(3) != 0;
}

int msw::CChrCoreVolleySiroma::SubStateBlockJumpActEnd_(STATE_WORK_BLOCK_JUMP *pWork)
{
    pWork->m_nFlag &= ~2;
    return 1;
}

// A ball hit on the block ends it with its own motion; a hit of kind 0 or 3
// swaps the wall for a hit effect.
void msw::CChrCoreVolleySiroma::SubStateBlockJumpBlockHit_(const CCollDefUnit *pUnit)
{
    if (GetState() == 5) {
        STATE_WORK_BLOCK_JUMP *pWork = static_cast<STATE_WORK_BLOCK_JUMP *>(FsmqGetWork_());
        pWork->m_nFlag = (pWork->m_nFlag & ~6) | 8;
        // (guessed) Compared unsigned (cmplwi in the original); the same for 0 and 3.
        unsigned long nKind = pUnit->GetKind();
        if (nKind == 0 || nKind == 3) {
            Eff_StopForce(0x12);
            bcs::FVEC3 pos;
            bcs::BCS_VecZ_RotY(pos, GetCcParam_()->m_Unk84, GetBlockTargetRot_());
            bcs::FVEC3Add(&pos, &pos, &pWork->m_EffOfs);
            unsigned long hEffect = Eff_CreateSimple(0x6A6, pos, GetBlockTargetRot_(), 3, 0, 0);
            float fZSort = GetCcParam_()->m_Unk88;
            if (hEffect && fZSort != 0.0f) {
                CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                if (pCluster) {
                    pCluster->SetZSortOffset(fZSort);
                }
            }
        }
    }
    m_fBlockUnk888 = 0.0f;
    m_fBlockUnk88C = 0.0f;
    AnimPlay(3, 0x3F, 0, 4.0f, 0.0f);
    DriverSendMessage((CChrDrv::MSG_ID)0x53, 5, 0, 0, 0);
}

void msw::CChrCoreVolleySiroma::SubStateBlockJumpCollSet_(STATE_WORK_BLOCK_JUMP *pWork)
{
    // Not used in this build. A guess at where the original had it: the
    // value is in .rodata between those of the block and receive functions
    // (the other characters' units have the same one).
    float fUnused = 0.23f;

    if (pWork->m_nFlag & 2) {
        UpdateCollDefSetParam_(pWork->m_EffOfs, pWork->m_fCollSize, GetBlockTargetRot_());
    }
}

void msw::CChrCoreVolleySiroma::SubStateSpikeDoSmashSpikeJumpEffect_(STATE_WORK_SPIKE *)
{
    Eff_CreateAttach(0x13, 0x6A8, m_nEffNode, 0, 0, 3, 0);
}

void msw::CChrCoreVolleySiroma::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
    Eff_StopForce(0x13);
    Eff_CreateAttach(0x13, 0x6A9, m_nEffNode, 0, 0, 3, 0);
}

void msw::CChrCoreVolleySiroma::SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *pWork)
{
    Eff_StopForce(0x13);
    CChrCoreVolley::SubStateSpikeDoLandStart_(pWork);
}

void msw::CChrCoreVolleySiroma::DtorStateSpike()
{
    Eff_StopForce(0x13);
    CChrCoreVolley::DtorStateSpike();
}

// The diving receive: she does not dive but catches the ball with a spell.
// Step 0 starts it (turning to the target direction if it is more than 160
// degrees off), 1 waits for the motion, 3 for the catch or miss motion.
// Once the ball is held from step 3 on, the normal receive state takes over
// with the ball's landing point. Returns not zero when the state is over.
int msw::CChrCoreVolleySiroma::SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0: {
            SE_PlayWrapAct(0xD, 0, -1);
            PlayVoice(0x3D, 0);
            SetMoveMotType(0);
            bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
            SetBallCatchEnableRoot_(GetCcParam_()->m_UnkDC, 0.5f, ofs);
            if (pWork->m_fTargetRotY >= 0.0f) {
                float fLimit = 160.0f;
                float fDiff = WrapRadPI(pWork->m_fTargetRotY - pWork->m_fRotY);
                if (fabsf(fDiff) > DegToRad(fLimit)) {
                    pWork->m_fRotY = pWork->m_fTargetRotY;
                }
            }
            Eff_CreateSimple(0x6A7, GetPos(), pWork->m_fRotY, 3, 0, 0);
            AnimPlay(3, 0x1E, 0, 4.0f, 0.0f);
            pWork->m_fTrgFrame = 16.0f;
            pWork->m_nStep = 1;
            break;
        }
        case 1:
            if (IsAnimEnd(3)) {
                SetBallCatchDisable_();
                if (pWork->m_nFlag & 1) {
                    AnimPlay(3, 0x1F, 0, 8.0f, 0.0f);
                    pWork->m_nStep = 3;
                } else {
                    AnimPlay(3, 0x22, 0, 8.0f, 0.0f);
                    pWork->m_nStep = 3;
                }
            }
            break;
        case 3:
            if (IsAnimEnd(3)) {
                pWork->m_nStep = 4;
                bEnd = 1;
            }
            break;
        default:
            bEnd = 1;
            break;
    }

    if (m_nStatusFlag & 1) {
        if (pWork->m_nStep >= 3) {
            CBall *pBall = GetAttachBall();
            long nPose = GetReceivePoseDir_(GetReceiveBaseDir_());
            STATE_WORK_RECEIVE *pReceive = SetStateReceive_(pBall, 0, nPose);
            float fFrame = pBall->GetTouchDownFrame(m_afReceiveHeight[nPose] + GetPosY());
            pBall->GetPosAfterFrame(pReceive->m_BallPos, fFrame);
            pReceive->m_nFlag |= 0x200;
            return 0;
        }

        long nType = 0;
        if (pWork->m_nFlag & 2) {
            nType = 1;
        }
        CBall *pBall = GetAttachBall();
        if (pBall) {
            unsigned long nReact = pBall->GetVolleyReceiveReaction(3, GetCcParam_()->m_Unk04);
            pBall->GetVolleyFlickDownPower(3);
            long nCatch = 0;
            nReact = SubStateReceiveDoCheckDiveReactFlag_(pWork, pBall, nReact);
            if (nReact & 0x100) {
                nCatch = (unsigned char)nReact;
            }
            if (nReact & (0x10000 | 0x8000 | 0x1000)) {
                if (nReact & 0x8000) {
                    nType = 3;
                } else {
                    nType = 2;
                    if (nReact & 0x10000) {
                        nType = 4;
                    }
                }
            }
            pBall->TrgVolleyReceiveReaction(0, 1);
            ReceiveBall_(3, nCatch, -1.0f, nType, pWork->m_nReceiveType, (pWork->m_nFlag >> 8) & 1);
        }
    }

    m_afCountWork[0] = pWork->m_afCountWork[0];
    m_afCountWork[1] = pWork->m_afCountWork[1];
    return bEnd;
}

// The trick goes to the middle of the attack zone of the court the
// character defends.
void msw::CChrCoreVolleySiroma::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    bcs::FVEC4 rect;
    Util_GetGuardCourtAttackZoneRect_(rect);
    pWork->m_TargetPos.Set(0.5f * (rect.x + rect.y), 0.0f, 0.5f * (rect.z + rect.w));
    pWork->m_MoveParam.m_nType = 0xB;

    pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_TargetPos, GetPos());
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
}

float msw::CChrCoreVolleySiroma::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return 30.0f > pWork->m_fTrickFrame ? pWork->m_fTrickFrame : 30.0f;
}

void msw::CChrCoreVolleySiroma::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xA2, 3);
    Eff_CreateAttachAuto(0x16, 0xA3, 3);
}

void msw::CChrCoreVolleySiroma::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0xA4, 3);
    Eff_CreateAttachAuto(0x16, 0xA5, 3);
    SE_PlayMove(8, 0x47D, GetPosPtr(), 0, -1, 0.0f, 1.0f, 0.0f);
}

// The shot goes to a random point of the magic circle (4.6 across, drawn at
// 1.4 times its size, less a margin of 1.4), and the circle shrinks onto it.
void msw::CChrCoreVolleySiroma::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);

    Eff_Stop(0x14);
    Eff_CreateAttachAuto(0x15, 0xA6, 3);
    Eff_CreateAttachAuto(0x16, 0xA7, 3);
    Util_ArtsShootGetTargetPos_(pWork->m_ShootPos, 0);

    float fMargin = 1.4f;
    float fRange = 4.6f * 1.4f - fMargin;
    bcs::FVEC3 ofs(0.0f, 0.0f, 0.0f);
    if (fRange > 0.0f) {
        bcs::BCS_VecZ_RotY(ofs, fRange, WrapRad(bcs::BCS_RandMax(6.2831855f)));
    }
    pWork->m_ShootPos.x += ofs.x;
    pWork->m_ShootPos.z += ofs.z;
    CheckBallTargetRangeSpike_(pWork->m_ShootPos, GetTargetCourtIdx());

    CC15_VOLLEY::CcSiromaMagicCircle *pMagic =
        static_cast<CC15_VOLLEY::CcSiromaMagicCircle *>(bcs::CTaskMan::GetTask(m_hMagic));
    if (pMagic && !pMagic->IsDelete()) {
        pMagic->StartReduction(pWork->m_ShootPos);
    }
}

int msw::CChrCoreVolleySiroma::SubStateAirTrickIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    return CChrCoreVolley::SubStateAirTrickIsEndArtsShot_(pWork);
}

void msw::CChrCoreVolleySiroma::SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *pWork)
{
    CBall *pBall = GetAttachBall();
    DetachBall();
    if (pBall) {
        pWork->m_nFlag |= 1;
        SetTargetMarkerDisable_();
        Util_ArtsShootBall_(pBall, pWork->m_ShootPos);
    }
    SetBallCatchDisable_();
}

// After the shot she is knocked back up over what is left of the motion.
void msw::CChrCoreVolleySiroma::SubStateAirTrickStartArtsHitback_(STATE_WORK_AIR_TRICK *pWork)
{
    ClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fUnk20 = 0.0f;
    float fFrame = AnimGetFrame(3);
    Util_SetJumpParam(1.0f, Max(AnimGetFrameNum(3) - fFrame, 1.0f), 0.0f);
    m_fJumpGravity = -0.0125f;
}

// The target marker comes up, and the magic circle with it.
void msw::CChrCoreVolleySiroma::SubStateAirTrickStartTMDisp_(STATE_WORK_AIR_TRICK *)
{
    SetTargetMarkerEnable_(6, GetPos(), 4);
    m_fTargetMarkerUnk868 = 5.29f;

    ReleaseMagic_();
    bcs::FVEC3 pos(m_TargetMarkerPos);
    CC15_VOLLEY::CcSiromaMagicCircle *pMagic =
        new (0) CC15_VOLLEY::CcSiromaMagicCircle(CGame::GetTempParent(), this, pos, m_hBall);
    m_hMagic = pMagic->GetId();
}

void msw::CChrCoreVolleySiroma::SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *pWork, CBall *pBall)
{
    pWork->m_nAdsorbStep = 0;
    pWork->m_bAdsorbEnd = 0;
    pWork->m_fAdsorbFrameMax = SubStateAirTrickGetBallAdsorbFrame_(pWork);
    pWork->m_fAdsorbFrame = 0.0f;
    pWork->m_AdsorbOfs = m_AdsorbOfs;
    AttachBall(pBall->GetBallNo(), -1, pWork->m_fAdsorbUnk60, 6);
    m_nStatusFlag &= ~0x2C;
}

// The ball comes in over the trick, at most 48 frames.
float msw::CChrCoreVolleySiroma::SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return Min(pWork->m_fTrickFrame, 48.0f);
}

// The ball is pulled in; it is held 10 above the hand. The original also
// scales a lift of 20 by the progress and then drops it.
void msw::CChrCoreVolleySiroma::SubStateAirTrickDoBallAdsorb_(STATE_WORK_AIR_TRICK *pWork)
{
    CBall *pBall = GetAttachBall();
    if (pBall && pWork->m_nAdsorbStep == 0) {
        bcs::FVEC3 pos(0.0f, 20.0f, 0.0f);
        if (pWork->m_bAdsorbEnd == 0 && pWork->m_fAdsorbFrameMax > 0.0f) {
            float fFrameMax = pWork->m_fAdsorbFrameMax;
            pWork->m_fAdsorbFrame += m_fFrameStep * m_fTimeScale;
            pWork->m_fAdsorbFrame = Min(pWork->m_fAdsorbFrame, fFrameMax);
            float fRate = Clamp(pWork->m_fAdsorbFrame / fFrameMax, 0.0f, 1.0f);
            bcs::FVEC3Scale(&pos, &pos, fRate);
            if (fRate >= 1.0f) {
                pWork->m_bAdsorbEnd = 1;
                SubStateAirTrickTrgBallAdsorbEnd_(pWork);
            }
        }
        pos.x = 0.0f;
        pos.y = 10.0f;
        pos.z = 0.0f;
        pBall->SetAttachPos(pos, 0);
    }
}

void msw::CChrCoreVolleySiroma::SubStateAirTrickTrgBallAdsorbEnd_(STATE_WORK_AIR_TRICK *)
{
    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->StopTailEffect();
    }
}

void msw::CChrCoreVolleySiroma::DtorStateAirTrick()
{
    CChrCoreVolley::DtorStateAirTrick();
}

namespace msw {
    namespace CC15_VOLLEY {

        CcSiromaMagicCircleObj::CcSiromaMagicCircleObj(bcs::CTask *pParent, bcs::CScnGroup *pGroup, long nType,
                                                       nw4r::g3d::ResFile file)
            : CGameTask(pParent, -1)
        {
            // (guessed) Written as assignments: with an initialiser list the
            // constructor is small enough to be inlined into its only caller,
            // which the original does not do.
            m_File = file;
            m_nType = nType;
            m_pMdl = 0;
            m_pAnmChr = 0;
            m_pAnmClr = 0;
            m_pAnmTexSrt = 0;
            m_pAnmTexPat = 0;
            m_nChrStep = 0;
            m_nClrStep = 0;
            m_nTexSrtStep = 0;
            m_nTexPatStep = 0;
            Create_(pGroup, file);
        }

        CcSiromaMagicCircleObj::~CcSiromaMagicCircleObj()
        {
            DeleteScnObj(m_pMdl);
        }

        // The model, drawn on top without depth test, with every animation
        // the file has, put in pGroup.
        void CcSiromaMagicCircleObj::Create_(bcs::CScnGroup *pGroup, nw4r::g3d::ResFile file)
        {
            if (!file.IsValid()) {
                return;
            }
            nw4r::g3d::ResMdl mdl = file.GetResMdl(0);
            if (!mdl.IsValid()) {
                return;
            }

            m_pMdl = new bcs::CMdlObj;
            unsigned long anmFlag = 0;
            file.HasResAnmChr(); // (guessed) tested, but adds no flag
            if (file.HasResAnmClr()) {
                anmFlag |= 0x108;
            }
            if (file.HasResAnmTexSrt()) {
                anmFlag |= 0x204;
            }
            if (file.HasResAnmTexPat()) {
                anmFlag |= 0x3;
            }
            if (file.HasResAnmShp()) {
                anmFlag |= 0x7000;
            }
            m_pMdl->Create(mdl, anmFlag, 0);
            m_pMdl->SetExBlendMode(0);
            m_pMdl->OnFlag(0x100);
            m_pMdl->OnFlag(0x4000);
            m_pMdl->MatSetZModeAll(0, GX_ALWAYS, 0);
            m_pMdl->SetDrawPriority(0xC9, 0xC9);
            m_pMdl->OnFlag(0x2000);

            if (file.HasResAnmChr()) {
                m_pAnmChr = new bcs::CAnmMotion;
                if (m_pAnmChr) {
                    m_pAnmChr->Create(m_pMdl, file, 0, 3, 0);
                    m_pMdl->AttachAnmObj(m_pAnmChr, bcs::CMdlObj::ANMOBJTYPE_CHR);
                }
            }
            if (file.HasResAnmClr()) {
                nw4r::g3d::ResAnmClr res = file.GetResAnmClr(0);
                m_pAnmClr = new bcs::CAnmMatClr;
                if (m_pAnmClr) {
                    m_pAnmClr->Create(m_pMdl, res, 0, 0);
                    m_pMdl->AttachAnmObj(m_pAnmClr, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
                }
            }
            if (file.HasResAnmTexSrt()) {
                nw4r::g3d::ResAnmTexSrt res = file.GetResAnmTexSrt(0);
                m_pAnmTexSrt = new bcs::CAnmTexSrt;
                if (m_pAnmTexSrt) {
                    m_pAnmTexSrt->Create(m_pMdl, res, 0, 0);
                    m_pMdl->AttachAnmObj(m_pAnmTexSrt, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
                }
            }
            if (file.HasResAnmTexPat()) {
                nw4r::g3d::ResAnmTexPat res = file.GetResAnmTexPat(0);
                m_pAnmTexPat = new bcs::CAnmTexPat;
                if (m_pAnmTexPat) {
                    m_pAnmTexPat->Create(m_pMdl, res, 0, 0);
                    m_pMdl->AttachAnmObj(m_pAnmTexPat, bcs::CMdlObj::ANMOBJTYPE_3);
                }
            }

            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
            pGroup->AppendChild(m_pMdl);
        }

        // The start motion, then the loop motion once it is over (for the
        // skeletal and the colour animation alike).
        void CcSiromaMagicCircleObj::Main()
        {
            if (m_pMdl == 0) {
                return;
            }

            if (m_pAnmChr) {
                switch (m_nChrStep) {
                    case 0: {
                        nw4r::g3d::ResAnmChr res = m_File.GetResAnmChr(s_apAnmName[m_nType][0]);
                        if (res.IsValid()) {
                            m_pAnmChr->Play(res, 0, 1.0f, 0.0f, 0.0f);
                        }
                        m_nChrStep++;
                        break;
                    }
                    case 1:
                        if (m_pAnmChr->IsEnd()) {
                            nw4r::g3d::ResAnmChr res = m_File.GetResAnmChr(s_apAnmName[m_nType][1]);
                            if (res.IsValid()) {
                                m_pAnmChr->Play(res, 2, 1.0f, 0.0f, 0.0f);
                            }
                            m_nChrStep++;
                        }
                        break;
                }
            }

            if (m_pAnmClr) {
                switch (m_nClrStep) {
                    case 0: {
                        nw4r::g3d::ResAnmClr res = m_File.GetResAnmClr(s_apAnmName[m_nType][0]);
                        if (res.IsValid()) {
                            m_pAnmClr->ChangeRes(m_pMdl->GetResMdl(), res);
                        }
                        m_nClrStep++;
                        break;
                    }
                    case 1:
                        if (m_pAnmClr->IsEnd()) {
                            nw4r::g3d::ResAnmClr res = m_File.GetResAnmClr(s_apAnmName[m_nType][1]);
                            if (res.IsValid()) {
                                m_pAnmClr->ChangeRes(m_pMdl->GetResMdl(), res);
                            }
                            m_nClrStep++;
                        }
                        break;
                }
            }

            m_pMdl->UpdateFrame(GetFrameRateF());
        }

        CcSiromaMagicCircle::CcSiromaMagicCircle(bcs::CTask *pParent, CChrCore *pChr, const bcs::FVEC3 &rPos,
                                                 unsigned long hBall)
            : CGameTask(pParent, -1), m_nStep(0), m_nPlayerNo(-1), m_hBall(hBall), m_pGroup(0)
        {
            m_nPlayerNo = pChr->GetPlayerNo();
            m_Pos = rPos;
            m_apObj[0] = 0;
            m_apObj[1] = 0;
            m_hEffect = 0;

            InitFsm(this, STATE_NUM);
            SetState(STATE_SLEEP, &CcSiromaMagicCircle::CtorStateSleep, &CcSiromaMagicCircle::LoopStateSleep,
                     &CcSiromaMagicCircle::DtorStateSleep);
            SetState(STATE_APPEAR, &CcSiromaMagicCircle::CtorStateAppear, &CcSiromaMagicCircle::LoopStateAppear,
                     &CcSiromaMagicCircle::DtorStateAppear);
            SetState(STATE_LOOP, &CcSiromaMagicCircle::CtorStateLoop, &CcSiromaMagicCircle::LoopStateLoop,
                     &CcSiromaMagicCircle::DtorStateLoop);
            SetState(STATE_REDUCTION, &CcSiromaMagicCircle::CtorStateReduction,
                     &CcSiromaMagicCircle::LoopStateReduction, &CcSiromaMagicCircle::DtorStateReduction);
            SetState(STATE_DISAPPEAR, &CcSiromaMagicCircle::CtorStateDisappear,
                     &CcSiromaMagicCircle::LoopStateDisappear, &CcSiromaMagicCircle::DtorStateDisappear);
            ChangeState(STATE_SLEEP);

            m_pGroup = new (0) bcs::CScnGroup(0x10, 0);
            m_pGroup->m_vTrans = m_Pos;
            m_pGroup->CalcRootMtx();
            bcs::CRenderMan::PushBack(m_pGroup, 5);
        }

        CcSiromaMagicCircle::~CcSiromaMagicCircle()
        {
            if (m_hEffect) {
                CEffectCtrl::EraseAttachEffect(m_hEffect);
                m_hEffect = 0;
            }
            DeleteScnObj(m_pGroup);
        }

        void CcSiromaMagicCircle::Main()
        {
            ExecState();
        }

        void CcSiromaMagicCircle::CtorStateSleep()
        {
        }

        void CcSiromaMagicCircle::LoopStateSleep()
        {
            ChangeState(STATE_APPEAR);
        }

        void CcSiromaMagicCircle::DtorStateSleep()
        {
        }

        // Both circles are made, 4.6 times their size.
        void CcSiromaMagicCircle::CtorStateAppear()
        {
            CGameSnd::Play(0x4E5, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);

            bcs::EF::CResource *pRes = bcs::EF::CResource::GetInstance();
            if (pRes) {
                for (long i = 0; i < 2; i++) {
                    if (s_apMdlName[i]) {
                        nw4r::g3d::ResFile file = pRes->GetG3DResFileByMdlName(s_apMdlName[i]);
                        if (file.IsValid()) {
                            m_apObj[i] = new CcSiromaMagicCircleObj(this, m_pGroup, i, file);
                            bcs::CMdlObj *pMdl = m_apObj[i]->GetMdl();
                            if (pMdl) {
                                pMdl->m_vScale.x = 4.6f;
                                pMdl->m_vScale.y = 4.6f;
                                pMdl->m_vScale.z = 4.6f;
                            }
                        }
                    }
                }
            }
        }

        void CcSiromaMagicCircle::LoopStateAppear()
        {
            ChangeState(STATE_LOOP);
        }

        void CcSiromaMagicCircle::DtorStateAppear()
        {
        }

        void CcSiromaMagicCircle::CtorStateLoop()
        {
            m_nStep = 0;
        }

        // On the target marker (on the ground) while the ball is held or
        // being shot; gone with the character or the ball.
        void CcSiromaMagicCircle::LoopStateLoop()
        {
            CChrCore *pChr = CChrMan::GetChrCore(m_nPlayerNo);
            if (!pChr || pChr->IsDisable()) {
                ChangeState(STATE_DISAPPEAR);
                return;
            }

            pChr->GetVolleyTargetMarkerPos(m_Pos, 0);
            m_Pos.y = 0.0f;
            m_TargetPos = m_Pos;
            m_pGroup->m_vTrans = m_Pos;
            m_pGroup->CalcRootMtx();

            CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_hBall));
            if (!pBall) {
                ChangeState(STATE_DISAPPEAR);
            } else if (!pBall->IsAttach() && !pBall->IsArtsShootMode()) {
                ChangeState(STATE_DISAPPEAR);
            }
        }

        void CcSiromaMagicCircle::DtorStateLoop()
        {
        }

        void CcSiromaMagicCircle::CtorStateReduction()
        {
            CGameSnd::Play(0x47E, m_TargetPos, 1, -1, 0.0f, 1.0f, 0.0f);
            float fFrame = 30.0f;
            m_fFrame = Max(fFrame, 1.0f);
            m_nStep = 0;
        }

        // Over 30 frames the first circle slides onto the target and shrinks
        // from 4.6 to 1 times its size; the second grows to 7 and fades.
        void CcSiromaMagicCircle::LoopStateReduction()
        {
            if (m_nStep != 0) {
                return;
            }

            m_fFrame -= GetFrameRateF();
            m_fFrame = Max(m_fFrame, 0.0f);
            float fFrameMax = 30.0f;
            fFrameMax = Max(fFrameMax, 1.0f);
            float fRate = Clamp(1.0f - m_fFrame / fFrameMax, 0.0f, 1.0f);

            if (m_pGroup) {
                m_pGroup->CalcRootMtx();
                if (m_apObj[0]) {
                    float fMove = 1.0f - (float)pow(1.0f - fRate, 3.3333333f);
                    bcs::FVEC3 ofs;
                    bcs::FVEC3Sub(&ofs, &m_TargetPos, &m_Pos);
                    ofs.x *= fMove;
                    ofs.z *= fMove;
                    bcs::CMdlObj *pMdl = m_apObj[0]->GetMdl();
                    if (pMdl) {
                        pMdl->m_vTrans = ofs;
                    }
                    float fScale = fMove * -3.6f + 4.6f;
                    pMdl = m_apObj[0]->GetMdl();
                    if (pMdl) {
                        pMdl->m_vScale.x = fScale;
                        pMdl->m_vScale.y = fScale;
                        pMdl->m_vScale.z = fScale;
                    }
                }
                if (m_apObj[1]) {
                    float fAlpha = (float)pow(1.0f - fRate, 2.0f);
                    float fScale = (1.0f - fAlpha) * 2.4f + 4.6f;
                    bcs::CMdlObj *pMdl = m_apObj[1]->GetMdl();
                    if (pMdl) {
                        pMdl->m_vScale.x = fScale;
                        pMdl->m_vScale.y = fScale;
                        pMdl->m_vScale.z = fScale;
                    }
                    pMdl = m_apObj[1]->GetMdl();
                    if (pMdl) {
                        pMdl->SetAlphaRateF(fAlpha);
                    }
                }
            }

            if (m_fFrame <= 0.0f) {
                ChangeState(STATE_DISAPPEAR);
            }
        }

        void CcSiromaMagicCircle::DtorStateReduction()
        {
        }

        // The circle settles on the target with the vanishing effect.
        void CcSiromaMagicCircle::CtorStateDisappear()
        {
            m_Pos = m_TargetPos;
            CGameSnd::Play(0x47F, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);

            if (m_hEffect) {
                CEffectCtrl::StopAttachEffect(m_hEffect);
                m_hEffect = 0;
            }

            const EFFECT_INFO info = s_DisappearEffect;
            const EFFECT_INFO *pInfo = &info;
            long nZSortType = pInfo->nZSortType;
            if (pInfo->nEffect >= 0) {
                m_hEffect = CEffectCtrl::CreateAttachEffect(pInfo->nEffect, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                if (m_hEffect) {
                    CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
                    if (pCluster) {
                        pCluster->SetTranslate(&m_Pos);
                        if (nZSortType >= 0) {
                            long nNum = pCluster->GetMdlObjNum();
                            for (long i = 0; i < nNum; i++) {
                                bcs::CMdlObj *pMdl = pCluster->GetMdlObj(i);
                                if (pMdl) {
                                    pMdl->OnFlag(0x50000);
                                    pMdl->SetBBoxZSortCalcType(nZSortType);
                                }
                            }
                        }
                    }
                }
            }

            m_nStep = 0;
            m_fWait = 10.0f;
        }

        // Waits for the ball to be gone, fades the first circle out over 10
        // frames, then ends with the effect.
        void CcSiromaMagicCircle::LoopStateDisappear()
        {
            if (m_fWait > 0.0f) {
                m_fWait -= GetFrameRateF();
            }

            switch (m_nStep) {
                case 0:
                    if (IsBallReleased_()) {
                        m_fFrame = 10.0f;
                        m_nStep++;
                    }
                    break;
                case 1:
                    m_fFrame -= GetFrameRateF();
                    m_fFrame = Max(m_fFrame, 0.0f);
                    if (m_apObj[0]) {
                        float fFrameMax = 10.0f;
                        if (fFrameMax > 0.0f) {
                            float fAlpha = Clamp(m_fFrame / fFrameMax, 0.0f, 1.0f);
                            bcs::CMdlObj *pMdl = m_apObj[0]->GetMdl();
                            if (pMdl) {
                                pMdl->SetAlphaRateF(fAlpha);
                            }
                        }
                    }
                    if (m_fFrame <= 0.0f) {
                        m_nStep++;
                    }
                    break;
                default: {
                    int bEnd = 1;
                    if (m_hEffect && CEffectCtrl::GetObject(m_hEffect)) {
                        bEnd = 0;
                    }
                    if (bEnd) {
                        Delete();
                    }
                    break;
                }
            }
        }

        void CcSiromaMagicCircle::DtorStateDisappear()
        {
        }
    }
}
