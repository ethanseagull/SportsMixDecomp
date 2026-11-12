#include "game/msw/CBallBoss.hpp"
#include "game/msw/BallBossSpikeShotArg.hpp"
#include "game/msw/CBallInline.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CItemBall.hpp"
#include "game/msw/CItemBallBossKinds.hpp"
#include "game/msw/CStageMan.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/New.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/equation.h"

#include <math.h>

// The ball of the boss game: msw::CBallBoss and the constructor of the
// spike parameters it takes (msw::BALL::BOSS::SPIKE_SHOT_ARG).

namespace {

    // The colour animations of the six power levels in the ball's file.
    const char *s_aPowerAnmClrName[] = {
        "power00",
        "power01",
        "power02",
        "power03",
        "power04",
        "power05",
    };

    // The trail effect of each power level (the last one for a shot at
    // full power).
    const long s_aPowerTailEffect[] = {
        0x54D, 0x54E, 0x54F, 0x550, 0x551, 0x552, 0x553,
    };

    // The power level (0 to 5) of a coin power.
    inline long GetPowerLevel(long nPower)
    {
        return nPower > 5 ? 5 : (nPower < 0 ? 0 : nPower);
    }

    // The point at t between a and b.
    inline float Lerp(float a, float b, float t)
    {
        return t * (b - a) + a;
    }

    // Maps x from [x0, x1] onto [y0, y1], clamped at both ends.
    inline float MapRange(float x, float x0, float x1, float y0, float y1)
    {
        if (x <= x0) {
            return y0;
        }
        if (x >= x1) {
            return y1;
        }
        float t = x - x0;
        if (x1 > x0) {
            t /= x1 - x0;
        }
        return t * (y1 - y0) + y0;
    }

    // One step of a flight: rPos moves by fTime frames of rVel plus half
    // of rAccel (the change of velocity over the step), then rVel takes
    // rAccel.
    inline void MoveFlight(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, const bcs::FVEC3 &rAccel, float fTime)
    {
        bcs::FVEC3 move;
        bcs::FVEC3 half;
        bcs::FVEC3Scale(&move, &rVel, fTime);
        bcs::FVEC3Scale(&half, &rAccel, 0.5f * fTime);
        bcs::FVEC3Add(&move, &move, &half);
        bcs::FVEC3Add(&rVel, &rVel, &rAccel);
        bcs::FVEC3Add(&rPos, &rPos, &move);
    }

    // rOut = rIn flattened onto the XZ plane and scaled to length 1 (zero
    // for a vertical rIn).
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::FVEC3 xz(rIn.x, 0.0f, rIn.z);
        bcs::BCS_Vec_Normalize(rOut, xz);
    }

    // Tells every game camera where the ball is heading.
    inline void SetCameraBallTargetPos(int bFlag, const bcs::FVEC3 &rPos)
    {
        for (long i = 0; i < msw::CGameCamera::GetInstanceNum(); i++) {
            msw::CGameCamera *pCamera = msw::CGameCamera::GetInstance(i);
            if (pCamera != 0) {
                pCamera->SetBallTargetPos(bFlag, rPos);
            }
        }
    }
}

namespace msw {

    CBallBoss::CBallBoss(bcs::CTask *pParent, long nBallNo)
        : CBall(pParent, nBallNo), m_Unk318(0), m_nBossFlag(0), m_bCollOff(0), m_v3CC(0.0f, 1.0f, 0.0f), m_n3D8(0),
          m_nVsBossBallMode(0)
    {
        ResetParam_();
    }

    CBallBoss::~CBallBoss()
    {
    }

    void CBallBoss::Disp()
    {
    }

    inline void CBallBoss::ClearSpin_()
    {
        m_nSpinType = 0;
        m_fSpinCount = 0.0f;
        m_vSpinAccel.x = 0.0f;
        m_vSpinAccel.y = 0.0f;
        m_vSpinAccel.z = 0.0f;
    }

    inline void CBallBoss::UpdateCameraTarget_(int bFlag)
    {
        if (m_bCollOff != 0) {
            bFlag = 0;
        }
        SetCameraBallTargetPos(bFlag, m_vTargetPos);
    }

    inline void CBallBoss::SetLooseBall_()
    {
        EffectStopTail_();
        CGame::SetLooseBallMode(1);
        if (TrgItemEffect_(4) & 0x10) {
            m_nBossFlag |= BOSSFLAG_ITEM_HIT;
        }
        m_nFlag |= 0x3000;
        long state = GetState();
        if (state >= 3 && state <= 9) {
            ChangeState(1);
        }
    }

    inline void CBallBoss::SetShotChrNoHit_()
    {
        unsigned long bit = 1 << m_nShotChrIdx;
        m_Unk17C |= bit;
        if (m_pBodyColl != 0) {
            m_pBodyColl->SetTmpNoHitFlag(0, bit);
        }
    }

    inline void CBallBoss::InitFlight_(const bcs::FVEC3 &rTarget)
    {
        ResetVelGravity_();
        m_f320 = 0.0f;
        ClearSpin_();
        m_PrevPos = m_Pos;
        m_PosD4 = m_Pos;
        m_vTargetPos = rTarget;
    }

    inline void CBallBoss::CalcFlightAccel_(bcs::FVEC3 &rAccel, const bcs::FVEC3 &rDir, float fDecel, float fGravity,
                                            float fTime, int bSpin) const
    {
        float decel = fDecel * fTime;
        rAccel.x = rDir.x * decel;
        rAccel.z = rDir.z * decel;
        if (bSpin) {
            rAccel.x = m_vSpinAccel.x * fTime + rAccel.x;
            rAccel.y = fTime * (fGravity + m_vSpinAccel.y);
            rAccel.z = m_vSpinAccel.z * fTime + rAccel.z;
        } else {
            rAccel.y = fGravity * fTime;
        }
    }

    inline void CBallBoss::ChangePowerColor_(long n)
    {
        if (m_pMdlObj != 0 && m_pAnmMatClr != 0 && m_aPowerAnmClr[n].IsValid()) {
            m_pAnmMatClr->ChangeRes(m_pMdlObj->GetResMdl(), m_aPowerAnmClr[n]);
        }
    }

    void CBallBoss::ResetParam_()
    {
        CBall::ResetParam_();
        m_nBossFlag = 0;
        m_f320 = 0.0f;
        m_nShotChrIdx = 0;
        m_n328 = 0;
        m_nPassChrIdx = 0;
        m_nShotTeamIdx = 0;
        m_n334 = 0;
        m_n338 = 0;
        m_n33C = 0;
        m_n340 = 0;
        m_fFadeCount = 0.0f;
        m_n34C = -1;
        m_f350 = 0.0f;
        m_f354 = 0.0f;
        m_f358 = 0.0f;
        m_n35C = 0;
        m_n360 = 1;
        m_nFloorHitSe = -1;
        m_n3E4 = -1;
        m_n3DC = 0;
        m_f364 = 150.0f;
        m_f368 = 20.0f;
        m_v3CC.Set(0.0f, 1.0f, 0.0f);
        m_vRideObjPos.Set(0.0f, 0.0f, 0.0f);
        m_nSpinType = 0;
        m_fSpinCount = 0.0f;
        m_vSpinAccel.x = 0.0f;
        m_vSpinAccel.y = 0.0f;
        m_vSpinAccel.z = 0.0f;
        InitArtsShotStep_();
        m_f3E8 = 0.0f;
        m_f3EC = 0.0f;
        m_f3F0 = 0.0f;
        m_f3F4 = 0.0f;
        m_f3F8 = 0.0f;
        m_nBossFlag &= ~BOSSFLAG_RIDE;
        m_nRideObjHandle = 0;
        m_n40C = 0;
        m_n410 = 0;
        m_f414 = 0.0f;
        m_nVsBossBallMode = 0;
        m_fScale = 1.0f;
        m_bReflectCheckEnd = 0;
        m_nCoinPower = 0;
        m_nPrevCoinPower = 0;
        ChangePowerColor_(0);
        m_n428 = 0;
        m_n42C = 0;
    }

    void CBallBoss::ResetOnAttach_()
    {
        m_n40C = 0;
        m_n410 = 0;
        m_n384 = 0;
        m_fScale = 1.0f;
        m_nBossFlag &= ~BOSSFLAG_RIDE;
        m_nRideObjHandle = 0;
    }

    void CBallBoss::ResetOnDetach_()
    {
        m_n384 = 0;
    }

