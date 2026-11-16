// msw::CBallBasket, the ball of the basketball game, and
// msw::CBallBasketDummy.
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallInline.hpp"
#include "game/CGoalObj.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBallManBasket.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBasket.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CItemBallBasketKinds.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CTournament.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/FMTX34.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"
#include "revolution/mtx.h"

#include <math.h>

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it (see
// CBallBasketFruit.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace {

    // The stage being played.
    inline long GetStage()
    {
        return msw::CGame::g_GameWork.m_nStage;
    }

    // The courts of the Hungry Boss Packun (stages 70 to 72) and stage 7,
    // which have their own ball rules.
    inline int IsSpecialCourt()
    {
        long nStage = GetStage();
        return (nStage >= 70 && nStage <= 72) || nStage == 7;
    }

    // The sound of the ball hitting the ring.
    inline long GetRingHitSe()
    {
        long nStage = GetStage();
        if (nStage >= 70 && nStage <= 72) {
            return 0xDCC;
        }
        if (nStage == 7) {
            return 0xD43;
        }
        return 0xC68;
    }

    // The flags DetectMapHit takes for the ball (the party game has its
    // own walls).
    inline unsigned long GetMapHitFlag()
    {
        return msw::CGame::g_GameWork.m_nGameType == msw::CGame::GAME_TYPE_PARTY ? 0x1000000 : 0;
    }

    inline float Min(float a, float b)
    {
        return a > b ? b : a;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    // A random value between a and b, whichever is larger.
    inline float RandBetween(float a, float b)
    {
        if (a > b) {
            float t = a;
            a = b;
            b = t;
        }
        return bcs::BCS_RandRange(a, b);
    }

    // rOut = rIn flattened onto the XZ plane and scaled to length 1 (zero
    // for a vertical rIn).
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::FVEC3 xz(rIn.x, 0.0f, rIn.z);
        bcs::BCS_Vec_Normalize(rOut, xz);
    }

    // rAccel += rCurve * fTime.
    inline void AddCurveAccel(bcs::FVEC3 &rAccel, const bcs::FVEC3 &rCurve, float fTime)
    {
        bcs::FVEC3 curve;
        bcs::FVEC3Scale(&curve, &rCurve, fTime);
        bcs::FVEC3Add(&rAccel, &rAccel, &curve);
    }

    // One step of a flight: rMove = how far it goes in fTime frames, then
    // rVel takes rAccel.
    inline void StepFlight(bcs::FVEC3 &rMove, bcs::FVEC3 &rVel, const bcs::FVEC3 &rAccel, float fTime)
    {
        bcs::FVEC3 half;
        bcs::FVEC3Scale(&rMove, &rVel, fTime);
        bcs::FVEC3Add(&rVel, &rVel, &rAccel);
        bcs::FVEC3Scale(&half, &rAccel, 0.5f * fTime);
        bcs::FVEC3Add(&rMove, &rMove, &half);
    }

    // The change of velocity over fTime frames: the XZ deceleration along
    // rDir, gravity and (with bCurve) the curve's acceleration rCurve.
    inline void CalcFlightAccel(bcs::FVEC3 &rAccel, const bcs::FVEC3 &rDir, float fDecel, float fGravity, float fTime,
                                int bCurve, const bcs::FVEC3 &rCurve)
    {
        float fDec = fDecel * fTime;
        rAccel.x = rDir.x * fDec;
        rAccel.z = rDir.z * fDec;
        if (bCurve) {
            rAccel.x = rCurve.x * fTime + rAccel.x;
            rAccel.y = fTime * (fGravity + rCurve.y);
            rAccel.z = rCurve.z * fTime + rAccel.z;
        } else {
            rAccel.y = fGravity * fTime;
        }
    }

    // The direction of rVec in the XZ plane as a Y rotation.
    inline float GetRotYXZ(const bcs::FVEC3 &rVec)
    {
        return bcs::ROUND_PI(nw4r::math::Atan2Rad(rVec.x, rVec.z));
    }

    // Whether the point rCross where the step from rFrom along rDir crosses
    // a plane lies within the step (no farther than fLen, not behind).
    inline bool IsInStep(const bcs::FVEC3 &rCross, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rDir, float fLen)
    {
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &rCross, &rFrom);
        bcs::FVEC3 dist = diff;
        if (!(bcs::FVEC3Len(&dist) <= fLen)) {
            return false;
        }
        bcs::FVEC3Sub(&diff, &rCross, &rFrom);
        bcs::FVEC3 ahead = diff;
        return bcs::FVEC3Dot(&ahead, &rDir) >= 0.0f;
    }

    // The character with index nChr, or null when there is none.
    inline msw::CChrCore *GetChrCoreIfValid(long nChr)
    {
        int bValid = 0;
        if (nChr >= 0 && nChr < msw::CChrMan::GetChrCoreNum()) {
            bValid = 1;
        }
        if (bValid) {
            return msw::CChrMan::GetChrCore(nChr);
        }
        return 0;
    }

    // The rotation (x: pitch, y: yaw) of an effect pointing along rDir.
    inline void GetEffectRot(bcs::FVEC3 &rRot, const bcs::FVEC3 &rDir)
    {
        float fLen = bcs::BCS_Vec_GetLenXZ(rDir);
        rRot.x = bcs::ROUND_PI(nw4r::math::Atan2Rad(fLen, rDir.y));
        rRot.x = bcs::ROUND_PI(rRot.x - 3.14159265f / 2.0f);
        rRot.y = GetRotYXZ(rDir);
        rRot.z = 0.0f;
    }
}

namespace msw {

    inline void CBallBasket::SetLooseBall_()
    {
        if (IsShootMode() && !(m_nFlag & FLAG_SCORED)) {
            SetShootResult(0);
        }
        EffectStopTail_();
        if (!IsDummy()) {
            CGame::SetLooseBallMode(1);
        }
        TrgItemEffect_(4);
        m_fGoalInRate = 0.0f;
        m_nFlag |= 0x3000;
        m_nBasketFlag &= ~0x100;
        m_nBasketFlag &= ~0x10;
        if (GetState() > 2) {
            ChangeState(1);
        }
    }

    inline void CBallBasket::SetRollAxis_(const bcs::FVEC3 &rVel)
    {
        bcs::FVEC3 dir = rVel;
        if (rVel.x * rVel.x + rVel.z * rVel.z > 0.0f) {
            NormalizeXZ(dir, rVel);
            bcs::FVEC3 axis(0.0f, 1.0f, 0.0f);
            PSVECCrossProduct((const Vec *)&axis, (const Vec *)&dir, (Vec *)&axis);
            if (axis.x * axis.x + axis.z * axis.z > 0.0f) {
                NormalizeXZ(m_vRotAxis, axis);
                m_fRotSpeed = 15.0f * (3.14159265f / 180.0f);
            }
        }
    }

    CBallBasket::CBallBasket(bcs::CTask *pParent, long nBallNo) : CBall(pParent, nBallNo)
    {
        m_n360 = 0;
        m_nTouchChrNo = -1;
        ResetParam_();
    }

    CBallBasket::~CBallBasket()
    {
    }

    void CBallBasket::ResetParam_()
    {
        CBall::ResetParam_();
        m_nBasketFlag = 0;
        m_nTargetGoalIdx = 0;
        m_nShotChrIdx = 0;
        m_nPassChrIdx = 0;
        m_nPassTargetNo = 0;
        m_nShotTeamIdx = 0;
        m_nHitDefChrNo = -1;
        m_nAttachNode = 0;
        m_fChargePower = 0.0f;
        m_fGoalInRate = 0.0f;
        m_fPassTime = 0.0f;
        m_n368 = -1;
        m_nTouchChrNo = -1;
        m_n360 = 0;
        ClearCurve_();
        m_fNoCatchCount = 0.0f;
        m_fTimer37C = 0.0f;
        m_fDefCounter = 0.0f;
        m_fArtsShotUnk384 = 0.25f;
    }

    void CBallBasket::ResetOnAttach_()
    {
        m_nBasketFlag &= ~0x7D;
    }

    void CBallBasket::ResetOnDetach_()
    {
        m_nAttachNode = 0;
        m_nBasketFlag &= ~0x6D;
        if (m_nBasketFlag & 0x4000) {
            m_nBasketFlag &= ~0x4000;
            m_nFlag &= ~0x4;
        }
        m_nBasketFlag &= ~0x400000;
        Eff_Stop(0xF);
    }

    void CBallBasket::CreateCollBody_()
    {
        if (m_pMdlObj != 0) {
            m_pBodyColl = new (0) CBodyCollision(2, 2, 0, m_pMdlObj->GetHandle());
            m_pBodyColl->SetPushEnable(0);
            m_pBodyColl->SetBodyEnable(1);
            m_pBodyColl->SetIndexAll(m_nBallNo);
            m_pBodyColl->SetMyNoHitFlagAll(1 << (m_nBallNo + 8));
            m_pBodyColl->SetPushRank(0, 0);
            m_pBodyColl->SetPushPower(0, 0);
            m_pBodyColl->SetBodyDamageCallbackAll(CallbackBodyDamage, this);
            m_pBodyColl->SetBodyPushCallbackAll(CallbackBodyPush, this);
        }
    }

    void CBallBasket::CreateCollAttack_()
    {
        if (m_pMdlObj != 0) {
            m_pAtkColl = new (0) CAtkCollision(1, 2, 0, m_pMdlObj->GetHandle());
            m_pAtkColl->SetIndexAll(m_nBallNo);
            m_pAtkColl->SetMyNoHitFlagAll(1 << (m_nBallNo + 8));
            m_pAtkColl->SetAttackHitCallbackAll(CallbackAttackHit, this);
            m_pAtkColl->SetDisableAll();
            m_pAtkColl->SetTargetFlagAll(2);
        }
    }

    void CBallBasket::Main()
    {
        CBall::Main();
    }

    void CBallBasket::Reset()
    {
        DetachItemEffect();
        EffectStopTail_();
        ResetParam_();
        ChangeState(0);
    }

    void CBallBasket::UpdateCollBody_()
    {
        CBall::UpdateCollBody_();
    }

    void CBallBasket::UpdateCollAtk_()
    {
        if (m_pAtkColl != 0 && m_pAtkColl->IsEnable(0)) {
            CCollUnit *pUnit = m_pAtkColl->GetColl(0);
            pUnit->m_Size.x = m_fAtkRadius;
            pUnit->m_Size.y = 0.0f;
            pUnit->m_Size.z = 0.0f;
            pUnit->SetPos(m_Pos);
        }
    }

    long CBallBasket::CallbackBodyDamage_(const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos)
    {
        if (IsShootMode() || IsArtsShootMode()) {
            SetShootResult(0);
        }
        if (pAtk->m_nNoHitFlag & (1 << (m_nBallNo + 8))) {
            return 0;
        }

        if (!IsAttach()) {
            // A character in state 9 does not knock a passed ball away.
            if (pAtk->GetOwnerKind() == 1 && pAtk->m_nAtkType == 0x20000) {
                CChrCore *pChr = CChrMan::GetChrCore(pAtk->m_nOwnerIdx);
                if (pChr != 0 && IsPassMode() && pChr->GetState() == 9) {
                    return 0;
                }
            }
            if (!IsInvincible() && (TrgItemEffect_(7) & 4)) {
                // Knocked away from the hit, upwards.
                bcs::FVEC3 dir;
                bcs::FVEC3Sub(&dir, &m_Pos, &rPos);
                if (bcs::BCS_Vec_GetLenXZ(dir) > 0.0f) {
                    NormalizeXZ(dir, dir);
                    dir.y = 1.0f;
                    bcs::BCS_Vec_Normalize(dir, dir);
                } else {
                    dir.x = 0.0f;
                    dir.y = 1.0f;
                    dir.z = 0.0f;
                }
                bcs::FVEC3Scale(&dir, &dir, 0.08f);
                SetBound_(dir);
                SetLooseBall_();
                if (pAtk->GetOwnerKind() == 1) {
                    m_nTouchChrNo = pAtk->m_nOwnerIdx;
                }
                return 0x101;
            }
        } else if (m_pBodyDamageHook != 0) {
            long nResult = m_pBodyDamageHook(this, pAtk, rPos, m_pBodyDamageHookArg);
            if (nResult != 0 && pAtk->GetOwnerKind() == 1) {
                m_nTouchChrNo = pAtk->m_nOwnerIdx;
            }
            return nResult;
        }
        return 2;
    }

    void CBallBasket::CallbackBodyPush_(CCollBodyUnit *pSelf, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        // The slowest sideways speed of a ball pushed on the Hungry Boss
        // Packun court.
        static const float fMinSpeedX = 0.03f;

        if (pPush == 0) {
            return;
        }
        // In game state 5 a character only pushes a shot ball, or one it
        // could catch.
        if (CGame::m_pInstance->GetState() == 5 && pOther->GetOwnerKind() == 1) {
            if (IsShootMode() || IsArtsShootMode()) {
                return;
            }
            CChrCore *pChr = CChrMan::GetChrCore(pOther->m_nOwnerIdx);
            if (pChr == 0 || pChr->CanbeCatch()) {
                return;
            }
        }
        if (pOther->m_nNoHitFlag & (1 << (m_nBallNo + 8))) {
            return;
        }
        if (IsAttach()) {
            pOther->DoBodyPushCallback(pSelf, 0);
            return;
        }
        if (pPush == 0) {
            return;
        }

        bcs::FVEC3 from = m_Pos;
        bcs::FVEC3 to(from.x + pPush->x, from.y, from.z + pPush->z);
        bcs::FVEC3 pos;
        bcs::FVEC3 normal;
        float fTime;
        if (CCollMan::DetectMapHit(pos, normal, fTime, from, to, m_pBodyColl->GetRadius(0), 2, GetMapHitFlag())) {
            // Against the wall: the pusher takes back what the ball could
            // not move.
            if (pOther->GetOwnerKind() == 2) {
                normal.x = pos.x - to.x;
                normal.z = pos.z - to.z;
                pOther->DoBodyPushCallback(pSelf, &normal);
            } else {
                pOther->DoBodyPushCallback(pSelf, 0);
            }
        } else {
            pOther->DoBodyPushCallback(pSelf, 0);
        }
        SetPos(pos);
        if (m_pBodyColl != 0) {
            to.x = pos.x - from.x;
            to.z = pos.z - from.z;
            m_pBodyColl->AddPosXZAll(to, to);
            m_pBodyColl->CalcAABBAll();
        }

        ClearShot_();
        EffectStopTail_();
        m_nFlag &= ~0x10;
        m_fGoalInRate = 0.0f;
        m_fGravity = GetGravityDefault();

        // Rolls away from the pusher at the speed it had (within limits).
        float fSpeed = bcs::FVEC3Len(&m_Velocity);
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &m_Pos, &pOther->m_Pos);
        bcs::FVEC3 dir = diff;
        if (fSpeed > 2.0f) {
            fSpeed = 2.0f;
        } else if (fSpeed < 0.1f) {
            fSpeed = 0.1f;
        }
        bcs::BCS_Vec_Normalize(dir, dir);

        float fSpeedXZ = fSpeed * GetFloorBoundRateXZ();
        float fSpeedY = fSpeed * GetFloorBoundRateY();
        bcs::FVEC3 vel;
        if (GetStage() == 0x11 && pOther->GetOwnerKind() == 5) {
            float fScale = 1.15f * fSpeedXZ;
            vel.y = dir.y * fSpeedY;
            vel.x = dir.x * fScale;
            vel.z = dir.z * fScale;
            if (fabsf(vel.x) < fMinSpeedX) {
                vel.x = dir.x > 0.0f ? fMinSpeedX : -fMinSpeedX;
            }
        } else {
            vel.x = dir.x * fSpeedXZ;
            vel.z = dir.z * fSpeedXZ;
            vel.y = dir.y * fSpeedY;
        }
        m_Velocity = vel;
        SetRollAxis_(vel);

        TrgItemEffect_(5);
        if (GetState() != 1) {
            ChangeState(1);
        }
    }

    void CBallBasket::CallbackAttackHit_(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                         unsigned long nFlag)
    {
        if (IsShootMode() && pBody->GetOwnerKind() == 1) {
            CChrCore *pChr = CChrMan::GetChrCore(pBody->m_nOwnerIdx);
            m_nHitDefChrNo = pChr->GetPlayerNo();
            if (pChr->IsInvincible()) {
                SetHitDef_();
            }
        }
    }

}

namespace CBallBasket_cpp {

    // uncalled stand-in: the original has the weak copies of these two
    // inline functions (and the radius literal) here, left by stripped code
    // that called them directly.
    void UnusedCollRadius(msw::CBall *pBall, msw::CItemBallBasket *pItem)
    {
        pItem->msw::CItemBallBasket::GetDispScale();
        pBall->msw::CBall::GetCollBodyRadiusDefault();
    }
}

