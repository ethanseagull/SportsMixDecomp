// Written from src/game/volley/CStgGmk_WaterWorks_Volley_Act.cpp (the
// same gimmick in VolleyF) and checked against DodgeF.
#include "game/msw/CStgGmk_WaterWorks_Dodge.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CAnmTblBase.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageAnm.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgGenericGmkMdl.hpp"
#include "game/msw/CStgSrt.hpp"
#include "game/msw/DodgePrm.hpp"
#include "nw4r/math/triangular.h"

// The water jets of the dodge fountain stage (stage 03): the model
// (CStgGmk_WaterWorks_Dodge_Mdl), the actor (CStgGmk_WaterWorks_Dodge_Act),
// the driver and the gimmick (CStgGmk_WaterWorks_Dodge). One jet has four
// model slots: the water wall, two drop models (only the second is made)
// and the spray. The wall rises from the ground; the jets are a wall the
// ball cannot pass while they stand (the stage asks IsSwoosh), and a ball
// meeting the water makes a splash sound.

namespace {

    // The node of the water wall the effects and the drop model are
    // attached to.
    const char *s_pBornNode = "born";

    // Suffixes of the water wall's animations, appended to the model name.
    const char *s_AnmSuffix[] = { "00", "00_00", "00_01", "01", "01_00", "01_01", "02", "02_00", "02_01", 0 };

    // The four models of a jet.
    const msw::CStgMdl::MDL_PARAM s_MdlParam[] = {
        { "s03_gm_waterwall00", 0, 0, 0x30C, s_AnmSuffix, "s03_gm_waterwall", 0, 0 },
        { "s03_gm_sibukli00", 0, 0, 0x30C, msw::CAnmTblBase::null_tbl, "s03_gm_sibukli00", 0, 0 },
        { "s03_gm_sibukli01", 0, 0, 0x30C, msw::CAnmTblBase::null_tbl, "s03_gm_sibukli01", 0, 0 },
        { "s03_gm_tubu00", 0, 0, 0x30C, msw::CAnmTblBase::null_tbl, "s03_gm_tubu00", 0, 0 },
    };
}

namespace msw {

    // The gimmick's and the actor's inline functions (weak copies here; see
    // the header).

    inline unsigned long CStgGmk_WaterWorks_Dodge::GetMdlNum() const
    {
        return 4;
    }

    inline void CStgGmk_WaterWorks_Dodge_Act::SetParam_StayTime(float time)
    {
        m_fStayTime = time;
    }