    void CBallBoss::CreateMdlObj_(nw4r::g3d::ResFile file, nw4r::g3d::ResMdl mdl, nw4r::g3d::ResMdl shadowMdl,
                                  nw4r::g3d::ResMdl refMdl, long nShadow)
    {
        if (mdl.IsValid()) {
            m_pMdlObj = new (0x100) bcs::CMdlObj;

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
            m_pMdlObj->Create(mdl, anmFlag, 0);
            m_pMdlObj->OnFlag(0x100);
            m_pMdlObj->OnFlag(0x2);
            m_pMdlObj->SetExBlendMode(1);
            m_pMdlObj->OnFlag(0x2000);

            if (file.HasResAnmChr()) {
                m_pAnmMotion = new (0x100) bcs::CAnmMotion;
                m_pAnmMotion->Create(m_pMdlObj, file, 0, 3, 0);
                m_pMdlObj->AttachAnmObj(m_pAnmMotion, bcs::CMdlObj::ANMOBJTYPE_CHR);
            }
            if (file.HasResAnmClr()) {
                long first = -1;
                for (long i = 0; i < 6; i++) {
                    m_aPowerAnmClr[i] = file.GetResAnmClr(s_aPowerAnmClrName[i]);
                    if (m_aPowerAnmClr[i].IsValid() && first < 0) {
                        first = i;
                    }
                }
                if (first >= 0) {
                    m_pAnmMatClr = new (0x100) bcs::CAnmMatClr;
                    if (m_pAnmMatClr != 0) {
                        m_pAnmMatClr->Create(m_pMdlObj, m_aPowerAnmClr[first], 0, 0);
                        m_pMdlObj->AttachAnmObj(m_pAnmMatClr, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
                    }
                }
            }
            if (file.HasResAnmTexSrt()) {
                nw4r::g3d::ResAnmTexSrt res = file.GetResAnmTexSrt(0);
                bcs::CAnmTexSrt *pAnm = new (0x100) bcs::CAnmTexSrt;
                if (pAnm != 0) {
                    pAnm->Create(m_pMdlObj, res, 0, 0);
                    m_pMdlObj->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
                }
            }
            if (file.HasResAnmTexPat()) {
                nw4r::g3d::ResAnmTexPat res = file.GetResAnmTexPat(0);
                bcs::CAnmTexPat *pAnm = new (0x100) bcs::CAnmTexPat;
                if (pAnm != 0) {
                    pAnm->Create(m_pMdlObj, res, 0, 0);
                    m_pMdlObj->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_3);
                }
            }
            m_pMdlObj->CalcRootMtx();
            m_pMdlObj->CalcWorldMatrixAll(0);
            bcs::CRenderMan::PushBack(m_pMdlObj, 2);
        }

        if (m_pMdlObj != 0 && shadowMdl.IsValid()) {
            m_pSubObj = new (0x100) bcs::CMdlShadow(m_pMdlObj, shadowMdl, nShadow, 0, 0);
            bcs::CRenderMan::PushBack(m_pSubObj, 2);
        }
        if (m_pMdlObj != 0 && refMdl.IsValid()) {
            m_pSubObj2 = new (0x100) bcs::CMdlRef(m_pMdlObj, refMdl, 0, 0, 0);
            bcs::CRenderMan::PushBack(m_pSubObj2, 4);
        }
    }

    void CBallBoss::CreateCollBody_()
    {
        if (m_pMdlObj != 0) {
            m_pBodyColl = new (0) CBodyCollision(2, 2, 0, m_pMdlObj->GetHandle());
            m_pBodyColl->SetPushEnable(0);
            m_pBodyColl->SetBodyEnable(1);
            m_pBodyColl->SetIndexAll(m_nBallNo);
            m_pBodyColl->SetMyNoHitFlagAll(1 << (m_nBallNo + 8));
            m_pBodyColl->SetPushRank(0, 1);
            m_pBodyColl->SetPushPower(0, 0);
            m_pBodyColl->SetBodyDamageCallbackAll(CallbackBodyDamage, this);
            m_pBodyColl->SetBodyPushCallbackAll(CallbackBodyPush, this);
        }
    }

    void CBallBoss::Reset()
    {
        DetachItemEffect();
        EffectStopTail_();
        ResetParam_();
        ChangeState(0);
    }

    int CBallBoss::IsActive() const
    {
        if (IsVolleyReqDelete()) {
            return 0;
        }
        return CBall::IsActive();
    }

    int CBallBoss::IsSleepMode() const
    {
        return CBall::IsSleepMode();
    }

    int CBallBoss::IsFreeMoveMode() const
    {
        return CBall::IsFreeMoveMode();
    }

    int CBallBoss::IsPassMode() const
    {
        return CBall::IsPassMode();
    }

    int CBallBoss::IsAlleyoopPassMode() const
    {
        return CBall::IsAlleyoopPassMode();
    }

    int CBallBoss::IsShootMode() const
    {
        return CBall::IsShootMode();
    }

    int CBallBoss::IsArtsShootMode() const
    {
        return CBall::IsArtsShootMode();
    }

    int CBallBoss::CanbeVsBossReceive() const
    {
        if (IsSleepMode()) {
            return 0;
        }
        return m_nVsBossBallMode >= 3;
    }

    int CBallBoss::CanbeVsBossReflect() const
    {
        if (IsSleepMode()) {
            return 0;
        }
        if (!IsActive()) {
            return 0;
        }
        IsShootMode();
        return 0;
    }

    void CBallBoss::SetVsBossReflectCheckEnd()
    {
        m_bReflectCheckEnd = 1;
    }

    int CBallBoss::CanbeVsBossAirTrick() const
    {
        if (IsSleepMode()) {
            return 0;
        }
        if (!IsActive()) {
            return 0;
        }
        if (m_n40C != 0) {
            return 0;
        }
        if (m_nVsBossBallMode == 0) {
            return 0;
        }
        return m_nVsBossBallMode != 2;
    }

    void CBallBoss::SetVsBossCoinPower(long nPower)
    {
        m_nCoinPower = nPower;
        m_nCoinPower = nPower > 100 ? 100 : (nPower < 0 ? 0 : nPower);
    }

    void CBallBoss::AddVsBossCoinPower(long nPower)
    {
        m_nCoinPower += nPower;
        long n = m_nCoinPower;
        m_nCoinPower = n > 100 ? 100 : (n < 0 ? 0 : n);
    }

    long CBallBoss::GetVsBossCoinPower() const
    {
        return m_nCoinPower;
    }

    void CBallBoss::ClearVsBossCoinPower()
    {
        m_nCoinPower = 0;
    }

    void CBallBoss::CmnShotInit_()
    {
        m_nBossFlag &= ~0x880;
        m_n35C = 0;
        m_n360 = 1;
        m_f358 = 0.0f;
        m_n3DC = 0;
        m_nFloorHitSe = -1;
        m_n3E4 = -1;
        m_n328 = 0;
        m_fScale = 1.0f;
        m_n3A4 = 0;
        m_f364 = 150.0f;
        m_f368 = 20.0f;
        m_f350 = 0.0f;
        m_f380 = 0.0f;
        SE_Stop(0, 0.0f);
        SE_Stop(1, 0.0f);
        m_nSpinType = 0;
        m_fSpinCount = 0.0f;
        m_vSpinAccel.x = 0.0f;
        m_vSpinAccel.y = 0.0f;
        m_vSpinAccel.z = 0.0f;
        InitArtsShotStep_();
        m_f3E8 = 0.0f;
        m_f3EC = 0.0f;
        m_f3F0 = 0.0f;
        m_f3F4 = 0.0f;
        m_f3F8 = 0.0f;
        m_nBossFlag &= ~BOSSFLAG_RIDE;
        m_nRideObjHandle = 0;
        m_nVsBossBallMode = 2;
        m_bReflectCheckEnd = 0;
    }

    void CBallBoss::InitArtsShotStep_()
    {
        m_n384 = 0;
        m_n388 = 0;
        m_f38C = 0.0f;
        m_n390 = -1;
        m_f398 = 0.0f;
        m_n394 = 0;
        m_f39C = 0.0f;
        m_f3A0 = 0.0f;
        m_n3A4 = 0;
        m_f3A8 = 0.0f;
        m_f3AC = 0.0f;
        m_f3B0 = 0.0f;
        m_f3B4 = 0.0f;
        m_f3B8 = 0.0f;
        m_f3BC = 0.0f;
        m_n3C0 = -1;
        m_n3C4 = -1;
    }

    void CBallBoss::PlayFloorHitSe(unsigned long nArg)
    {
        if (m_nFloorHitSe >= 0) {
            PlaySePos(m_nFloorHitSe, 0, nArg, -1, 0.0f, 1.0f, 0.0f);
            m_nFloorHitSe = -1;
        } else {
            CBall::PlayFloorHitSe(nArg);
        }
    }

    void CBallBoss::SetHighlight_()
    {
        CBall::SetHighlight_();
    }

    void CBallBoss::UpdateRainbow_()
    {
        if (m_bCollOff != 0 || IsSleepMode()) {
            return;
        }
        long prev = m_nPrevCoinPower;
        long power = m_nCoinPower;
        if (power == prev) {
            return;
        }
        long level = power;
        if (power > 0) {
            level = GetPowerLevel(power);
            if (prev < 5 && power >= 5) {
                PlaySePos(0xE0C, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            if (s_aPowerTailEffect[level] >= 0) {
                Eff_CreateAttach(8, s_aPowerTailEffect[level], 0, 0, 3);
            } else {
                Eff_Stop(8);
            }
        }
        ChangePowerColor_(level);
        m_nPrevCoinPower = m_nCoinPower;
    }

    void CBallBoss::UpdateCollAtk_()
    {
        if (m_bCollOff != 0) {
            if (m_pAtkColl != 0) {
                m_pAtkColl->SetDisableAll();
            }
        } else {
            CBall::UpdateCollAtk_();
        }
    }

    void CBallBoss::UpdateCollBody_()
    {
        if (m_bCollOff != 0) {
            if (m_pBodyColl != 0) {
                m_pBodyColl->SetDisableAll();
            }
        } else {
            CBall::UpdateCollBody_();
        }
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            float add = pItem->GetVolleyCatchCollRadius();
            SetCollCatchRadius(GetCollCatchRadiusDefault() + add);
        } else {
            SetCollCatchRadius(GetCollCatchRadiusDefault());
        }
    }

    void CBallBoss::UpdateDisp_()
    {
        if ((m_nBossFlag & BOSSFLAG_ITEM_HIT) && !IsFlag2()) {
            m_nFlag |= 0x6;
            StopTailEffect();
            m_nVsBossBallMode = 0;
            ChangeState(0);
            m_f354 = 99999.9f;
            SetAlphaRate(0.0f);
        }
        CBall::UpdateDisp_();
    }

    float CBallBoss::GetRadius() const
    {
        float scale = m_fScale;
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            scale *= pItem->GetScale();
        }
        return 0.25f * scale;
    }