namespace msw {

    void CBallBasket::SetBound_(const bcs::FVEC3 &rVel)
    {
        ClearShot_();
        EffectStopTail_();
        SetBlinkDisable();
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag &= ~0x40;
        m_nBasketFlag &= ~0x18;
        m_nFlag &= ~0x10;
        m_nFlag |= 0x20;
        m_Velocity = rVel;
        m_PrevVel = rVel;
        m_PrevPos = m_Pos;
        m_fGoalInRate = 0.0f;
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 15.0f * (3.14159265f / 180.0f);
        m_fNoCatchCount = 30.0f;
        ChangeState(1);
    }

    void CBallBasket::RingHit()
    {
        long nStage = GetStage();
        if ((nStage < 70 || nStage > 72) && nStage != 7) {
            CGoalObj *pGoal = CStageMan::GetInstance()->GetGoal(m_nTargetGoalIdx);
            if (pGoal != 0) {
                pGoal->RingHit();
            }
        }
    }

    unsigned long CBallBasket::SIDtoSeID_(long nId)
    {
        if (nId == 0xC2A) {
            long nSe;
            long nStage = GetStage();
            if (nStage >= 70 && nStage <= 72) {
                nSe = 0xDD0;
            } else {
                // The bounce sounds of the floor materials.
                nSe = 0xC2A;
                switch (CStageMan::GetInstance()->GetHitMaterial(m_Pos)) {
                    case 1:
                        nSe = 0xC2B;
                        break;
                    case 2:
                        nSe = 0xC2C;
                        break;
                    case 3:
                        nSe = 0xC2D;
                        break;
                }
            }
            nId = nSe;
        }
        return nId;
    }

    void CBallBasket::PlaySeArtsOrbit_()
    {
        CChrCore *pChr = CChrMan::GetChrCore(GetShotChrIdx());
        if (pChr == 0) {
            return;
        }
        // The sound of each character's special shot in flight.
        long nSe = -1;
        switch (pChr->GetChrTypeID()) {
            case 1:
                nSe = 0x3F4;
                break;
            case 2:
                nSe = 0x3FD;
                break;
            case 3:
                nSe = 0x405;
                break;
            case 4:
                if (!IsDummy()) {
                    nSe = 0x40E;
                }
                break;
            case 5:
                nSe = 0x416;
                break;
            case 6:
                nSe = 0x420;
                break;
            case 8:
                nSe = 0x433;
                break;
            case 9:
                nSe = 0x43D;
                break;
            case 10:
                nSe = 0x448;
                break;
            case 11:
                nSe = 0x452;
                break;
            case 12:
                nSe = 0x45D;
                break;
            case 15:
                nSe = 0x47A;
                break;
            case 16:
                nSe = 0x482;
                break;
            case 17:
                nSe = 0x48A;
                break;
        }
        if (nSe != -1) {
            SE_Stop(7, 0.0f);
            SE_PlayMove(7, nSe, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    inline long CBallBasket::GetShotChrIdx() const
    {
        return m_nShotChrIdx;
    }

    void CBallBasket::CtorStateFreeMove()
    {
        m_fNoCatchCount = 0.0f;
        m_fTimer37C = 0.0f;
        m_fDefCounter = 0.0f;
        if (m_pBodyColl != 0) {
            m_pBodyColl->SetTmpNoHitFlag(0, 0);
        }
        CBall::CtorStateFreeMove();
        m_nBasketFlag &= ~0x2000;
        ResetBaPad0Shot();
    }

    void CBallBasket::LoopStateFreeMove()
    {
        if (m_fNoCatchCount > 0.0f) {
            m_fNoCatchCount -= m_fFrameRate;
            if (m_fNoCatchCount < 0.0f) {
                m_fNoCatchCount = 0.0f;
            }
        }
        if (m_fDefCounter > 0.0f) {
            m_fDefCounter -= m_fFrameRate;
            if (m_fDefCounter < 0.0f) {
                m_fDefCounter = 0.0f;
            }
        }
        if (m_fTimer37C > 0.0f) {
            m_fTimer37C -= m_fFrameRate;
            if (m_fTimer37C < 0.0f) {
                m_fTimer37C = 0.0f;
            }
        }
        if (m_fTimer320 > 0.0f) {
            m_fTimer320 -= m_fMoveRate;
            if (m_fTimer320 < 0.0f) {
                m_fTimer320 = 0.0f;
            }
        }

        CheckCurveStep_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();

        if ((CGame::g_GameWork.m_nFlag18 & 1) && !IsDummy()) {
            AdjustGravity_();
        }
    }

    void CBallBasket::CtorStateAttach()
    {
        m_fNoCatchCount = 0.0f;
        m_fDefCounter = 0.0f;
        m_fTimer37C = 0.0f;
        ClearCurve_();
        CBall::CtorStateAttach();
        m_nBasketFlag &= ~0x80;
        m_nBasketFlag &= ~0x2000;
        CChrCore *pChr = CChrMan::Util_SearchHoldBallChr(m_nBallNo);
        if (pChr != 0) {
            m_nTouchChrNo = pChr->GetPlayerNo();
        }
    }

    void CBallBasket::LoopStateAttach()
    {
        if (m_nBasketFlag & 0x20) {
            // Just caught: spin about an axis across the direction it came
            // from, a little off at random.
            bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(GetAttachMdlObj());
            bcs::FMTX34 mtx;
            pMdl->GetScnMtxPos(&mtx, m_nAttachNode, bcs::CScnObj::ScnObjMtxType(1));
            bcs::FVEC3 dir;
            dir.x = m_Pos.x - mtx.m[0][3];
            dir.y = mtx.m[1][3];
            dir.z = m_Pos.z - mtx.m[2][3];
            float fRand = (3.14159265f / 180.0f) * bcs::BCS_RandRange(-11L, 11L);
            float fRot = GetRotYXZ(dir);
            fRot = bcs::ROUND_PI(fRot - pMdl->m_fRotY);
            fRot = bcs::ROUND_PI(fRot + fRand);
            bcs::BCS_VecOneX_RotY(m_vRotAxis, fRot);
            m_fRotSpeed = -0.049087387f;
            m_nBasketFlag &= ~0x20;
        } else if (m_nFlag & 0x100) {
            float fRot = bcs::ROUND_PI(m_fRotSpeed * m_fMoveRate);
            float fSin;
            float fCos;
            nw4r::math::SinCosFIdx(&fSin, &fCos, NW4R_MATH_RAD_TO_FIDX(0.5f * fRot));
            Quaternion q;
            q.x = m_vRotAxis.x * fSin;
            q.y = m_vRotAxis.y * fSin;
            q.z = m_vRotAxis.z * fSin;
            q.w = fCos;
            PSQUATMultiply(&q, &m_Rot, &m_Rot);
            PSQUATNormalize(&m_Rot, &m_Rot);

            // The spin dies down.
            float fSpeed = m_fRotSpeed;
            float fDec = 0.05f * fSpeed * m_fMoveRate;
            if (fSpeed < 0.0f) {
                m_fRotSpeed = fSpeed - fDec;
                m_fRotSpeed = m_fRotSpeed > -0.0061086523f ? -0.0061086523f : m_fRotSpeed;
            } else {
                m_fRotSpeed = fSpeed - fDec;
                m_fRotSpeed = m_fRotSpeed < 0.0061086523f ? 0.0061086523f : m_fRotSpeed;
            }
        }
        CalcAttachPosInter_();
    }

    void CBallBasket::DtorStateAttach()
    {
        CBall::DtorStateAttach();
    }

    void CBallBasket::CtorStatePass()
    {
        EffectSetTail_(0x4DA);
        m_nFlag |= 0x3000;
        m_nBasketFlag &= ~0x400000;
        Eff_Stop(0xF);
        m_nBasketFlag &= ~0x2000;
        m_fDefCounter = 30.0f;
    }

    void CBallBasket::LoopStatePass()
    {
        LoopStateFreeMove();
    }

    void CBallBasket::DtorStatePass()
    {
        m_nBasketFlag &= ~0x10000;
        m_nBasketFlag &= ~0x200000;
    }

    void CBallBasket::CtorStatePassmeet()
    {
        EffectSetTail_(0x4DA);
        m_nFlag |= 0x3000;
        m_nBasketFlag &= ~0x400000;
        Eff_Stop(0xF);
        m_nBasketFlag &= ~0x2000;
        m_fDefCounter = 30.0f;
    }

    void CBallBasket::LoopStatePassmeet()
    {
        LoopStateFreeMove();
    }

    void CBallBasket::DtorStatePassmeet()
    {
        m_nBasketFlag &= ~0x200000;
        m_nBasketFlag &= ~0x10000;
    }

    void CBallBasket::CtorStateAlleyoop()
    {
        m_nBasketFlag &= ~0x400000;
        Eff_Stop(0xF);
        m_nBasketFlag &= ~0x2000;
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag |= 0x40;
        m_nFlag &= ~0x40;
        m_nFlag |= 0x3000;
        EffectSetTail_(0x4DA);
        m_fDefCounter = 30.0f;
    }

    void CBallBasket::LoopStateAlleyoop()
    {
        LoopStateFreeMove();
    }

    void CBallBasket::DtorStateAlleyoop()
    {
        m_nBasketFlag &= ~0x200000;
        m_nBasketFlag &= ~0x10000;
    }

    void CBallBasket::CtorStateNormalShoot()
    {
        m_fChargePower = 0.0f;
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag |= 0x40;
        m_nFlag &= ~0x40;
        m_nFlag |= 0x3000;
        TrgItemEffect_(0);

        long nSe = 0xC21;
        if (m_nBasketFlag & 0x2000) {
            nSe = 0xC22;
        }
        SE_PlayMove(7, nSe, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
        m_pBodyColl->SetTargetFlag(0, 0x7F);
        m_nFlag |= 0x1000;
        SetCollBodyRadius(-1.0f);
        m_fNoCatchCount = 0.0f;
        m_fTimer37C = 0.0f;
        m_fDefCounter = 0.0f;

        if (IsItemEffect()) {
            if (m_pBodyColl != 0) {
                m_pBodyColl->ResetTargetFlag(1, 2);
            }
            m_fDefCounter = 0.0f;
            CItemBall *pItem = GetItemEffect_();
            if (pItem != 0) {
                if (pItem->GetType() == 1) {
                    m_fDefCounter = 0.0f;
                }
                long nTail = pItem->GetJumpShootTailEffect();
                if (nTail != 0) {
                    EffectSetTail_(nTail);
                }
            }
            CItemBallBasket *pItemBasket = static_cast<CItemBallBasket *>(GetItemEffect_());
            if (pItemBasket != 0) {
                float fScale = pItemBasket->GetDispScale();
                float fRadius = fScale * GetCollBodyRadiusDefault();
                SetCollPushRadius(fRadius);
                SetCollBodyRadius(fRadius);
                SetCollCatchRadius(fRadius);
                SetScale(fScale);
            }
        } else if (m_nBasketFlag & 0x2000) {
            EffectSetTail_(0x4F0);
        } else {
            EffectSetTail_(0x4D9);
        }

        m_nBasketFlag |= 0x80000;
        if ((m_nCurveStep == 2 || m_nCurveStep == 3) && ((m_nBasketFlag & 0x2000) || GetItemEffect_() != 0)) {
            m_nBasketFlag |= 0x100000;
        }
    }

    void CBallBasket::LoopStateNormalShoot()
    {
        LoopStateFreeMove();
        if (GetStage() == 6) {
            // On stage 6 a ball rebounding off the ring is shown pulled
            // towards the goal.
            if (IsNowRebound()) {
                SetOffsetPosZ(CGame::GetGoalPos(GetTargetGoalIdx()).z > 0.0f ? 0.5f : -0.5f);
                SetOffsetPosY(-0.03f);
            } else {
                bcs::FVEC3 pos;
                bcs::FVEC3Add(&pos, &m_Pos, &m_OffsetPos);
                bcs::FVEC3 newPos = pos;
                ClearOffsetPos();
                SetPos(newPos);
            }
        }
    }

    inline long CBallBasket::GetTargetGoalIdx() const
    {
        return m_nTargetGoalIdx;
    }

    void CBallBasket::DtorStateNormalShoot()
    {
        bcs::FVEC3 pos;
        bcs::FVEC3Add(&pos, &m_Pos, &m_OffsetPos);
        bcs::FVEC3 newPos = pos;
        ClearOffsetPos();
        SetPos(newPos);
        ResetBaPad0Shot();
        SE_Stop(7, 0.0f);
        if (m_pBodyColl != 0) {
            m_pBodyColl->AddTargetFlag(1, 2);
        }
        ResetBodyCollRadius_();
        m_nBasketFlag &= ~0x400000;
        Eff_Stop(0xF);
        m_nBasketFlag &= ~0x2000;
        m_nBasketFlag &= ~0x100004;
    }

    void CBallBasket::GetPosAfterFrame(bcs::FVEC3 &rPos, float fFrame) const
    {
        GetBasketPosAfterFrame(rPos, fFrame, 1);
    }

    void CBallBasket::GetBasketPosAfterFrame(bcs::FVEC3 &rPos, float fFrame, int bCurve) const
    {
        rPos = m_Pos;
        bcs::FVEC3 vel = m_Velocity;
        float fGravity = m_fGravity;
        if (fFrame <= m_fTimer320) {
            return;
        }

        float fTime = fFrame - m_fTimer320;
        float fSpeed = bcs::BCS_Vec_GetLenXZ(vel);
        float fDecel = m_fCurveDecel;
        bcs::FVEC3 dir = vel;
        NormalizeXZ(dir, dir);
        // The ball stops slowing down sideways once its XZ speed is gone.
        if (m_fCurveDecel * fTime + fSpeed < 0.0f) {
            float fStop = bcs::BCS_Abs(fSpeed / m_fCurveDecel);
            bcs::FVEC3 accel;
            CalcFlightAccel(accel, dir, m_fCurveDecel, fGravity, fStop, bCurve, m_vCurveAccel);
            bcs::FVEC3 move;
            StepFlight(move, vel, accel, fStop);
            bcs::FVEC3Add(&rPos, &rPos, &move);
            fTime -= fStop;
            fDecel = 0.0f;
        }
        bcs::FVEC3 accel;
        CalcFlightAccel(accel, dir, fDecel, fGravity, fTime, bCurve, m_vCurveAccel);
        bcs::FVEC3 move;
        StepFlight(move, vel, accel, fTime);
        bcs::FVEC3Add(&rPos, &rPos, &move);

        // A shot stops where it hits the ring.
        if (IsShootMode() || IsArtsShootMode()) {
            bcs::FVEC3 move;
            bcs::FVEC3Sub(&move, &rPos, &m_Pos);
            bcs::FVEC3 normal = move;
            bcs::BCS_Vec_Normalize(normal, normal);
            CollSubCheckGoalIn_(m_nTargetGoalIdx, rPos, normal, m_Pos, rPos, (m_nBasketFlag >> 2) & 1);
        }
    }

    void CBallBasket::CheckCurveStep_()
    {
        switch (m_nCurveStep) {
            case 0:
            case 1:
                m_fCurveTime = 0.0f;
                m_vCurveAccel.x = 0.0f;
                m_vCurveAccel.y = 0.0f;
                m_vCurveAccel.z = 0.0f;
                break;
            case 2:
            case 3:
                if (m_fTimer320 <= 0.0f) {
                    m_fCurveTime -= m_fMoveRate;
                    if (m_fCurveTime <= 0.0f) {
                        m_fCurveTime = 0.0f;
                        m_vCurveAccel.x = 0.0f;
                        m_vCurveAccel.y = 0.0f;
                        m_vCurveAccel.z = 0.0f;
                        m_nCurveStep = 0;
                    }
                }
                break;
        }
    }

    void CBallBasket::FuncMove_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fGravity, float fRate)
    {
        if (fRate <= m_fTimer320) {
            return;
        }
        float fTime = fRate - m_fTimer320;
        float fSpeed = bcs::BCS_Vec_GetLenXZ(rVel);
        bcs::FVEC3 dir = rVel;
        NormalizeXZ(dir, dir);
        // The ball stops slowing down sideways once its XZ speed is gone.
        if (m_fCurveDecel * fTime + fSpeed < 0.0f) {
            float fStop = bcs::BCS_Abs(fSpeed / m_fCurveDecel);
            float fDecel = m_fCurveDecel * fStop;
            bcs::FVEC3 accel(dir.x * fDecel, fGravity * fStop, dir.z * fDecel);
            AddCurveAccel(accel, m_vCurveAccel, fStop);
            bcs::FVEC3 move;
            StepFlight(move, rVel, accel, fStop);
            bcs::FVEC3Add(&rPos, &rPos, &move);
            fTime -= fStop;
            m_fCurveDecel = 0.0f;
        }
        float fDecel = m_fCurveDecel * fTime;
        bcs::FVEC3 accel(dir.x * fDecel, fGravity * fTime, dir.z * fDecel);
        AddCurveAccel(accel, m_vCurveAccel, fTime);
        bcs::FVEC3 move;
        StepFlight(move, rVel, accel, fTime);
        bcs::FVEC3Add(&rPos, &rPos, &move);

        if (m_fPassTime > 0.0f) {
            m_fPassTime -= fTime;
            if (m_fPassTime < 0.0f) {
                m_fPassTime = 0.0f;
            }
        }
    }

    void CBallBasket::CtorStateChargeShoot()
    {
        m_nFlag &= ~0x40;
        m_nFlag |= 0x3000;
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag |= 0x40;
        EffectSetTail_(0x4D9);
        TrgItemEffect_(0);
        if (IsItemEffect() && m_pBodyColl != 0) {
            m_pBodyColl->ResetTargetFlag(1, 2);
        }
    }

    void CBallBasket::LoopStateChargeShoot()
    {
        LoopStateNormalShoot();
    }

    void CBallBasket::DtorStateChargeShoot()
    {
        if (m_pBodyColl != 0) {
            m_pBodyColl->AddTargetFlag(1, 2);
        }
    }

    void CBallBasket::CtorStateLayupShoot()
    {
        long nChr = -1;
        m_nFlag &= ~0x40;
        m_nFlag |= 0x3000;
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag |= 0x40;
        CChrCore *pChr = CChrMan::GetChrCore(GetShotChrIdx());
        if (pChr != 0) {
            nChr = pChr->GetChrTypeID();
        }
        switch (nChr) {
            case 17:
                // (character 17) Its lay-up can be blocked.
                m_nBasketFlag &= ~0x40;
                m_nBasketFlag |= 0x1000;
                EffectSetTail_(0x698);
                break;
            case 15:
                break;
            default:
                EffectSetTail_(0x4D9);
                break;
        }
        m_fChargePower = 0.0f;
        TrgItemEffect_(0);
        if (IsItemEffect() && m_pBodyColl != 0) {
            m_pBodyColl->ResetTargetFlag(1, 2);
        }
        m_fDefCounter = 10.0f;
        m_nBasketFlag |= 0x80000;
    }

    void CBallBasket::LoopStateLayupShoot()
    {
        LoopStateNormalShoot();
    }

    void CBallBasket::DtorStateLayupShoot()
    {
        m_nBasketFlag &= ~0x1000;
        ResetBaPad0Shot();
        if (m_pAtkColl != 0) {
            m_pAtkColl->SetDisable(0);
        }
        if (m_pBodyColl != 0) {
            m_pBodyColl->AddTargetFlag(1, 2);
        }
        m_nBasketFlag &= ~0x4;
    }

    void CBallBasket::CtorStateArtsShoot()
    {
        m_nFlag &= ~0x2000;
        m_nFlag |= 0x1040;
        m_nBasketFlag &= ~0x42;
        PlayArtsEff();
        m_nArtsStep = 0;
        m_fArtsTimer = 0.0f;
        m_fArtsWarpDist = 0.0f;
        m_fArtsWarpCount = 0.0f;
        m_fChargePower = 0.0f;
        TrgItemEffect_(0);
        if (CChrMan::GetChrCore(GetShotChrIdx())->GetChrId() == 12) {
            // Kamek's ball warps in front of the goal: 0, 1 or 2 times.
            const long aWarpRate[] = {0, 30, 70};
            m_fArtsWarpNum = bcs::BCS_LotProbability(aWarpRate, 3);
        } else {
            m_fArtsWarpNum = 0.0f;
        }
        PlaySeArtsOrbit_();
        m_fNoCatchCount = 0.0f;
        m_fTimer37C = 0.0f;
        m_fDefCounter = 0.0f;
        CChrCoreBasket *pChr = static_cast<CChrCoreBasket *>(CChrMan::GetChrCore(GetShotChrIdx()));
        if (pChr != 0) {
            m_fDefCounter = pChr->GetArtsShotRefelectDisableFrame();
        }
        m_nBasketFlag &= ~0x80000;
        m_nBasketFlag |= 0x100000;
    }

    void CBallBasket::LoopStateArtsShoot()
    {
        CChrCore *pChr = CChrMan::GetChrCore(GetShotChrIdx());
        if (pChr->GetChrId() == 12) {
            // Kamek's special shot: the ball vanishes near the goal and comes
            // back somewhere around it.
            float fFrameRate = GetFrameRateF();
            switch (m_nArtsStep) {
                case 0: {
                    long nWarp = m_fArtsWarpCount;
                    if (nWarp < 2) {
                        const float aDist[2][2] = {{4.0f, 4.01f}, {1.5f, 1.51f}};
                        m_fArtsWarpDist = RandBetween(aDist[nWarp][0], aDist[nWarp][1]);
                    }
                    m_nArtsStep++;
                }
                    // fall through
                case 1: {
                    LoopStateFreeMove();
                    if (!(m_fArtsWarpNum > 0.0f)) {
                        break;
                    }
                    bcs::FVEC3 goal = CGame::GetGoalPos(GetTargetGoalIdx());
                    bcs::FVEC3 diff;
                    bcs::FVEC3Sub(&diff, &goal, &m_Pos);
                    bcs::FVEC3 toGoal = diff;
                    if (!(m_fArtsWarpDist >= bcs::BCS_Vec_GetLenXZ(toGoal))) {
                        break;
                    }
                    CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x33), 0, m_nBallNo, 0, 0);
                    EffectStopTail_();
                    CEffectCluster *pEffect =
                        CEffectCtrl::GetInstance()->FindList(CEffectCtrl::CreateEffect(0x590, 3, 0, 0, 0));
                    if (pEffect != 0) {
                        bcs::FVEC3 vel = m_Velocity;
                        bcs::FVEC3 rot;
                        GetEffectRot(rot, vel);
                        pEffect->SetTranslate(&m_Pos);
                        pEffect->SetRotateRad(&rot);
                    }
                    SE_Stop(7, 0.0f);
                    PlaySePos(0x460, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    m_fArtsWarpNum -= 1.0f;
                    m_fArtsWarpCount += 1.0f;
                    m_fArtsTimer = 20.0f;
                    m_nFlag |= 0x4;
                    m_nArtsStep++;
                    break;
                }
                case 2: {
                    m_fArtsTimer -= fFrameRate;
                    if (m_fArtsTimer > 0.0f) {
                        break;
                    }
                    const bcs::FVEC3 &rGoal = CGame::GetGoalPos(GetTargetGoalIdx());
                    bcs::FVEC3 pos;
                    if ((long)m_fArtsWarpCount == 1) {
                        // Somewhere around the goal, not inside a wall.
                        float fRadius = RandBetween(8.0f, 8.01f);
                        for (;;) {
                            pos = m_Pos;
                            float fAngle = RandBetween(20.0f, 40.0f);
                            if (bcs::BCS_RandMax(1L) != 0) {
                                fAngle = -fAngle;
                            }
                            fAngle = (3.14159265f / 180.0f) * fAngle;
                            bcs::BCS_VecZ_RotY(pos, 1.0f,
                                               bcs::ROUND_PI(CChrManBasket::GetRotY_Pos1ToPos2XZ(rGoal, pos) + fAngle));
                            pos.x = pos.x * fRadius + rGoal.x;
                            pos.y = m_Pos.y;
                            pos.z = pos.z * fRadius + rGoal.z;
                            bcs::FVEC3 hitPos;
                            bcs::FVEC3 hitNormal;
                            float fHitTime;
                            if (!CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, pos, pos, m_fAtkRadius, 2,
                                                        GetMapHitFlag())) {
                                break;
                            }
                        }
                        pos.y = 0.5f + rGoal.y;
                    } else {
                        // Right in front of the goal.
                        bcs::BCS_VecZ_RotY(pos, 1.0f, CGame::GetGoalYRot(pChr->GetTargetGoalIdx()));
                        pos.x = pos.x + rGoal.x;
                        pos.y = 0.5f + rGoal.y;
                        pos.z = pos.z + rGoal.z;
                    }
                    WarpPos(pos);
                    m_fArtsTimer = 10.0f;
                    Eff_Stop(8);
                    Eff_CreateAttach(8, 0x591, 0, 0, 3);
                    CEffectCluster *pEffect = static_cast<CEffectCluster *>(Eff_GetCluster(8));
                    if (pEffect != 0) {
                        bcs::FVEC3 diff;
                        bcs::FVEC3Sub(&diff, &CGame::GetGoalPos(GetTargetGoalIdx()), &m_Pos);
                        bcs::FVEC3 dir = diff;
                        bcs::BCS_Vec_Normalize(dir, dir);
                        bcs::FVEC3 rot;
                        GetEffectRot(rot, dir);
                        pEffect->SetRotateRad(&rot);
                    }
                    m_nArtsStep++;
                }
                    // fall through
                case 3:
                    m_fArtsTimer -= fFrameRate;
                    if (m_fArtsTimer > 0.0f) {
                        break;
                    }
                    CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x33), 1, m_nBallNo, 0, 0);
                    m_nArtsStep++;
                    m_fArtsTimer = 15.0f;
                    // fall through
                case 4: {
                    m_fArtsTimer -= fFrameRate;
                    if (m_fArtsTimer > 0.0f) {
                        break;
                    }
                    Eff_Stop(8);
                    CEffectCluster *pEffect =
                        CEffectCtrl::GetInstance()->FindList(CEffectCtrl::CreateEffect(0x592, 3, 0, 0, 0));
                    if (pEffect != 0) {
                        const bcs::FVEC3 &rGoal = CGame::GetGoalPos(GetTargetGoalIdx());
                        bcs::FVEC3 pos = m_Pos;
                        bcs::FVEC3 diff;
                        bcs::FVEC3Sub(&diff, &rGoal, &pos);
                        bcs::FVEC3 dir = diff;
                        bcs::FVEC3 rot;
                        GetEffectRot(rot, dir);
                        pEffect->SetTranslate(&pos);
                        pEffect->SetRotateRad(&rot);
                    }
                    m_fArtsTimer = 15.0f;
                    m_nArtsStep++;
                }
                    // fall through
                case 5:
                    m_fArtsTimer -= fFrameRate;
                    if (m_fArtsTimer > 0.0f) {
                        break;
                    }
                    // Back in play: the shot goes on from here.
                    if ((long)m_fArtsWarpCount == 2) {
                        m_nBasketFlag |= 0x80000;
                    }
                    m_nFlag &= ~0x4;
                    SetArtsShootParam_(1.0f);
                    PlayArtsEff();
                    PlaySePos(0x461, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    PlaySeArtsOrbit_();
                    m_fDefCounter = 0.0f;
                    m_nArtsStep = 0;
                    break;
            }
        } else {
            LoopStateFreeMove();
        }

        // (the names of these constants are from the symbols; the code that
        // read them is gone. static-order swap: declared in reverse.)
        static const float fDist = 1.0f;
        static const float fEndAppendEffectTime = 15.0f;
        static const float fStartAppendEffectTime = 10.0f;
    }

