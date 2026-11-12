#include "game/msw/CStgGmk_WaterWorks_Volley.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CAnmTblBase.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageAnm.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgGenericGmkMdl.hpp"
#include "game/msw/CStgSrt.hpp"
#include "game/msw/VolleyPrm.hpp"
#include "nw4r/math/triangular.h"

// The water jets of the volley fountain stage (stage 03): the model
// (CStgGmk_WaterWorks_Volley_Mdl), the actor (CStgGmk_WaterWorks_Volley_Act),
// the driver and the gimmick (CStgGmk_WaterWorks_Volley). One jet has four
// models: the water wall, two drop models and the spray. The wall rises
// from the ground to GetParam_Height() and is as wide as the net divided
// by the number of jets; while it stands it reflects the ball.

namespace {

    // The node of the water wall the effects and the other models are
    // attached to, and the one whose height the collision follows (two
    // pointers to one string in the original).
    const char *s_pBornNode = "born";
    const char *s_pBornNodeColl = "born";

    // Suffixes of the water wall's animations, appended to the model name.
    const char *s_AnmSuffix[] = { "00", "00_00", "00_01", "01", "01_00", "01_01", 0 };

    // The four models of a jet.
    const msw::CStgMdl::MDL_PARAM s_MdlParam[] = {
        { "s03_gm_waterwall00", 0, 0, 0x30F, s_AnmSuffix, "s03_gm_waterwall", 0, 0 },
        { "s03_gm_sibukli00", 0, 0, 0x30F, msw::CAnmTblBase::null_tbl, "s03_gm_sibukli00", 0, 0 },
        { "s03_gm_sibukli01", 0, 0, 0x30F, msw::CAnmTblBase::null_tbl, "s03_gm_sibukli01", 0, 0 },
        { "s03_gm_tubu00", 0, 0, 0x30F, msw::CAnmTblBase::null_tbl, "s03_gm_tubu00", 0, 0 },
    };
}

namespace msw {

    // The creator interface's defaults: this file is the only one in the
    // game with their weak copies and the vtable (see CStgMdl.hpp).

    inline bool CStgMdl::CCreator::CreateMdlObj(bcs::CMdlObj *pMdl) const
    {
        return false;
    }

    inline bool CStgMdl::CCreator::CreateShadowMdl(bcs::CMdlShadow *pMdl) const
    {
        return false;
    }

    inline bool CStgMdl::CCreator::CreateRefMdl(bcs::CMdlRef *pMdl) const
    {
        return false;
    }

    inline bool CStgMdl::CCreator::CreateAnm(CStageAnm *pAnm, bcs::CMdlObj *pMdl) const
    {
        return false;
    }

    // The gimmick's and the actor's inline functions (weak copies here; see
    // the header).

    inline unsigned long CStgGmk_WaterWorks_Volley::GetMdlNum() const
    {
        return 4;
    }

    inline void CStgGmk_WaterWorks_Volley::SetDefaultPos(const bcs::FVEC3 &rPos)
    {
        m_DefaultPos = rPos;
    }

    inline const bcs::FVEC3 &CStgGmk_WaterWorks_Volley::GetDefaultPos() const
    {
        return m_DefaultPos;
    }

    inline void CStgGmk_WaterWorks_Volley_Act::SetParam_Height(float height)
    {
        m_fHeight = height;
    }

    inline void CStgGmk_WaterWorks_Volley_Act::SetParam_StayTime(float time)
    {
        m_fStayTime = time;
    }