    float CBallBoss::GetBgCollRadius() const
    {
        float scale = m_fScale;
        CItemBall *pItem = GetItemEffect_();
        if (pItem != 0) {
            scale *= pItem->GetScale();
        }
        return 0.23f * scale;
    }

    float CBallBoss::GetBgCollRadiusXZ() const
    {
        return GetBgCollRadius();
    }

    int CBallBoss::GetTargetPos(bcs::FVEC3 &rPos) const
    {
        if (m_nFlag & FLAG_SCORED) {
            return 0;
        }
        if (IsAttach()) {
            return 0;
        }
        if (IsAlleyoopPassMode()) {
            return 0;
        }
        rPos = m_vTargetPos;
        return 1;
    }

    long CBallBoss::CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &)
    {
        return 0;
    }

    void CBallBoss::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (pPush == 0) {
            return;
        }
        if (IsAttach()) {
            pOther->DoBodyPushCallback(pBody, 0);
            return;
        }
        int bWallPush = 0;
        if (m_n40C != 0 && pOther->GetOwnerKind() == 5) {
            bWallPush = 1;
        }
        if (bWallPush) {
            bcs::FVEC3 from = m_Pos;
            float dist = 0.1f * GetFrameRateF();
            bcs::FVEC3 push = *pPush;
            if (push.x * push.x + push.z * push.z >= dist * dist) {
                bcs::FVEC3 xz(push.x, 0.0f, push.z);
                bcs::BCS_Vec_Normalize(push, xz);
                push.x *= dist;
                push.z *= dist;
            }
            bcs::FVEC3 to;
            to.x = from.x + push.x;
            to.z = from.z + push.z;
            to.y = from.y;
            bcs::FVEC3 pos;
            bcs::FVEC3 normal;
            float time;
            CCollMan::DetectMapHit(pos, normal, time, from, to, -10.0f, 2, 0);
            pOther->DoBodyPushCallback(pBody, 0);
            SetPos(pos);
        } else {
            pOther->DoBodyPushCallback(pBody, 0);
        }
    }

    void CBallBoss::SetBound_(const bcs::FVEC3 &rVel)
    {
        m_nSpinType = 0;
        m_fSpinCount = 0.0f;
        m_vSpinAccel.x = 0.0f;
        m_vSpinAccel.y = 0.0f;
        m_vSpinAccel.z = 0.0f;
        m_n35C = 0;
        EffectStopTail_();
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        m_nFlag |= 0x20;
        m_Velocity = rVel;
        m_PrevVel = rVel;
        m_PrevPos = m_Pos;
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 0.2617994f;
        UpdateCameraTarget_(0);
        m_n3DC = 0;
        m_nFloorHitSe = -1;
        ChangeState(1);
    }

    void CBallBoss::SetSpikeJumpTgtWork_(float)
    {
    }

    void CBallBoss::UpdateCountWork_()
    {
        if (m_f350 > 0.0f) {
            m_f350 -= m_fMoveRate;
            if (m_f350 < 0.0f) {
                m_f350 = 0.0f;
            }
        }
        if (m_f354 > 0.0f) {
            m_f354 -= m_fMoveRate;
            if (m_f354 < 0.0f) {
                m_f354 = 0.0f;
            }
        }
        if (m_f358 > 0.0f) {
            m_f358 -= m_fMoveRate;
            if (m_f358 < 0.0f) {
                m_f358 = 0.0f;
            }
        }
        if (m_f380 > 0.0f) {
            m_f380 -= m_fMoveRate;
            if (m_f380 < 0.0f) {
                m_f380 = 0.0f;
            }
        }
    }

    void CBallBoss::AttachTailEffect_()
    {
        if (m_bCollOff != 0) {
            EffectStopTail_();
            Eff_StopAll();
        }
        if (m_nCoinPower > 0) {
            long level = GetPowerLevel(m_nCoinPower);
            if (IsShootMode() && m_nCoinPower >= 5 && m_n42C == 0) {
                level++;
            }
            if (s_aPowerTailEffect[level] >= 0) {
                Eff_CreateAttach(8, s_aPowerTailEffect[level], 0, 0, 3);
            } else {
                Eff_Stop(8);
            }
        }
    }

    void CBallBoss::CtorStatePass()
    {
        m_nFlag |= 0x3000;
        AttachTailEffect_();
    }

    void CBallBoss::LoopStatePass()
    {
        UpdateCountWork_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();
    }

    void CBallBoss::DtorStatePass()
    {
    }

    void CBallBoss::CtorStateFreeMove()
    {
        CBall::CtorStateFreeMove();
    }

    void CBallBoss::LoopStateFreeMove()
    {
        UpdateCountWork_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();
        if (CGame::g_GameWork.m_nFlag18 & 1) {
            AdjustGravity_();
        }
    }

    void CBallBoss::DtorStateFreeMove()
    {
        CBall::DtorStateFreeMove();
    }

    void CBallBoss::CtorStateAttach()
    {
        if (m_n384 != 0) {
            m_n388 = 0;
            m_f38C = 60.0f;
        }
        CBall::CtorStateAttach();
    }

    void CBallBoss::LoopStateAttach()
    {
        if (m_n384 != 0 && (CGame::g_uPauseBit & 0x4)) {
            if (m_n388 == 0) {
                m_f38C -= m_fMoveRate;
                if (m_f38C < 0.0f) {
                    m_f38C = 0.0f;
                    m_n388++;
                }
            } else {
                m_f38C += 0.33333334f * m_fMoveRate;
                float lift = m_f38C > 10.0f ? 10.0f : (m_f38C < 0.0f ? 0.0f : m_f38C);
                m_f38C = lift;
                m_f18C = lift;
            }
        } else {
            CBall::LoopStateAttach();
        }
    }

    void CBallBoss::DtorStateAttach()
    {
        CBall::DtorStateAttach();
    }

    void CBallBoss::CtorStateAlleyoop()
    {
        m_nFlag |= 0x3000;
        AttachTailEffect_();
    }

    void CBallBoss::LoopStateAlleyoop()
    {
        LoopStatePass();
    }

    void CBallBoss::DtorStateAlleyoop()
    {
    }

    void CBallBoss::CtorStateChargeShoot()
    {
    }

    void CBallBoss::LoopStateChargeShoot()
    {
        ChangeState(6);
    }

    void CBallBoss::DtorStateChargeShoot()
    {
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
    }

    void CBallBoss::CtorStateLayupShoot()
    {
        m_nFlag &= ~0x40;
        m_nFlag |= 0x3000;
        AttachTailEffect_();
        if (IsItemEffect()) {
            if (m_pBodyColl != 0) {
                m_pBodyColl->ResetTargetFlag(1, 2);
            }
        }
    }

    inline void CBallBoss::UpdateSpin_()
    {
        switch (m_nSpinType) {
            case 0:
                m_fSpinCount = 0.0f;
                m_vSpinAccel.x = 0.0f;
                m_vSpinAccel.y = 0.0f;
                m_vSpinAccel.z = 0.0f;
                break;
            case 1:
            case 2:
            case 3:
                if (m_f354 <= 0.0f) {
                    m_fSpinCount -= m_fMoveRate;
                    if (m_fSpinCount <= 0.0f) {
                        m_fSpinCount = 0.0f;
                        m_vSpinAccel.x = 0.0f;
                        m_vSpinAccel.y = 0.0f;
                        m_vSpinAccel.z = 0.0f;
                        m_nSpinType = 0;
                    }
                }
                break;
        }
    }

    void CBallBoss::LoopStateLayupShoot()
    {
        float scale = 1.0f;
        switch (GetPowerLevel(m_nPrevCoinPower)) {
            case 1:
                scale = 1.125f;
                break;
            case 2:
                scale = 1.3f;
                break;
            case 3:
                scale = 1.7f;
                break;
            case 4:
                scale = 2.2f;
                break;
            case 5:
                scale = 3.0f;
                break;
        }
        if (m_fScale < scale) {
            m_fScale += 0.125f * (scale - m_fScale);
            m_fScale = scale > m_fScale ? m_fScale : scale;
        }
        UpdateCountWork_();
        UpdateSpin_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();
    }

    void CBallBoss::DtorStateLayupShoot()
    {
        if (m_pBodyColl != 0) {
            m_pBodyColl->AddTargetFlag(1, 2);
        }
    }

    void CBallBoss::CtorStateNormalShoot()
    {
        m_nFlag &= ~0x40;
        m_nFlag |= 0x3000;
        AttachTailEffect_();
        if (IsItemEffect()) {
            if (m_pBodyColl != 0) {
                m_pBodyColl->ResetTargetFlag(1, 2);
            }
        }
        ClearVolleyChargePower();
    }

    void CBallBoss::LoopStateNormalShoot()
    {
        UpdateCountWork_();
        UpdateSpin_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();
    }

    void CBallBoss::DtorStateNormalShoot()
    {
        if (m_pBodyColl != 0) {
            m_pBodyColl->AddTargetFlag(1, 2);
        }
    }

    void CBallBoss::CtorStateArtsShoot()
    {
        m_nFlag &= ~0x2000;
        m_nFlag |= 0x3000;
        if (TrgItemEffect_(0) & 0x10) {
            m_nBossFlag |= BOSSFLAG_ITEM_HIT;
        }
        ClearVolleyChargePower();
    }

    void CBallBoss::LoopStateArtsShoot()
    {
        UpdateCountWork_();
        UpdateSpin_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        RotNormal_();
        CheckCollision_();
    }

    void CBallBoss::DtorStateArtsShoot()
    {
        ClearOffsetPos();
    }

    void CBallBoss::CtorStateJumpBall()
    {
        Eff_CreateSimple(0x566, m_Pos, 3, 0);
        PlaySePos(0xE09, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        Eff_CreateAttach(8, 0x54D, 0, 0, 3);
        SetAlphaRate(1.0f);
        m_f350 = 30.0f;
        bcs::BCS_Phys_CalcYVG(60.0f, m_Pos.y, 0.0f, m_Pos.y + 2.0f, m_Velocity.y, m_fGravity);
    }

    void CBallBoss::LoopStateJumpBall()
    {
        UpdateCountWork_();
        FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
        CheckCollision_();
        RotNormal_();
        if (CGame::g_GameWork.m_nFlag18 & 1) {
            AdjustGravity_();
        }
    }

    void CBallBoss::DtorStateJumpBall()
    {
    }

    void CBallBoss::CtorStateTouchDown()
    {
        m_nSpinType = 0;
        m_fFadeCount = 16.0f;
        m_f350 = 26.0f;
        m_f354 = 0.0f;
        m_f320 = 0.0f;
        m_fSpinCount = 0.0f;
        m_vSpinAccel.x = 0.0f;
        m_vSpinAccel.y = 0.0f;
        m_vSpinAccel.z = 0.0f;
        bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
        if (m_fFadeCount > 0.0f) {
            m_nStateStep = 0;
            SetSquashParam(up, 1.0f, m_fFadeCount, 2.0f, 6.0f);
        } else {
            m_nStateStep = 1;
            SetSquashParam(up, 1.0f, 1.0f, 2.0f, 6.0f);
        }
    }

    void CBallBoss::LoopStateTouchDown()
    {
        int bEnd = 0;
        switch (m_nStateStep) {
            case 0:
                if (m_fFadeCount > 0.0f) {
                    m_fFadeCount -= m_fMoveRate;
                } else {
                    if (CGame::IsReqArtsEvent() || CGame::IsArtsEventMode()) {
                        break;
                    }
                    m_nStateStep++;
                }
                if (m_pMdlObj != 0 && m_n1E0 > 0) {
                    float squash;
                    if (m_n1E0 == 3) {
                        squash = (1.0f - m_f1E4) * m_f1DC;
                    } else if (m_n1E0 == 2) {
                        squash = m_f1DC;
                    } else {
                        squash = m_f1E4 * m_f1DC;
                    }
                    m_pMdlObj->m_vTransOffset.y = (squash * GetBgCollRadius()) * -1.0f;
                }
                {
                    float alpha = m_fFadeCount * 0.0625f;
                    SetAlphaRate(alpha > 1.0f ? 1.0f : (alpha < 0.0f ? 0.0f : alpha));
                }
                break;
            case 1: {
                m_Velocity.Set(0.0f, 0.0f, 0.0f);
                if (m_pMdlObj != 0) {
                    m_pMdlObj->m_vTransOffset.Set(0.0f, 0.0f, 0.0f);
                }
                Eff_StopAll();
                m_nSpinType = 0;
                m_fSpinCount = 0.0f;
                m_vSpinAccel.x = 0.0f;
                m_vSpinAccel.y = 0.0f;
                m_vSpinAccel.z = 0.0f;
                m_n35C = 0;
                m_nFlag &= ~0x40;
                SetBlinkDisable();
                RotReduce_(0.95f);
                SetLooseBall_();
                bEnd = 1;
                m_nStateStep++;
                break;
            }
            default:
                bEnd = 1;
                break;
        }
        if (bEnd) {
            m_nVsBossBallMode = 0;
            ChangeState(0);
        }
    }

    void CBallBoss::DtorStateTouchDown()
    {
        m_nVsBossBallMode = 0;
        m_fScale = 1.0f;
        ClearVsBossCoinPower();
        m_fFadeCount = 0.0f;
        if (m_pMdlObj != 0) {
            m_pMdlObj->m_vTransOffset.Set(0.0f, 0.0f, 0.0f);
        }
    }

    void CBallBoss::CheckCollision_()
    {
        bcs::FVEC3 pos;
        bcs::FVEC3 normal;
        int hit = CollSubCheckBG_(pos, normal);
        if (hit == 0) {
            m_nFlag &= ~0x80;
            return;
        }
        int bWall = 0;
        m_nFlag &= ~0x40;
        m_Pos = pos;
        if (hit == 1) {
            ClearSpin_();
            m_n35C = 0;
            if (!(m_nFlag & 0x80)) {
                PlayWallHitSe(1);
                m_nFlag |= 0x80;
            }
            VelReduce_(CBallMan::GetWallBoundDecRate());
            RotReduce_(0.95f);
            bWall = 1;
        }
        SetBlinkDisable();
        float dot = 2.0f * bcs::FVEC3Dot(&m_Velocity, &normal);
        bcs::FVEC3 reflect;
        bcs::FVEC3Scale(&reflect, &normal, dot);
        bcs::FVEC3Sub(&m_Velocity, &m_Velocity, &reflect);
        SetLooseBall_();
        if (bWall) {
            GetPosAfterFrame(m_vTargetPos, floor(GetTouchDownFrame(GetBgCollRadius())));
            m_vTargetPos.y = 0.0f;
            UpdateCameraTarget_(0);
        }
        CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x87), 0, 0, 0, 0);
    }

    int CBallBoss::CollSubCheckNet_(bcs::FVEC3 &, bcs::FVEC3 &)
    {
        return 0;
    }

    void CBallBoss::CollSubCheckTgtPos_()
    {
    }

    int CBallBoss::CollSubCheckBG_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal)
    {
        if (GetState() == 12) {
            return 0;
        }
        float radius = GetBgCollRadius();
        bcs::FVEC3 flow(0.0f, 0.0f, 0.0f);
        int ret = 0;
        if (m_Pos.y <= radius && m_Velocity.y <= 0.0f) {
            unsigned long attr;
            CCollMan::GetGrdAttr(m_Pos, attr, flow);
            if (CCollMan::IsGrdAttrFlow(attr)) {
                m_Pos.x += flow.x;
                m_Pos.z += flow.z;
            } else {
                flow.Set(0.0f, 0.0f, 0.0f);
            }
        }
        if (m_nBossFlag & BOSSFLAG_RIDE) {
            bcs::CScnObj *pObj = bcs::CScnObj::GetScnObj(m_nRideObjHandle);
            if (pObj == 0) {
                m_nRideObjHandle = 0;
                m_nBossFlag &= ~BOSSFLAG_RIDE;
            } else {
                bcs::FVEC3 objPos = pObj->m_vTrans;
                bcs::FVEC3 move;
                move.x = objPos.x - m_vRideObjPos.x;
                move.z = objPos.z - m_vRideObjPos.z;
                m_Pos.x += move.x;
                m_Pos.z += move.z;
                m_PrevPos.x += move.x;
                m_PrevPos.z += move.z;
                m_vRideObjPos = objPos;
            }
        }
        bcs::FVEC3 hitPos;
        float time;
        int bHit = 0;
        if (CCollMan::DetectMapHit(hitPos, rNormal, time, m_PrevPos, m_Pos, -10.0f, 2, 0)) {
            bHit = 1;
        }
        if (m_nFlag & 0x40) {
            if (bHit) {
                hitPos = m_Pos;
                bHit = 0;
            } else {
                m_nFlag &= ~0x40;
            }
        }
        if (bHit) {
            SetSquashParam(rNormal, 1.0f, 0.0f, 0.0f, 6.0f);
            if (hitPos.y < radius) {
                hitPos.y = radius;
            }
            ret = 1;
            rPos = hitPos;
            if (hitPos.y <= radius && m_Velocity.y < 0.0f && m_Velocity.y >= -0.0375f) {
                m_Velocity.y = 0.0f;
            }
        } else if (m_Pos.y < radius && m_Velocity.y < 0.0f) {
            if (m_fMoveRate > 0.0f) {
                m_PrevPos.x += flow.x;
                m_PrevPos.z += flow.z;
                rNormal.Set(0.0f, 1.0f, 0.0f);
                bcs::FVEC3 floorPoint(0.0f, radius, 0.0f);
                bcs::FVEC3 cross;
                float t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(cross, rNormal, floorPoint, m_PrevPos,
                                                                           m_Pos);
                if (t >= 0.0f && t <= 1.0f) {
                    m_Velocity = m_PrevVel;
                    m_Pos = m_PrevPos;
                    FuncMove_(m_Pos, m_Velocity, m_fGravity, t * m_fMoveRate);
                    if (m_Pos.y < radius) {
                        m_Pos.y = radius;
                    }
                    TrgHitFloor_();
                    ChangeState(12);
                    return 0;
                }
            }
            if (m_Pos.y < radius) {
                m_Pos.y = radius;
            }
            ClearSpin_();
            m_n35C = 0;
            m_nFlag &= ~0x40;
            SetBlinkDisable();
            RotReduce_(0.95f);
            SetLooseBall_();
            ret = 0;
        }
        return ret;
    }

    int CBallBoss::CanbeTouchDown_()
    {
        return 0;
    }

    void CBallBoss::TrgGoalIn(long, long, long, long, long)
    {
    }

    void CBallBoss::TrgHitFloor_()
    {
        StopTailEffect();
        Eff_StopAll();
        if (m_n40C == 0 && !(m_nFlag & FLAG_SCORED) && m_bCollOff == 0) {
            m_f350 = 999.0f;
            long nEffect;
            long nSe;
            if (GetVsBossCoinPower() >= 5) {
                nEffect = 0x556;
                nSe = 0xE0D;
            } else {
                nEffect = 0x565;
                nSe = 0xE0B;
            }
            if (nEffect >= 0) {
                bcs::FVEC3 pos = m_Pos;
                pos.y = 0.0f;
                Eff_CreateSimple(nEffect, pos, 3, 0);
            }
            if (nSe >= 0) {
                PlaySePos(nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            if (TrgItemEffect_(3) & 0x10) {
                m_nBossFlag |= BOSSFLAG_ITEM_HIT;
            }
            m_nFlag |= 0x3000;
            UpdateCameraTarget_(0);
            CGame::AddTeamCoin(0, -CGame::GetTeamCoin(0));
            ClearVsBossCoinPower();
            SetTouch_(-1, -1);
            CGame::SetOffenseTeam(0);
            m_n338 = 0;
            m_n40C = 1;
            m_n410 = 0;
            m_f414 = 0.0f;
        }
    }

    void CBallBoss::SetTouch_(long nTeam, long nChr)
    {
        if (m_nShotTeamIdx == nTeam) {
            if (m_nShotChrIdx != nChr) {
                m_n338++;
                m_n33C = m_n338;
            }
        } else {
            m_n338 = 1;
        }
        m_nShotChrIdx = nChr;
        m_nShotTeamIdx = nTeam;
    }

    void CBallBoss::SetPassModeParam_(const bcs::FVEC3 &rTarget, float fSpeed, float fRate, float fFrame,
                                      float fHeight)
    {
        bcs::FVEC3 target = rTarget;
        float frame = fFrame;
        InitFlight_(target);
        float dist = CalcXZVel2(m_Pos, target, m_Velocity, m_f320, frame, fSpeed, fSpeed * fRate, fFrame, fHeight);
        if (dist <= 0.0f && frame <= 0.0f) {
            UpdateCameraTarget_(1);
            return;
        }
        float top = MapRange(dist, 4.0f, 10.0f, 6.0f, 7.0f);
        float minTop = 5.0f;
        float add = 1.0f;
        if (fHeight > 0.0f) {
            minTop = 0.0f;
            top = 0.0f;
            add *= 0.25f;
        }
        if (m_Pos.y > top) {
            top = m_Pos.y + add;
        }
        if (target.y > top) {
            top = target.y + add;
        }
        if (minTop < top) {
            minTop = top;
        }
        CalcYVG(m_Pos.y, minTop, target.y, frame, m_Velocity.y, m_fGravity);
        m_fRotSpeed = 0.2617994f;
        if (m_vTargetPos.y > 0.0f) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(GetBgCollRadius()));
            m_vTargetPos.y = 0.0f;
        }
        UpdateCameraTarget_(1);
    }

    void CBallBoss::SetVsBossPassMode(const bcs::FVEC3 &rTarget, long nType, long nChr, long nTeam, long nPassChr,
                                      long, unsigned long)
    {
        Detach_();
        CmnShotInit_();
        CGame::AddTeamCoin(0, 1);
        m_nShootType = 0;
        SetTouch_(nTeam, nChr);
        m_nPassChrIdx = nPassChr;
        bcs::FVEC3 target = rTarget;
        SetShotChrNoHit_();
        m_nFlag &= ~0x40;
        m_f358 = 20.0f;
        GetItemEffect_();
        PASS_PARAM_ param;
        SetBadPassParam_(0, nType, param);
        bcs::FVEC3 from(m_Pos.x, 0.0f, m_Pos.z);
        bcs::FVEC3 to(target.x, 0.0f, target.z);
        float dist = nw4r::math::FSqrt(bcs::FVEC3DistSq(&from, &to));
        float top = MapRange(dist, 4.0f, 10.0f, 6.0f, 7.0f);
        float minTop = 5.0f;
        float add = 0.0f;
        if (param.m_f0C > 0.0f) {
            minTop = 0.0f;
            top = 0.0f;
            add *= 0.25f;
        }
        if (m_Pos.y > top) {
            top = m_Pos.y + add;
        }
        if (target.y > top) {
            top = target.y + add;
        }
        if (minTop < top) {
            minTop = top;
        }
        float speed = param.m_f00;
        InitFlight_(target);
        float frame = param.m_f08;
        if (CalcXZVel2(m_Pos, target, m_Velocity, m_f320, frame, speed, speed * param.m_f04, param.m_f08,
                       param.m_f0C) <= 0.0f &&
            frame <= 0.0f) {
            UpdateCameraTarget_(1);
        } else {
            CalcYVG(m_Pos.y, minTop, target.y, frame, m_Velocity.y, m_fGravity);
            if (m_vTargetPos.y > 0.0f) {
                GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(GetBgCollRadius()));
                m_vTargetPos.y = 0.0f;
            }
            SetSpikeJumpTgtWork_(target.y);
            UpdateCameraTarget_(1);
        }
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        m_f350 = 8.0f;
        m_fRotSpeed = 0.2617994f;
        m_nVsBossBallMode = 3;
        ChangeState(3);
    }

    void CBallBoss::SetBadPassParam_(long, long nType, PASS_PARAM_ &rParam)
    {
        rParam.m_f10 = 0.0f;
        rParam.m_f14 = 0.0f;
        rParam.m_f18 = 0.0f;
        rParam.m_f1C = 0.0f;
        rParam.m_f20 = 0.0f;
        rParam.m_f24 = 0.0f;
        rParam.m_f28 = 0.0f;
        rParam.m_f2C = 0.0f;
        rParam.m_n30 = 0;
        if (nType == 2) {
            rParam.m_f00 = 0.15f;
            rParam.m_f04 = 0.6f;
            rParam.m_f0C = 40.0f;
            rParam.m_f08 = 35.0f;
        } else {
            rParam.m_f00 = 0.116666675f;
            rParam.m_f04 = 0.5f;
            rParam.m_f0C = 0.0f;
            rParam.m_f08 = 120.0f;
        }
    }
}