    void CBallBasket::DtorStateArtsShoot()
    {
        SE_Stop(7, 0.0f);
        Eff_Stop(9);
        m_nFlag &= ~0x4;
        m_nBasketFlag &= ~0x181000;
    }

    void CBallBasket::CtorStateItemShoot()
    {
        CBallBasket::CtorStateNormalShoot();
    }

    void CBallBasket::LoopStateItemShoot()
    {
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0 && pItem->GetType() != 3 && m_Pos.y <= 1.9f) {
            // Comes to rest at this height.
            m_fGravity = 0.0f;
            m_Velocity.y = 0.0f;
        }
        LoopStateFreeMove();
    }

    void CBallBasket::DtorStateItemShoot()
    {
        DetachItemEffect();
        if (m_pBodyColl != 0) {
            m_pBodyColl->AddTargetFlag(1, 2);
        }
        ResetBodyCollRadius_();
    }

    void CBallBasket::CtorStateJumpBall()
    {
        CGame::SendMessage(0x28, 0, 0, 0);
        m_nStateStep = 0;
        m_nFlag |= 0x3000;
    }

    void CBallBasket::LoopStateJumpBall()
    {
        if (m_nStateStep == 0) {
            if (!(CGame::g_GameWork.m_nFlag18 & 0x4306)) {
                CalcJumpBallVel(8.5f, 9.0f, 7.0f, 0.0f, 200.0f, 200.0f);
                m_nStateStep++;
            }
        } else {
            LoopStateFreeMove();
        }
    }

    void CBallBasket::DtorStateJumpBall()
    {
        CGame::SetJumpBallMode(0);
        CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x12), 0, 0, 0, 0);
        CGame::SendMessage(0x29, 0, 0, 0);
    }

    void CBallBasket::CheckCollision_()
    {
        CheckCollision__();
    }

    long CBallBasket::CheckCollision__()
    {
        bcs::FVEC3 pos;
        bcs::FVEC3 normal;
        long nHit = CollSubCheckAll_(pos, normal);
        bcs::FVEC3 prevPos = m_Pos;
        if (nHit == 0) {
            m_nFlag &= ~0x80;
            return nHit;
        }

        m_nFlag &= ~0x40;
        m_Pos = pos;
        if (IsActive() && !IsDelete()) {
            DoExplode(3.5f, 0.0f);
        }

        switch (nHit) {
            case 8:
            case 9:
                // Through the ring.
                m_nFlag &= ~0x80;
                m_nBasketFlag |= 0x1;
                VelReduce_(0.95f);
                break;
            case 2:
                // The floor.
                ClearShot_();
                m_nBasketFlag &= ~0x2;
                if (!(m_nFlag & 0x80)) {
                    PlaySePos(0xC2E, &pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    m_nFlag |= 0x80;
                }
                SetPos(pos);
                VelReduce_(GetWallBoundRateXZ());
                RotReduce_(0.95f);
                break;
            case 5:
                // The ring.
                RingHit();
                CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x31), m_nBallNo, 0, 0, 0);
                ClearShot_();
                m_nFlag &= ~0x80;
                m_nBasketFlag &= ~0x2;
                PlaySePos(GetRingHitSe(), &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                VelReduce_(1.625f);
                RotReduce_(0.95f);
                break;
            case 3:
                // The backboard.
                ClearShot_();
                m_nFlag &= ~0x80;
                PlaySePos(0xC69, &pos, 1, -1, 0.0f, 1.0f, 0.0f);
                VelReduce_(0.8f);
                m_Velocity.y *= 0.8f;
                RotReduce_(0.95f);
                break;
            case 6:
                SetHitDef_();
                break;
            case 4:
                // A rebound: the ball is already on its way.
                RingHit();
                CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x31), m_nBallNo, 0, 0, 0);
                m_nFlag &= ~0x80;
                RotReduce_(0.95f);
                SE_Stop(7, 0.0f);
                return nHit;
        }

        // Bounces off the surface.
        SetBlinkDisable();
        bcs::FVEC3 reflect;
        bcs::FVEC3Scale(&reflect, &normal, 2.0f * bcs::FVEC3Dot(&m_Velocity, &normal));
        bcs::FVEC3Sub(&m_Velocity, &m_Velocity, &reflect);
        SetLooseBall_();
        return nHit;
    }

    void CBallBasket::DoExplode(float, float)
    {
    }

    void CBallBasket::SetHighlight_()
    {
        CBall::SetHighlight_();
    }

    void CBallBasket::OnBaPad0Shot()
    {
        m_nBasketFlag |= 0x8000;
    }

    void CBallBasket::ResetBaPad0Shot()
    {
        m_nBasketFlag &= ~0x8000;
    }

    int CBallBasket::IsBaPad0Shot() const
    {
        return (m_nBasketFlag >> 15) & 1;
    }

    long CBallBasket::CollSubCheckAll_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal)
    {
        long nHit;
        if (!(m_nBasketFlag & 0x1)) {
            nHit = CollSubCheckGoalIn_(m_nTargetGoalIdx, rPos, rNormal, m_PrevPos, m_Pos, (m_nBasketFlag >> 2) & 1);
            if (nHit == 8 || nHit == 10) {
                int bBound = 0;
                m_Pos = rPos;
                if (ReboundDemoCheck_(nHit, bBound)) {
                    TrgItemEffect_(1);
                    ReboundDemoSetParam_();
                    nHit = 4;
                } else if (!CheckGoalIn_()) {
                    if (bBound) {
                        RingHit();
                        PlaySePos(GetRingHitSe(), &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                        CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x31), m_nBallNo, 0, 0, 0);
                    }
                    nHit = 0;
                }
            }
        } else {
            nHit = CollSubCheckGoalOut_(m_nTargetGoalIdx, rPos, rNormal, m_PrevPos, m_Pos);
            if (nHit == 0) {
                // Out under the ring: falls straight down.
                m_fGravity = GetGravityDefault();
                m_Velocity.z = 0.0f;
                m_Velocity.x = 0.0f;
                m_nBasketFlag &= ~0x1;
            }
        }
        if (nHit == 0) {
            nHit = CollSubCheckDef_(rPos, rNormal);
        }
        if (nHit == 0) {
            nHit = CollSubCheckBG_(rPos, rNormal);
        }
        return nHit;
    }

    long CBallBasket::CollSubCheckGoalIn_(long nGoal, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, const bcs::FVEC3 &rFrom,
                                          const bcs::FVEC3 &rTo, int bRebound) const
    {
        // Half the size of the box around the ring.
        static const float fSize = 0.5f;

        int bShoot = IsShootMode();
        int bFree = IsFreeMoveMode();
        float fRadius = GetRadius();
        // A curving shot (m_n360 2 or 3) only checks the ring from above
        // once it rebounds.
        int bCurveShot;
        if (bShoot) {
            bCurveShot = 1;
            if (m_n360 != 2 && m_n360 != 3) {
                bCurveShot = 0;
            }
        } else {
            bCurveShot = 0;
        }

        bcs::FVEC3 goal = CGame::GetGoalPos(nGoal);
        if (GetStage() == 6) {
            // (the result is not used)
            IsNowRebound();
        }

        if (bShoot || bFree) {
            if (!bCurveShot || IsNowRebound()) {
                // Down through the plane of the ring.
                bcs::FVEC3 hit;
                bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
                float fTime = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(hit, up, goal, rFrom, rTo);
                float fInRadius;
                if (bRebound) {
                    fInRadius = 0.4f + fRadius;
                } else {
                    fInRadius = 0.1f + fRadius;
                }
                if (fTime >= 0.0f && fTime <= 1.0f) {
                    bcs::FVEC3 hitXZ(hit.x, 0.0f, hit.z);
                    bcs::FVEC3 goalXZ(goal.x, 0.0f, goal.z);
                    float fDist = bcs::FVEC3Dist(&hitXZ, &goalXZ);
                    if (fDist <= fInRadius) {
                        // Clean through.
                        rPos = hit;
                        rNormal.x = 0.0f;
                        rNormal.y = 0.0f;
                        rNormal.z = 0.0f;
                        return 8;
                    }
                    if (fDist <= fSize + fRadius) {
                        // On the rim.
                        rPos = hit;
                        rNormal.x = 0.0f;
                        rNormal.y = 0.0f;
                        rNormal.z = 0.0f;
                        return 10;
                    }
                }
            }

            if (bShoot && !bRebound && (!(CGame::g_GameWork.m_nFlag18 & 1) || IsDummy())) {
                // A fast ball can pass the ring between two frames: check
                // the faces of the box around the ring the step crosses.
                bcs::FVEC3 from = rFrom;
                bcs::FVEC3 to = rTo;
                bcs::FVEC3 move;
                bcs::FVEC3Sub(&move, &to, &from);
                bcs::FVEC3 center = goal;
                bcs::FVEC3 dir = move;
                float fLen = bcs::FVEC3Len(&dir);
                bcs::FVEC3 cross;

                bcs::FVEC3 normalY(0.0f, 1.0f, 0.0f);
                if (bcs::BCS_GetPointToPlaneCross(cross, from, dir, center, normalY) &&
                    IsInStep(cross, from, dir, fLen)) {
                    bcs::FVEC3 off;
                    bcs::FVEC3Sub(&off, &cross, &center);
                    bcs::FVEC3 ofs = off;
                    if (ofs.x >= -fSize && ofs.x <= fSize && ofs.z >= -fSize && ofs.z <= fSize) {
                        rPos = cross;
                        rNormal.x = 0.0f;
                        rNormal.y = 0.0f;
                        rNormal.z = 0.0f;
                        return 8;
                    }
                }
                if (bCurveShot) {
                    bcs::FVEC3 normalX(1.0f, 0.0f, 0.0f);
                    if (bcs::BCS_GetPointToPlaneCross(cross, from, dir, center, normalX) &&
                        IsInStep(cross, from, dir, fLen)) {
                        bcs::FVEC3 off;
                        bcs::FVEC3Sub(&off, &cross, &center);
                        bcs::FVEC3 ofs = off;
                        if (ofs.y >= -fSize && ofs.y <= fSize && ofs.z >= -fSize && ofs.z <= fSize) {
                            rPos = cross;
                            rNormal.x = 0.0f;
                            rNormal.y = 0.0f;
                            rNormal.z = 0.0f;
                            return 8;
                        }
                    }
                    bcs::FVEC3 normalZ(0.0f, 0.0f, 1.0f);
                    if (bcs::BCS_GetPointToPlaneCross(cross, from, dir, center, normalZ) &&
                        IsInStep(cross, from, dir, fLen)) {
                        bcs::FVEC3 off;
                        bcs::FVEC3Sub(&off, &cross, &center);
                        bcs::FVEC3 ofs = off;
                        if (ofs.y >= -fSize && ofs.y <= fSize && ofs.x >= -fSize && ofs.x <= fSize) {
                            rPos = cross;
                            rNormal.x = 0.0f;
                            rNormal.y = 0.0f;
                            rNormal.z = 0.0f;
                            return 8;
                        }
                    }
                }
            }
        }

        // Up against the ring from below.
        int bSkip = 0;
        if (bShoot && bCurveShot && !(m_nFlag & FLAG_SCORED)) {
            bSkip = 1;
        }
        if (!bSkip && rTo.y > rFrom.y) {
            goal.y -= 0.1f;
            bcs::FVEC3 hit;
            bcs::FVEC3 down(0.0f, -1.0f, 0.0f);
            float fTime = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(hit, down, goal, rFrom, rTo);
            if (fTime >= 0.0f && fTime <= 1.0f) {
                bcs::FVEC3 hitXZ(hit.x, 0.0f, hit.z);
                bcs::FVEC3 goalXZ(goal.x, 0.0f, goal.z);
                if (bcs::FVEC3Dist(&hitXZ, &goalXZ) <= 0.4f + fRadius) {
                    rPos = hit;
                    rNormal = down;
                    return 5;
                }
            }
        }

        if (GetStage() == 6 && IsNowRebound()) {
            return 0;
        }

        // The backboard.
        bcs::FVEC3 board = CGame::GetBoardPos(nGoal);
        bcs::FVEC3 boardDir = CGame::GetBoardDir(nGoal);
        bcs::FVEC3 ofs;
        bcs::FVEC3Scale(&ofs, &boardDir, fRadius);
        bcs::FVEC3Add(&board, &board, &ofs);
        bcs::FVEC3 hit;
        float fTime = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(hit, boardDir, board, rFrom, rTo);
        if (fTime >= 0.0f && fTime <= 1.0f && fabsf(hit.x - board.x) < 1.5f && fabsf(hit.y - board.y) < 0.9f) {
            rPos = hit;
            rNormal = boardDir;
            return 3;
        }
        return 0;
    }

    long CBallBasket::CollSubCheckGoalOut_(long nGoal, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, const bcs::FVEC3 &rFrom,
                                           const bcs::FVEC3 &rTo)
    {
        // Under the ring after going through it: kept inside the net.
        bcs::FVEC3 ring;
        CChrManBasket::GetInstance()->GetRingCSetPos_(nGoal, ring);
        float fRadius = m_pBodyColl->GetRadius(0);
        bcs::FVEC3 hitPos;
        bcs::FVEC3 hitNormal;
        float fHitTime;
        if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, CGame::GetGoalPos(nGoal), ring, fRadius, 2,
                                   GetMapHitFlag())) {
            return 0;
        }
        if (rTo.y < ring.y - 0.5f) {
            return 0;
        }

        // The net narrows towards its bottom.
        float fNetRadius = (0.5f - (ring.y - rTo.y)) * 0.4f;
        bcs::FVEC3 dir;
        dir.x = rTo.x - ring.x;
        dir.z = rTo.z - ring.z;
        bcs::FVEC3 dirXZ(dir.x, 0.0f, dir.z);
        if (bcs::BCS_Vec_GetLen(dirXZ) <= fNetRadius) {
            rPos = rTo;
            rNormal.x = 0.0f;
            rNormal.y = 0.0f;
            rNormal.z = 0.0f;
            return 9;
        }
        NormalizeXZ(dir, dir);
        bcs::FVEC3 ofs;
        bcs::FVEC3Scale(&ofs, &dir, fNetRadius);
        bcs::FVEC3Add(&rPos, &ring, &ofs);
        rPos.y = rTo.y;
        rNormal.y = 0.0f;
        rNormal.x = -1.0f * dir.x;
        rNormal.z = -1.0f * dir.z;
        return 8;
    }

    long CBallBasket::CollSubCheckDef_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
    {
        // Above this height a ball pushed along the floor gets no lift.
        static const float BaseHeight = 5.0f;

        int bCanDef = m_fDefCounter <= 0.0f;
        if (!bCanDef) {
            return 0;
        }
        int bNoDef = (m_nBasketFlag & 0x1000) != 0;
        if (bNoDef) {
            return 0;
        }

        CCollDefUnit *pDef = CCollMan::CheckBallReflect(m_PrevPos, m_Pos, GetRadius(), rPos, rVel);
        if (pDef != 0) {
            unsigned long nResult = pDef->DoReflectHitCallback(this, rPos, rVel);
            if (pDef->GetOwnerKind() == 1) {
                m_nHitDefChrNo = pDef->m_nOwnerIdx;
            }
            switch ((unsigned char)nResult) {
                case 1:
                    // Blocked.
                    if (CGame::g_GameWork.m_nGameType == CGame::GAME_TYPE_PARTY) {
                        if (!IsShootMode()) {
                            return 0;
                        }
                    } else if (CGame::g_GameWork.m_nFlag18 & 1) {
                        return 0;
                    }
                    if (pDef->GetOwnerKind() == 1) {
                        m_nTouchChrNo = pDef->m_nOwnerIdx;
                    }
                    return 6;
                case 2:
                    if ((unsigned char)(nResult >> 8) != 1) {
                        break;
                    }
                    if (CGame::g_GameWork.m_nFlag18 & 1) {
                        // Pushed along the floor.
                        float fFloor = GetBgCollRadius();
                        if (m_Pos.y <= fFloor && m_PrevPos.y <= fFloor) {
                            m_Velocity.y = 0.0f;
                            bcs::FVEC3 pos = m_Pos;
                            pos.y = fFloor;
                            WarpPos(pos);
                            RotReduce_(15.0f);
                        } else {
                            RotReduce_(0.95f);
                        }
                        // (the length is computed but not used)
                        bcs::FVEC3Len(&rVel);
                        bcs::BCS_Vec_Normalize(rVel, rVel);
                        rVel.x *= 0.01f;
                        rVel.z *= 0.01f;
                        float fHeight = m_Pos.y;
                        if (fHeight > BaseHeight) {
                            fHeight = BaseHeight;
                        }
                        float fRate = fHeight / BaseHeight;
                        fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
                        rVel.y = 0.025f * (1.0f - fRate);
                        bcs::FVEC3 add = rVel;
                        bcs::FVEC3Add(&m_Velocity, &m_Velocity, &add);
                    } else {
                        // Hit from the side facing away from the goal: the
                        // ball loses speed in the water.
                        bcs::FVEC3 goal = CGame::GetGoalPos(GetTargetGoalIdx());
                        float fGoalRot = CChrManBasket::GetRotY_Pos1ToPos2XZ(m_Pos, goal);
                        float fRot = bcs::ROUND_PI(CChrManBasket::GetRotY_Pos1ToPos2XZ(m_Pos, pDef->m_Pos) - fGoalRot);
                        if (fRot > 3.14159265f) {
                            fRot -= 2.0f * 3.14159265f;
                        }
                        if (fabsf(fRot) > 2.0943952f) {
                            break;
                        }
                        float fSlow = (100 - BASKET_PRM_S03_WATER_SLOWDOWN_RATE) / 100.0f;
                        m_Velocity.x *= fSlow;
                        m_Velocity.y *= fSlow;
                        m_Velocity.z *= fSlow;
                        if (GetState() == 6) {
                            PlaySePos(0xD15, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                        } else {
                            PlaySePos(0xD14, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                        }
                    }
                    SetLooseBall_();
                    break;
            }
        }

        if (m_fDefCounter > 0.0f) {
            // A defender in the line of the shot lowers its chance.
            float fRate = 0.0f;
            bcs::FVEC3 pos;
            if (CCollMan::CheckBallObstruct(m_PrevPos, m_Pos, GetRadius(), m_PosD4, 3.0f, pos, fRate)) {
                m_fDefCounter = 0.0f;
                m_fGoalInRate -= fRate;
                if (Eff_GetEffectID(0) == 0x4D9) {
                    EffectSetTail_(0x4D9);
                }
            }
        }
        return 0;
    }

    long CBallBasket::CollSubCheckBG_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal)
    {
        // (static-order swap: declared in reverse) The fastest a ball
        // leaves the floor downwards after a bounce; the height of the
        // river bed.
        static const float fMax = -0.18f;
        static const float CHECK_HEIGHT = -3.0f;

        bcs::FVEC3 flow(0.0f, 0.0f, 0.0f);
        int bHit = 0;
        long nHit = 0;

        // Carried by a moving floor.
        if (m_Pos.y <= GetBgCollRadius() && m_Velocity.y <= 0.0f) {
            unsigned long nAttr;
            bcs::FVEC3 move;
            CCollMan::GetGrdAttr(m_Pos, nAttr, move);
            if (CCollMan::IsGrdAttrFlow(nAttr)) {
                if (!CCollMan::IsGrdMove(m_Pos)) {
                    RollBall_(move);
                }
                m_Pos.x += move.x;
                m_Pos.z += move.z;
                flow = move;
            }
        }

        if (m_nBasketFlag & 0x20000) {
            // Fallen into the river.
            if (m_Pos.y < CHECK_HEIGHT) {
                m_Pos.y = CHECK_HEIGHT;
                m_Velocity.x = 0.0f;
                m_Velocity.y = 0.0f;
                m_Velocity.z = 0.0f;
                if (!(m_nBasketFlag & 0x40000)) {
                    // The game restarts with the team of the last
                    // character to touch the ball.
                    long nChr = m_nTouchChrNo;
                    long nTeam = CGame::g_GameWork.m_nUserTeam;
                    if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum()) {
                        CChrCore *pChr = CChrMan::GetChrCore(nChr);
                        if (pChr != 0) {
                            nTeam = pChr->GetTeamNo();
                        }
                    }
                    static_cast<CGameBasket *>(CGame::m_pInstance)->SetRiverFallRestart_(nTeam);
                    m_nFlag |= 0x4;
                    EffectStopTail_();
                    Eff_CreateSimple(0x14D, m_Pos, 3, 0);
                    m_fRiverTimer = 120.0f;
                    m_nBasketFlag |= 0x40000;
                    PlaySePos(0xD2A, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    SetLooseBall_();
                }
            }
            return nHit;
        }

        // The walls.
        bcs::FVEC3 hitPos;
        float fHitTime;
        if (CCollMan::DetectMapHit(hitPos, rNormal, fHitTime, m_PrevPos, m_Pos, GetBgCollRadius(), 2,
                                   GetMapHitFlag()) &&
            !(m_nBasketFlag & 0x100000)) {
            bHit |= 1;
        }
        m_PrevPos.x += flow.x;
        m_PrevPos.z += flow.z;
        if (m_nFlag & 0x40) {
            if (!bHit) {
                m_nFlag &= ~0x40;
            } else {
                bHit = 0;
                hitPos = m_Pos;
            }
        }

        if (bHit) {
            float fFloor = GetBgCollRadius();
            hitPos.y = hitPos.y < fFloor ? fFloor : hitPos.y;
            nHit = 2;
            rPos = hitPos;
            if (hitPos.y <= GetBgCollRadius() && m_Velocity.y < 0.0f) {
                if (m_Velocity.y >= -0.0375f) {
                    m_Velocity.y = 0.0f;
                } else {
                    m_Velocity.y *= GetFloorBoundRateY();
                }
            }
            if (GetStage() == 9 || GetStage() == 0x11) {
                RollBall_(flow);
            }
            return nHit;
        }

        // The floor.
        if (m_Pos.y >= GetBgCollRadius()) {
            return nHit;
        }
        if (m_Velocity.y >= 0.0f) {
            return nHit;
        }
        if (CStageMan::GetInstance()->GetFloorAttribute(m_Pos, CStageMan::FLOW_TYPE(6)) == 8) {
            m_nBasketFlag |= 0x20000;
            return nHit;
        }

        int bStop = 0;
        m_nBasketFlag &= ~0x80000;
        rNormal.x = 0.0f;
        rNormal.y = 1.0f;
        rNormal.z = 0.0f;
        float fRest = m_fMoveRate;
        float fFloor = GetBgCollRadius();
        bcs::FVEC3 floor(0.0f, fFloor, 0.0f);
        // Steps of the frame between bounces.
        while (m_Pos.y < fFloor && m_Velocity.y < 0.0f && fRest > 0.0f) {
            bcs::FVEC3 hit;
            float fTime = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(hit, rNormal, floor, m_PrevPos, m_Pos);
            if (fTime < 0.0f || fTime > 1.0f) {
                break;
            }

            // Back to the start of the frame, on to the floor.
            float fStep = fTime * fRest;
            m_Velocity = m_PrevVel;
            float fFall = m_fGravity * fStep;
            bcs::FVEC3 move;
            bcs::FVEC3Scale(&move, &m_Velocity, fStep);
            move.y = 0.5f * (fFall * fStep) + move.y;
            m_Pos = m_PrevPos;
            bcs::FVEC3Add(&m_Pos, &m_Pos, &move);
            m_Velocity.y = m_PrevVel.y + fFall;

            if (m_Velocity.y < -0.0375f) {
                // A bounce.
                m_Velocity.x = 0.3f * flow.x + m_PrevVel.x;
                m_Velocity.z = 0.3f * flow.z + m_PrevVel.z;
                PlaySePos(0xC2A, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                if (CStageMan::GetInstance()->GetHitMaterial(m_Pos) == 3) {
                    Eff_CreateSimple(0x44, m_Pos, 3, 0);
                }
                CalcBoundRandXZBG();
                VelReduce_(GetFloorBoundRateXZ());
                m_Velocity.y *= GetFloorBoundRateY();
                if (m_Velocity.y < fMax) {
                    m_Velocity.y = fMax;
                }
            } else {
                // Too slow to bounce: rolls on.
                VelReduce_(CBallMan::GetFloorRoundDecRateXZ());
                m_Velocity.y = 0.0f;
                bStop = 1;
            }

            // The rest of the frame from the floor.
            fRest -= fStep;
            bcs::FVEC3 reflect;
            bcs::FVEC3Scale(&reflect, &rNormal, 2.0f * bcs::FVEC3Dot(&m_Velocity, &rNormal));
            m_PrevPos = m_Pos;
            bcs::FVEC3Sub(&m_Velocity, &m_Velocity, &reflect);
            float fFall2 = m_fGravity * fRest;
            bcs::FVEC3 move2;
            bcs::FVEC3Scale(&move2, &m_Velocity, fRest);
            move2.y = 0.5f * (fFall2 * fRest) + move2.y;
            m_PrevVel = m_Velocity;
            bcs::FVEC3Add(&m_Pos, &m_Pos, &move2);
            m_Velocity.y = m_Velocity.y + fFall2;
            if (bStop) {
                m_Velocity.y = 0.0f;
            }
        }
        if (m_Pos.y < fFloor) {
            m_Pos.y = fFloor;
        }

        ClearShot_();
        SetBlinkDisable();
        m_nFlag &= ~0x40;
        m_nBasketFlag &= ~0x2;
        RotReduce_(0.95f);
        if (GetState() == 11 || (!(CGame::g_GameWork.m_nFlag18 & 1) && !(m_nBasketFlag & 0x80))) {
            SetLooseBall_();
        }
        m_nBasketFlag &= ~0x80;
        DoExplode(3.5f, 0.0f);
        return 0;
    }

    void CBallBasket::CalcBoundRandXZBG()
    {
    }

    void CBallBasket::SetHitDef_()
    {
        ClearShot_();
        m_nFlag &= ~0x80;
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag &= ~0x8;
        VelReduce_(0.25f);
        // (computed but not used)
        bcs::FVEC3 dir = m_Velocity;
        dir.y = 0.0f;
        bcs::BCS_Vec_GetLenXZ(dir);
        bcs::BCS_Vec_Normalize(dir, dir);
        RotReduce_(0.95f);

        CChrCore *pChr = GetChrCoreIfValid(m_nHitDefChrNo);
        Eff_Stop(0xB);
        if (pChr == 0) {
            Eff_CreateAttach(0xB, 0x4E4, 0, 0, 3);
        }
        // A short hit stop.
        static_cast<CGameBasket *>(CGame::m_pInstance)->StopEffect_(7.0f, 5.0f, 20.0f);
    }

    int CBallBasket::ReboundDemoCheck_(long nHit, int &rbBound)
    {
        rbBound = 0;
        if (!(m_nBasketFlag & 0x8)) {
            // The first time on the ring: decides how the ball bounces.
            CChrManBasket::GetInstance()->ResetClockOver_();
            m_nBasketFlag |= 0x8;
            EffectStopTail_();
            if (nHit == 10) {
                m_fGoalInRate = 0.0f;
            }
            JudgeGoalIn_();
            if (!(CGame::g_GameWork.m_nFlag18 & 0x100)) {
                ReboundDemoMakeParam_(nHit);
            }
            m_nReboundCount = 0;
            if (m_nReboundNum > 0) {
                if (!IsItemEffect()) {
                    static_cast<CGameBasket *>(CGame::m_pInstance)->PlayReboundEffectCamera_(GetTargetGoalIdx());
                }
                return 1;
            }
            rbBound = 1;
            return 0;
        }

        if (nHit == 10 && m_nReboundNum <= 0) {
            ReboundDemoMakeParam_(nHit);
            return 0;
        }
        if (++m_nReboundCount < m_nReboundNum) {
            return 1;
        }
        if (CBallMan::IsInfinityReboundMode()) {
            m_nReboundCount = 0;
            return 1;
        }
        rbBound = 1;
        return 0;
    }

    void CBallBasket::ReboundDemoMakeParam_(long nHit)
    {
        // (stripped constant; name from the symbol)
        static const long i = 0;

        memset(&m_nReboundNum, 0, 0xB4);

        int bRebound = 1;
        if (!(m_nBasketFlag & 0x40) || nHit == 10) {
            bRebound = 0;
        }
        if (CBallMan::IsDisableReboundMode() || (CGame::g_GameWork.m_nFlag18 & 1)) {
            bRebound = 0;
        }
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0 && pItem->IsDisableRebound()) {
            bRebound = 0;
        }
        if (CChrManBasket::GetInstance()->IsCreateBossPackun_()) {
            bRebound = 0;
        }

        if (bRebound) {
            m_nReboundNum = ReboundDemoCalcNum_();
            if (m_nReboundNum > 0) {
                float fRand = bcs::BCS_RandMax(2.0f * 3.14159265f);
                int bInfinity = CBallMan::IsInfinityReboundMode();
                float fFrameMin = 40.0f;
                float fFrameMax = 50.0f;
                float fHeightMin = 1.2f;
                float fHeightMax = 1.5f;
                // (this tests the state, not the stage, against the
                // special courts)
                long nState = GetState();
                if ((nState >= 70 && nState <= 72) || nState == 7) {
                    fFrameMin = BASKET_PRM_S07_REBOUND_FRM_MIN;
                    fHeightMin = BASKET_PRM_S07_BOUND_HEIGHT_MIN;
                    fFrameMax = BASKET_PRM_S07_REBOUND_FRM_MAX;
                    fHeightMax = BASKET_PRM_S07_BOUND_HEIGHT_MAX;
                }
                float fFrame = bcs::BCS_RandRange(fFrameMin, fFrameMax);
                float fHeight = bcs::BCS_RandRange(fHeightMin, fHeightMax);
                float fRot =
                    bcs::ROUND_PI(fRand + (3.14159265f + bcs::BCS_RandRange(-3.14159265f / 4.0f, 3.14159265f / 4.0f)));
                m_aRebound[0].m_fRotY = fRot;
                m_aRebound[0].m_bValid = 1;
                m_aRebound[0].m_fFrame = fFrame;
                m_aRebound[0].m_fDist = bcs::BCS_RandRange(0.25f, 0.3f);
                m_aRebound[0].m_fHeight = fHeight;
                if (!bInfinity && (m_nBasketFlag & 0x10) && m_nReboundNum > 0) {
                    // A ball that will go in drops straight in at the end.
                    m_aRebound[m_nReboundNum - 1].m_fDist = 0.0f;
                }
            }
        }

        // The bounce off the floor after the ring.
        if (IsSpecialCourt()) {
            float fRand = bcs::BCS_RandRange((3.14159265f / 180.0f) * -BASKET_PRM_S07_BOUND_RANG_DEG,
                                             (3.14159265f / 180.0f) * BASKET_PRM_S07_BOUND_RANG_DEG);
            m_fBoundRotY = CGame::GetGoalYRot(m_nTargetGoalIdx) + fRand;
            m_fBoundRotY = bcs::ROUND_PI(m_fBoundRotY);
            m_fBoundAngle = bcs::BCS_RandRange((3.14159265f / 180.0f) * BASKET_PRM_S07_BOUND_ANGLE_MIN,
                                               (3.14159265f / 180.0f) * BASKET_PRM_S07_BOUND_ANGLE_MAX);
            m_fBoundDist = bcs::BCS_RandRange(BASKET_PRM_S07_BOUND_DIST_MIN, BASKET_PRM_S07_BOUND_DIST_MAX);
        } else if (m_nReboundNum > 0) {
            m_fBoundRotY = bcs::BCS_RandRange(-3.14159265f / 2.0f, 3.14159265f / 2.0f);
            m_fBoundAngle = bcs::BCS_RandRange(1.1868238f, 1.2042771f);
            m_fBoundDist = bcs::BCS_RandRange(1.8f, 2.0f);
            m_fBoundRotY = bcs::ROUND_PI(m_fBoundRotY + CGame::GetGoalYRot(m_nTargetGoalIdx));
        } else {
            m_fBoundRotY = bcs::BCS_RandMax(2.0f * 3.14159265f);
            m_fBoundAngle = bcs::BCS_RandRange(3.14159265f / 4.0f, 1.2042771f);
            m_fBoundDist = bcs::BCS_RandRange(3.0f, 5.0f);
            m_fBoundRotY = bcs::ROUND_PI(m_fBoundRotY);
        }
    }

    long CBallBasket::ReboundDemoCalcNum_()
    {
        long nStage = GetStage();
        if (nStage >= 70 && nStage <= 72) {
            return 0;
        }
        if (m_nBasketFlag & 0x2000) {
            return 0;
        }
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0 && pItem->GetType() == 2) {
            return 7;
        }

        float fRate = m_fGoalInRate;
        long nNum = 0;
        if (CBallMan::IsInfinityReboundMode()) {
            return 8;
        }

        // The worse the shot, the more it bounces on the ring.
        switch (m_nShotArea) {
            case 0:
            case 1:
                if (fRate >= 0.8f) {
                    if (bcs::BCS_RandMax(1.0f) < 0.1f) {
                        nNum = 1;
                    }
                } else if (fRate >= 0.6f) {
                    float fRand = bcs::BCS_RandMax(1.0f);
                    if (fRand < 0.05f) {
                        nNum = 2;
                    } else if (fRand < 0.2f) {
                        nNum = 1;
                    }
                } else if (fRate >= 0.4f) {
                    float fRand = bcs::BCS_RandMax(1.0f);
                    if (fRand < 0.1f) {
                        nNum = 2;
                    } else if (fRand < 0.35f) {
                        nNum = 1;
                    }
                } else {
                    float fRand = bcs::BCS_RandMax(1.0f);
                    if (fRand < 0.2f) {
                        nNum = 2;
                    } else if (fRand < 0.5f) {
                        nNum = 1;
                    }
                }
                break;
            case 2:
            case 3:
                if (fRate >= 0.8f) {
                } else if (fRate >= 0.6f) {
                    nNum = !(bcs::BCS_RandMax(1.0f) < 0.9f);
                } else if (fRate >= 0.4f) {
                    float fRand = bcs::BCS_RandMax(1.0f);
                    if (fRand < 0.05f) {
                        nNum = 2;
                    } else if (fRand < 0.2f) {
                        nNum = 1;
                    }
                } else {
                    float fRand = bcs::BCS_RandMax(1.0f);
                    if (fRand < 0.1f) {
                        nNum = 2;
                    } else if (fRand < 0.35f) {
                        nNum = 1;
                    }
                }
                break;
        }
        if (nNum > 0) {
            nNum += bcs::BCS_RandRange(0L, 2L);
        }
        return nNum;
    }

    void CBallBasket::ReboundDemoSetParam_()
    {
        // (stripped constant; name from the symbol)
        static const long i = 0;
        static const float fHeightAttenuation = 0.9f;
        static const float fFrmAttenuation = 0.95f;

        // Off to the point of the next bounce on the ring.
        bcs::FVEC3 ofs;
        bcs::BCS_VecZ_RotY(ofs, m_aRebound[0].m_fDist, m_aRebound[0].m_fRotY);
        bcs::FVEC3 target = CGame::GetGoalPos(m_nTargetGoalIdx);
        bcs::FVEC3Add(&target, &target, &ofs);
        bcs::FVEC3 dir;
        dir.x = target.x - m_Pos.x;
        dir.z = target.z - m_Pos.z;
        bcs::FVEC3 dirXZ(dir.x, 0.0f, dir.z);
        float fSpeed = bcs::BCS_Vec_GetLen(dirXZ) / m_aRebound[0].m_fFrame;
        bcs::BCS_VecZ_RotY(m_Velocity, fSpeed, nw4r::math::Atan2Rad(dir.x, dir.z));
        CalcYVG(m_Pos.y, m_Pos.y + m_aRebound[0].m_fHeight, m_Pos.y - 0.1f, m_aRebound[0].m_fFrame, m_Velocity.y,
                m_fGravity);
        PlaySePos(GetRingHitSe(), &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
        m_nBasketFlag |= 0x4;
        CGame::SetReboundMode(1);

        // The bounce after this one: lower and quicker, the other way.
        float fRot = m_aRebound[0].m_fRotY;
        fRot = bcs::ROUND_PI(fRot + (3.14159265f + bcs::BCS_RandRange(-3.14159265f / 4.0f, 3.14159265f / 4.0f)));
        m_aRebound[0].m_bValid = 1;
        m_aRebound[0].m_fRotY = fRot;
        m_aRebound[0].m_fFrame *= fFrmAttenuation;
        m_aRebound[0].m_fDist = bcs::BCS_RandRange(0.25f, 0.3f);
        m_aRebound[0].m_fHeight *= fHeightAttenuation;
    }

    int CBallBasket::JudgeGoalIn_()
    {
        long nState = GetState();
        if (nState < 6 || nState > 9) {
            m_nBasketFlag &= ~0x10;
            return 0;
        }
        // Mission rules that make a shot of some kind miss.
        switch (m_nShootType) {
            case 0:
            case 1:
                if (CTournament::IsMissionThrowingShootMissing(0)) {
                    m_nBasketFlag &= ~0x10;
                    return 0;
                }
                break;
            case 6:
                if (CTournament::IsMissionLayupShootMissing(0)) {
                    m_nBasketFlag &= ~0x10;
                    return 0;
                }
                break;
        }
        if (CTournament::IsMissionJustShootOnly(0) && !(m_nBasketFlag & 0x2000)) {
            m_nBasketFlag &= ~0x10;
            return 0;
        }
        m_nBasketFlag &= ~0x10;
        if (m_fGoalInRate > 0.0f && bcs::BCS_RandMax(1.0f) <= m_fGoalInRate) {
            m_nBasketFlag |= 0x10;
        }
        return 1;
    }

    int CBallBasket::CheckGoalIn_()
    {
        if (m_nBasketFlag & 0x800) {
            return 0;
        }
        int bGoalIn = 1;
        if (m_nBasketFlag & 0x10) {
            PlaySePos(0xC67, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            TrgGoalIn_();
            SetBlinkDisable();
            m_nFlag &= ~0x10;
        } else {
            // Off the rim, away from the goal.
            PlaySePos(GetRingHitSe(), &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
            TrgItemEffect_(2);
            float fRot = m_fBoundRotY;
            float fAngle = m_fBoundAngle;
            float fDist = m_fBoundDist;
            bcs::FVEC3 goal = CGame::GetGoalPos(m_nTargetGoalIdx);
            bcs::FVEC3 target;
            bcs::BCS_VecZ_RotY(target, fDist, fRot);
            target.x = target.x + goal.x;
            target.z = target.z + goal.z;
            CalcVelByShootAngle(m_Velocity, m_Pos, target, fAngle, -1.0f * GetGravityDefault());
            m_fGravity = GetGravityDefault();
            m_fRotSpeed = 15.0f * (3.14159265f / 180.0f);
            m_nBasketFlag |= 0x2;
            ChangeState(1);
            SetLooseBall_();
            bGoalIn = 0;
            SetShootResult(0);
        }
        m_nBasketFlag &= ~0x14;
        return bGoalIn;
    }

    void CBallBasket::TrgGoalIn(long nChr, long nTeam, long nGoal, long nArea, long nShootType)
    {
        m_nShootType = nShootType;
        m_nBasketFlag |= 0x1;
        m_nTargetGoalIdx = nGoal;
        m_nShotChrIdx = nChr;
        m_nPassChrIdx = nChr;
        m_nShotArea = nArea;
        m_nShotTeamIdx = nTeam;
        TrgGoalIn_();
    }

    void CBallBasket::TrgGoalIn_()
    {
        // (stripped constant; name from the symbol)
        static const float fPower = 0.0f;

        if (!(m_nFlag & FLAG_SCORED)) {
            long nPoint = 3;
            if (m_nShotArea <= 1) {
                nPoint = 2;
            }
            long nType = m_nShootType;
            if (nType >= 9 && nType <= 11) {
                CChrCoreBasket *pChr = static_cast<CChrCoreBasket *>(CChrMan::GetChrCore(GetShotChrIdx()));
                if (pChr != 0) {
                    pChr->PlaySeSPSuccess(m_Pos);
                }
            } else if (nType == 3 || nType == 5 || nType == 8) {
                PlaySePos(0xC6A, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            PlayGoalInEff();

            long nShootType = m_nShootType;
            if (IsBaPad0Shot()) {
                ResetBaPad0Shot();
                long nMsg;
                switch (nShootType) {
                    case 0:
                        nMsg = 8;
                        break;
                    case 1:
                        nMsg = 10;
                        break;
                    case 6:
                        nMsg = 6;
                        break;
                    case 3:
                        nMsg = 4;
                        break;
                    default:
                        goto noMessage;
                }
                CGame::SendMessage(0x2E, nMsg, 0, 0);
            noMessage:;
            }

            // How the ball drops out of the net.
            switch (m_nShootType) {
                case 0:
                case 1:
                case 2:
                case 6:
                case 7:
                    bcs::FVEC3Scale(&m_Velocity, &m_Velocity, 0.3f);
                    break;
                case 3:
                case 5:
                case 8:
                case 10:
                    m_Velocity.x = 0.0f;
                    m_Velocity.y = -0.3f;
                    m_Velocity.z = 0.0f;
                    break;
                case 9:
                    m_Velocity.z = 0.0f;
                    m_Velocity.x = 0.0f;
                    break;
            }
            if (m_Velocity.y > 0.0f) {
                m_Velocity.y = 0.0f;
            }

            SetShootResult(1);
            CGame::TrgBasketGoalIn(m_nShotChrIdx, m_nShotTeamIdx, m_nShootType, nPoint, 0.0f,
                                   CGame::GetGoalPos(CGame::GetTargetGoalIdx(m_nShotTeamIdx)), m_nBallNo);
            m_nFlag |= 0x3001;
            if (CChrManBasket::GetInstance()->IsCreateBossPackun_()) {
                SetLooseBall_();
                SetSleepMode();
            }
        }
        TrgItemEffect_(3);
    }

    // The fewest and most frames of a pass of type nType.
    inline float CBallBasket::GetPassFrameMin_(long nType) const
    {
        const float aFrameMin[7] = {20.0f, 20.0f, 22.0f, 20.0f, 25.0f, 25.0f, 25.0f};
        return aFrameMin[nType];
    }

    inline float CBallBasket::GetPassFrameMax_(long nType) const
    {
        const float aFrameMax[7] = {120.0f, 120.0f, 120.0f, 120.0f, 120.0f, 120.0f, 120.0f};
        return aFrameMax[nType];
    }

    void CBallBasket::SetPassMode(const bcs::FVEC3 &rTarget, long nType, float fSpeed, long nChr, long nTargetNo,
                                  int bArg5, int bArg6, unsigned long nNoHit)
    {
        ClearCurve_();
        Detach_();
        m_nPassType = nType;
        switch (nType) {
            case 1:
                ChangeState(4);
                break;
            case 2:
                ChangeState(5);
                break;
            default:
                ChangeState(3);
                break;
        }
        m_nShotChrIdx = nChr;
        m_nPassChrIdx = nChr;
        m_nPassTargetNo = nTargetNo;
        m_fGoalInRate = 0.0f;
        m_nBasketFlag &= ~0x2;
        if (m_pBodyColl != 0) {
            m_pBodyColl->SetTmpNoHitFlag(0, nNoHit);
        }
        switch (nType) {
            case 2:
                m_nFlag |= 0x40;
                break;
            case 4:
                m_nBasketFlag |= 0x80;
                // fall through
            default:
                m_nFlag &= ~0x40;
                break;
        }
        m_fChargePower = 0.0f;
        float fFrame;
        SetPassModeParam__(nType, rTarget, fSpeed, fFrame, bArg5, bArg6);
    }

    // (name from the symbols of its constants) The top of a lob from rFrom
    // to rTo: higher the farther it goes.
    inline float CBallBasket::GetMidHeight(long, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo) const
    {
        // (static-order swap: declared in reverse)
        static const float fDistMax = 15.0f;
        static const float fDistMin = 5.0f;
        static const float fHeightMax = 3.5f;
        static const float fHeightMin = 1.5f;

        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &rTo, &rFrom);
        bcs::FVEC3 dir = diff;
        float fLen = bcs::BCS_Vec_GetLenXZ(dir);
        fLen = fLen > fDistMax ? fDistMax : (fLen < fDistMin ? fDistMin : fLen);
        float fRate = (fLen - fDistMin) / (fDistMax - fDistMin);
        fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
        return (fHeightMax - fHeightMin) * fRate + fHeightMin;
    }

    float CBallBasket::SetPassModeParam__(long nType, const bcs::FVEC3 &rTarget, float fSpeed, float &rFrame, int, int)
    {
        // The lowest height a pass starts from.
        static const float fMinHeight = 0.7f;

        CChrCore *pChr = GetChrCoreIfValid(m_nPassTargetNo);
        m_fPassTime = 0.0f;
        m_Velocity.x = 0.0f;
        m_Velocity.y = 0.0f;
        m_Velocity.z = 0.0f;
        m_fGravity = GetGravityDefault();
        m_PrevPos = m_Pos;
        m_PosD4 = m_Pos;

        bcs::FVEC3 target = rTarget;
        if (pChr != 0) {
            // To the receiver's hands, except for an alley-oop.
            int bAlleyoop = 0;
            if (nType == 2) {
                bAlleyoop = 1;
            }
            if (!bAlleyoop) {
                bcs::FVEC3 node;
                pChr->GetVNodePos(1, node);
                target.y = node.y;
            }
        }
        if (nType == 4) {
            // A bounce pass: hits the floor 70% of the way.
            bcs::FVEC3 diff;
            bcs::FVEC3Sub(&diff, &target, &m_Pos);
            bcs::FVEC3 dir = diff;
            float fDist = 0.7f * bcs::BCS_Vec_GetLenXZ(diff);
            NormalizeXZ(dir, dir);
            target.y = 0.0f;
            target.x = dir.x * fDist + m_Pos.x;
            target.z = dir.z * fDist + m_Pos.z;
        }
        m_vTargetPos = target;

        // The speed by pass type and game level.
        float fDist;
        if (fSpeed <= 0.0f) {
            float fMin = GetPassFrameMin_(nType);
            float fMax = GetPassFrameMax_(nType);
            fDist = CalcXZVel(m_Pos, target, 0.0f, m_Velocity, rFrame, GetPassRate_(nType), fMin, fMax);
        } else {
            fDist = CalcXZVel(m_Pos, target, 0.0f, m_Velocity, rFrame, GetPassRate_(nType), fSpeed, fSpeed);
        }
        if (fDist <= 0.0f) {
            return 0.0f;
        }

        // The height of the arc.
        float fFrame = rFrame;
        float fTopY = target.y;
        if (nType == 4) {
            if (m_Pos.y < fMinHeight) {
                m_Pos.y = fMinHeight;
            }
        } else {
            fTopY = (m_Pos.y - target.y) * 0.5f + target.y;
            if (nType == 5) {
                // A lob.
                fTopY = GetMidHeight(nType, target, m_Pos);
            }
            if (m_Pos.y < 0.5f) {
                m_Pos.y = 0.5f;
            }
            if (target.y < 0.5f) {
                target.y = 0.5f;
            }
        }
        CalcYVG(m_Pos.y, fTopY, target.y, fFrame, m_Velocity.y, m_fGravity);
        m_fNoCatchCount = 16.0f;
        m_fRotSpeed = 15.0f * (3.14159265f / 180.0f);
        m_fPassTime = fFrame;
        // (the original returns this through a flag that is always set)
        return 9999.9f;
    }

    void CBallBasket::SetShootMode(long nType, float fRate, long nChr, long nArea, unsigned long nNoHit)
    {
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        Detach_();
        m_n368 = pChr->IsPadCtrl() ? pChr->GetPadNo() : -1;
        m_nShootType = nType;
        m_vShootStartPos = m_Pos;
        m_fShootCurveValue = 0.0f;
        if (nType == 6 || nType == 7) {
            ChangeState(8);
        } else if (nType == 9) {
            ChangeState(9);
        } else {
            ChangeState(6);
        }
        m_nFlag &= ~0x6;

        // On stage 6 an ordinary shot only goes in while the chandelier is
        // lit.
        int bCanScore;
        if (GetStage() == 6 && !static_cast<CGameBasket *>(CGame::m_pInstance)->IsChandelierOn_()) {
            switch (m_nShootType) {
                case 0:
                case 1:
                    if ((m_nBasketFlag & 0x2000) != 0 || IsItemEffect()) {
                        bCanScore = 1;
                    } else {
                        bCanScore = 0;
                    }
                    break;
                case 6:
                    bCanScore = 0;
                    break;
                default:
                    bCanScore = 1;
                    break;
            }
        } else {
            bCanScore = 1;
        }
        if (!bCanScore) {
            fRate = 0.0f;
        }

        if (m_pBodyColl != 0) {
            m_pBodyColl->SetTmpNoHitFlag(0, nNoHit);
        }
        long nTeam = pChr->GetTeamNo();
        m_nTargetGoalIdx = pChr->GetTargetGoalIdx();
        m_nShotChrIdx = nChr;
        m_nPassChrIdx = nChr;
        m_nShotArea = nArea;
        m_nShotTeamIdx = nTeam;
        if (nType == 9) {
            SetArtsShootParam_(fRate);
        } else if (IsItemEffect()) {
            SetShootParamItem_();
        } else {
            SetShootParam_(fRate);
        }
    }

    inline void CBallBasket::SetShootParamItem_()
    {
        // How far ahead the bomb goes off.
        static const float fDist = 7.5f;

        m_nBasketFlag &= ~0x400000;
        Eff_Stop(0xF);
        m_nBasketFlag &= ~0x2000;
        CItemBallBasket *pItem = static_cast<CItemBallBasket *>(GetItemEffect_());
        BALL_BASKET::SHOT_PARAM param;
        pItem->GetShotParam_(param);
        bcs::FVEC3 center = CGame::GetGoalPos(m_nTargetGoalIdx);
        if (pItem->GetType() == 3) {
            bcs::FVEC3 diff;
            bcs::FVEC3Sub(&diff, &center, &m_Pos);
            NormalizeXZ(center, diff);
            center.x = fDist * center.x + m_Pos.x;
            center.y = 0.0f;
            center.z = fDist * center.z + m_Pos.z;
            pItem->SetExplodeCenterPos(center);
        }
        if (CChrManBasket::GetInstance()->IsCreateBossPackun_()) {
            param.m_fUnk08 = 1.0f;
        }
        SetShootParamTest2_(param);
    }

    inline void CBallBasket::AddAirBallMiss_(bcs::FVEC3 &rTarget)
    {
        bcs::FVEC3 miss;
        if (GetItemEffect_() != 0) {
            miss.x = 0.0f;
            miss.y = 0.0f;
            miss.z = 0.0f;
        } else if (IsAirBall()) {
            bcs::FVEC3 dir(0.0f, 1.0f, 0.0f);
            bcs::BCS_Vec_RotZ(dir, dir, bcs::BCS_RandRange(0.0f, 2.0f * 3.14159265f));
            CBallManBasket *pBallMan = static_cast<CBallManBasket *>(CBallMan::GetInstance());
            float fMin = pBallMan->GetAirBallRangeDistMin_();
            float fMax = static_cast<CBallManBasket *>(CBallMan::GetInstance())->GetAirBallRangeDistMax_();
            float fDist = bcs::BCS_RandRange(fMin, fMax);
            dir.x *= fDist;
            dir.y *= fDist;
            dir.z *= fDist;
            miss = dir;
        } else {
            miss.x = 0.0f;
            miss.y = 0.0f;
            miss.z = 0.0f;
        }
        bcs::FVEC3Add(&rTarget, &rTarget, &miss);
    }

    float CBallBasket::SetShootParamTest2_(const BALL_BASKET::SHOT_PARAM &rParam)
    {
        // (stripped constant; name from the symbol)
        static const float fSpeedDec = 1.0f;

        m_fPassTime = 0.0f;
        m_fGoalInRate = rParam.m_fUnk08 + CBallMan::GetRaiseGoalInRate();
        m_Velocity.x = 0.0f;
        m_Velocity.y = 0.0f;
        m_Velocity.z = 0.0f;
        m_fGravity = GetGravityDefault();
        m_fUnk374 = rParam.m_fUnk0C;
        m_PrevPos = m_Pos;
        m_PosD4 = m_Pos;

        // An air ball misses the target by a random amount.
        bcs::FVEC3 target = rParam.m_vTarget;
        AddAirBallMiss_(target);

        float fSpeed = (1.0f / 60.0f) * rParam.m_fUnk0C;
        m_fTimer320 = 0.0f;
        m_fCurveDecel = 0.0f;
        m_vCurveAccel.x = 0.0f;
        m_vCurveAccel.y = 0.0f;
        m_vCurveAccel.z = 0.0f;
        float fFrame;
        float fDist =
            CalcXZVel2(m_Pos, target, m_Velocity, m_fCurveDecel, fFrame, fSpeed, fSpeed, rParam.m_fUnk18, rParam.m_fUnk1C);
        m_n360 = 0;
        if (fDist <= 0.0f) {
            return 0.0f;
        }

        float fFlight = ceil(fFrame);
        float fCurve = rParam.m_fCurve;
        m_fShootCurveValue = fCurve;
        // The curve starts and ends on the line of the shot.
        float fSide = 0.0f;
        if (rParam.m_nUnk04 != 0) {
            // A curving shot: bends left or right, as the court allows.
            if (m_nCurveDir != 2 && m_nCurveDir != 4) {
                m_nCurveDir = bcs::BCS_RandRange(0L, 1L) != 0 ? 2 : 4;
            }
            if (!IsDummy()) {
                float fRadius = GetBgCollRadius();
                const bcs::FVEC3 &rGoal = CGame::GetGoalPos(m_nTargetGoalIdx);
                float fCheck = rParam.m_fCurve;
                switch (static_cast<CBallManBasket *>(CBallMan::GetInstance())
                            ->CheckCurveDir_(fCheck, m_Pos, rGoal, fRadius)) {
                    case 2:
                        m_nCurveDir = 2;
                        break;
                    case 4:
                        m_nCurveDir = 4;
                        break;
                }
            }
            switch (m_nCurveDir) {
                case 2:
                    m_nCurveStep = 2;
                    break;
                case 4:
                    m_nCurveStep = 3;
                    break;
            }

            // Sideways acceleration that takes the ball out by fCurve and
            // back over the flight.
            m_fCurveTime = 0.0f;
            bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
            bcs::FVEC3 dir = m_Velocity;
            NormalizeXZ(dir, m_Velocity);
            PSVECCrossProduct((const Vec *)&up, (const Vec *)&dir, (Vec *)&m_vCurveAccel);
            bcs::FVEC3 side;
            NormalizeXZ(side, m_vCurveAccel);
            float fVel = 0.0f;
            float fAccel = 0.0f;
            bcs::BCS_Phys_CalcYVG(fFlight, fSide, -fSide, fCurve, fVel, fAccel);
            if (m_nCurveStep == 3) {
                fVel = -fVel;
                fAccel = -fAccel;
            }
            m_vCurveAccel.x = side.x * fAccel;
            m_vCurveAccel.z = side.z * fAccel;
            bcs::FVEC3 sideVel;
            sideVel.x = side.x * fVel;
            sideVel.z = side.z * fVel;
            m_fCurveTime = fFlight;
            m_Velocity.x += sideVel.x;
            m_Velocity.z += sideVel.z;
            m_n360 = 2;
        }

        if (rParam.m_nUnk00 == 0) {
            // An arc whose top rises with the distance.
            float fTop;
            if (fDist <= 3.0f) {
                fTop = rParam.m_fUnk10;
            } else if (fDist >= 15.0f) {
                fTop = rParam.m_fUnk14;
            } else {
                fTop = ((rParam.m_fUnk14 - rParam.m_fUnk10) * (fDist - 3.0f)) / 12.0f + rParam.m_fUnk10;
            }
            if (fTop < 1.0f + m_Pos.y) {
                fTop = 1.0f + m_Pos.y;
            }
            fTop = fTop > rParam.m_fUnk14 ? rParam.m_fUnk14 : (fTop < rParam.m_fUnk10 ? rParam.m_fUnk10 : fTop);
            CalcYVG(m_Pos.y, fTop, target.y, fFlight, m_Velocity.y, m_fGravity);
            if (m_n360 == 0) {
                m_n360 = 1;
            }
        } else {
            // Straight at the target.
            m_fGravity = 0.0f;
            m_Velocity.y = (1.0f / fFlight) * (target.y - m_Pos.y);
            if (m_n360 == 0) {
                m_n360 = 3;
            }
        }

        // The spin: faster for a quick shot.
        if (fFlight > 1.0f) {
            float fSpin = 16.0f * (20.0f / fFlight);
            fSpin = fSpin > 32.0f ? 32.0f : (fSpin < 10.0f ? 10.0f : fSpin);
            m_fRotSpeed = (3.14159265f / 180.0f) * fSpin;
        } else {
            m_fRotSpeed = (3.14159265f / 180.0f) * (16.0f * (fFlight / 20.0f));
        }
        m_fNoCatchCount = 30.0f;
        m_fDefCounter = 40.0f;
        m_fPassTime = fFlight;
        return fDist;
    }

    float CBallBasket::CalcXZVel2(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, bcs::FVEC3 &rVel, float &rDecel,
                                  float &rFrame, float fSpeed0, float fSpeed1, float fMinFrame, float fMaxFrame) const
    {
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &rTo, &rFrom);
        rVel.x = 0.0f;
        rVel.z = 0.0f;
        rDecel = 0.0f;
        rFrame = 0.0f;
        float fDist = bcs::BCS_Vec_GetLenXZ(diff);
        if (fDist <= 0.0f) {
            return 0.0f;
        }
        // The frames at the mean of the two speeds, within the limits.
        float fFrame = fDist / (0.5f * (fSpeed0 + fSpeed1));
        if (fMinFrame > 0.0f) {
            fFrame = Max(fMinFrame, fFrame);
        }
        if (fMaxFrame > 0.0f) {
            fFrame = Min(fMaxFrame, fFrame);
        }
        float fAvg = fDist / fFrame;
        rFrame = fFrame;
        // Starts at fSpeed and slows down evenly.
        float fSpeed = (2.0f * fAvg) / (1.0f + fSpeed1 / fSpeed0);
        rDecel = ((2.0f * fAvg - fSpeed) - fSpeed) / fFrame;
        NormalizeXZ(rVel, diff);
        rVel.x *= fSpeed;
        rVel.z *= fSpeed;
        return fDist;
    }

    float CBallBasket::SetShootParam_(float fRate)
    {
        CChrCoreBasket *pChr = static_cast<CChrCoreBasket *>(CChrMan::GetChrCore(m_nShotChrIdx));
        const CHR_CORE_BASKET::CC_PARAM_BASKET2 *pParam = pChr->GetCcParam2_();
        BALL_BASKET::SHOT_PARAM param;
        param.m_nUnk04 = 0;
        param.m_nUnk00 = 0;
        param.m_fUnk0C = 0.0f;
        param.m_fUnk08 = 0.0f;
        // A "just" shot flies as the character's just shot.
        long nKind = (m_nBasketFlag & 0x2000) ? pParam->m_nJustShotKind : pParam->m_nShotKind;
        param.m_fUnk08 = fRate;
        param.m_fUnk0C = (m_nBasketFlag & 0x2000) ? pParam->m_fJustShotSpeed : pParam->m_fShotSpeed;
        param.m_fUnk10 = 4.1f;
        param.m_fUnk14 = 5.0f;
        param.m_fCurve = pParam->m_fShotCurve;
        param.m_vTarget = pChr->GetTargetShootPos();
        switch (nKind) {
            case 0:
                param.m_fUnk1C = 120.0f;
                param.m_fUnk18 = GetShotFrameMin_();
                break;
            case 1:
                param.m_fUnk18 = 15.0f;
                param.m_fUnk1C = 400.0f;
                param.m_nUnk00 = 1;
                break;
            case 2:
                param.m_fUnk18 = 50.0f;
                param.m_fUnk1C = 400.0f;
                param.m_nUnk04 = 1;
                break;
        }
        if (pChr->GetChrId() == 14 && pChr->GetState() == 0x26) {
            // (character 14 in state 0x26) A straight, fast shot.
            param.m_nUnk00 = 1;
            param.m_fUnk0C = 20.0f;
            param.m_fUnk18 = 10.0f;
        }
        float fDist = SetShootParamTest2_(param);

        if (pChr->GetChrId() == 17 && m_nShootType == 6) {
            // (character 17) Its lay-up knocks down the other team.
            float fDownFrame = 90.0f;
            m_Velocity.y = -0.2f;
            CCollUnit *pUnit = m_pAtkColl->GetColl(0);
            unsigned long nNoHit = 0;
            pUnit->m_nFlag10 |= 1;
            CChrCore *pShooter = CChrMan::GetChrCore(m_nShotChrIdx);
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pOther = CChrMan::GetChrCore(i);
                if (pOther != 0 && !pOther->IsDisable() && pOther->GetTeamNo() == pShooter->GetTeamNo()) {
                    nNoHit |= 1 << i;
                }
            }
            pUnit->m_nNoHitFlag = nNoHit;
            pUnit->SetAtkType(1);
            pUnit->m_fVanishPower = 0.0f;
            pUnit->m_fDownFrame = fDownFrame;
        }
        return fDist;
    }

}

// (inline in the original; its weak copy is here)
inline const msw::CHR_CORE_BASKET::CC_PARAM_BASKET2 *msw::CChrCoreBasket::GetCcParam2_() const
{
    return m_pCcParam2;
}

namespace msw {

    void CBallBasket::SetArtsShootParam_(float fRate)
    {
        CChrCoreBasket *pChr = static_cast<CChrCoreBasket *>(CChrMan::GetChrCore(m_nShotChrIdx));
        pChr->GetCcParam2_();
        BALL_BASKET::SHOT_PARAM param;
        param.m_nUnk04 = 0;
        param.m_nUnk00 = 0;
        param.m_fUnk0C = 0.0f;
        param.m_fUnk08 = 0.0f;
        const bcs::FVEC3 &rGoal = pChr->GetTargetGoalPos();
        param.m_fUnk08 = fRate;
        param.m_fUnk0C = 60.0f * m_fArtsShotUnk384;
        param.m_fUnk10 = rGoal.y;
        param.m_fUnk14 = rGoal.y;
        param.m_fUnk18 = 5.0f;
        param.m_fUnk1C = 120.0f;
        param.m_fCurve = CChrManBasket::GetInstance()->GetArtsCurveValue_(pChr->GetChrTypeID());
        param.m_vTarget = CGame::GetGoalPos(m_nTargetGoalIdx);
        // Each character's special shot flies its own way.
        switch (pChr->GetChrTypeID()) {
            case 16:
                param.m_fUnk10 = 4.9f;
                break;
            case 15:
                param.m_fUnk14 = 4.5f;
                param.m_fUnk10 = 4.5f;
                break;
            case 4:
                param.m_nUnk00 = 0;
                param.m_nUnk04 = 1;
                param.m_fUnk10 = 5.0f;
                param.m_fUnk14 = 5.0f;
                param.m_fUnk18 = 50.0f;
                param.m_fUnk1C = 400.0f;
                break;
            case 3:
                param.m_nUnk00 = 0;
                param.m_nUnk04 = 1;
                param.m_fUnk10 = 5.0f;
                param.m_fUnk14 = 5.0f;
                param.m_fUnk18 = 50.0f;
                param.m_fUnk1C = 400.0f;
                break;
            default:
                param.m_nUnk00 = 1;
                break;
        }
        SetShootParamTest2_(param);
    }

    int CBallBasket::IsFreeMoveMode() const
    {
        if (GetState() == 1) {
            return 1;
        }
        return 0;
    }

    int CBallBasket::IsPassMode() const
    {
        if (!(CGame::g_GameWork.m_nFlag18 & 1)) {
            long nState = GetState();
            if (nState >= 3 && nState <= 5) {
                return 1;
            }
        }
        return 0;
    }

    int CBallBasket::IsAlleyoopPassMode() const
    {
        if (!(CGame::g_GameWork.m_nFlag18 & 1) && GetState() == 5) {
            return 1;
        }
        return 0;
    }

    int CBallBasket::IsShootMode() const
    {
        long nState = GetState();
        if (nState >= 6 && nState <= 9) {
            return 1;
        }
        return 0;
    }

    int CBallBasket::IsArtsShootMode() const
    {
        if (GetState() == 9) {
            return 1;
        }
        return (m_nBasketFlag >> 8) & 1;
    }

    int CBallBasket::IsArtsShootAtkEnbale() const
    {
        return (m_nBasketFlag >> 19) & 1;
    }

    int CBallBasket::IsLayUpShootMode() const
    {
        return GetState() == 8;
    }

    int CBallBasket::IsChrNeedBlockMode(long nTeam) const
    {
        if (CGame::g_GameWork.m_nFlag18 & 1) {
            return 0;
        }
        long nState = GetState();
        if (nState < 3 || nState > 9) {
            return 0;
        }
        // Passes and shots of the other team.
        long nChr = m_nShotChrIdx;
        long nShotTeam = -2;
        if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum()) {
            nShotTeam = CChrMan::GetChrCore(nChr)->GetTeamNo();
        }
        return nShotTeam != nTeam;
    }

    int CBallBasket::IsCanbeTurnOver() const
    {
        if (m_nBasketFlag & 0x1) {
            return 1;
        }
        if (static_cast<CGameBasket *>(CGame::m_pInstance)->IsNowShotClock_()) {
            return 1;
        }
        if (CGame::g_GameWork.m_nFlag18 & 1) {
            if (m_Pos.y <= 3.4f || (m_nFlag & 0x2)) {
                return 1;
            }
            return 0;
        }
        long nState = GetState();
        if (nState >= 5 && nState <= 9) {
            return 0;
        }
        return 1;
    }

    // The fewest frames of a shot at the game level.
    inline long CBallBasket::GetShotFrameMin_() const
    {
        const long aFrameMin[4] = {60, 50, 40, 40};
        return aFrameMin[CGame::g_GameWork.m_nGameLevel];
    }

    // The rate of a pass of type nType at the game level (how it slows
    // down, as CBall::CalcXZVel takes it).
    inline float CBallBasket::GetPassRate_(long nType) const
    {
        float fRate = 0.0f;
        switch (CGame::g_GameWork.m_nGameLevel) {
            case 0: {
                const float aRate[7] = {0.23f, 0.23f, 0.2f, 0.23f, 0.3f, 0.22f, 0.23f};
                fRate = aRate[nType];
                break;
            }
            case 1: {
                const float aRate[7] = {0.24f, 0.24f, 0.2f, 0.24f, 0.3f, 0.23f, 0.24f};
                fRate = aRate[nType];
                break;
            }
            case 2: {
                const float aRate[7] = {0.26f, 0.26f, 0.2f, 0.26f, 0.3f, 0.25f, 0.26f};
                fRate = aRate[nType];
                break;
            }
            case 3: {
                const float aRate[7] = {0.26f, 0.26f, 0.2f, 0.26f, 0.3f, 0.25f, 0.26f};
                fRate = aRate[nType];
                break;
            }
        }
        return fRate;
    }

    int CBallBasket::IsNowRebound() const
    {
        return (m_nBasketFlag >> 2) & 1;
    }

    int CBallBasket::IsInvincible() const
    {
        return (m_nFlag >> 4) & 1;
    }

    int CBallBasket::CanbeCatch() const
    {
        if (m_nBasketFlag & 0x20000) {
            return 0;
        }
        if (GetAttachMdlObj() != 0 && !(m_nFlag & 0x8000)) {
            return 0;
        }
        if ((m_nFlag & 0x2) || (m_nFlag & 0x4)) {
            return 0;
        }
        if (GetState() == 10) {
            return 0;
        }
        return 1;
    }

    void CBallBasket::SetChargePower(float fPower)
    {
        m_fChargePower = fPower > 1.0f ? 1.0f : (fPower < 0.0f ? 0.0f : fPower);
    }

    void CBallBasket::TrgBound(unsigned long nNode)
    {
        if (GetAttachMdlObj() != 0) {
            m_nAttachNode = nNode;
            m_nBasketFlag |= 0x20;
        } else {
            m_nAttachNode = 0;
            m_nBasketFlag &= ~0x20;
        }
    }

    void CBallBasket::PlayBoundSe()
    {
        long nSe = -1;
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            nSe = pItem->GetBoundSe();
        }
        if (nSe < 0) {
            long nStage = GetStage();
            if (nStage >= 70 && nStage <= 72) {
                nSe = 0xDD0;
            } else {
                nSe = 0xC2A;
                switch (CStageMan::GetInstance()->GetHitMaterial(m_Pos)) {
                    case 1:
                        nSe = 0xC2B;
                        break;
                    case 2:
                        nSe = 0xC2C;
                        break;
                    case 3:
                        nSe = 0xC2D;
                        break;
                }
            }
        }
        CGameSnd::Play(nSe, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
    }

    void CBallBasket::SetJumpBallMode()
    {
        if (GetAttachMdlObj() != 0) {
            Detach_();
        }
        ClearShot_();
        EffectStopTail_();
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag &= ~0x40;
        m_nBasketFlag &= ~0x18;
        // Up from the centre of the court.
        m_Pos.x = 0.0f;
        m_Pos.y = 3.0f;
        m_Pos.z = 0.0f;
        m_PrevPos.x = 0.0f;
        m_PrevPos.y = 3.0f;
        m_PrevPos.z = 0.0f;
        m_Velocity.x = 0.0f;
        m_Velocity.y = 0.0f;
        m_Velocity.z = 0.0f;
        m_PrevVel.x = 0.0f;
        m_PrevVel.y = 0.0f;
        m_PrevVel.z = 0.0f;
        m_fGoalInRate = 0.0f;
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 15.0f * (3.14159265f / 180.0f);
        m_fNoCatchCount = 0.0f;
        ChangeState(11);
    }

    void CBallBasket::SetSleepMode()
    {
        if (GetAttachMdlObj() != 0) {
            Detach_();
        }
        EffectStopTail_();
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        ResetVelGravity_();
        m_fRotSpeed = 15.0f * (3.14159265f / 180.0f);
        m_nFlag |= 0x6;
        m_nFlag &= ~0x3000;
        if (m_pAtkColl != 0) {
            m_pAtkColl->SetDisableAll();
        }
        if (m_pBodyColl != 0) {
            m_pBodyColl->SetDisableAll();
        }
        SetHighlight_();
        ResetSquashParam();
        CalcRoot_();
        UpdateDisp_();
        ChangeState(0);
        ClearShot_();
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag &= ~0x18;
    }

    long CBallBasket::AttachItemEffect(long nKind)
    {
        if (m_nFlag & FLAG_SCORED) {
            return 0;
        }
        CItemBall *pItem;
        switch (nKind) {
            case 0:
                pItem = new CItemBallBasketGShell(this);
                break;
            case 1:
                pItem = new CItemBallBasketRShell(this);
                break;
            case 3:
                pItem = new CItemBallBasketBomb(this);
                break;
            case 4:
                pItem = new CItemBallBasketStar(this);
                break;
            case 2:
                pItem = new CItemBallBasketToadstool(this);
                break;
            case 5:
                pItem = new CItemBallBasketBanana(this);
                break;
            default:
                pItem = new CItemBallBasketGShell(this);
                break;
        }
        if (pItem == 0) {
            return 0;
        }
        DetachItemEffect();
        LinkItemEffect_(pItem->GetId());
        pItem->DoTrg(CItemBall::TRG_CONSTRUCT);
        new CItemBallChangeEffectBasket(CGame::GetTempParent(), GetId());
        return 1;
    }

    void CBallBasket::DetachItemEffect()
    {
        CBall::DetachItemEffect();
        SetScale(1.0f);
        SetCollPushRadius(-1.0f);
        SetCollBodyRadius(-1.0f);
        SetCollCatchRadius(-1.0f);
        if (GetState() == 10) {
            ChangeState(1);
        }
    }

    void CBallBasket::DetachItemEffect(CItemBall *pItem)
    {
        if (pItem == GetItemEffect_()) {
            DetachItemEffect();
        } else {
            pItem->DoTrg(CItemBall::TRG_DESTROY);
        }
    }

    void CBallBasket::ReleaseBall(const bcs::FVEC3 &rVel)
    {
        if (GetAttachMdlObj() != 0) {
            Detach_();
        }
        ClearShot_();
        EffectStopTail_();
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        m_nFlag |= 0x20;
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag &= ~0x40;
        m_nBasketFlag &= ~0x18;
        m_Velocity = rVel;
        m_PrevPos = m_Pos;
        m_fGoalInRate = 0.0f;
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 15.0f * (3.14159265f / 180.0f);
        m_fNoCatchCount = 30.0f;
        ChangeState(1);
    }

    void CBallBasket::SetChrReflectMode(long, float)
    {
    }

    void CBallBasket::SetChrReflectMode(const bcs::FVEC3 &rVel)
    {
        if (GetAttachMdlObj() != 0) {
            Detach_();
        }
        ClearShot_();
        EffectStopTail_();
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        m_nFlag |= 0x20;
        m_nBasketFlag &= ~0x2;
        m_nBasketFlag &= ~0x40;
        m_nBasketFlag &= ~0x18;
        m_Velocity = rVel;
        m_PrevPos = m_Pos;
        m_fGoalInRate = 0.0f;
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 15.0f * (3.14159265f / 180.0f);
        m_fNoCatchCount = 30.0f;
        TrgItemEffect_(5);
        ChangeState(1);
    }

    void CBallBasket::SetChrReflectMode(long, float fRotY, float fSpeed, float fVelY)
    {
        bcs::FVEC3 vel;
        bcs::BCS_VecZ_RotY(vel, 1.0f, fRotY);
        bcs::BCS_Vec_Normalize(vel, vel);
        if (fSpeed <= 0.0f) {
            bcs::FVEC3Scale(&vel, &vel, CBallMan::GetChrReflectSpeed());
        } else {
            bcs::FVEC3Scale(&vel, &vel, fSpeed);
        }
        vel.y = fVelY;
        SetChrReflectMode(vel);
    }

    float CBallBasket::GetPassFlyFrame(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nType) const
    {
        float fFrame = CalcXZFrame(rFrom, rTo, GetPassRate_(nType), 0.0f);
        float fMax = GetPassFrameMax_(nType);
        float fMin = GetPassFrameMin_(nType);
        return fFrame > fMax ? fMax : (fFrame < fMin ? fMin : fFrame);
    }

    float CBallBasket::GetShootFlyFrame(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long) const
    {
        float fResult = 0.0f;
        float fSpeed = 0.0f;
        if (IsShootMode()) {
            fSpeed = m_fUnk374;
        } else {
            CChrCoreBasket *pChr = static_cast<CChrCoreBasket *>(CChrMan::Util_SearchHoldBallChr(m_nBallNo));
            if (pChr != 0) {
                fSpeed = pChr->GetCcParam2_()->m_fShotSpeed;
            }
        }
        if (fSpeed > 0.0f) {
            float fFrame = CalcXZFrame(rFrom, rTo, fSpeed / 60.0f, 0.0f);
            fResult = 120.0f;
            if (fFrame <= fResult) {
                if (fFrame < GetShotFrameMin_()) {
                    fFrame = GetShotFrameMin_();
                }
                fResult = fFrame;
            }
        }
        return fResult;
    }

    void CBallBasket::PlayEffLayUp(const CChrCore *pChr)
    {
        if (pChr->GetChrId() == 15) {
            EffectSetTail_(0x685);
        }
    }

    float CBallBasket::CalcJumpShootVel(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, bcs::FVEC3 &rVel,
                                        float &rFrame)
    {
        float fSpeed = 0.0f;
        rVel.x = 0.0f;
        rVel.y = 0.0f;
        rVel.z = 0.0f;
        rFrame = 0.0f;
        if (IsShootMode()) {
            fSpeed = m_fUnk374;
        } else {
            CChrCoreBasket *pChr = static_cast<CChrCoreBasket *>(CChrMan::Util_SearchHoldBallChr(m_nBallNo));
            if (pChr != 0) {
                fSpeed = pChr->GetCcParam2_()->m_fShotSpeed;
            }
        }
        if (fSpeed > 0.0f) {
            rFrame = CalcXZFrame(rFrom, rTo, fSpeed / 60.0f, 0.0f);
        }
        return rFrame;
    }

    void CBallBasket::PlayArtsEff()
    {
        long nChr = CGame::g_nEventChrIdx;
        if (CGame::g_nEventChrIdx == -1) {
            return;
        }
        StopTailEffect();
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (pChr == 0) {
            return;
        }
        // The trail of each character's special shot.
        switch (pChr->GetChrTypeID()) {
            case 1:
                EffectSetTail_(0x51D);
                break;
            case 2:
                EffectSetTail_(0x528);
                break;
            case 3:
                EffectSetTail_(0x530);
                break;
            case 4:
                EffectSetTail_(0x53B);
                break;
            case 5:
                EffectSetTail_(0x544);
                break;
            case 6:
                EffectSetTail_(0x553);
                break;
            case 7:
                EffectSetTail_(0x55C);
                break;
            case 8:
                EffectSetTail_(0x563);
                break;
            case 9:
                EffectSetTail_(0x56E);
                break;
            case 10:
                EffectSetTail_(0x578);
                break;
            case 11:
                EffectSetTail_(0x581);
                EffectStopTail_(1);
                EffectSetTail_(1, 0x588, 0);
                break;
            case 12:
                EffectSetTail_(0x58F);
                break;
            case 14:
                EffectSetTail_(0x5A9);
                break;
            case 15:
                EffectSetTail_(0x5B6);
                break;
            case 16: {
                // The slime's trail takes the colours of its variant.
                EffectSetTail_(1, 0x5C5, 0);
                long nColor = pChr->GetColor();
                const CChrCoreBasketSlime::SLIME_COL_SET *pSet =
                    CChrCoreBasketSlime::GetArtsEffectUnit(nColor > 2 ? 2 : (nColor < 0 ? 0 : nColor));
                Eff_SetPointLightColor(1, pSet->m_LightColor0, pSet->m_LightColor1);
                break;
            }
            case 17:
                EffectSetTail_(0x5DB);
                break;
            default:
                EffectSetTail_(0x4DA);
                break;
        }
    }

    void CBallBasket::PlayGoalInEff()
    {
        static const long nGoalEffID = 0x5CC;
        static const long nMdlEffID = 0x5CE;

        CChrCore *pChr = CChrMan::GetChrCore(GetShotChrIdx());
        long nEffect = -1;
        long nGoalEffect = -1;
        const bcs::FVEC3 &rGoal = pChr->GetTargetGoalPos();
        long nType = m_nShootType;
        int bBossPackun = CChrManBasket::GetInstance()->IsCreateBossPackun_();
        if (nType >= 9 && nType <= 11) {
            // A special shot: each character's own effect.
            switch (pChr->GetChrTypeID()) {
                case 0:
                    nEffect = 0x514;
                    break;
                case 1:
                    nEffect = 0x51E;
                    break;
                case 2:
                    nEffect = 0x529;
                    break;
                case 3:
                    nEffect = 0x531;
                    break;
                case 4:
                    nEffect = 0x53D;
                    break;
                case 5:
                    nEffect = 0x545;
                    break;
                case 6:
                    nEffect = 0x554;
                    break;
                case 7:
                    nEffect = 0x55D;
                    break;
                case 8:
                    nEffect = 0x564;
                    break;
                case 9:
                    nEffect = 0x56F;
                    break;
                case 10:
                    nEffect = 0x57B;
                    break;
                case 11:
                    nEffect = 0x582;
                    break;
                case 12:
                    nEffect = 0x593;
                    break;
                case 13:
                    nEffect = 0x59C;
                    break;
                case 14:
                    nEffect = 0x5AA;
                    break;
                case 15:
                    nEffect = 0x5B7;
                    break;
                case 16: {
                    // The slime's effects, in the colours of its variant,
                    // facing the way the ball came from.
                    long nColor = pChr->GetColor();
                    const CChrCoreBasketSlime::SLIME_COL_SET *pSet =
                        CChrCoreBasketSlime::GetArtsEffectUnit(nColor > 2 ? 2 : (nColor < 0 ? 0 : nColor));
                    bcs::FVEC3 diff;
                    bcs::FVEC3Sub(&diff, &m_PrevPos, &m_Pos);
                    bcs::FVEC3 dir;
                    NormalizeXZ(dir, diff);
                    Eff_Stop(0xB);
                    Eff_CreateSimple(0xB, nMdlEffID, rGoal, GetRotYXZ(dir), 3, 0);
                    Eff_SetPointLightColor(0xB, pSet->m_LightColor0, pSet->m_LightColor1);
                    Eff_SetMdlKColorReg(0xB, GX_KCOLOR0, pSet->m_KColor0);
                    Eff_SetMdlKColorReg(0xB, GX_KCOLOR1, pSet->m_KColor1);
                    Eff_Stop(0xC);
                    Eff_CreateSimple(0xC, nGoalEffID, rGoal, 3, 0);
                    Eff_SetPointLightColor(0xC, pSet->m_LightColor0, pSet->m_LightColor1);
                    Eff_SetMdlKColorReg(0xC, GX_KCOLOR0, pSet->m_KColor0);
                    Eff_SetMdlKColorReg(0xC, GX_KCOLOR1, pSet->m_KColor1);
                    break;
                }
                case 17:
                    nEffect = 0x5DC;
                    break;
                case 19:
                case 21:
                case 23:
                    nEffect = 0x5E5;
                    break;
                case 20:
                case 22:
                case 24:
                    nEffect = 0x5EC;
                    break;
                case 18:
                default:
                    nEffect = 0x514;
                    break;
            }
        } else if (nType == 3 || nType == 5 || nType == 8) {
            if (!bBossPackun) {
                nGoalEffect = 0x63;
            }
        } else {
            if (!bBossPackun) {
                nGoalEffect = 0x62;
            }
        }

        if (nEffect >= 0) {
            Eff_CreateSimple(nEffect, rGoal, 3, 0);
        }
        if (nGoalEffect >= 0) {
            // On the goal's model.
            CGoalObj *pGoal = CStageMan::GetInstance()->GetGoal(pChr->GetTargetGoalIdx());
            if (pGoal != 0) {
                unsigned long hMdl = pGoal->GetMdlHandle();
                if (hMdl != 0) {
                    bcs::CScnObj *pMdl = bcs::CScnObj::GetScnObj(hMdl);
                    if (pMdl != 0) {
                        CEffectCtrl::CreateAttachEffectAuto(nGoalEffect, static_cast<bcs::CMdlObj *>(pMdl), 3);
                    }
                }
            }
        }
        if (nType == 6 && pChr->GetChrId() == 17) {
            // (character 17) Its lay-up leaves an effect under the goal,
            // down at the water on the river court.
            float fY;
            if (CStageMan::GetInstance()->GetFloorAttribute(m_Pos, CStageMan::FLOW_TYPE(6)) == 8) {
                fY = -3.0f;
            } else {
                fY = 0.0f;
            }
            bcs::FVEC3 pos;
            pos.x = rGoal.x;
            pos.y = fY;
            pos.z = rGoal.z;
            Eff_CreateSimple(0x699, pos, 3, 0);
        }
    }

    void CBallBasket::SetJustShootFlg(int bJust)
    {
        if (bJust) {
            if (!(m_nBasketFlag & 0x400000)) {
                m_nBasketFlag |= 0x400000;
                Eff_CreateAttach(0xF, GetJustShotSignEffectID(), 0, 0, 3);
            }
            m_nBasketFlag |= 0x2000;
        } else {
            StopJustSignEff();
            m_nBasketFlag &= ~0x2000;
        }
    }

    long CBallBasket::GetJustShotSignEffectID() const
    {
        return 0x4F8;
    }

    void CBallBasket::StopJustSignEff()
    {
        m_nBasketFlag &= ~0x400000;
        Eff_Stop(0xF);
    }

    float CBallBasket::GetFloorBoundRateY() const
    {
        return CBallMan::GetFloorBoundDecRateY();
    }

    float CBallBasket::GetFloorBoundRateXZ() const
    {
        return CBallMan::GetFloorBoundDecRateXZ();
    }

    float CBallBasket::GetWallBoundRateXZ() const
    {
        return CBallMan::GetWallBoundDecRate();
    }

    void CBallBasket::SetShootResult(int bGoalIn)
    {
        CGame::SendMessage(0x32, bGoalIn, GetShotChrIdx(), 0);
    }

    int CBallBasket::IsAirBall() const
    {
        // A shot whose chance is below the air ball rate (in percent).
        long nRate = static_cast<CBallManBasket *>(CBallMan::GetInstance())->GetAirBallRate_();
        return (long)(100.0f * m_fGoalInRate) < nRate;
    }

    CBallBasketDummy::CBallBasketDummy(bcs::CTask *pParent, long nBallNo) : CBallBasket(pParent, nBallNo)
    {
    }

    CBallBasketDummy::~CBallBasketDummy()
    {
    }

    void CBallBasketDummy::LoopStateFreeMove()
    {
        // Goes away unless a special shot is playing.
        int bDelete;
        if (CChrManBasket::GetInstance()->IsGhostAppearEnable()) {
            bDelete = 1;
        } else {
            bDelete = CChrManBasket::GetInstance()->GetArtsChr() == 0;
        }
        if (bDelete && !IsDelete()) {
            CBallMan::DelBall(m_nBallNo);
        }
        CBallBasket::LoopStateFreeMove();
    }

    void CBallBasketDummy::DtorStateArtsShoot()
    {
        CBallBasket::DtorStateArtsShoot();
        CBallMan::DelBall(m_nBallNo);
    }

    void CBallBasketDummy::CreateCollBody_()
    {
        CBallBasket::CreateCollBody_();
        m_pBodyColl->ResetTargetFlagAll(4);
    }

    int CBallBasketDummy::CanbeCatch() const
    {
        return 0;
    }

    void CBallBasketDummy::TrgGoalIn_()
    {
        CChrCore *pChr = CChrMan::GetChrCore(GetShotChrIdx());
        if (pChr != 0 && GetState() == 9) {
            if (pChr->GetChrId() != 6) {
                // Only the real ball scores.
                ChangeState(1);
                SetLooseBall_();
            } else {
                CBallBasket::TrgGoalIn_();
            }
        }
    }

    // The inline functions the original emits as weak copies here.

    inline long CBallBasket::GetJustShotMaxEndFrm() const
    {
        return 0;
    }

    inline long CBallBasket::GetJustShotMidEndFrm() const
    {
        return 0;
    }

    inline long CBallBasket::GetJustShotMinEndFrm() const
    {
        return 0;
    }

    inline long CBallBasket::GetJustShotMaxStartFrm() const
    {
        return 0;
    }

    inline long CBallBasket::GetJustShotMidStartFrm() const
    {
        return 0;
    }

    inline long CBallBasket::GetJustShotMinStartFrm() const
    {
        return 0;
    }

    inline float CBallBasket::GetShootCurveValue() const
    {
        return m_fShootCurveValue;
    }

    inline long CBallBasket::GetShootCalcType() const
    {
        return m_nCurveStep;
    }

    inline bcs::FVEC3 CBallBasket::GetShootStartPos() const
    {
        return m_vShootStartPos;
    }

    inline int CBallBasket::IsBaExplode() const
    {
        return 0;
    }

    inline void CBallBasket::SetReboundDisable()
    {
        m_nBasketFlag &= ~0x40;
    }

    inline float CBallBasket::GetChargePower() const
    {
        return m_fChargePower;
    }

    inline long CBallBasket::GetShotArea() const
    {
        return m_nShotArea;
    }

    inline long CBallBasket::GetPassChrIdx() const
    {
        return m_nPassChrIdx;
    }

    inline float CBallBasket::GetDefCounter() const
    {
        return m_fDefCounter;
    }

    inline float CBallBasket::GetGoalInRate() const
    {
        return m_fGoalInRate;
    }

    inline float CBallBasket::GetNoCatchCount() const
    {
        return m_fNoCatchCount;
    }

    inline int CBallBasketDummy::IsDummy() const
    {
        return 1;
    }

    // (weak copy at 0x5B00 in an earlier BasketF unit)
    inline int CBallBasket::IsDummy() const
    {
        return 0;
    }
}