    inline void CStgGmk_WaterWorks_Volley::SetParam_Height(float height)
    {
        CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->SetParam_Height(height);
        }
    }

    inline void CStgGmk_WaterWorks_Volley::SetParam_StayTime(float time)
    {
        CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->SetParam_StayTime(time);
        }
    }

    inline float CStgGmk_WaterWorks_Volley::GetParam_Width() const
    {
        const CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            return pAct->GetParam_Width();
        }
        return -1.0f;
    }

    inline CStgAct *CStgGmk_WaterWorks_Volley::NewAct(long heap)
    {
        return new (heap) CStgGmk_WaterWorks_Volley_Act(this);
    }

    inline CStgDrv *CStgGmk_WaterWorks_Volley::NewDrv(long heap)
    {
        return new (heap) CStgGmk_WaterWorks_Volley_Drv(this);
    }

    CStgGmk_WaterWorks_Volley_Mdl::~CStgGmk_WaterWorks_Volley_Mdl()
    {
    }

    void CStgGmk_WaterWorks_Volley_Mdl::Create()
    {
        long idx = m_nMdlIdx;
        idx = idx > 3 ? 3 : (idx < 0 ? 0 : idx);
        CStgMdl::Create(CGenericMVAGimmickCreator(CLoadData::GetRes(2), &s_MdlParam[idx]));
    }

    bcs::CMdlObj *CStgGmk_WaterWorks_Volley_Mdl::NewMdl(long heap) const
    {
        CFgMdl *pMdl = new (heap) CFgMdl;
        if (pMdl != 0) {
            pMdl->SetAlphaTestNearRange(0.5f);
            pMdl->SetAlphaTestFarRange(3.0f);
            pMdl->SetExBlendMode(1);
            pMdl->CalcBoundingVolume();
            pMdl->OnFlag(0x10000);
        }
        return pMdl;
    }

    void CStgGmk_WaterWorks_Volley_Mdl::Main(float rate)
    {
        CStgAct *pAct = GetParent()->GetAct();
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pAct != 0 && pMdl != 0) {
            pMdl->CalcWorldMtx(0);
        }
        CStgMdl::Main(rate);
    }

    CStgGmk_WaterWorks_Volley_Act::CStgGmk_WaterWorks_Volley_Act(CStgGmk *pParent)
        : CStgGmkAct(pParent), m_fTimer(0.0f), m_bCollEnable(0), m_fHeight(0.0f), m_fStayTime(0.0f)
    {
        FsmqInit(this, 7, 4, sizeof(WORK));
        FsmqSetState(0, &CStgGmk_WaterWorks_Volley_Act::CtorState_DEFAULT,
                     &CStgGmk_WaterWorks_Volley_Act::LoopState_DEFAULT,
                     &CStgGmk_WaterWorks_Volley_Act::DtorState_DEFAULT);
        FsmqSetState(1, &CStgGmk_WaterWorks_Volley_Act::CtorState_STANDBY,
                     &CStgGmk_WaterWorks_Volley_Act::LoopState_STANDBY,
                     &CStgGmk_WaterWorks_Volley_Act::DtorState_STANDBY);
        FsmqSetState(2, &CStgGmk_WaterWorks_Volley_Act::CtorState_PRESTART,
                     &CStgGmk_WaterWorks_Volley_Act::LoopState_PRESTART,
                     &CStgGmk_WaterWorks_Volley_Act::DtorState_PRESTART);
        FsmqSetState(3, &CStgGmk_WaterWorks_Volley_Act::CtorState_START,
                     &CStgGmk_WaterWorks_Volley_Act::LoopState_START,
                     &CStgGmk_WaterWorks_Volley_Act::DtorState_START);
        FsmqSetState(4, &CStgGmk_WaterWorks_Volley_Act::CtorState_STAY,
                     &CStgGmk_WaterWorks_Volley_Act::LoopState_STAY,
                     &CStgGmk_WaterWorks_Volley_Act::DtorState_STAY);
        FsmqSetState(5, &CStgGmk_WaterWorks_Volley_Act::CtorState_END,
                     &CStgGmk_WaterWorks_Volley_Act::LoopState_END,
                     &CStgGmk_WaterWorks_Volley_Act::DtorState_END);
        FsmqSetState(6, &CStgGmk_WaterWorks_Volley_Act::CtorState_INACTIVE,
                     &CStgGmk_WaterWorks_Volley_Act::LoopState_INACTIVE,
                     &CStgGmk_WaterWorks_Volley_Act::DtorState_INACTIVE);
        GetSrt()->ClearVelocity();
        for (long i = 0; i < 2; i++) {
            m_ahEffect[i] = 0;
        }
    }

    CStgGmk_WaterWorks_Volley_Act::~CStgGmk_WaterWorks_Volley_Act()
    {
        for (long i = 0; i < 2; i++) {
            if (m_ahEffect[i] != 0) {
                CEffectCtrl::EraseAttachEffect(m_ahEffect[i]);
                m_ahEffect[i] = 0;
            }
        }
    }

    // The ball collides with a flat box as wide as the jet; the body
    // collision only marks the water for the characters.
    void CStgGmk_WaterWorks_Volley_Act::OnInit()
    {
        float width = GetParam_Width();
        float height = GetParam_Height();
        float depth = 0.1f;

        m_BodyColl.Create(1, 5, 3, 0);
        RegistBodyPushCallback(m_BodyColl);
        RegistBodyDamageCallback(m_BodyColl);
        RegistBallCatchCallback(m_BodyColl);

        m_DefColl.Create(1, 5, 2, 0);
        m_DefColl.GetColl(0)->SetCollType(2);
        m_DefColl.GetColl(0)->SetShape(4);
        m_DefColl.GetColl(0)->SetSize(bcs::FVEC3(0.5f * width, height, depth));
        m_DefColl.GetColl(0)->OnFlagA0(1);
        RegistReflectHitCallback(m_DefColl);
    }

    // Runs the states, keeps the jet above the ground and scales the water
    // wall to the jet's width and height.
    void CStgGmk_WaterWorks_Volley_Act::OnUpdate(float rate)
    {
        CStgSrtMove *pSrt = GetSrt();

        FsmqExec();
        pSrt->Update(rate);

        bcs::FVEC3 pos = pSrt->GetPos();
        if (pos.y < 0.05f) {
            pos.y = 0.05f;
            pSrt->SetPos(pos);
            pSrt->ClearUnkE4();
        }

        float scaleX = GetParam_Width() / 2.1666667f;
        pSrt->SetScale(scaleX, GetParam_Height() * 0.5f, 1.0f);
        UpdateColl();
    }

    // The water wall and the spray follow the actor; the first drop model
    // is attached to the wall's node (OnPostCreate); the spray keeps an
    // unscaled height.
    void CStgGmk_WaterWorks_Volley_Act::UpdateMdlMtx()
    {
        for (long i = 0; i < 4; i++) {
            CStgMdl *pMdl = GetParent()->GetMdl(i);
            if (pMdl != 0) {
                if (i == 1) {
                    bcs::FMTX34 mtx;
                    PSMTXIdentity(mtx.m);
                    pMdl->SetMtx(mtx);
                } else {
                    pMdl->SetMtx(GetSrt()->GetMtx());
                }
                if (i == 2 || i == 3) {
                    bcs::FMTX34 mtx;
                    bcs::FMTX34Copy(&mtx, &pMdl->GetMtx());
                    mtx.m[1][1] = mtx.m[0][0];
                    pMdl->SetMtx(mtx);
                }
            }
        }
    }

    // The collisions follow the water wall while it collides (its height
    // from the wall's node), and are off otherwise.
    void CStgGmk_WaterWorks_Volley_Act::UpdateColl()
    {
        CStgSrt *pSrt = GetSrt();
        if (m_bCollEnable && !IsWaterSuspend()) {
            float width = GetParam_Width();
            float height = GetParam_Height();
            bcs::FVEC3 pos = pSrt->GetPos();
            float depth = 0.1f;
            CStgMdl *pMdl = GetMdl(0);
            bcs::FMTX34 mtx;
            if (pMdl != 0 && pMdl->GetNodeWorldMtx(mtx, s_pBornNodeColl)) {
                height = mtx.m[1][3] - pSrt->GetPos().y;
            }
            m_BodyColl.SetEnableAll();
            m_DefColl.GetColl(0)->SetPos(bcs::FVEC3(pos));
            m_DefColl.GetColl(0)->SetSize(bcs::FVEC3(0.5f * width, height, depth));
            m_DefColl.GetColl(0)->OnFlag10(1);
            m_DefColl.GetColl(0)->OnFlagA0(1);
        } else {
            m_BodyColl.SetDisableAll();
            m_DefColl.SetDisableAll();
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
    }

    unsigned long CStgGmk_WaterWorks_Volley_Act::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                              const bcs::FVEC3 &rPos)
    {
        return 2;
    }

    // Passes the catch on to the ball (CBall's slot 0x2D4; the header's
    // name for it is a guess).
    void CStgGmk_WaterWorks_Volley_Act::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
        pBall->IsShootMode();
    }

    // The ball is reflected by the collision; only a splash is added.
    unsigned long CStgGmk_WaterWorks_Volley_Act::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                              bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
    {
        long eff = GetParam_Effect(0);
        if (eff >= 0) {
            CEffectCtrl::SimpleEffect(eff, 0, &rPos, 0, 3, 0, 0, 0);
        }
        return 0x101;
    }

    unsigned long CStgGmk_WaterWorks_Volley_Act::OnCalcSum() const
    {
        return *reinterpret_cast<const unsigned long *>(&m_fTimer);
    }

    // The net divided among the jets.
    float CStgGmk_WaterWorks_Volley_Act::GetParam_Width() const
    {
        float num = VOLLEY_PRM_S03_WATER_NUM;
        if (num <= 0.0f) {
            return 0.0f;
        }
        return CStageMan::GetInstance()->GetNetWidth() / num;
    }

    float CStgGmk_WaterWorks_Volley_Act::GetParam_Height() const
    {
        return VOLLEY_PRM_S03_WATER_HEIGHT;
    }

    float CStgGmk_WaterWorks_Volley_Act::GetParam_WaitTime() const
    {
        return 15.0f;
    }

    float CStgGmk_WaterWorks_Volley_Act::GetParam_StayTime() const
    {
        return m_fStayTime;
    }

    float CStgGmk_WaterWorks_Volley_Act::GetParam_PrestartTime(long type) const
    {
        if (type != 0) {
            return VOLLEY_PRM_S03_PAT2_PRESTART_TIME;
        }
        return VOLLEY_PRM_S03_WATER_PRESTART_TIME;
    }

    // The parameter is in seconds; the actor counts frames.
    float CStgGmk_WaterWorks_Volley_Act::GetParam_Interval() const
    {
        return 60.0f * VOLLEY_PRM_S03_WATER_INTERVAL;
    }

    long CStgGmk_WaterWorks_Volley_Act::GetParam_Effect(long type) const
    {
        switch (type) {
            case 0:
                return 0x6C1;
            case 1:
                return 0x6BF;
            case 2:
                return 0x6C0;
        }
        return -1;
    }

    float CStgGmk_WaterWorks_Volley_Act::GetParam_MoveResist() const
    {
        return 0.0f;
    }

    float CStgGmk_WaterWorks_Volley_Act::GetParam_WaterSpeed() const
    {
        return VOLLEY_PRM_S03_WATER_SPEED;
    }

    // Plays animation anm on the water wall; the other models play their
    // only animation (bit 1 of sub added, bit 4 of flag dropped).
    void CStgGmk_WaterWorks_Volley_Act::AnimPlay(long anm, unsigned long sub, float speed, float frame, float blend,
                                                 unsigned long flag)
    {
        CStgGmk_WaterWorks_Volley *pParent = static_cast<CStgGmk_WaterWorks_Volley *>(GetParent());
        long num = pParent != 0 ? pParent->GetMdlNum() : 0;
        for (long i = 0; i < num; i++) {
            CStgMdl *pMdl = pParent->GetMdl(i);
            if (pMdl != 0) {
                if (i == 0) {
                    pMdl->SetAnm(anm, sub, speed, frame, blend, flag);
                } else {
                    pMdl->SetAnm(0, sub | 2, speed, frame, blend, flag & ~0x10);
                }
            }
        }
    }

    bool CStgGmk_WaterWorks_Volley_Act::IsAnimEnd()
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

    // Shows or hides the water wall; the other models stay visible.
    void CStgGmk_WaterWorks_Volley_Act::SetVisibleAll(int bVisible)
    {
        CStgGmk_WaterWorks_Volley *pParent = static_cast<CStgGmk_WaterWorks_Volley *>(GetParent());
        long num = pParent != 0 ? pParent->GetMdlNum() : 0;
        for (long i = 0; i < num; i++) {
            CStgMdl *pMdl = pParent->GetMdl(i);
            if (pMdl != 0) {
                if (i == 0) {
                    pMdl->SetVisible(bVisible);
                } else {
                    pMdl->SetVisible(1);
                }
            }
        }
    }

    // Fades model mdl in; the models after the wall restart their
    // animation.
    void CStgGmk_WaterWorks_Volley_Act::Mdl_FadeIn(long mdl, float time, float alpha)
    {
        CStgObj *pParent = GetParent();
        CStgMdl *pMdl = pParent != 0 ? pParent->GetMdl(mdl) : 0;
        if (pMdl != 0) {
            pMdl->FadeIn(time, alpha);
            if (mdl != 0) {
                pMdl->SetAnm(0, 2, 1.0f, 0.0f, 0.0f, ~0x10);
            }
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::Mdl_FadeOut(long mdl, float time, float alpha)
    {
        CStgObj *pParent = GetParent();
        CStgMdl *pMdl = pParent != 0 ? pParent->GetMdl(mdl) : 0;
        if (pMdl != 0) {
            pMdl->FadeOut(time, alpha);
        }
    }

    // While the water rises, it can be held down and let go again.
    void CStgGmk_WaterWorks_Volley_Act::WaterSuspend()
    {
        if (FsmqGetState() == 3 && !IsWaterSuspend()) {
            AnimPlay(2, 0, GetParam_WaterSpeed(), 0.0f, 0.0f, -1);
            m_fCollTimer = 0.0f;
            m_bSuspend = 1;
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::WaterResume()
    {
        if (FsmqGetState() == 3 && IsWaterSuspend()) {
            SetVisibleAll(1);
            AnimPlay(0, 0, GetParam_WaterSpeed(), 0.0f, 0.0f, -1);
            m_fCollTimer = GetParam_WaitTime();
            m_bSuspend = 0;
        }
    }

    int CStgGmk_WaterWorks_Volley_Act::IsWaterSuspend() const
    {
        return m_bSuspend;
    }

    // From STANDBY only: after delay frames, or at once.
    void CStgGmk_WaterWorks_Volley_Act::ForceStart(float delay, int type)
    {
        if (delay > 0.0f) {
            if (FsmqGetState() == 1) {
                WORK *pWork = static_cast<WORK *>(FsmqSetState(1));
                pWork->nDelay = delay;
                pWork->nType = type;
            }
        } else {
            if (FsmqGetState() == 1) {
                WORK *pWork = static_cast<WORK *>(FsmqSetState(2));
                pWork->nType = type;
            }
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::ForceStop()
    {
        long state = FsmqGetState();
        if (state == 3 || state == 4) {
            FsmqChangeState(5);
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::ForceReset()
    {
        FsmqChangeState(0);
    }

    bool CStgGmk_WaterWorks_Volley_Act::IsActive() const
    {
        return FsmqGetState() != 6;
    }

    bool CStgGmk_WaterWorks_Volley_Act::IsSwoosh() const
    {
        long state = FsmqGetState();
        if (state >= 2 && state <= 5) {
            return true;
        }
        return false;
    }

    bool CStgGmk_WaterWorks_Volley_Act::IsSwoosh2() const
    {
        long state = FsmqGetState();
        if (state >= 2 && state <= 4) {
            return true;
        }
        return false;
    }

    bool CStgGmk_WaterWorks_Volley_Act::IsSwoosh3() const
    {
        long state = FsmqGetState();
        if (state >= 3 && state <= 4) {
            return true;
        }
        return false;
    }

    // The top of the water above the net's base height: rising along a
    // quarter sine while the wall rises (START) and falls (END).
    float CStgGmk_WaterWorks_Volley_Act::GetNetHeightTop() const
    {
        float top = 2.0f;
        switch (FsmqGetState()) {
            case 3: {
                float time = m_fCollTime;
                float rate = 0.0f;
                if (time > rate) {
                    rate = (time - m_fCollTimer) / time;
                }
                top += GetParam_Height() * nw4r::math::SinRad(1.5707964f * rate);
                break;
            }
            case 4:
                top += GetParam_Height();
                break;
            case 5: {
                float time = m_fCollTime;
                float rate = 0.0f;
                if (time > rate) {
                    rate = m_fCollTimer / time;
                }
                top += GetParam_Height() * nw4r::math::SinRad(1.5707964f * rate);
                break;
            }
        }
        return top;
    }

    float CStgGmk_WaterWorks_Volley_Act::GetNetHeightBottom() const
    {
        return CStageMan::GetInstance()->GetNetHeightBottom();
    }

    // Back to the default position, then off.
    void CStgGmk_WaterWorks_Volley_Act::CtorState_DEFAULT()
    {
        CStgSrt *pSrt = GetSrt();
        CStgGmk_WaterWorks_Volley *pParent = static_cast<CStgGmk_WaterWorks_Volley *>(GetParent());
        m_fTimer = 0.0f;
        m_bCollEnable = 0;
        SetVisibleAll(0);
        bcs::FVEC3 pos = pParent->GetDefaultPos();
        pSrt->SetPos(pos);
        FsmqChangeState(6);
        FsmqRetry();
    }

    void CStgGmk_WaterWorks_Volley_Act::LoopState_DEFAULT()
    {
    }

    void CStgGmk_WaterWorks_Volley_Act::DtorState_DEFAULT()
    {
    }

    // Waits for ForceStart, or counts the delay it gave down.
    void CStgGmk_WaterWorks_Volley_Act::CtorState_STANDBY()
    {
        WORK *pWork = FsmqGetWork<WORK>();
        m_fTimer = pWork->nDelay;
        m_bCollEnable = 0;
        SetVisibleAll(0);
    }

    void CStgGmk_WaterWorks_Volley_Act::LoopState_STANDBY()
    {
        if (FsmqGetWork<WORK>()->nDelay > 0.0f) {
            m_fTimer -= GetFrameRateF();
            if (m_fTimer <= 0.0f) {
                long type = FsmqGetWork<WORK>()->nType;
                static_cast<WORK *>(FsmqSetState(2))->nType = type;
                FsmqRetry();
            }
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::DtorState_STANDBY()
    {
    }

    // The warning: the spray and the drops fade in; with type 0 the water
    // wall starts its warning animation.
    void CStgGmk_WaterWorks_Volley_Act::CtorState_PRESTART()
    {
        WORK *pWork = FsmqGetWork<WORK>();
        Eff_CreateAttach(1, GetParam_Effect(2), 0, 0UL, 0, 0, 0, 0, 3);
        Mdl_FadeIn(2, 8.0f, 0.0f);
        Mdl_FadeIn(3, 8.0f, 0.0f);
        if (pWork->nType == 0) {
            SetVisibleAll(1);
            AnimPlay(3, 0, 1.0f, 0.0f, 0.0f, -1);
            Eff_CreateAttach(0, GetParam_Effect(1), 0, s_pBornNode, 0, 0, 0, 0, 3);
            Mdl_FadeIn(1, 8.0f, 0.0f);
        }
        m_fTimer = GetParam_PrestartTime(pWork->nType);
    }

    void CStgGmk_WaterWorks_Volley_Act::LoopState_PRESTART()
    {
        WORK *pWork = FsmqGetWork<WORK>();
        float rate = GetFrameRateF();
        if (IsAnimEnd()) {
            AnimPlay(4, 0, 1.0f, 0.0f, 0.0f, -1);
        }
        m_fTimer -= rate;
        if (m_fTimer <= 0.0f) {
            long type = pWork->nType;
            static_cast<WORK *>(FsmqSetState(3))->nType = type;
            FsmqRetry();
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::DtorState_PRESTART()
    {
    }

    // The water wall rises (with type 0 it is already up and only goes on).
    void CStgGmk_WaterWorks_Volley_Act::CtorState_START()
    {
        WORK *pWork = FsmqGetWork<WORK>();
        GetSrt();
        if (pWork->nType != 0) {
            SetVisibleAll(1);
            AnimPlay(0, 0, GetParam_WaterSpeed(), 0.0f, 0.0f, -1);
            m_bSuspend = 0;
        } else {
            AnimPlay(5, 0, GetParam_WaterSpeed(), 3.0f, 0.0f, -1);
        }
        Eff_CreateAttach(0, GetParam_Effect(1), 0, s_pBornNode, 0, 0, 0, 0, 3);
        Mdl_FadeIn(1, 8.0f, 0.0f);
        m_fTimer = GetParam_StayTime();
        m_fCollTimer = GetParam_WaitTime();
        m_fCollTime = GetParam_WaitTime();
    }

    void CStgGmk_WaterWorks_Volley_Act::LoopState_START()
    {
        float rate = GetFrameRateF();
        GetSrt();
        m_fCollTimer -= rate;
        if (m_fCollTimer > 0.0f) {
            m_bCollEnable = 0;
        } else {
            m_bCollEnable = 1;
            m_fCollTimer = 0.0f;
        }
        m_fTimer -= rate;
        if (IsAnimEnd()) {
            FsmqChangeState(4);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::DtorState_START()
    {
        GetSrt();
        m_bCollEnable = 0;
    }

    void CStgGmk_WaterWorks_Volley_Act::CtorState_STAY()
    {
        GetSrt();
        m_bCollEnable = 1;
        m_fTimer = GetParam_StayTime();
        AnimPlay(1, 2, 1.0f, 0.0f, 0.0f, -1);
    }

    void CStgGmk_WaterWorks_Volley_Act::LoopState_STAY()
    {
        m_fTimer -= GetFrameRateF();
        if (m_fTimer <= 0.0f) {
            FsmqChangeState(5);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::DtorState_STAY()
    {
        m_BodyColl.SetDisableAll();
        m_DefColl.SetDisableAll();
        m_bCollEnable = 0;
    }

    // The water wall falls (it collides while it does), then STANDBY.
    void CStgGmk_WaterWorks_Volley_Act::CtorState_END()
    {
        AnimPlay(2, 0, 1.0f, 6.0f, 0.0f, -1);
        m_fCollTimer = GetParam_WaitTime();
        m_fCollTime = GetParam_WaitTime();
        m_bCollEnable = 1;
    }

    void CStgGmk_WaterWorks_Volley_Act::LoopState_END()
    {
        m_fCollTimer -= GetFrameRateF();
        if (IsAnimEnd()) {
            m_fCollTimer = 0.0f;
            FsmqChangeState(1);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterWorks_Volley_Act::DtorState_END()
    {
        m_bCollEnable = 0;
        Eff_Stop(0);
        Eff_Stop(1);
        Mdl_FadeOut(1, 8.0f, 0.0f);
        Mdl_FadeOut(2, 8.0f, 0.0f);
        Mdl_FadeOut(3, 8.0f, 0.0f);
    }

    void CStgGmk_WaterWorks_Volley_Act::CtorState_INACTIVE()
    {
        m_bCollEnable = 0;
        SetVisibleAll(0);
        Eff_EraseAll();
        Mdl_FadeOut(1, 0.0f, 0.0f);
        Mdl_FadeOut(2, 0.0f, 0.0f);
        Mdl_FadeOut(3, 0.0f, 0.0f);
    }

    void CStgGmk_WaterWorks_Volley_Act::LoopState_INACTIVE()
    {
    }

    void CStgGmk_WaterWorks_Volley_Act::DtorState_INACTIVE()
    {
    }

    CStgGmk_WaterWorks_Volley_Drv::~CStgGmk_WaterWorks_Volley_Drv()
    {
    }

    void CStgGmk_WaterWorks_Volley_Drv::Main(float rate)
    {
        CStgDrv::Main(rate);
    }

    CStgGmk_WaterWorks_Volley::CStgGmk_WaterWorks_Volley(bcs::CTask *pParent, int) : CStgGmk(pParent, 4)
    {
    }

    CStgGmk_WaterWorks_Volley::~CStgGmk_WaterWorks_Volley()
    {
    }

    // Model 0 is the water wall, 1 to 3 plain gimmick models.
    CStgMdl *CStgGmk_WaterWorks_Volley::NewMdl(long heap, unsigned long idx)
    {
        CStgMdl *pMdl = 0;
        switch (idx) {
            case 0:
                pMdl = new (heap) CStgGmk_WaterWorks_Volley_Mdl(this, idx);
                break;
            case 1:
            case 2:
            case 3:
                pMdl = new (heap) CStgGenericGmkMdl(this, CLoadData::GetRes(2), &s_MdlParam[idx], 0);
                break;
        }
        return pMdl;
    }

    // The first drop model hangs from the water wall's node; the other
    // models start faded out.
    void CStgGmk_WaterWorks_Volley::OnPostCreate()
    {
        CStgMdl *pWall = GetMdl(0);
        CStgMdl *pDrop = GetMdl(1);
        bcs::CMdlObj *pWallObj = pWall != 0 ? pWall->GetMdlObj() : 0;
        bcs::CMdlObj *pDropObj = pDrop != 0 ? pDrop->GetMdlObj() : 0;
        if (pWallObj != 0 && pDropObj != 0) {
            nw4r::g3d::ResNode node = pWallObj->GetResMdl().GetResNode(s_pBornNode);
            pDropObj->AttachToObject(pWallObj, node.GetID(), static_cast<bcs::CScnObj::AttachMode>(1));
        }
        for (long i = 1; i < 4; i++) {
            CStgMdl *pMdl = GetMdl(i);
            if (pMdl != 0) {
                pMdl->FadeOut(0.0f, 0.0f);
            }
        }
    }

    void CStgGmk_WaterWorks_Volley::OnSceneEntry()
    {
    }

    void CStgGmk_WaterWorks_Volley::OnSceneLeave()
    {
    }

    void CStgGmk_WaterWorks_Volley::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    // STANDBY.
    void CStgGmk_WaterWorks_Volley::Activate()
    {
        CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0 && !pAct->IsActive()) {
            pAct->FsmqChangeState(1);
        }
    }

    // INACTIVE.
    void CStgGmk_WaterWorks_Volley::Inactivate()
    {
        CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0 && pAct->IsActive()) {
            pAct->FsmqChangeState(6);
        }
    }

    void CStgGmk_WaterWorks_Volley::Start(float delay, int bDelayed)
    {
        CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->ForceStart(delay, bDelayed);
        }
    }

    void CStgGmk_WaterWorks_Volley::Stop()
    {
        CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->ForceStop();
        }
    }

    void CStgGmk_WaterWorks_Volley::Reset()
    {
        CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->ForceReset();
        }
    }

    bool CStgGmk_WaterWorks_Volley::IsSwoosh() const
    {
        const CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            return pAct->IsSwoosh();
        }
        return false;
    }

    bool CStgGmk_WaterWorks_Volley::IsSwoosh2() const
    {
        const CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            return pAct->IsSwoosh2();
        }
        return false;
    }

    bool CStgGmk_WaterWorks_Volley::IsSwoosh3() const
    {
        const CStgGmk_WaterWorks_Volley_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            return pAct->IsSwoosh3();
        }
        return false;
    }

    void CStgGmk_WaterWorks_Volley::Disp()
    {
    }
}