namespace msw {
    namespace BALL {
        namespace BOSS {
            SPIKE_SHOT_ARG::SPIKE_SHOT_ARG()
            {
                m_vUnk00.Set(0.0f, 0.0f, 0.0f);
                m_nUnk0C = 0;
                m_nUnk10 = 0;
                m_nUnk14 = 0;
                m_nUnk18 = 0;
                m_nUnk1C = 0;
                m_nUnk20 = 0;
                m_nUnk24 = 0;
                m_fUnk28 = 0.0f;
                m_nUnk2C = -1;
                m_nUnk30 = -1;
                m_nUnk34 = -1;
                m_fUnk38 = 1.0f;
                m_nUnk3C = -1;
                m_fUnk40 = 0.0f;
                m_nUnk44 = -1;
                m_nUnk48 = -1;
                m_nUnk4C = -1;
                m_nUnk50 = -1;
                m_nUnk54 = 1;
                m_fUnk58 = 10.0f;
                m_fUnk5C = 10.0f;
                m_fUnk60 = 10.0f;
                m_fUnk64 = 1.0f;
                m_fUnk6C = 0.0f;
                m_fUnk70 = 0.0f;
                m_fUnk74 = 0.5f;
                m_fUnk78 = 0.5f;
                m_fUnk7C = 0.0f;
                m_fUnk80 = 0.0f;
                m_fUnk84 = 0.0f;
                m_fUnk88 = 0.0f;
                m_fUnk8C = 0.0f;
                m_fUnk90 = 0.0f;
                // (guessed) Dead comparisons: the original's literal pool has
                // 2 pi / 256, 2 pi and pi / 180 here, ahead of the spike
                // code's constants, most likely from a function the module
                // link stripped.
                if (m_fUnk90 == 0.024543693f) {
                }
                if (m_fUnk90 == 6.2831855f) {
                }
                if (m_fUnk90 == 0.017453292f) {
                }
            }
        }
    }
}