    inline void CStgGmk_WaterWorks_Dodge::SetParam_StayTime(float time)
    {
        CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->SetParam_StayTime(time);
        }
    }

    inline CStgAct *CStgGmk_WaterWorks_Dodge::NewAct(long heap)
    {
        return new (heap) CStgGmk_WaterWorks_Dodge_Act(this);
    }

    inline CStgDrv *CStgGmk_WaterWorks_Dodge::NewDrv(long heap)
    {
        return new (heap) CStgGmk_WaterWorks_Dodge_Drv(this);
    }

    CStgGmk_WaterWorks_Dodge_Mdl::~CStgGmk_WaterWorks_Dodge_Mdl()
    {
    }

    void CStgGmk_WaterWorks_Dodge_Mdl::Create()
    {
        long idx = m_nMdlIdx;
        idx = idx > 3 ? 3 : (idx < 0 ? 0 : idx);
        CStgMdl::Create(CGenericMVAGimmickCreator(CLoadData::GetRes(2), &s_MdlParam[idx]));
    }

    bcs::CMdlObj *CStgGmk_WaterWorks_Dodge_Mdl::NewMdl(long heap) const
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

    void CStgGmk_WaterWorks_Dodge_Mdl::Main(float rate)
    {
        CStgAct *pAct = GetParent()->GetAct();
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pAct != 0 && pMdl != 0) {
            pMdl->CalcRootMtx();
            pMdl->CalcWorldMtx(0);
        }
        CStgMdl::Main(rate);
    }

    CStgGmk_WaterWorks_Dodge_Act::CStgGmk_WaterWorks_Dodge_Act(CStgGmk *pParent)
        : CStgGmkAct(pParent), m_bCollEnable(0), m_fHeight(0.0f), m_fStayTime(-1.0f), m_fSplashWait(0.0f)
    {
        FsmqInit(this, 10, 4, sizeof(WORK));
        FsmqSetState(0, &CStgGmk_WaterWorks_Dodge_Act::CtorState_DEFAULT,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_DEFAULT,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_DEFAULT);
        FsmqSetState(1, &CStgGmk_WaterWorks_Dodge_Act::CtorState_STANDBY,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_STANDBY,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_STANDBY);
        FsmqSetState(2, &CStgGmk_WaterWorks_Dodge_Act::CtorState_PRESTART,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_PRESTART,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_PRESTART);
        FsmqSetState(3, &CStgGmk_WaterWorks_Dodge_Act::CtorState_START,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_START,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_START);
        FsmqSetState(4, &CStgGmk_WaterWorks_Dodge_Act::CtorState_STAY,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_STAY,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_STAY);
        FsmqSetState(5, &CStgGmk_WaterWorks_Dodge_Act::CtorState_END,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_END,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_END);
        FsmqSetState(6, &CStgGmk_WaterWorks_Dodge_Act::CtorState_TALL_START,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_TALL_START,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_TALL_START);
        FsmqSetState(7, &CStgGmk_WaterWorks_Dodge_Act::CtorState_TALL_STAY,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_TALL_STAY,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_TALL_STAY);
        FsmqSetState(8, &CStgGmk_WaterWorks_Dodge_Act::CtorState_TALL_END,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_TALL_END,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_TALL_END);
        FsmqSetState(9, &CStgGmk_WaterWorks_Dodge_Act::CtorState_INACTIVE,
                     &CStgGmk_WaterWorks_Dodge_Act::LoopState_INACTIVE,
                     &CStgGmk_WaterWorks_Dodge_Act::DtorState_INACTIVE);
        GetSrt()->ClearVelocity();
    }

    CStgGmk_WaterWorks_Dodge_Act::~CStgGmk_WaterWorks_Dodge_Act()
    {
    }

    // The collisions are never made: the ball is tested by hand in
    // UpdateColl.
    void CStgGmk_WaterWorks_Dodge_Act::OnInit()
    {
    }

    // Not while the game is held (flags 0x8000 and 0x2000 of the game
    // work): runs the states, keeps the jet above the ground and scales the
    // water wall to the jet's width and height.
    void CStgGmk_WaterWorks_Dodge_Act::OnUpdate(float rate)
    {
        if ((CGame::g_GameWork.m_nFlag18 & 0x8000) || (CGame::g_GameWork.m_nFlag18 & 0x2000)) {
            return;
        }

        CStgSrtMove *pSrt = GetSrt();

        if (rate > 0.0f && m_fSplashWait > 0.0f) {
            m_fSplashWait -= rate;
            if (m_fSplashWait < 0.0f) {
                m_fSplashWait = 0.0f;
            }
        }

        FsmqExec();
        pSrt->Update(rate);

        bcs::FVEC3 pos = pSrt->GetPos();
        if (pos.y < 0.05f) {
            pos.y = 0.05f;
            pSrt->SetPos(pos);
            pSrt->ClearUnkE4();
        }

        float scaleX = GetParam_Width() / 9.5f;
        pSrt->SetScale(scaleX, GetParam_Height() * 0.5f, 1.0f);
        UpdateColl();
    }

    // The water wall and the spray follow the actor; the drop model is
    // attached to the wall's node (OnPostCreate).
    void CStgGmk_WaterWorks_Dodge_Act::UpdateMdlMtx()
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
            }
        }
    }

    // While the water stands, a flying ball (shot, not held) passing
    // through the jet's plane below the water's height makes the splash.
    void CStgGmk_WaterWorks_Dodge_Act::UpdateColl()
    {
        if (m_bCollEnable && !IsWaterSuspend()) {
            bcs::FVEC3 pos = GetSrt()->GetPos();
            long num = CBallMan::GetBallNum();
            for (long i = 0; i < num; i++) {
                CBall *pBall = CBallMan::GetBallPtr(i);
                if (pBall == 0 || !pBall->IsActive() || pBall->IsFlag2() || pBall->IsFlag4() ||
                    !pBall->IsShootMode()) {
                    continue;
                }
                bcs::FVEC3 ballPos = pBall->GetPos();
                if (__fabsf(ballPos.z - pos.z) < 0.2f && ballPos.y < GetParam_Height()) {
                    if (m_fSplashWait <= 0.0f && pBall != 0) {
                        bcs::FVEC3 sePos = pBall->GetPos();
                        CGameSnd::Play(0xD15, sePos, 1, -1, 0.0f, 1.0f, 0.0f);
                        m_fSplashWait = 15.0f;
                    }
                    break;
                }
            }
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
    }

    unsigned long CStgGmk_WaterWorks_Dodge_Act::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                             const bcs::FVEC3 &rPos)
    {
        return 0;
    }

    void CStgGmk_WaterWorks_Dodge_Act::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
    }

    unsigned long CStgGmk_WaterWorks_Dodge_Act::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                             bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
    {
        if (m_fSplashWait <= 0.0f) {
            CGameSnd::Play(0xD15, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
            m_fSplashWait = 15.0f;
        }
        return 0;
    }

    // The court divided among the jets.
    float CStgGmk_WaterWorks_Dodge_Act::GetParam_Width() const
    {
        float num = DODGE_PRM_S03_WATER_NUM;
        if (num <= 0.0f) {
            return 0.0f;
        }
        CStageMan *pStage = CStageMan::GetInstance();
        return (pStage->GetCourtRight() - pStage->GetCourtLeft()) / num;
    }

    float CStgGmk_WaterWorks_Dodge_Act::GetParam_Height() const
    {
        return DODGE_PRM_S03_WATER_HEIGHT;
    }

    float CStgGmk_WaterWorks_Dodge_Act::GetParam_WaitTime() const
    {
        return 15.0f;
    }

    float CStgGmk_WaterWorks_Dodge_Act::GetParam_StayTime() const
    {
        return m_fStayTime;
    }

    // Below zero: the tall jet stands until it is stopped.
    float CStgGmk_WaterWorks_Dodge_Act::GetParam_TallTime() const
    {
        return -1.0f;
    }

    // Type 0 waits in PRESTART until ForceStart gives it a time.
    float CStgGmk_WaterWorks_Dodge_Act::GetParam_PrestartTime(long type) const
    {
        if (type != 0) {
            return 0.0f;
        }
        return -1.0f;
    }

    float CStgGmk_WaterWorks_Dodge_Act::GetParam_Interval() const
    {
        return 0.0f;
    }

    long CStgGmk_WaterWorks_Dodge_Act::GetParam_Effect(long type) const
    {
        return -1;
    }

    float CStgGmk_WaterWorks_Dodge_Act::GetParam_MoveResist() const
    {
        return 0.0f;
    }

    float CStgGmk_WaterWorks_Dodge_Act::GetParam_WaterSpeed() const
    {
        return DODGE_PRM_S03_WATER_SPEED;
    }

    // Plays animation anm on the water wall; the other models play their
    // only animation (bit 1 of sub added, bit 4 of flag dropped).
    void CStgGmk_WaterWorks_Dodge_Act::AnimPlay(long anm, unsigned long sub, float speed, float frame, float blend,
                                                unsigned long flag)
    {
        CStgGmk_WaterWorks_Dodge *pParent = static_cast<CStgGmk_WaterWorks_Dodge *>(GetParent());
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

    bool CStgGmk_WaterWorks_Dodge_Act::IsAnimEnd()
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
    void CStgGmk_WaterWorks_Dodge_Act::SetVisibleAll(int bVisible)
    {
        CStgGmk_WaterWorks_Dodge *pParent = static_cast<CStgGmk_WaterWorks_Dodge *>(GetParent());
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

    void CStgGmk_WaterWorks_Dodge_Act::Mdl_FadeIn(long mdl, float time, float alpha)
    {
        CStgObj *pParent = GetParent();
        CStgMdl *pMdl = pParent != 0 ? pParent->GetMdl(mdl) : 0;
        if (pMdl != 0) {
            pMdl->FadeIn(time, alpha);
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::Mdl_FadeOut(long mdl, float time, float alpha)
    {
        CStgObj *pParent = GetParent();
        CStgMdl *pMdl = pParent != 0 ? pParent->GetMdl(mdl) : 0;
        if (pMdl != 0) {
            pMdl->FadeOut(time, alpha);
        }
    }

    // While the water rises, it can be held down and let go again.
    void CStgGmk_WaterWorks_Dodge_Act::WaterSuspend()
    {
        if (FsmqGetState() == 3 && !IsWaterSuspend()) {
            AnimPlay(2, 0, GetParam_WaterSpeed(), 0.0f, 0.0f, -1);
            m_fCollTimer = 0.0f;
            m_bSuspend = 1;
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::WaterResume()
    {
        if (FsmqGetState() == 3 && IsWaterSuspend()) {
            SetVisibleAll(1);
            AnimPlay(0, 0, GetParam_WaterSpeed(), 0.0f, 0.0f, -1);
            m_fCollTimer = GetParam_WaitTime();
            m_bSuspend = 0;
        }
    }

    int CStgGmk_WaterWorks_Dodge_Act::IsWaterSuspend() const
    {
        return m_bSuspend;
    }

    // From DEFAULT, STANDBY or INACTIVE: STANDBY with a delay, or PRESTART
    // at once. In PRESTART: a delay sets the warning's length, none starts
    // the rise at once.
    void CStgGmk_WaterWorks_Dodge_Act::ForceStart(float delay, int type)
    {
        if (delay > 0.0f) {
            switch (FsmqGetState()) {
                case 0:
                case 1:
                case 9: {
                    WORK *pWork = static_cast<WORK *>(FsmqSetState(1));
                    pWork->nDelay = delay;
                    pWork->nType = type;
                    break;
                }
                case 2:
                    m_fTime = delay;
                    break;
            }
        } else {
            switch (FsmqGetState()) {
                case 0:
                case 1:
                case 9:
                    static_cast<WORK *>(FsmqSetState(2))->nType = type;
                    break;
                case 2: {
                    long type2 = FsmqGetWork<WORK>()->nType;
                    static_cast<WORK *>(FsmqSetState(3))->nType = type2;
                    break;
                }
            }
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::ForceStop()
    {
        switch (FsmqGetState()) {
            case 3:
            case 4:
                FsmqChangeState(5);
                break;
            case 6:
            case 7:
                FsmqChangeState(8);
                break;
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::ForceTallStart()
    {
        long state = FsmqGetState();
        if (state == 3 || state == 4) {
            FsmqChangeState(6);
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::ForceReset()
    {
        SE_Stop(0, 0.0f);
        FsmqClear();
        FsmqChangeState(0);
    }

    bool CStgGmk_WaterWorks_Dodge_Act::IsActive() const
    {
        return FsmqGetState() != 9;
    }

    bool CStgGmk_WaterWorks_Dodge_Act::IsSwoosh() const
    {
        long state = FsmqGetState();
        if (state >= 2 && state <= 8) {
            return true;
        }
        return false;
    }

    bool CStgGmk_WaterWorks_Dodge_Act::IsSwoosh2() const
    {
        long state = FsmqGetState();
        if ((state >= 2 && state <= 4) || (state >= 6 && state <= 7)) {
            return true;
        }
        return false;
    }

    bool CStgGmk_WaterWorks_Dodge_Act::IsSwooshTall() const
    {
        long state = FsmqGetState();
        if (state >= 6 && state <= 7) {
            return true;
        }
        return false;
    }

    // The top of the water above the net's base height: rising along a
    // quarter sine while the wall rises (START) and falls (END).
    float CStgGmk_WaterWorks_Dodge_Act::GetNetHeightTop() const
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

    float CStgGmk_WaterWorks_Dodge_Act::GetNetHeightBottom() const
    {
        return CStageMan::GetInstance()->GetNetHeightBottom();
    }

    // Back to the default position, then the queued state or INACTIVE.
    void CStgGmk_WaterWorks_Dodge_Act::CtorState_DEFAULT()
    {
        CStgSrt *pSrt = GetSrt();
        CStgGmk_WaterWorks_Dodge *pParent = static_cast<CStgGmk_WaterWorks_Dodge *>(GetParent());
        m_bCollEnable = 0;
        SetVisibleAll(0);
        bcs::FVEC3 pos = pParent->GetDefaultPos();
        pSrt->SetPos(pos);
        if (FsmqIsEmpty()) {
            FsmqChangeState(9);
        } else {
            FsmqNext();
        }
        FsmqRetry();
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_DEFAULT()
    {
    }

    void CStgGmk_WaterWorks_Dodge_Act::DtorState_DEFAULT()
    {
    }

    // Waits for ForceStart, or counts the delay it gave down (only while
    // the stage's gimmicks run).
    void CStgGmk_WaterWorks_Dodge_Act::CtorState_STANDBY()
    {
        WORK *pWork = FsmqGetWork<WORK>();
        m_fTimer = pWork->nDelay;
        m_bCollEnable = 0;
        SetVisibleAll(0);
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_STANDBY()
    {
        WORK *pWork = FsmqGetWork<WORK>();
        CStageMan *pStage = CStageMan::GetInstance();
        if (pWork->nDelay > 0.0f && pStage->IsGimmickRunning()) {
            m_fTimer -= GetFrameRateF();
            if (m_fTimer <= 0.0f) {
                long type = FsmqGetWork<WORK>()->nType;
                static_cast<WORK *>(FsmqSetState(2))->nType = type;
                FsmqRetry();
            }
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::DtorState_STANDBY()
    {
    }

    // The warning: the spray fades in and the jet sound starts; with type 0
    // the water wall starts its warning animation and the drops show.
    void CStgGmk_WaterWorks_Dodge_Act::CtorState_PRESTART()
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
        m_fTimer = 0.0f;
        m_fTime = GetParam_PrestartTime(pWork->nType);
        if (!SE_IsAlive(0)) {
            SE_PlayPos(0, 0xD12, 0.0f, 3, 0);
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_PRESTART()
    {
        WORK *pWork = FsmqGetWork<WORK>();
        float rate = GetFrameRateF();
        if (IsAnimEnd()) {
            AnimPlay(4, 0, 1.0f, 0.0f, 0.0f, -1);
        }
        if (m_fTime >= 0.0f) {
            m_fTimer += rate;
            if (m_fTimer >= m_fTime) {
                long type = pWork->nType;
                static_cast<WORK *>(FsmqSetState(3))->nType = type;
                FsmqRetry();
            }
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::DtorState_PRESTART()
    {
    }

    // The water wall rises (with type 0 it is already up and only goes on).
    void CStgGmk_WaterWorks_Dodge_Act::CtorState_START()
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
        m_fTimer = 0.0f;
        m_fTime = GetParam_StayTime();
        m_fCollTimer = GetParam_WaitTime();
        m_fCollTime = GetParam_WaitTime();
        if (!SE_IsAlive(0)) {
            SE_PlayPos(0, 0xD12, 0.0f, 3, 0);
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_START()
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
        if (m_fTime >= 0.0f) {
            m_fTimer += rate;
        }
        if (IsAnimEnd()) {
            FsmqChangeState(4);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::DtorState_START()
    {
        GetSrt();
        m_bCollEnable = 0;
    }

    void CStgGmk_WaterWorks_Dodge_Act::CtorState_STAY()
    {
        GetSrt();
        m_bCollEnable = 1;
        m_fTime = GetParam_StayTime();
        AnimPlay(1, 2, 1.0f, 0.0f, 0.0f, -1);
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_STAY()
    {
        float rate = GetFrameRateF();
        if (m_fTime >= 0.0f) {
            m_fTimer += rate;
            if (m_fTimer >= m_fTime) {
                FsmqChangeState(5);
                FsmqRetry();
            }
        }
    }

    // The end sound, unless the jet goes on to the tall stage.
    void CStgGmk_WaterWorks_Dodge_Act::DtorState_STAY()
    {
        m_BodyColl.SetDisableAll();
        m_DefColl.SetDisableAll();
        m_bCollEnable = 0;
        if (FsmqGetState() != 6) {
            SE_PlayPos(0, 0xD13, 0.0f, 0, 0);
        }
    }

    // The water wall falls (it collides while it does), then STANDBY.
    void CStgGmk_WaterWorks_Dodge_Act::CtorState_END()
    {
        AnimPlay(2, 0, 1.0f, 6.0f, 0.0f, -1);
        m_fCollTimer = GetParam_WaitTime();
        m_fCollTime = GetParam_WaitTime();
        m_bCollEnable = 1;
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_END()
    {
        m_fCollTimer -= GetFrameRateF();
        if (IsAnimEnd()) {
            m_fCollTimer = 0.0f;
            FsmqChangeState(1);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::DtorState_END()
    {
        m_bCollEnable = 0;
        Eff_Stop(0);
        Eff_Stop(1);
        Mdl_FadeOut(1, 8.0f, 0.0f);
        Mdl_FadeOut(2, 8.0f, 0.0f);
        Mdl_FadeOut(3, 8.0f, 0.0f);
    }

    // The tall jet: from START or STAY the wall grows further.
    void CStgGmk_WaterWorks_Dodge_Act::CtorState_TALL_START()
    {
        GetSrt();
        AnimPlay(6, 0, GetParam_WaterSpeed(), 3.0f, 0.0f, -1);
        m_fTimer = 0.0f;
        m_fTime = GetParam_TallTime();
        SE_Stop(0, 0.0f);
        SE_PlayPos(0, 0xD12, 0.0f, 3, 0);
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_TALL_START()
    {
        float rate = GetFrameRateF();
        GetSrt();
        if (m_fTime >= 0.0f) {
            m_fTimer += rate;
        }
        if (IsAnimEnd()) {
            FsmqChangeState(7);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::DtorState_TALL_START()
    {
        GetSrt();
        m_bCollEnable = 0;
    }

    void CStgGmk_WaterWorks_Dodge_Act::CtorState_TALL_STAY()
    {
        GetSrt();
        m_bCollEnable = 1;
        m_fTime = GetParam_TallTime();
        AnimPlay(7, 2, 1.0f, 0.0f, 0.0f, -1);
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_TALL_STAY()
    {
        float rate = GetFrameRateF();
        if (m_fTime >= 0.0f) {
            m_fTimer += rate;
            if (m_fTimer >= m_fTime) {
                FsmqChangeState(8);
                FsmqRetry();
            }
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::DtorState_TALL_STAY()
    {
        m_BodyColl.SetDisableAll();
        m_DefColl.SetDisableAll();
        m_bCollEnable = 0;
        SE_PlayPos(0, 0xD13, 0.0f, 0, 0);
    }

    void CStgGmk_WaterWorks_Dodge_Act::CtorState_TALL_END()
    {
        AnimPlay(8, 0, 1.0f, 6.0f, 0.0f, -1);
        m_fCollTimer = GetParam_WaitTime();
        m_fCollTime = GetParam_WaitTime();
        m_bCollEnable = 1;
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_TALL_END()
    {
        m_fCollTimer -= GetFrameRateF();
        if (IsAnimEnd()) {
            m_fCollTimer = 0.0f;
            FsmqChangeState(1);
            FsmqRetry();
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::DtorState_TALL_END()
    {
        m_bCollEnable = 0;
        Eff_Stop(0);
        Eff_Stop(1);
        Mdl_FadeOut(1, 8.0f, 0.0f);
        Mdl_FadeOut(2, 8.0f, 0.0f);
        Mdl_FadeOut(3, 8.0f, 0.0f);
    }

    void CStgGmk_WaterWorks_Dodge_Act::CtorState_INACTIVE()
    {
        m_bCollEnable = 0;
        SetVisibleAll(0);
        Eff_EraseAll();
        Mdl_FadeOut(1, 0.0f, 0.0f);
        Mdl_FadeOut(2, 0.0f, 0.0f);
        Mdl_FadeOut(3, 0.0f, 0.0f);
    }

    void CStgGmk_WaterWorks_Dodge_Act::LoopState_INACTIVE()
    {
        if (FsmqIsEmpty()) {
            FsmqRetry();
        }
    }

    void CStgGmk_WaterWorks_Dodge_Act::DtorState_INACTIVE()
    {
    }

    CStgGmk_WaterWorks_Dodge_Drv::~CStgGmk_WaterWorks_Dodge_Drv()
    {
    }

    void CStgGmk_WaterWorks_Dodge_Drv::Main(float rate)
    {
        CStgDrv::Main(rate);
    }

    CStgGmk_WaterWorks_Dodge::CStgGmk_WaterWorks_Dodge(bcs::CTask *pParent, int) : CStgGmk(pParent, 4)
    {
    }

    CStgGmk_WaterWorks_Dodge::~CStgGmk_WaterWorks_Dodge()
    {
    }

    // Model 0 is the water wall, 2 and 3 plain gimmick models; slot 1 stays
    // empty.
    CStgMdl *CStgGmk_WaterWorks_Dodge::NewMdl(long heap, unsigned long idx)
    {
        CStgMdl *pMdl = 0;
        switch (idx) {
            case 0:
                pMdl = new (heap) CStgGmk_WaterWorks_Dodge_Mdl(this, idx);
                break;
            case 2:
            case 3:
                pMdl = new (heap) CStgGenericGmkMdl(this, CLoadData::GetRes(2), &s_MdlParam[idx], 0);
                break;
        }
        return pMdl;
    }

    // The first drop model hangs from the water wall's node; the other
    // models start faded out.
    void CStgGmk_WaterWorks_Dodge::OnPostCreate()
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

    void CStgGmk_WaterWorks_Dodge::OnSceneEntry()
    {
    }

    void CStgGmk_WaterWorks_Dodge::OnSceneLeave()
    {
    }

    void CStgGmk_WaterWorks_Dodge::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    // STANDBY.
    void CStgGmk_WaterWorks_Dodge::Activate()
    {
        CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0 && !pAct->IsActive()) {
            pAct->FsmqChangeState(1);
        }
    }

    // INACTIVE.
    void CStgGmk_WaterWorks_Dodge::Inactivate()
    {
        CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0 && pAct->IsActive()) {
            pAct->FsmqChangeState(9);
        }
    }

    void CStgGmk_WaterWorks_Dodge::Start(float delay, int type)
    {
        CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->ForceStart(delay, type);
        }
    }

    void CStgGmk_WaterWorks_Dodge::Stop()
    {
        CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->ForceStop();
        }
    }

    void CStgGmk_WaterWorks_Dodge::TallStart()
    {
        CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->ForceTallStart();
        }
    }

    // DEFAULT, entered at once.
    void CStgGmk_WaterWorks_Dodge::Reset()
    {
        CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            pAct->ForceReset();
            pAct->FsmqDoChain();
            pAct->FsmqExec();
        }
    }

    bool CStgGmk_WaterWorks_Dodge::IsSwoosh() const
    {
        const CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            return pAct->IsSwoosh();
        }
        return false;
    }

    bool CStgGmk_WaterWorks_Dodge::IsSwoosh2() const
    {
        const CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            return pAct->IsSwoosh2();
        }
        return false;
    }

    bool CStgGmk_WaterWorks_Dodge::IsSwooshTall() const
    {
        const CStgGmk_WaterWorks_Dodge_Act *pAct = GetWaterAct();
        if (pAct != 0) {
            return pAct->IsSwooshTall();
        }
        return false;
    }

    void CStgGmk_WaterWorks_Dodge::Disp()
    {
    }
}
