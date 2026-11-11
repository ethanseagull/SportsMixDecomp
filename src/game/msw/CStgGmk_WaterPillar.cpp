#include "game/msw/CStgGmk_WaterPillar.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStageAnm.hpp"
#include "game/msw/CStgSrt.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "revolution/mtx.h"
#include <math.h>

// The water pillars of the fountain stage (stage 03), one original file
// like HockeyF's CStgGmk_WaterWorks_Hockey.cpp: the model
// (CStgGmk_WaterPillar_Mdl), the actor (CStgGmk_WaterPillar_Act), the
// driver (CStgGmk_WaterPillar_Drv), the gimmick (CStgGmk_WaterPillar) and
// the cluster (CStgGmk_WaterPillarCluster). Evidence: the gimmick's NewMdl
// inlines the model constructor and the cluster's NewGmk the gimmick's;
// one .data block holds the animation suffixes, the actor's state table
// and all five vtables. Weak functions of the classes follow at the end
// of .text (0x80195D7C to 0x801960B4).
//
// The cluster: GetParam_LocNum() pillars stand on an arc in front of the
// guarded goal, cut into three zones (0 to 2). While a character of the
// guarding team holds the ball, the pillars of the zone it stands in stop
// and those of the others run.

namespace {

    // Suffixes of the pillar's animations, appended to the model name.
    const char *s_AnmSuffix[] = { "", "_start", "_end", "_loop", 0 };

    // The model's node the rising water's effect is attached to.
    const char *s_pTopNodeName = "s03_gm_funsui_00_top";
    // The actor's effects fade in from s_fEffFadeNear to s_fEffFadeFar
    // from the camera.
    float s_fEffFadeNear = 6.0f;
    float s_fEffFadeFar = 20.0f;
    // The model's alpha test range (CFgMdl).
    float s_fAlphaTestNear = 2.0f;
    float s_fAlphaTestFar = 6.0f;

    // The pillar models; GetMdlIdx picks one.
    const msw::CStgMdl::MDL_PARAM s_MdlParam[] = {
        { "s03_gm_funsui_00", 0, 0, 0, s_AnmSuffix, "s03_gm_funsui_00", 0, 0 },
    };

    // (name guessed) rVec scaled to length len (zero stays zero).
    inline void SetLength(bcs::FVEC3 &rVec, float len)
    {
        bcs::BCS_Vec_Normalize(rVec, rVec);
        bcs::FVEC3Scale(&rVec, &rVec, len);
    }
}

namespace msw {

    CStgGmk_WaterPillar_Mdl::CStgGmk_WaterPillar_Mdl(CStgGmk *pParent, long idx) : CStgMdl(pParent)
    {
        m_nMdlIdx = idx;
    }

    CStgGmk_WaterPillar_Mdl::~CStgGmk_WaterPillar_Mdl()
    {
    }

    void CStgGmk_WaterPillar_Mdl::Create()
    {
        long idx = m_nMdlIdx;
        idx = idx > 0 ? 0 : (idx < 0 ? 0 : idx);
        CGenericMVAGimmickCreator creator(CLoadData::GetRes(2), &s_MdlParam[idx]);
        CStgMdl::Create(creator);
        SetAnm(0, 0, 1.0f, 0.0f, 0.0f, 8);
    }

    bcs::CMdlObj *CStgGmk_WaterPillar_Mdl::NewMdl(long heap) const
    {
        CFgMdl *pMdl = new (heap) CFgMdl;
        if (pMdl != 0) {
            pMdl->SetAlphaTestNearRange(2.0f);
            pMdl->SetAlphaTestFarRange(6.0f);
            pMdl->SetExBlendMode(1);
            pMdl->CalcBoundingVolume();
            pMdl->OnFlag(0x10000);
        }
        return pMdl;
    }