namespace msw {

    int CBallBoss::SetVsBossSpikeShotMode(BALL::BOSS::SPIKE_SHOT_ARG &rArg)
    {
        int result = 0;
        GetItemEffect_();
        Detach_();
        CmnShotInit_();
        if (rArg.m_nUnk0C == 5) {
            ClearVolleyChargePower();
        }
        SetTouch_(rArg.m_nUnk30, rArg.m_nUnk2C);
        m_nShotTeamIdx = rArg.m_nUnk30;
        m_nShotChrIdx = rArg.m_nUnk2C;
        m_n334 = rArg.m_nUnk34;
        SetShotChrNoHit_();
        m_nShootType = 1;
        m_n340 = 0x7F;
        if (rArg.m_nUnk3C >= 0 && AttachItemEffect(rArg.m_nUnk3C)) {
            result |= 0x8;
        }
        CItemBall *pItem = GetItemEffect_();
        result |= SetSpikeParam_(rArg);
        if (TrgItemEffect_(0) & 0x10) {
            m_nBossFlag |= BOSSFLAG_ITEM_HIT;
        }
        if (rArg.m_nUnk44 >= 0) {
            PlaySePos(rArg.m_nUnk44, 0, rArg.m_nUnk54, -1, 0.0f, 1.0f, 0.0f);
        }
        if (rArg.m_nUnk48 >= 0) {
            PlaySePos(rArg.m_nUnk48, 0, rArg.m_nUnk54, -1, 0.0f, 1.0f, 0.0f);
        }
        if (rArg.m_nUnk4C >= 0) {
            Eff_CreateSimple(rArg.m_nUnk4C, m_Pos, rArg.m_fUnk40, 3, 0);
        }
        if (pItem != 0) {
            result |= 0x10;
            ChangeState(7);
        } else {
            SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
            ChangeState(6);
        }
        return result;
    }

    int CBallBoss::SetSpikeParam_(BALL::BOSS::SPIKE_SHOT_ARG &rArg)
    {
        bcs::FVEC3 target = rArg.m_vUnk00;
        m_n35C = rArg.m_nUnk0C;
        if (!((1 << m_n35C) & CanbeVolleySpikeVariation())) {
            m_n35C = 0;
        }
        float radius = GetBgCollRadius();
        if (target.y < radius) {
            target.y = radius;
        }
        int bDirect = 1;
        InitFlight_(target);
        float speedMin = rArg.m_fUnk58;
        float speedMax = rArg.m_fUnk60;
        float speedRate = rArg.m_fUnk64;
        float minFrame = rArg.m_fUnk6C;
        float heightMin = rArg.m_fUnk74;
        float heightMax = rArg.m_fUnk78;
        switch (rArg.m_nUnk20) {
            case 0:
                bDirect = 1;
                break;
            case 2:
                bDirect = 0;
                break;
        }
        float slow = 1.0f;
        if (!bDirect && rArg.m_fUnk70 > 0.0f) {
            slow -= 0.01f * rArg.m_fUnk70;
        }
        slow = slow < 0.1f ? 0.1f : slow;
        switch (rArg.m_nUnk0C) {
            case 4:
                m_n3DC = 0;
                m_nFloorHitSe = 0xC31;
                break;
            case 1:
                m_n3DC = 2;
                m_nFloorHitSe = 0xC33;
                break;
            case 3:
                m_n3DC = 1;
                m_nFloorHitSe = 0xC32;
                break;
            default:
                if (rArg.m_nUnk10 == 1) {
                    m_n3DC = 2;
                    m_nFloorHitSe = 0xC33;
                } else {
                    m_n3DC = 1;
                    m_nFloorHitSe = 0xC32;
                }
                break;
        }
        m_nShootType = 1;
        float speed = 0.016666668f * (slow * Lerp(speedMin, speedMax, 0.0f));
        float frame;
        float dist = CalcXZVel2(m_Pos, target, m_Velocity, m_f320, frame, speed, speed * speedRate, minFrame, 0.0f);
        if (dist <= 0.0f) {
            UpdateCameraTarget_(1);
            return 0;
        }
        float frameUp = ceil(frame);
        if (rArg.m_fUnk28 > 0.0f) {
            m_f350 = ceil(frameUp * rArg.m_fUnk28);
        }
        float curve = 0.0f;
        float drop = 0.0f;
        int bSpin = 0;
        long nSpin = rArg.m_nUnk18;
        if (rArg.m_nUnk0C == 3) {
            curve = bcs::BCS_Abs(dist * rArg.m_fUnk7C);
            if (curve < rArg.m_fUnk80) {
                curve = rArg.m_fUnk80;
            }
            if (curve > rArg.m_fUnk84) {
                curve = rArg.m_fUnk84;
            }
            drop = bcs::BCS_Abs(dist * rArg.m_fUnk88);
            if (drop < rArg.m_fUnk8C) {
                drop = rArg.m_fUnk8C;
            }
            if (drop > rArg.m_fUnk90) {
                drop = rArg.m_fUnk90;
            }
            int bFlip = rArg.m_nUnk1C;
            if (rArg.m_fUnk7C < 0.0f) {
                curve += drop;
                drop = -drop;
                bFlip = !bFlip;
                if (rArg.m_fUnk88 < 0.0f) {
                    curve = -curve;
                    drop = -drop;
                }
            } else if (rArg.m_fUnk88 < 0.0f) {
                curve += drop;
                drop = -drop;
            }
            bSpin = 1;
            switch (nSpin) {
                case 1:
                    if (bFlip) {
                        nSpin = 2;
                    }
                    break;
                case 2:
                    if (bFlip) {
                        nSpin = 1;
                    }
                    break;
                default: {
                    float left;
                    float right;
                    CStageMan::GetInstance()->GetTeamCourtRect(GetTargetCourtIdx(), &left, &right, 0, 0);
                    float center = 0.5f * (left + right);
                    bcs::FVEC3 toTarget = target;
                    bcs::FVEC3 toMirror = target;
                    toMirror.x = center + (center - target.x);
                    toTarget.x -= m_Pos.x;
                    toTarget.z -= m_Pos.z;
                    toMirror.x -= m_Pos.x;
                    toMirror.z = toTarget.z;
                    bcs::FVEC3 cross;
                    PSVECCrossProduct((const Vec *)&toTarget, (const Vec *)&toMirror, (Vec *)&cross);
                    if (cross.y > 0.0f) {
                        nSpin = drop < 0.0f ? 2 : 1;
                    } else {
                        nSpin = drop < 0.0f ? 1 : 2;
                    }
                    break;
                }
            }
        }
        if (bSpin) {
            m_nSpinType = nSpin;
            m_fSpinCount = 0.0f;
            bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
            bcs::FVEC3 dir = m_Velocity;
            NormalizeXZ(dir, dir);
            PSVECCrossProduct((const Vec *)&up, (const Vec *)&dir, (Vec *)&m_vSpinAccel);
            bcs::FVEC3 side;
            NormalizeXZ(side, m_vSpinAccel);
            float vel = 0.0f;
            float accel = 0.0f;
            bcs::BCS_Phys_CalcYVG(frameUp, 0.0f, -drop, curve, vel, accel);
            if (nSpin == 1) {
                vel = -vel;
                accel = -accel;
            }
            m_vSpinAccel.x = side.x * accel;
            m_vSpinAccel.z = side.z * accel;
            bcs::FVEC3 add;
            add.x = side.x * vel;
            add.z = side.z * vel;
            m_fSpinCount = frameUp;
            m_Velocity.x += add.x;
            m_Velocity.z += add.z;
        }
        if (bDirect) {
            m_fGravity = 0.0f;
            m_Velocity.y = (1.0f / frameUp) * (target.y - m_Pos.y);
        } else {
            float net = CStageMan::GetInstance()->GetNetHeightTop();
            if (heightMin > heightMax) {
                float tmp = heightMin;
                heightMin = heightMax;
                heightMax = tmp;
            }
            float top;
            if (heightMin != heightMax) {
                top = net + bcs::BCS_RandRange(heightMin, heightMax);
            } else {
                top = net + heightMin;
            }
            CalcYVG(m_Pos.y, top, target.y, frame, m_Velocity.y, m_fGravity);
        }
        m_fRotSpeed = 0.27925268f;
        float r = GetBgCollRadius();
        if (m_vTargetPos.y > r) {
            GetPosAfterFrame(m_vTargetPos, GetTouchDownFrame(r));
            m_vTargetPos.y = 0.0f;
        }
        UpdateCameraTarget_(1);
        return 0;
    }