    // Stretches the model to the height of the actor's water column.
    void CStgGmk_WaterPillar_Mdl::Main(float rate)
    {
        CStgAct *pAct = GetParent()->GetAct();
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pAct != 0 && pMdl != 0) {
            float mdlHeight = GetParam_MdlHeight();
            float scale = static_cast<CStgGmk_WaterPillar_Act *>(pAct)->GetParam_Height() / mdlHeight;
            pMdl->m_vScale.x = 1.0f;
            pMdl->m_vScale.y = scale;
            pMdl->m_vScale.z = 1.0f;
            pMdl->CalcRootMtx();
            pMdl->CalcWorldMtx(0);
        }
        CStgMdl::Main(rate);
    }

    float CStgGmk_WaterPillar_Mdl::GetParam_MdlHeight() const
    {
        return 5.0f;
    }

    CStgGmk_WaterPillar_Act::CStgGmk_WaterPillar_Act(CStgGmk *pParent)
        : CStgGmkAct(pParent), m_fTimer(0.0f), m_fCollTimer(0.0f), m_bCollEnable(0), m_bAuto(0)
    {
        FsmqInit(this, 7, 4, 4);
        FsmqSetState(0, &CStgGmk_WaterPillar_Act::CtorState_RESET, &CStgGmk_WaterPillar_Act::LoopState_RESET,
                     &CStgGmk_WaterPillar_Act::DtorState_RESET);
        FsmqSetState(1, &CStgGmk_WaterPillar_Act::CtorState_WAIT, &CStgGmk_WaterPillar_Act::LoopState_WAIT,
                     &CStgGmk_WaterPillar_Act::DtorState_WAIT);
        FsmqSetState(2, &CStgGmk_WaterPillar_Act::CtorState_PRESTART, &CStgGmk_WaterPillar_Act::LoopState_PRESTART,
                     &CStgGmk_WaterPillar_Act::DtorState_PRESTART);
        FsmqSetState(3, &CStgGmk_WaterPillar_Act::CtorState_START, &CStgGmk_WaterPillar_Act::LoopState_START,
                     &CStgGmk_WaterPillar_Act::DtorState_START);
        FsmqSetState(4, &CStgGmk_WaterPillar_Act::CtorState_STAY, &CStgGmk_WaterPillar_Act::LoopState_STAY,
                     &CStgGmk_WaterPillar_Act::DtorState_STAY);
        FsmqSetState(5, &CStgGmk_WaterPillar_Act::CtorState_END, &CStgGmk_WaterPillar_Act::LoopState_END,
                     &CStgGmk_WaterPillar_Act::DtorState_END);
        FsmqSetState(6, &CStgGmk_WaterPillar_Act::CtorState_STOP, &CStgGmk_WaterPillar_Act::LoopState_STOP,
                     &CStgGmk_WaterPillar_Act::DtorState_STOP);
        GetSrt()->ClearVelocity();
        m_nTopNode = 0;
    }

    CStgGmk_WaterPillar_Act::~CStgGmk_WaterPillar_Act()
    {
    }

    // Sets up the collisions: a body the characters are pushed back by and
    // a defence volume that reflects the ball, both as big as the water.
    void CStgGmk_WaterPillar_Act::OnInit()
    {
        float radius = GetParam_Radius();
        float radius2 = GetParam_Radius2();
        float height = GetParam_Height();

        m_BodyColl.Create(3, 5, 3, 0);
        m_BodyColl.SetCollType(0, 1);
        m_BodyColl.SetSize(0, bcs::FVEC3(radius2, height, radius2));
        m_BodyColl.SetPushRank(0, 5);
        m_BodyColl.SetPushPower(0, 6);
        m_BodyColl.SetTargetFlag(0, 2);
        RegistBodyPushCallback(m_BodyColl);
        RegistBodyDamageCallback(m_BodyColl);
        RegistBallCatchCallback(m_BodyColl);

        m_DefColl.Create(1, 5, 1, 0);
        m_DefColl.GetColl(0)->SetCollType(1);
        m_DefColl.GetColl(0)->SetShape(4);
        m_DefColl.GetColl(0)->SetSize(bcs::FVEC3(radius, height, radius));
        m_DefColl.GetColl(0)->OnFlagA0(1);
        RegistReflectHitCallback(m_DefColl);

        CStgMdl *pMdl = GetMdl(0);
        bcs::CMdlObj *pMdlObj = pMdl != 0 ? pMdl->GetMdlObj() : 0;
        nw4r::g3d::ResMdl mdl;
        if (pMdlObj != 0) {
            mdl = pMdlObj->GetResMdl();
        }
        if (mdl.IsValid()) {
            nw4r::g3d::ResNode node = mdl.GetResNode(s_pTopNodeName);
            if (node.IsValid()) {
                m_nTopNode = node.GetID();
            }
        }
    }

    // Runs the states, holds the water down while a character stands in
    // it, and fades the effects out as the camera comes close.
    void CStgGmk_WaterPillar_Act::OnUpdate(float rate)
    {
        CStgSrt *pSrt = GetSrt();

        FsmqExec();
        UpdateColl();
        if (FsmqGetState() == 3) {
            bool bCap = CheckWaterCap();
            if (!IsWaterSuspend() && bCap) {
                WaterSuspend();
            } else if (IsWaterSuspend() && !bCap) {
                WaterResume();
            }
        }

        bcs::FVEC3 ofs;
        bcs::FVEC3 eye = bcs::CWorldEnv::GetViewCameraData(0)->m_Eye;
        bcs::FVEC3Sub(&ofs, &pSrt->GetPos(), &eye);
        float alpha = 1.0f;
        float dist = bcs::FVEC3Len(&ofs);
        if (dist < s_fEffFadeFar) {
            float d = dist - s_fEffFadeNear;
            if (0.0f > d) {
                d = 0.0f;
            }
            alpha = d / (s_fEffFadeFar - s_fEffFadeNear);
        }
        for (long i = 0; i < 7; i++) {
            CEffectCluster *pEff = Eff_GetObject(i);
            if (pEff != 0) {
                pEff->SetAlpha(alpha);
            }
        }

        CFgMdl *pMdl = GetMdl(0) != 0 ? static_cast<CFgMdl *>(GetMdl(0)->GetMdlObj()) : 0;
        if (pMdl != 0) {
            pMdl->SetAlphaTestNearRange(s_fAlphaTestNear);
            pMdl->SetAlphaTestFarRange(s_fAlphaTestFar);
        }
    }

    void CStgGmk_WaterPillar_Act::UpdateMdlMtx()
    {
        CStgMdl *pMdl = GetParent()->GetMdl(0);
        if (pMdl != 0) {
            pMdl->SetMtx(GetSrt()->GetMtx());
        }
    }

    // The collisions follow the water while it collides, and are off
    // otherwise.
    void CStgGmk_WaterPillar_Act::UpdateColl()
    {
        CStgSrt *pSrt = GetSrt();
        if (m_bCollEnable && !IsWaterSuspend()) {
            float radius = GetParam_Radius();
            float radius2 = GetParam_Radius2();
            float height = GetParam_Height();
            // Both units get a copy of the position (the original builds
            // one on the stack for each call).
            bcs::FVEC3 pos = pSrt->GetPos();
            m_BodyColl.SetEnableAll();
            m_BodyColl.SetPos(0, bcs::FVEC3(pos));
            m_BodyColl.SetSize(0, radius2, height > 0.0f ? height : 0.0f, radius2);
            m_BodyColl.SetPushEnable(0);
            m_DefColl.GetColl(0)->SetPos(bcs::FVEC3(pos));
            m_DefColl.GetColl(0)->SetSize(bcs::FVEC3(radius, height, radius));
            m_DefColl.GetColl(0)->OnFlag10(1);
            m_DefColl.GetColl(0)->OnFlagA0(1);
        } else {
            m_BodyColl.SetDisableAll();
            m_AtkColl.SetDisableAll();
            m_DefColl.SetDisableAll();
        }
    }

    // A character walking into the water is pushed back by its own
    // movement, scaled by GetParam_MoveResist().
    void CStgGmk_WaterPillar_Act::OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (pPush != 0) {
            if (pOther->GetOwnerKind() == 1 && !IsWaterSuspend()) {
                CChrCore *pChr = CChrMan::GetChrCore(pOther->m_nOwnerIdx);
                float resist = GetParam_MoveResist();
                bcs::FVEC3 prev = pChr->GetPrevPos();
                bcs::FVEC3 pos = pChr->GetPos();
                bcs::FVEC3 move(pos.x - prev.x, 0.0f, pos.z - prev.z);
                bcs::FVEC3 push;
                bcs::FVEC3Scale(&push, &move, -resist);
                pOther->DoBodyPushCallback(pBody, &push);
            }
        } else {
            pOther->DoBodyPushCallback(pBody, 0);
        }
    }

    unsigned long CStgGmk_WaterPillar_Act::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                        const bcs::FVEC3 &rPos)
    {
        return 2;
    }

    // Passes the catch on to the ball (CBall's slot 0x2D4; the header's
    // name for it is a guess).
    void CStgGmk_WaterPillar_Act::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
        pBall->IsShootMode();
    }

    // The ball bounces off the water away from its axis, at the speed it
    // came with.
    unsigned long CStgGmk_WaterPillar_Act::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                        bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
    {
        float speed = bcs::FVEC3Len(&rVel);
        bcs::FVEC3 dir;
        bcs::FVEC3Sub(&dir, &pBall->GetPos(), &GetSrt()->GetPos());
        bcs::BCS_Vec_Normalize(dir, dir);
        bcs::FVEC3Scale(&rVel, &dir, speed);
        Eff_KickHit(rPos);
        return 2 | 0x100;
    }

    unsigned long CStgGmk_WaterPillar_Act::OnCalcSum() const
    {
        return *reinterpret_cast<const unsigned long *>(&m_fTimer) +
               *reinterpret_cast<const unsigned long *>(&m_fCollTimer);
    }

    void CStgGmk_WaterPillar_Act::Eff_KickHit(const bcs::FVEC3 &rPos)
    {
        if (!Eff_IsAlive(0)) {
            long eff = GetParam_Effect(2);
            if (eff >= 0) {
                Eff_CreateEffect(0, eff, &rPos, 3);
            }
        }
    }

    // Plays animation anm on every model of the pillar (bit 3 of flag is
    // dropped).
    void CStgGmk_WaterPillar_Act::AnimPlay(long anm, unsigned long sub, float speed, float frame, float blend,
                                           unsigned long flag)
    {
        CStgGmk_WaterPillar *pParent = static_cast<CStgGmk_WaterPillar *>(GetParent());
        long num = pParent != 0 ? pParent->GetMdlNum() : 0;
        flag &= ~8;
        for (long i = 0; i < num; i++) {
            CStgMdl *pMdl = pParent->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetAnm(anm, sub, speed, frame, blend, flag);
            }
        }
    }

    bool CStgGmk_WaterPillar_Act::IsAnimEnd()
    {
        bool bEnd = false;
        CStgObj *pParent = GetParent();
        CStgMdl *pMdl = pParent != 0 ? pParent->GetMdl(0) : 0;
        CStageAnm *pAnm = pMdl != 0 ? pMdl->GetAnmObj() : 0;
        if (pAnm != 0) {
            if (pAnm->IsEnd(-9)) {
                bEnd = true;
            }
        } else {
            bEnd = true;
        }
        return bEnd;
    }

    float CStgGmk_WaterPillar_Act::AnimGetFrameNum()
    {
        float num = 0.0f;
        CStgObj *pParent = GetParent();
        CStgMdl *pMdl = pParent != 0 ? pParent->GetMdl(0) : 0;
        CStageAnm *pAnm = pMdl != 0 ? pMdl->GetAnmObj() : 0;
        if (pAnm != 0) {
            num = pAnm->GetTotalFrame(static_cast<CStageAnm::ANM_IDX>(0));
        }
        return num;
    }

    void CStgGmk_WaterPillar_Act::SetVisibleAll(int bVisible)
    {
        CStgGmk_WaterPillar *pParent = static_cast<CStgGmk_WaterPillar *>(GetParent());
        long num = pParent != 0 ? pParent->GetMdlNum() : 0;
        for (long i = 0; i < num; i++) {
            CStgMdl *pMdl = pParent->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetVisible(bVisible);
            }
        }
    }

    // The water rises: model, animation, effects and sound.
    void CStgGmk_WaterPillar_Act::WaterStartAnim_()
    {
        SetVisibleAll(1);
        CStgMdl *pMdl = GetMdl(0);
        if (pMdl != 0) {
            pMdl->FadeIn(5.0f, 0.0f);
        }
        AnimPlay(1, 0, 1.0f, 0.0f, 0.0f, -1);
        Eff_CreateAttach(1, GetParam_Effect(3), 0, m_nTopNode, 0, 0, 0, 0, 3);
        if (!Eff_IsAlive(2)) {
            Eff_CreateAttach(2, GetParam_Effect(4), 0, 0UL, 0, 0, 0, 0, 3);
        }
        if (!Eff_IsAlive(3)) {
            Eff_CreateAttach(3, GetParam_Effect(5), 0, 0UL, 0, 0, 0, 0, 3);
        }
        if (!Eff_IsAlive(4)) {
            Eff_CreateAttach(4, GetParam_Effect(6), 0, 0UL, 0, 0, 0, 0, 3);
        }
        long se = GetParam_SE(1);
        if (se >= 0) {
            SE_PlayPos(0, se, GetSrt()->GetPos(), 0.0f, 0);
        }
    }

    // The water falls.
    void CStgGmk_WaterPillar_Act::WaterEndAnim_()
    {
        CStgMdl *pMdl = GetMdl(0);
        if (pMdl != 0) {
            pMdl->FadeOut(20.0f, 0.0f);
        }
        AnimPlay(2, 0, 1.0f, 6.0f, 0.0f, -1);
        AnimGetFrameNum(); // not used
        Eff_Stop(1);
        Eff_DelayStop(2, 25.0f);
        Eff_DelayStop(3, 25.0f);
        Eff_DelayStop(4, 15.0f);
        Eff_CreateAttach(6, GetParam_Effect(8), 0, 0UL, 0, 0, 0, 0, 3);
        SE_Stop(0, 20.0f);
        long se = GetParam_SE(2);
        if (se >= 0) {
            CGameSnd::Play(se, GetSrt()->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    // While the water stands, a character in it holds it down.
    void CStgGmk_WaterPillar_Act::WaterSuspend()
    {
        if (FsmqGetState() == 3 && !IsWaterSuspend()) {
            float speed = 1.0f;
            CStgMdl *pMdl = GetMdl(0);
            if (pMdl != 0) {
                pMdl->FadeOut(10.0f, 0.0f);
            }
            AnimPlay(2, 0, speed, 0.0f, 0.0f, -1);
            Eff_Stop(1);
            Eff_Stop(4);
            m_bCollEnable = 0;
            m_fCollTimer = 0.0f;
            m_bSuspend = 1;
        }
    }

    void CStgGmk_WaterPillar_Act::WaterResume()
    {
        if (FsmqGetState() == 3 && IsWaterSuspend()) {
            WaterStartAnim_();
            m_fCollTimer = GetParam_WaitTime();
            m_bSuspend = 0;
        }
    }

    int CStgGmk_WaterPillar_Act::IsWaterSuspend() const
    {
        return m_bSuspend;
    }

    // Whether a character is within GetParam_Radius3() of the pillar.
    bool CStgGmk_WaterPillar_Act::CheckWaterCap() const
    {
        bool bCap = false;
        const CStgSrt *pSrt = GetSrt();
        long num = CChrMan::GetChrCoreNum();
        float radius = GetParam_Radius3();
        for (long i = 0; i < num; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && pChr->IsEnable() && pChr->GetDist_ToPos(pSrt->GetPos()) < radius) {
                bCap = true;
                break;
            }
        }
        return bCap;
    }

    void CStgGmk_WaterPillar_Act::ForceStart()
    {
        long state = FsmqGetState();
        if (state == 1 || state == 6) {
            FsmqChangeState(2);
        }
    }

    void CStgGmk_WaterPillar_Act::ForceStop()
    {
        long state = FsmqGetState();
        if (state == 3 || state == 4) {
            FsmqChangeState(5);
        }
    }

    void CStgGmk_WaterPillar_Act::ForceReset()
    {
        Eff_EraseAll();
        SE_StopAll();
        FsmqChangeState(0);
    }

    bool CStgGmk_WaterPillar_Act::IsSwoosh() const
    {
        bool bSwoosh = false;
        long state = FsmqGetState();
        if (state >= 2 && state <= 5) {
            bSwoosh = true;
        }
        return bSwoosh;
    }

    bool CStgGmk_WaterPillar_Act::IsWarning() const
    {
        bool bWarning = false;
        if (FsmqGetState() == 2) {
            bWarning = true;
        }
        return bWarning;
    }

    bool CStgGmk_WaterPillar_Act::IsActive() const
    {
        return FsmqGetState() != 6;
    }

    // Back to the default position, then off.
    void CStgGmk_WaterPillar_Act::CtorState_RESET()
    {
        CStgSrt *pSrt = GetSrt();
        CStgGmk_WaterPillar *pParent = static_cast<CStgGmk_WaterPillar *>(GetParent());
        m_fTimer = 0.0f;
        m_fCollTimer = 0.0f;
        m_bCollEnable = 0;
        SetVisibleAll(0);
        bcs::FVEC3 pos = pParent->GetDefaultPos();
        pSrt->SetPos(pos);
        FsmqChangeState(6);
        FsmqRetry();
    }

    void CStgGmk_WaterPillar_Act::LoopState_RESET()
    {
    }

    void CStgGmk_WaterPillar_Act::DtorState_RESET()
    {
    }

    void CStgGmk_WaterPillar_Act::CtorState_WAIT()
    {
        m_fTimer = GetParam_Interval();
        SetVisibleAll(0);
    }

    void CStgGmk_WaterPillar_Act::LoopState_WAIT()
    {
        m_fTimer -= GetFrameRateF();
        if (m_fTimer <= 0.0f && IsAuto()) {
            // Both not used.
            CStgSrt *pSrt = GetSrt();
            CStgObj *pParent = GetParent();
            FsmqChangeState(2);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterPillar_Act::DtorState_WAIT()
    {
    }

    // The warning before the water rises.
    void CStgGmk_WaterPillar_Act::CtorState_PRESTART()
    {
        CStgSrt *pSrt = GetSrt();
        if (!Eff_IsAlive(3)) {
            Eff_CreateAttach(3, GetParam_Effect(5), 0, 0UL, 0, 0, 0, 0, 3);
        }
        if (!Eff_IsAlive(5)) {
            Eff_CreateAttach(5, GetParam_Effect(7), 0, 0UL, 0, 0, 0, 0, 3);
        }
        long se = GetParam_SE(0);
        if (se >= 0) {
            SE_PlayPos(0, se, pSrt->GetPos(), 0.0f, 0);
        }
        m_fTimer = GetParam_PrestartTime();
    }

    void CStgGmk_WaterPillar_Act::LoopState_PRESTART()
    {
        m_fTimer -= GetFrameRateF();
        if (m_fTimer <= 0.0f) {
            FsmqChangeState(3);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterPillar_Act::DtorState_PRESTART()
    {
        Eff_Stop(5);
    }

    // The water stands (held down at once when a character is in it). It
    // collides after GetParam_WaitTime() frames and falls after
    // GetParam_StayTime(), kept in the state's work memory.
    void CStgGmk_WaterPillar_Act::CtorState_START()
    {
        long *pStayTime = FsmqGetWork<long>();
        GetSrt();
        SetVisibleAll(1);
        if (!CheckWaterCap()) {
            WaterStartAnim_();
        } else {
            SetVisibleAll(0);
            if (!Eff_IsAlive(2)) {
                Eff_CreateAttach(2, GetParam_Effect(4), 0, 0UL, 0, 0, 0, 0, 3);
            }
        }
        m_bSuspend = 0;
        *pStayTime = GetParam_StayTime();
        m_fTimer = 0.0f;
        m_fCollTimer = GetParam_WaitTime();
    }

    void CStgGmk_WaterPillar_Act::LoopState_START()
    {
        long *pStayTime = FsmqGetWork<long>();
        float rate = GetFrameRateF();
        GetSrt();
        m_fCollTimer -= rate;
        if (m_fCollTimer > 0.0f) {
            m_bCollEnable = 0;
        } else {
            m_bCollEnable = 1;
            m_fCollTimer = 0.0f;
        }

        CStgMdl *pMdl = GetMdl(0);
        CStageAnm *pAnm = pMdl != 0 ? pMdl->GetAnmObj() : 0;
        if (pAnm != 0 && pAnm->GetCurAnmId(static_cast<CStageAnm::ANM_IDX>(0)) == 1 && pAnm->IsEnd(1)) {
            AnimPlay(3, 2, 1.0f, 0.0f, 0.0f, -1);
        }

        if (*pStayTime > 0.0f) {
            m_fTimer += rate;
            if (m_fTimer >= *pStayTime) {
                FsmqChangeState(5);
                FsmqRetry();
            }
        }
    }

    void CStgGmk_WaterPillar_Act::DtorState_START()
    {
        GetSrt();
        m_bCollEnable = 0;
    }

    void CStgGmk_WaterPillar_Act::CtorState_STAY()
    {
        long *pStayTime = FsmqGetWork<long>();
        GetSrt();
        m_bCollEnable = 1;
        *pStayTime = GetParam_StayTime();
        m_fTimer = 0.0f;
    }

    void CStgGmk_WaterPillar_Act::LoopState_STAY()
    {
        long *pStayTime = FsmqGetWork<long>();
        float rate = GetFrameRateF();
        if (*pStayTime > 0.0f) {
            m_fTimer += rate;
            if (m_fTimer >= *pStayTime) {
                FsmqChangeState(5);
                FsmqRetry();
            }
        }
    }

    void CStgGmk_WaterPillar_Act::DtorState_STAY()
    {
        m_BodyColl.SetDisableAll();
        m_AtkColl.SetDisableAll();
        m_DefColl.SetDisableAll();
        m_bCollEnable = 0;
    }

    // The water falls, then waits; at once when it was held down.
    void CStgGmk_WaterPillar_Act::CtorState_END()
    {
        m_bCollEnable = 0;
        WaterEndAnim_();
        if (IsWaterSuspend()) {
            SetVisibleAll(0);
            FsmqChangeState(1);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterPillar_Act::LoopState_END()
    {
        if (IsAnimEnd()) {
            FsmqChangeState(1);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterPillar_Act::DtorState_END()
    {
        SetVisibleAll(0);
    }

    void CStgGmk_WaterPillar_Act::CtorState_STOP()
    {
        m_bCollEnable = 0;
        SetVisibleAll(0);
    }

    void CStgGmk_WaterPillar_Act::LoopState_STOP()
    {
    }

    void CStgGmk_WaterPillar_Act::DtorState_STOP()
    {
    }



    CStgGmk_WaterPillar_Drv::CStgGmk_WaterPillar_Drv(CStgGmk *pParent) : CStgDrv(pParent)
    {
    }

    CStgGmk_WaterPillar_Drv::~CStgGmk_WaterPillar_Drv()
    {
    }

    void CStgGmk_WaterPillar_Drv::Main(float rate)
    {
        CStgDrv::Main(rate);
    }



    CStgGmk_WaterPillar::CStgGmk_WaterPillar(bcs::CTask *pParent, int) : CStgGmk(pParent, 1)
    {
    }

    CStgGmk_WaterPillar::~CStgGmk_WaterPillar()
    {
    }

    CStgMdl *CStgGmk_WaterPillar::NewMdl(long heap, unsigned long idx)
    {
        return new (heap) CStgGmk_WaterPillar_Mdl(this, idx);
    }

    void CStgGmk_WaterPillar::OnPostCreate()
    {
    }

    void CStgGmk_WaterPillar::OnSceneEntry()
    {
    }

    void CStgGmk_WaterPillar::OnSceneLeave()
    {
    }

    void CStgGmk_WaterPillar::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    void CStgGmk_WaterPillar::Respawn()
    {
        GetAct();
    }

    // State 1 starts the water (the actor's START state, name guessed).
    void CStgGmk_WaterPillar::Activate()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0 && !pAct->IsActive()) {
            pAct->FsmqChangeState(1);
        }
    }

    // State 6 ends it.
    void CStgGmk_WaterPillar::Inactivate()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0 && pAct->IsActive()) {
            pAct->FsmqChangeState(6);
        }
    }

    void CStgGmk_WaterPillar::Start()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            pAct->ForceStart();
        }
    }

    void CStgGmk_WaterPillar::Stop()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            pAct->ForceStop();
        }
    }

    void CStgGmk_WaterPillar::Reset()
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            pAct->ForceReset();
        }
    }

    void CStgGmk_WaterPillar::SetAuto(int bAuto)
    {
        CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            pAct->SetAuto(bAuto);
        }
    }

    bool CStgGmk_WaterPillar::IsSwoosh() const
    {
        bool bSwoosh = false;
        const CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            bSwoosh = pAct->IsSwoosh();
        }
        return bSwoosh;
    }

    int CStgGmk_WaterPillar::IsAuto() const
    {
        int bAuto = 0;
        const CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            bAuto = pAct->IsAuto();
        }
        return bAuto;
    }

    bool CStgGmk_WaterPillar::IsWarning() const
    {
        const CStgGmk_WaterPillar_Act *pAct = GetPillarAct();
        if (pAct != 0) {
            return pAct->IsWarning();
        }
        return false;
    }

    void CStgGmk_WaterPillar::Disp()
    {
    }



    CStgGmk_WaterPillarCluster::CStgGmk_WaterPillarCluster(bcs::CTask *pParent)
        : CStgGmkCluster<CStgGmk_WaterPillar>(pParent), m_nGuardGoalIdx(0), m_CenterBasePos(0.0f, 0.0f, 0.0f),
          m_Vec(0.0f, 0.0f, 1.0f), m_nZone(-1), m_fStopTimer(0.0f)
    {
    }

    CStgGmk_WaterPillarCluster::~CStgGmk_WaterPillarCluster()
    {
    }

    // The first GetParam_LocNum() pillars reset, the others go out.
    void CStgGmk_WaterPillarCluster::Reset()
    {
        long num = GetTblSize();
        long locNum = GetParam_LocNum();
        for (long i = 0; i < num; i++) {
            CStgGmk_WaterPillar *pPillar = GetPillar(i);
            if (pPillar != 0) {
                if (i < locNum) {
                    pPillar->Reset();
                } else {
                    pPillar->Inactivate();
                }
            }
        }
        m_nZone = -1;
        m_fStopTimer = 0.0f;
        SE_StopAll();
    }

    // Starts the pillars of zone and plays the sound once at their centre,
    // delayed by the pillars' prestart time.
    void CStgGmk_WaterPillarCluster::StartZone(long zone)
    {
        long num = GetTblSize();
        long locNum = GetParam_LocNum(); // not used
        bcs::FVEC3 center(0.0f, 0.0f, 0.0f);
        float delay = 1.0f;
        long count = 0;
        for (long i = 0; i < num; i++) {
            if (IsEqualZone(i, zone)) {
                CStgGmk_WaterPillar *pPillar = GetPillar(i);
                if (pPillar != 0) {
                    pPillar->Start();
                    bcs::FVEC3 pos = pPillar->GetPos();
                    bcs::FVEC3Add(&center, &center, &pos);
                    count++;
                    delay = static_cast<CStgGmk_WaterPillar_Act *>(pPillar->GetAct())->GetParam_PrestartTime();
                }
            }
        }
        m_nZone = zone;
        if (count > 0) {
            bcs::FVEC3Scale(&center, &center, 1.0f / count);
            SE_DelayPlayPosEx(0, delay, 0xD12, center, 3, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CStgGmk_WaterPillarCluster::StopAll()
    {
        long num = GetTblSize();
        long locNum = GetParam_LocNum(); // not used
        for (long i = 0; i < num; i++) {
            CStgGmk_WaterPillar *pPillar = GetPillar(i);
            if (pPillar != 0) {
                pPillar->Stop();
            }
        }
        SE_Stop(0, 0.0f);
    }

    // Whether pillar idx belongs to zone: the first GetParam_NumPerGroup()
    // pillars are zone 0, as many in the middle zone 1, the last ones zone 2.
    bool CStgGmk_WaterPillarCluster::IsEqualZone(long idx, long zone) const
    {
        long locNum = GetParam_LocNum();
        long divNum = GetParam_DivNum(); // not used
        long perGroup = GetParam_NumPerGroup();
        long mid = locNum / 2;
        switch (zone) {
            case 0:
                if (idx < perGroup) {
                    return true;
                }
                break;
            case 1:
                if (idx >= mid - perGroup / 2 && idx < mid + perGroup / 2 + perGroup % 2) {
                    return true;
                }
                break;
            case 2:
                if (idx >= locNum - perGroup) {
                    return true;
                }
                break;
        }
        return false;
    }

    void CStgGmk_WaterPillarCluster::OnPostCreateGmk()
    {
        CStgGmkCluster<CStgGmk_WaterPillar>::OnPostCreateGmk();
        SE_Init();
    }

    void CStgGmk_WaterPillarCluster::OnUpdate(float rate)
    {
        ApplyLocate();
        const CChrCore *pChr = GetOnBallChr();
        if (IsReady() && pChr != 0) {
            long zone = -1;
            if (pChr != 0) {
                zone = GetZoneByPos(pChr->GetPos(), m_nZone, GetParam_ZoneMargin());
            }
            if (m_nZone != zone || !IsSwoosh()) {
                StopAll();
                if (m_nZone >= 0) {
                    m_fStopTimer = GetParam_StopTimer();
                    m_nZone = -1;
                }
            }
            if (m_nZone < 0) {
                m_fStopTimer -= rate;
                if (m_fStopTimer <= 0.0f) {
                    if (zone >= 0) {
                        StartZone(zone);
                    }
                    m_fStopTimer = 0.0f;
                }
            }
        }
        SE_Update(rate);
    }

    // The centre of the arc: GetParam_LocOffsetZ() along the goal's
    // direction from the base position.
    bcs::FVEC3 CStgGmk_WaterPillarCluster::GetCenterPos() const
    {
        bcs::FVEC3 pos = GetCenterBasePos();
        bcs::FVEC3 ofs;
        bcs::BCS_Vec_Normalize(ofs, GetVec());
        bcs::FVEC3Scale(&ofs, &ofs, GetParam_LocOffsetZ());
        bcs::FVEC3Add(&pos, &pos, &ofs);
        return pos;
    }

    // Sets the pillars out on a half circle of radius GetParam_LocRadius()
    // around the centre, from -90 to 90 degrees off the goal's direction.
    void CStgGmk_WaterPillarCluster::ApplyLocate()
    {
        bcs::FVEC3 center = GetCenterBasePos();
        const bcs::FVEC3 &rVec = GetVec();
        float radius = GetParam_LocRadius();
        long num = GetTblSize();
        long locNum = GetParam_LocNum();
        bcs::FVEC3 ofs;
        bcs::BCS_Vec_Normalize(ofs, rVec);
        bcs::FVEC3Scale(&ofs, &ofs, GetParam_LocOffsetZ());
        bcs::FVEC3Add(&center, &center, &ofs);
        center.y = 0.0f;
        for (long i = 0; i < num; i++) {
            CStgGmk_WaterPillar *pPillar = GetPillar(i);
            if (pPillar != 0) {
                if (i < locNum) {
                    float angle = 0.0f;
                    if (locNum > 1) {
                        angle = -1.5707964f + (float)i / (float)(locNum - 1) * 3.1415927f;
                    }
                    bcs::FVEC3 pos;
                    bcs::BCS_Vec_RotY(pos, rVec, angle);
                    bcs::FVEC3Scale(&pos, &pos, radius);
                    bcs::FVEC3Add(&pos, &pos, &center);
                    pPillar->SetPos(pos);
                } else {
                    pPillar->Stop();
                }
            }
        }
    }

    // The apex of the zones: the centre moved back along the goal's
    // direction so that GetParam_ZoneAngle() spans GetParam_ZoneWidth().
    bcs::FVEC3 CStgGmk_WaterPillarCluster::GetZoneCenter() const
    {
        bcs::FVEC3 pos = GetCenterPos();
        const bcs::FVEC3 &rVec = GetVec();
        float angle = GetParam_ZoneAngle();
        float width = GetParam_ZoneWidth();
        if (angle > 0.0f) {
            bcs::FVEC3 ofs;
            bcs::FVEC3Scale(&ofs, &rVec, -(width / (float)tan(0.5f * angle)));
            bcs::FVEC3Add(&pos, &pos, &ofs);
        }
        pos.y = 0.0f;
        return pos;
    }

    // The zone rPos is in: 1 within half the zone angle of the goal's
    // direction, 0 on one side, 2 on the other. With a current zone and a
    // margin, rPos stays in curZone while a point margin to either side of
    // it is still there.
    long CStgGmk_WaterPillarCluster::GetZoneByPos(const bcs::FVEC3 &rPos, long curZone, float margin) const
    {
        bcs::FVEC3 center = GetZoneCenter();
        const bcs::FVEC3 &rVec = GetVec();
        bcs::FVEC3 dir;
        dir.x = rPos.x;
        dir.y = center.y;
        dir.z = rPos.z;
        bcs::FVEC3Sub(&dir, &dir, &center);
        float rot = bcs::BCS_Vec_GetRot(rVec, dir);
        float zoneAngle = GetParam_ZoneAngle();
        bcs::FVEC3 cross;
        PSVECCrossProduct((const Vec *)&rVec, (const Vec *)&dir, (Vec *)&cross);
        if (cross.y < 0.0f) {
            rot *= -1.0f;
        }

        long zone;
        if (fabsf(rot) < 0.5f * zoneAngle) {
            zone = 1;
        } else if (rot < 0.0f) {
            zone = 0;
        } else {
            zone = 2;
        }

        if (curZone >= 0 && margin > 0.0f) {
            bcs::FVEC3 side0;
            bcs::FVEC3 side1;
            switch (curZone) {
                case 0:
                    bcs::BCS_Vec_RotY(side0, rVec, -0.5f * zoneAngle - 1.5707964f);
                    SetLength(side0, margin);
                    bcs::BCS_Vec_RotY(side1, rVec, -0.5f * zoneAngle + 1.5707964f);
                    SetLength(side1, margin);
                    break;
                case 1:
                    bcs::BCS_Vec_RotY(side0, rVec, -0.5f * zoneAngle - 1.5707964f);
                    SetLength(side0, margin);
                    bcs::BCS_Vec_RotY(side1, rVec, 0.5f * zoneAngle + 1.5707964f);
                    SetLength(side1, margin);
                    break;
                case 2:
                    bcs::BCS_Vec_RotY(side0, rVec, 0.5f * zoneAngle + 1.5707964f);
                    SetLength(side0, margin);
                    bcs::BCS_Vec_RotY(side1, rVec, 0.5f * zoneAngle - 1.5707964f);
                    SetLength(side1, margin);
                    break;
            }

            bcs::FVEC3 pos;
            bcs::FVEC3Add(&pos, &rPos, &side0);
            if (curZone == GetZoneByPos(pos, -1, 0.0f)) {
                return curZone;
            }
            bcs::FVEC3Add(&pos, &rPos, &side1);
            if (curZone == GetZoneByPos(pos, -1, 0.0f)) {
                return curZone;
            }
        }
        return zone;
    }

    const CChrCore *CStgGmk_WaterPillarCluster::GetOnBallChr() const
    {
        const CChrCore *pChr;
        long num = CChrMan::GetChrCoreNum();
        long team = CGame::GetTeamIdxByGoalIdx(GetGuardGoalIdx());
        for (long i = 0; i < num; i++) {
            pChr = CChrMan::GetChrCore(i);
            if (pChr != 0 && pChr->IsEnable() && (pChr->GetStatusFlag() & 1) && team == pChr->GetTeamNo()) {
                return pChr;
            }
        }
        return 0;
    }

    bool CStgGmk_WaterPillarCluster::IsSwoosh() const
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            const CStgGmk_WaterPillar *pPillar = GetPillar(i);
            if (pPillar != 0 && pPillar->IsSwoosh()) {
                return true;
            }
        }
        return false;
    }
}