    void CBallBoss::SetVsBossShootMode(const bcs::FVEC3 &rTarget, long nChr, float fSpeed)
    {
        bcs::FVEC3 target = rTarget;
        Detach_();
        CmnShotInit_();
        SetTouch_(0, nChr);
        m_nShootType = 6;
        m_n340 = 0x7F;
        m_nShotChrIdx = nChr;
        m_nShotTeamIdx = 0;
        m_n334 = 1;
        SetShotChrNoHit_();
        float speed = 0.016666668f * fSpeed;
        float frame = 60.0f;
        CalcXZVel2(m_Pos, target, m_Velocity, m_f320, frame, speed, 0.98f * speed, 0.0f, 0.0f);
        ceil(frame);
        CalcYVG(m_Pos.y, 1.0f + m_Pos.y, target.y, frame, m_Velocity.y, m_fGravity);
        m_fRotSpeed = 0.27925268f;
        if (m_nCoinPower >= 5) {
            PlaySePos(0xE0E, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        ChangeState(8);
    }

    void CBallBoss::SetVsBossDunkShotMode(const bcs::FVEC3 &rTarget, long nChr)
    {
        bcs::FVEC3 target = rTarget;
        Detach_();
        CmnShotInit_();
        SetTouch_(0, nChr);
        m_nShootType = 3;
        m_n340 = 0x7F;
        m_nShotChrIdx = nChr;
        m_nShotTeamIdx = 0;
        m_n334 = 1;
        SetShotChrNoHit_();
        bcs::FVEC3 dir;
        bcs::FVEC3Sub(&dir, &target, &m_Pos);
        bcs::BCS_Vec_Normalize(dir, dir);
        bcs::FVEC3Scale(&dir, &dir, 1.0f);
        m_Velocity = dir;
        m_f320 = 0.0f;
        m_fGravity = 0.0f;
        m_fRotSpeed = 0.13962634f;
        ChangeState(8);
    }

    void CBallBoss::SetVsBossArtsDunkMode(const bcs::FVEC3 &, long, int bEnd)
    {
        bcs::FVEC3 target(m_Pos.x, 0.0f, m_Pos.z);
        Eff_CreateSimple(0x555, m_Pos, 3, 0);
        PlaySePos(0xE0D, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        PlaySePos(0xDFD, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        if (bEnd) {
            Eff_StopAll();
            CGame::AddTeamCoin(0, -CGame::GetTeamCoin(0));
            ClearVsBossCoinPower();
            m_fFadeCount = 0.0f;
            m_f350 = 26.0f;
            m_f354 = 0.0f;
            m_f320 = 0.0f;
            ClearSpin_();
            m_n35C = 0;
            m_nFlag &= ~0x40;
            SetBlinkDisable();
            RotReduce_(0.95f);
            SetLooseBall_();
            m_n42C = 0;
            m_nVsBossBallMode = 0;
            m_fScale = 1.0f;
            ChangeState(0);
            return;
        }
        target.Set(m_Pos.x, 4.7f, m_Pos.z);
        if (m_n428 == 0) {
            bcs::FVEC3 ofs;
            bcs::BCS_VecZ_RotY(ofs, 6.0f, bcs::ROUND_PI(-0.7853982f));
            target.x += ofs.x;
            target.z += ofs.z;
        } else {
            bcs::FVEC3 ofs;
            bcs::BCS_VecZ_RotY(ofs, 6.0f, bcs::ROUND_PI(0.7853982f));
            target.x += ofs.x;
            target.z += ofs.z;
        }
        m_n428++;
        Detach_();
        CmnShotInit_();
        SetTouch_(0, -1);
        m_nShootType = 0;
        m_n340 = 0;
        SetShotChrNoHit_();
        bcs::FVEC3 dest = target;
        InitFlight_(dest);
        m_f358 = 0.0f;
        SetPassModeParam_(dest, 0.06666667f, 0.92f, 60.0f, 0.0f);
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        m_f350 = 99999.0f;
        m_nVsBossBallMode = 3;
        CGame::SetOffenseTeam(0);
        ChangeState(6);
        CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x9C), 1, 1, 0, 0);
        m_n40C = 0;
        m_n410 = 0;
    }

    int CBallBoss::IsCanbeTurnOver() const
    {
        return 1;
    }

    int CBallBoss::IsInvincible() const
    {
        return (m_nFlag >> 4) & 1;
    }

    int CBallBoss::CanbeCatch() const
    {
        if (m_bCollOff != 0) {
            return 0;
        }
        if (m_nBossFlag & BOSSFLAG_ITEM_HIT) {
            return 0;
        }
        if (GetAttachMdlObj() != 0 && !(m_nFlag & 0x8000)) {
            return 0;
        }
        if ((m_nFlag & 0x2) || (m_nFlag & 0x4)) {
            return 0;
        }
        if (m_f350 > 0.0f) {
            return 0;
        }
        return 1;
    }

    void CBallBoss::PlayBoundSe()
    {
        PlayFloorHitSe(1);
    }

    void CBallBoss::SetJumpBallMode()
    {
        if (GetAttachMdlObj() != 0) {
            Detach_();
        }
        ClearSpin_();
        m_n35C = 0;
        EffectStopTail_();
        Eff_StopAll();
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        m_Velocity.Set(0.0f, 0.0f, 0.0f);
        m_PrevVel.Set(0.0f, 0.0f, 0.0f);
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 0.2617994f;
        m_PrevPos = m_Pos;
        m_nShotChrIdx = -1;
        m_n40C = 0;
        m_n410 = 1;
        ChangeState(11);
    }

    void CBallBoss::SetChrReflectMode(long nChr, float fSpeed)
    {
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (GetAttachMdlObj() != 0) {
            Detach_();
        }
        ClearSpin_();
        m_n35C = 0;
        EffectStopTail_();
        UpdateCameraTarget_(0);
        SetBlinkDisable();
        m_nFlag &= ~0x10;
        m_nFlag |= 0x20;
        bcs::FVEC3 dir;
        pChr->GetCompPos(dir);
        bcs::FVEC3Sub(&dir, &m_Pos, &dir);
        if (bcs::BCS_Vec_GetLenXZ(dir) > 0.0f) {
            bcs::FVEC3 xz(dir.x, 0.0f, dir.z);
            bcs::BCS_Vec_Normalize(dir, xz);
            dir.y = 1.0f;
            bcs::BCS_Vec_Normalize(dir, dir);
        } else {
            dir.Set(0.0f, 1.0f, 0.0f);
        }
        if (fSpeed <= 0.0f) {
            bcs::FVEC3Scale(&m_Velocity, &dir, CBallMan::GetChrReflectSpeed());
        } else {
            bcs::FVEC3Scale(&m_Velocity, &dir, fSpeed);
        }
        m_PrevPos = m_Pos;
        m_fGravity = GetGravityDefault();
        m_fRotSpeed = 0.2617994f;
        if (TrgItemEffect_(5) & 0x10) {
            m_nBossFlag |= BOSSFLAG_ITEM_HIT;
        }
        m_n3DC = 0;
        m_nFloorHitSe = -1;
        ChangeState(1);
    }

    float CBallBoss::GetTouchDownFrame(float fHeight) const
    {
        return GetTouchDownFrame2(fHeight, 0.0f);
    }

    float CBallBoss::GetTouchDownFrame2(float fHeight, float fVelOfs) const
    {
        float vel = m_Velocity.y - fVelOfs;
        float gravity = m_fGravity + m_vSpinAccel.y;
        float y = m_Pos.y;
        float frame = 0.0f;
        if (bcs::BCS_Abs(gravity) <= 0.00001f) {
            if (!(y > fHeight && vel >= 0.0f) && !(y < fHeight && vel <= 0.0f)) {
                frame = (y - fHeight) / (-1.0f * vel);
            }
        } else {
            float roots[2];
            roots[1] = 0.0f;
            roots[0] = 0.0f;
            int num = nw4r::math::SolveEquation2(roots, 0.5f * gravity, vel, y - fHeight);
            if (num > 0) {
                if (num <= 1) {
                    frame = roots[0];
                } else {
                    frame = roots[0] < roots[1] ? roots[1] : roots[0];
                    if (frame < 0.0f) {
                        frame = 0.0f;
                    }
                }
            }
        }
        if (frame > 0.0f) {
            frame += m_f354;
        }
        return frame;
    }

    float CBallBoss::GetHeightPassFrame(float fHeight, int bFirst) const
    {
        float gravity = m_fGravity + m_vSpinAccel.y;
        float vel = m_Velocity.y;
        float y = m_Pos.y;
        float frame = 0.0f;
        if (bcs::BCS_Abs(gravity) <= 0.00001f) {
            frame = (y - fHeight) / (-1.0f * vel);
        } else {
            float roots[2];
            roots[1] = 0.0f;
            roots[0] = 0.0f;
            int num = nw4r::math::SolveEquation2(roots, 0.5f * gravity, vel, y - fHeight);
            if (num > 0) {
                if (num <= 1) {
                    frame = roots[0];
                } else if (bFirst) {
                    frame = roots[0] > roots[1] ? roots[1] : roots[0];
                    if (frame <= 0.0f) {
                        frame = roots[0] < roots[1] ? roots[1] : roots[0];
                    }
                } else {
                    frame = roots[0] < roots[1] ? roots[1] : roots[0];
                }
            }
        }
        if (frame > 0.0f) {
            frame += m_f354;
        }
        return frame;
    }

    void CBallBoss::GetVolleyPosAfterFrame(bcs::FVEC3 &rPos, float fFrame, int bSpin) const
    {
        float wait = m_f354;
        bcs::FVEC3 vel = m_Velocity;
        float gravity = m_fGravity;
        rPos = m_Pos;
        if (fFrame <= wait) {
            return;
        }
        float time = fFrame - wait;
        float speed = bcs::BCS_Vec_GetLenXZ(vel);
        bcs::FVEC3 dir = vel;
        NormalizeXZ(dir, dir);
        float decel = m_f320;
        if (m_f320 * time + speed < 0.0f) {
            float stop = bcs::BCS_Abs(speed / m_f320);
            bcs::FVEC3 accel;
            CalcFlightAccel_(accel, dir, m_f320, gravity, stop, bSpin);
            MoveFlight(rPos, vel, accel, stop);
            time -= stop;
            decel = 0.0f;
        }
        bcs::FVEC3 accel;
        CalcFlightAccel_(accel, dir, decel, gravity, time, bSpin);
        MoveFlight(rPos, vel, accel, time);
    }

    void CBallBoss::GetPosAfterFrame(bcs::FVEC3 &rPos, float fFrame) const
    {
        GetVolleyPosAfterFrame(rPos, fFrame, 1);
    }

    void CBallBoss::GetVelAfterFrame(bcs::FVEC3 &rVel, float fFrame) const
    {
        GetVolleyVelAfterFrame(rVel, fFrame, 1);
    }

    void CBallBoss::GetVolleyVelAfterFrame(bcs::FVEC3 &rVel, float fFrame, int bSpin) const
    {
        float wait = m_f354;
        bcs::FVEC3 vel = m_Velocity;
        float gravity = m_fGravity;
        rVel = vel;
        bcs::FVEC3 pos = m_Pos;
        if (fFrame <= wait) {
            return;
        }
        float time = fFrame - wait;
        float speed = bcs::BCS_Vec_GetLenXZ(vel);
        bcs::FVEC3 dir = vel;
        NormalizeXZ(dir, dir);
        float decel = m_f320;
        if (m_f320 * time + speed < 0.0f) {
            float stop = bcs::BCS_Abs(speed / m_f320);
            bcs::FVEC3 accel;
            CalcFlightAccel_(accel, dir, m_f320, gravity, stop, bSpin);
            bcs::FVEC3Add(&vel, &vel, &accel);
            time -= stop;
            decel = 0.0f;
        }
        bcs::FVEC3 accel;
        CalcFlightAccel_(accel, dir, decel, gravity, time, bSpin);
        bcs::FVEC3Add(&vel, &vel, &accel);
        rVel = vel;
    }

    long CBallBoss::GetTargetCourtIdx() const
    {
        return m_vTargetPos.z <= 0.0f;
    }

    int CBallBoss::IsTargetCourt(long nCourt) const
    {
        return GetTargetCourtIdx() == nCourt;
    }

    long CBallBoss::AttachItemEffect(long nKind)
    {
        CItemBall *pItem = 0;
        if (!(m_nFlag & FLAG_SCORED)) {
            switch (nKind) {
                case 2:
                    pItem = new (0x100) CItemBallBossToadstool(this);
                    break;
                case 3:
                    pItem = new (0x100) CItemBallBossBomb(this);
                    break;
                case 4:
                    pItem = new (0x100) CItemBallBossStar(this);
                    break;
                case 0:
                    pItem = new (0x100) CItemBallBossGShell(this);
                    break;
                case 1:
                    pItem = new (0x100) CItemBallBossRShell(this);
                    break;
                case 5:
                    pItem = new (0x100) CItemBallBossBanana(this);
                    break;
                default:
                    pItem = new (0x100) CItemBallBossGShell(this);
                    break;
            }
        }
        if (pItem != 0) {
            DetachItemEffect();
            LinkItemEffect_(pItem->GetId());
            pItem->DoTrg(CItemBall::TRG_CONSTRUCT);
            if (pItem->GetType() != 4) {
                new (0x100) CItemBallChangeEffectBoss(CGame::GetTempParent(), GetId());
            }
            return 1;
        }
        return 0;
    }

    float CBallBoss::CalcXZVel2(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, bcs::FVEC3 &rVel, float &rDecel,
                                float &rFrame, float fSpeed0, float fSpeed1, float fMinFrame, float fMaxFrame) const
    {
        bcs::FVEC3 diff;
        bcs::FVEC3Sub(&diff, &rTo, &rFrom);
        rVel.x = 0.0f;
        rVel.z = 0.0f;
        rDecel = 0.0f;
        rFrame = 0.0f;
        float dist = bcs::BCS_Vec_GetLenXZ(diff);
        if (dist <= 0.0f) {
            return 0.0f;
        }
        float frame = dist / (0.5f * (fSpeed0 + fSpeed1));
        if (fMinFrame > 0.0f && fMinFrame >= frame) {
            frame = fMinFrame;
        }
        if (fMaxFrame > 0.0f && fMaxFrame <= frame) {
            frame = fMaxFrame;
        }
        float avg = dist / frame;
        rFrame = frame;
        float speed = (2.0f * avg) / (1.0f + fSpeed1 / fSpeed0);
        rDecel = ((2.0f * avg - speed) - speed) / frame;
        NormalizeXZ(rVel, diff);
        rVel.x *= speed;
        rVel.z *= speed;
        return dist;
    }

    void CBallBoss::FuncMove_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fGravity, float fRate)
    {
        if (fRate <= m_f354) {
            return;
        }
        float time = fRate - m_f354;
        float speed = bcs::BCS_Vec_GetLenXZ(rVel);
        bcs::FVEC3 dir = rVel;
        NormalizeXZ(dir, dir);
        if (m_f320 * time + speed < 0.0f) {
            float stop = bcs::BCS_Abs(speed / m_f320);
            float decel = m_f320 * stop;
            bcs::FVEC3 accel(dir.x * decel, fGravity * stop, dir.z * decel);
            bcs::FVEC3 spin;
            bcs::FVEC3Scale(&spin, &m_vSpinAccel, stop);
            bcs::FVEC3Add(&accel, &accel, &spin);
            MoveFlight(rPos, rVel, accel, stop);
            time -= stop;
            m_f320 = 0.0f;
        }
        float decel = m_f320 * time;
        bcs::FVEC3 accel(dir.x * decel, fGravity * time, dir.z * decel);
        bcs::FVEC3 spin;
        bcs::FVEC3Scale(&spin, &m_vSpinAccel, time);
        bcs::FVEC3Add(&accel, &accel, &spin);
        MoveFlight(rPos, rVel, accel, time);
    }

    void CBallBoss::SetVsBossTossUpMode(const bcs::FVEC3 &, long nTeam, unsigned long)
    {
        bcs::FVEC3 target(m_Pos.x, 0.0f, m_Pos.z);
        int bKeep = 0;
        int bPower = 0;
        long nShootType = m_nShootType;
        if (GetVsBossCoinPower() >= 5) {
            bPower = 1;
            if (nShootType == 3 || nShootType == 6) {
                bKeep = 1;
            }
        }
        CGame::AddTeamCoin(0, -CGame::GetTeamCoin(0));
        ClearVsBossCoinPower();
        if (bPower) {
            Eff_CreateSimple(0x555, m_Pos, 3, 0);
            PlaySePos(0xE0D, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        m_nPrevCoinPower = 0;
        m_nCoinPower = 0;
        ChangePowerColor_(0);
        if (!bKeep) {
            Eff_StopAll();
            if (!bPower) {
                PlaySePos(0xE0B, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            m_fFadeCount = 0.0f;
            m_f350 = 26.0f;
            m_f354 = 0.0f;
            m_f320 = 0.0f;
            ClearSpin_();
            m_n35C = 0;
            m_nFlag &= ~0x40;
            SetBlinkDisable();
            RotReduce_(0.95f);
            SetLooseBall_();
            m_nVsBossBallMode = 0;
            m_fScale = 1.0f;
            ChangeState(0);
            return;
        }
        Eff_CreateAttach(8, 0x554, 0, 0, 3);
        float lenMin = 3.0f;
        float lenMax = 13.0f;
        if (lenMin > lenMax) {
            float tmp = lenMin;
            lenMin = lenMax;
            lenMax = tmp;
        }
        if (lenMax - lenMin > 0.0f) {
            lenMin = bcs::BCS_RandRange(lenMin, lenMax);
        }
        bcs::FVEC3 ofs;
        bcs::BCS_VecZ_RotY(ofs, lenMin, bcs::ROUND_PI(bcs::BCS_RandMax(6.2831855f)));
        target.x += ofs.x;
        target.z += ofs.z;
        long nChr = GetShotChrIdx();
        if (nChr >= 0 && nChr < CChrMan::GetChrCoreNum()) {
            CChrCore *pChr = CChrMan::GetChrCore(nChr);
            if (pChr != 0 && pChr->IsPadCtrl()) {
                const bcs::FVEC3 &rChrPos = pChr->GetPos();
                target.x = rChrPos.x;
                target.z = rChrPos.z;
                target.y = 0.0f;
            }
        }
        Detach_();
        CmnShotInit_();
        SetTouch_(nTeam, -1);
        m_nShootType = 0;
        m_n340 = 0;
        m_n34C = -1;
        EffectStopTail_();
        long nGoal = CGame::GetTargetGoalIdx(0);
        m_nShotChrIdx = 1;
        m_nShotTeamIdx = nTeam;
        m_n334 = nGoal;
        SetShotChrNoHit_();
        bcs::FVEC3 dest = target;
        bcs::FVEC3 move;
        bcs::FVEC3Sub(&move, &dest, &m_Pos);
        float angMin = bcs::BCS_Abs(10.0f);
        float angMax = bcs::BCS_Abs(30.0f);
        float maxLen = 13.0f;
        if (angMin > angMax) {
            float tmp = angMin;
            angMin = angMax;
            angMax = tmp;
        }
        if (angMin < angMax) {
            angMin = bcs::BCS_RandRange(angMin, angMax);
        }
        if (angMin > 0.0f) {
            if (bcs::BCS_RandMax(0x100L) & 1) {
                angMin = -angMin;
            }
            bcs::BCS_Vec_RotY(move, move, bcs::ROUND_PI(0.017453292f * angMin));
            bcs::FVEC3Add(&dest, &m_Pos, &move);
        }
        if (move.x * move.x + move.z * move.z > maxLen * maxLen) {
            bcs::FVEC3 dir;
            bcs::FVEC3 xz(move.x, 0.0f, move.z);
            bcs::BCS_Vec_Normalize(dir, xz);
            bcs::FVEC3Scale(&dest, &dir, maxLen);
            bcs::FVEC3Add(&dest, &dest, &m_Pos);
        }
        dest.y = GetBgCollRadius();
        InitFlight_(dest);
        m_f358 = 0.0f;
        float frame = 0.0f;
        if (frame <= 0.0f) {
            frame = 120.0f;
        }
        SetPassModeParam_(dest, 0.083333336f, 0.5f, frame, 0.0f);
        SetSquashParam(m_Velocity, 1.0f, 0.0f, 2.0f, 6.0f);
        m_f350 = 8.0f;
        m_nVsBossBallMode = 3;
        CGame::SetOffenseTeam(0);
        ChangeState(6);
        CChrMan::AllSendMsg(CChrDrv::MSG_ID(0x9C), 1, 1, 0, 0);
        m_n40C = 0;
        m_n410 = 0;
    }

    void CBallBoss::TrgVsBossArtsAttach(long, long, long, int)
    {
        long i = 0;
        do {
            CEffectCluster *pCluster = static_cast<CEffectCluster *>(Eff_GetCluster(i));
            if (pCluster != 0) {
                pCluster->ClearPauseFlag();
            }
            i++;
        } while (i < 16);
        Eff_EraseAll();
        m_n384 = 1;
        m_n388 = 0;
        m_f38C = 60.0f;
        m_f354 = 0.0f;
        SetAlphaRate(1.0f);
        m_nPrevCoinPower = 0;
        m_nCoinPower = 5;
        UpdateRainbow_();
        m_n428 = 0;
        m_n42C = 1;
        m_nFlag &= ~0x4;
    }
}
