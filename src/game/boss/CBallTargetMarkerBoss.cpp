// msw::CBallTargetMarkerBoss: the marker on the court where the boss game's
// ball will come down. The same design as VolleyF's CBallTargetMarker
// (game/volley/CBallTargetMarker.cpp) with only the ground mark and the
// chance-ball mark.
#include "game/msw/CBallTargetMarkerBoss.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrMarker.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/math/triangular.h"

namespace {
    // The ground mark and the chance-ball mark.
    const char *s_pMdlName = "mv_target_space01_01";
    const char *s_pChanceMdlName = "mv_chance_target01_01";
    // The two materials the colours go to.
    const char *s_pMatName0 = "marker_color00";
    const char *s_pMatName1 = "marker_color01";

    // Stand-in for an unknown user of -1 and 0.23, which the original keeps
    // in .rodata before Main's first constant; 0.23 has no reader in the
    // unit (likely stripped code), as in VolleyF's CBallTargetMarker. See
    // notes/decisions.md, "Unreferenced data kept by dead references".
    inline void UnusedReferences(float f)
    {
        if (f == -1.0f) {
        }
        if (f == 0.23f) {
        }
    }

    inline float Clamp01(float x)
    {
        return x > 1.0f ? 1.0f : (x < 0.0f ? 0.0f : x);
    }

    inline msw::CBall *GetBall(unsigned long nHandle)
    {
        return static_cast<msw::CBall *>(bcs::CTaskMan::GetTask(nHandle));
    }

    // Moves pObj over the court, keeping its height.
    inline void SetPosXZ(bcs::CScnObj *pObj, float x, float z)
    {
        pObj->m_vTrans.x = x;
        pObj->m_vTrans.z = z;
    }
}

msw::CBallTargetMarkerBoss::CBallTargetMarkerBoss(bcs::CTask *pParent, unsigned long nBallHandle, int nArg)
    : bcs::CTask(pParent, 0x19)
{
    m_pGroup = 0;
    m_pMdlBase = 0;
    m_pMdlChance = 0;
    m_nBallHandle = nBallHandle;
    m_fAppearRate = 0.0f;
    m_bChance = 0;
    m_nSpikeType = 0;

    InitFsm(this, STATE_NUM);
    SetState(STATE_SLEEP, &CBallTargetMarkerBoss::CtorStateSleep, &CBallTargetMarkerBoss::LoopStateSleep,
             &CBallTargetMarkerBoss::DtorStateSleep);
    SetState(STATE_APPEAR, &CBallTargetMarkerBoss::CtorStateAppear, &CBallTargetMarkerBoss::LoopStateAppear,
             &CBallTargetMarkerBoss::DtorStateAppear);
    SetState(STATE_MAIN, &CBallTargetMarkerBoss::CtorStateMain, &CBallTargetMarkerBoss::LoopStateMain,
             &CBallTargetMarkerBoss::DtorStateMain);
    SetState(STATE_DISAPPEAR, &CBallTargetMarkerBoss::CtorStateDisappear,
             &CBallTargetMarkerBoss::LoopStateDisappear, &CBallTargetMarkerBoss::DtorStateDisappear);
    ChangeState(STATE_SLEEP);

    Create_(nArg);
}

msw::CBallTargetMarkerBoss::~CBallTargetMarkerBoss()
{
    UnusedReferences(m_fAppearRate);
    if (m_pMdlChance) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlChance);
    }
    m_pMdlChance = 0;
    if (m_pMdlBase) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlBase);
    }
    m_pMdlBase = 0;
    if (m_pGroup) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pGroup);
    }
    m_pGroup = 0;
}

// The ground mark in m_pGroup and the chance-ball mark attached to it,
// drawn without depth test and hidden until the marker comes up.
void msw::CBallTargetMarkerBoss::Create_(int)
{
    nw4r::g3d::ResFile file = CChrMarker::GetRsrcPack();
    GetBall(m_nBallHandle); // (the result is not used)

    m_pGroup = new (0) bcs::CScnGroup(0x10, 0);
    m_pGroup->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pGroup->OnFlag(0x4000);

    nw4r::g3d::ResMdl mdl = file.GetResMdl(s_pMdlName);
    m_pMdlBase = new (0) bcs::CMdlObj;
    m_pMdlBase->Create(mdl, 0, 0);
    m_pMdlBase->OnFlag(0x4000);
    m_pMdlBase->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlBase->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlBase->SetDrawPriority(0, 0);
    m_pMdlBase->SetAlphaRate(0);
    m_pGroup->AppendChild(m_pMdlBase);

    mdl = file.GetResMdl(s_pChanceMdlName);
    m_pMdlChance = new (0) bcs::CMdlObj;
    nw4r::g3d::ResAnmClr resClr;
    nw4r::g3d::ResAnmTexSrt resTexSrt;
    nw4r::g3d::ResAnmTexPat resTexPat;
    unsigned long anmFlag = 0;
    if (file.HasResAnmClr()) {
        resClr = file.GetResAnmClr(s_pChanceMdlName);
        if (resClr.IsValid()) {
            anmFlag |= 0x2;
        }
    }
    if (file.HasResAnmTexSrt()) {
        resTexSrt = file.GetResAnmTexSrt(s_pChanceMdlName);
        if (resTexSrt.IsValid()) {
            anmFlag |= 0x204;
        }
    }
    if (file.HasResAnmTexPat()) {
        resTexPat = file.GetResAnmTexPat(s_pChanceMdlName);
        if (resTexPat.IsValid()) {
            anmFlag |= 0x3;
        }
    }
    m_pMdlChance->Create(mdl, anmFlag, 0);
    m_pMdlChance->OnFlag(0x2000);
    m_pMdlChance->OnFlag(0x4000);
    m_pMdlChance->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlChance->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlChance->SetDrawPriority(0, 0);
    m_pMdlChance->AttachToObject(m_pMdlBase, 0, bcs::CScnObj::ATTACHMODE_3);
    m_pMdlChance->SetAlphaRate(0);
    if (resClr.IsValid()) {
        bcs::CAnmMatClr *pAnm = new (0) bcs::CAnmMatClr;
        if (pAnm) {
            pAnm->Create(m_pMdlChance, resClr, 0, 0);
            m_pMdlChance->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
        }
    }
    if (resTexSrt.IsValid()) {
        bcs::CAnmTexSrt *pAnm = new (0) bcs::CAnmTexSrt;
        if (pAnm) {
            pAnm->Create(m_pMdlChance, resTexSrt, 0, 0);
            m_pMdlChance->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
        }
    }
    if (resTexPat.IsValid()) {
        bcs::CAnmTexPat *pAnm = new (0) bcs::CAnmTexPat;
        if (pAnm) {
            pAnm->Create(m_pMdlChance, resTexPat, 0, 0);
            m_pMdlChance->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_3);
        }
    }

    UpdateColor_();
    DtorStateSleep();
}

inline int msw::CBallTargetMarkerBoss::CheckAppear_()
{
    CBall *pBall = GetBall(m_nBallHandle);

    if (pBall == 0) {
        return 1;
    }
    if (pBall->IsSleep()) {
        return 1;
    }
    if (!pBall->IsActive()) {
        return 1;
    }
    if (!pBall->CanbeVsBossReceive()) {
        return 1;
    }
    return pBall->IsAttach() ? 3 : 2;
}

// The chance-ball mark shows only for a chance ball, and only while the
// ground mark does.
void msw::CBallTargetMarkerBoss::Main()
{
    if (m_pMdlBase != 0) {
        ExecState();
        SetHeightObjState_();
        UpdateViewFlag_();

        if (m_pMdlChance != 0) {
            float fAlpha = (m_bChance ? 1.0f : 0.0f) * m_pMdlChance->GetAlphaRateF();
            if (m_pMdlBase->GetAlphaRate() == 0) {
                fAlpha = 0.0f;
            }
            m_pMdlChance->SetAlphaRateF(fAlpha);
        }
    }
}

// Fades and scales the marker with the frames left before the ball comes
// down: hidden more than APPEAR_FRAME frames before the ball's display
// window, shrinking over the window, fading out once the ball is down.
void msw::CBallTargetMarkerBoss::SetHeightObjState_()
{
    static const float APPEAR_FRAME = 8.0f;
    CBall *pBall = GetBall(m_nBallHandle);
    float fFrame;

    if (pBall == 0) {
        fFrame = -1.0f;
    } else if (!pBall->IsActive()) {
        fFrame = -1.0f;
    } else if (pBall->CanbeVsBossReceive()) {
        fFrame = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
    } else {
        fFrame = -1.0f;
    }

    float fDispStart = 150.0f;
    float fDispEnd = 20.0f;
    m_bChance = 0;
    pBall = GetBall(m_nBallHandle);
    if (pBall != 0 && pBall->IsActive()) {
        pBall->GetVolleyBallMarkerDispFrame(fDispStart, fDispEnd);
        m_bChance = pBall->IsVolleyChanceBallMode();
        m_nSpikeType = 0;
    }

    if (fDispEnd >= fDispStart) {
        m_pMdlBase->SetAlphaRate(0);
    } else if (fFrame <= fDispEnd) {
        float fAlpha = m_pMdlBase->GetAlphaRateF();
        if (fAlpha > 0.0f) {
            fAlpha -= 0.0625f * (0.5f * GetFrameRateF());
            m_pMdlBase->SetAlphaRateF(0.0f < fAlpha ? fAlpha : 0.0f);
        }
    } else {
        float fLen = fDispStart - fDispEnd;
        float fRest = fFrame - fDispEnd;
        if (fRest >= APPEAR_FRAME + fLen) {
            m_pMdlBase->SetAlphaRate(0);
            if (m_pMdlChance != 0) {
                m_pMdlChance->SetAlphaRate(0);
            }
        } else if (fRest >= fLen) {
            float fRate = Clamp01((fRest - fLen) / APPEAR_FRAME);
            float fScale = 5.6f;
            m_pMdlBase->SetAlphaRateF((fRate * -1.0f + 1.0f) * m_fAppearRate);
            m_pMdlBase->m_vScale.Set(fScale, 1.0f, fScale);
            if (m_pMdlChance != 0) {
                float fChanceScale = 7.0f;
                m_pMdlChance->SetAlphaRateF((fRate * -0.6f + 0.6f) * m_fAppearRate);
                m_pMdlChance->m_vScale.Set(fChanceScale, 1.0f, fChanceScale);
            }
        } else {
            float fRate = Clamp01(1.0f - Clamp01(fRest / fLen));
            // Lerps written as (to - from): the original's constants are the
            // folded differences (0xBFCCCCCC, 0xBDCCCCD0), not -1.6f and -0.1f.
            float fScale = fRate * (4.0f - 5.6f) + 5.6f;
            m_pMdlBase->SetAlphaRateF((fRate * -0.5f + 1.0f) * m_fAppearRate);
            m_pMdlBase->m_vScale.Set(fScale, 1.0f, fScale);
            if (m_pMdlChance != 0) {
                float fChanceScale = fRate * -2.0f + 7.0f;
                m_pMdlChance->SetAlphaRateF((fRate * (0.5f - 0.6f) + 0.6f) * m_fAppearRate);
                m_pMdlChance->m_vScale.Set(fChanceScale, 1.0f, fChanceScale);
            }
        }
    }
}

// The boss game has one view: nothing to do.
void msw::CBallTargetMarkerBoss::UpdateViewFlag_()
{
}

void msw::CBallTargetMarkerBoss::CtorStateSleep()
{
    if (m_pGroup != 0) {
        m_pGroup->DetachFromParent();
    }
    if (m_pMdlChance != 0) {
        m_pMdlChance->DetachFromParent();
    }
    if (m_pMdlBase != 0) {
        m_pMdlBase->SetAlphaRate(0);
    }
    m_fAppearRate = 0.0f;
}

// Follows the ball and waits for it to come into play.
void msw::CBallTargetMarkerBoss::LoopStateSleep()
{
    if (m_pGroup != 0) {
        CBall *pBall = GetBall(m_nBallHandle);
        if (pBall != 0) {
            SetPosXZ(m_pGroup, pBall->GetPos().x, pBall->GetPos().z);
        }
    }
    if (CheckAppear_() == 2) {
        ChangeState(STATE_APPEAR);
    }
}

void msw::CBallTargetMarkerBoss::DtorStateSleep()
{
    if (m_pGroup != 0) {
        m_pGroup->DetachFromParent();
        bcs::CRenderMan::PushBack(m_pGroup, 5);
    }
    if (m_pMdlChance != 0) {
        m_pMdlChance->DetachFromParent();
        bcs::CRenderMan::PushBack(m_pMdlChance, 5);
    }
}

void msw::CBallTargetMarkerBoss::CtorStateAppear()
{
}

inline void msw::CBallTargetMarkerBoss::UpdateTargetPos_()
{
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);

    if (m_pGroup != 0) {
        pos = m_pGroup->m_vTrans;
    }
    CBall *pBall = GetBall(m_nBallHandle);
    if (pBall != 0 && pBall->IsActive()) {
        pos = pBall->GetTargetPos();
    }
    if (m_pGroup != 0) {
        SetPosXZ(m_pGroup, pos.x, pos.z);
    }
}

// Grows the marker to 4 times its size over 16 frames, along a quarter
// sine.
void msw::CBallTargetMarkerBoss::LoopStateAppear()
{
    switch (CheckAppear_()) {
        case 1:
            ChangeState(STATE_SLEEP);
            break;
        case 3:
            ChangeState(STATE_DISAPPEAR);
            break;
        default: {
            float fFrameRate = GetFrameRateF();
            UpdateTargetPos_();
            m_fAppearRate += 0.0625f * fFrameRate;
            m_fAppearRate = Clamp01(m_fAppearRate);
            float fScale = 4.0f * nw4r::math::SinRad(1.5707964f * m_fAppearRate);
            m_pMdlBase->m_vScale.Set(fScale, 1.0f, fScale);
            if (m_fAppearRate >= 1.0f) {
                ChangeState(STATE_MAIN);
            }
            break;
        }
    }
}

void msw::CBallTargetMarkerBoss::DtorStateAppear()
{
}

void msw::CBallTargetMarkerBoss::CtorStateMain()
{
}

void msw::CBallTargetMarkerBoss::LoopStateMain()
{
    switch (CheckAppear_()) {
        case 1:
            ChangeState(STATE_SLEEP);
            break;
        case 3:
            ChangeState(STATE_DISAPPEAR);
            break;
        default:
            GetFrameRateF(); // (the result is not used)
            UpdateTargetPos_();
            m_pMdlBase->m_vScale.Set(4.0f, 1.0f, 4.0f);
            break;
    }
}

void msw::CBallTargetMarkerBoss::DtorStateMain()
{
}

void msw::CBallTargetMarkerBoss::CtorStateDisappear()
{
}

// Brings m_fAppearRate down over 10 frames. (the scale is worked out as in
// LoopStateAppear but not set)
void msw::CBallTargetMarkerBoss::LoopStateDisappear()
{
    if (CheckAppear_() == 1) {
        ChangeState(STATE_SLEEP);
        return;
    }
    float fFrameRate = GetFrameRateF();
    UpdateTargetPos_();
    m_fAppearRate -= 0.1f * fFrameRate;
    m_fAppearRate = Clamp01(m_fAppearRate);
    float fScale = 4.0f * nw4r::math::SinRad(1.5707964f * m_fAppearRate);
    (void)fScale;
    if (m_fAppearRate <= 0.0f) {
        ChangeState(STATE_SLEEP);
    }
}

void msw::CBallTargetMarkerBoss::DtorStateDisappear()
{
}

void msw::CBallTargetMarkerBoss::UpdateColor_()
{
    if (m_pMdlBase != 0) {
        bcs::BCS_Color color0;
        bcs::BCS_Color color1;
        color0 = bcs::BCS_Color(0xA0, 0x00, 0x64, 0xFF);
        color1 = bcs::BCS_Color(0x50, 0x50, 0x50, 0x6E);
        m_pMdlBase->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlBase->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
    }
    if (m_pMdlChance != 0) {
        bcs::BCS_Color color0;
        bcs::BCS_Color color1;
        color0 = bcs::BCS_Color(0xFF, 0x42, 0xFF, 0xE0);
        color1 = bcs::BCS_Color(0xFB, 0xC0, 0xFF, 0xC0);
        m_pMdlChance->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlChance->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
    }
}

void msw::CBallTargetMarkerBoss::Disp()
{
    UpdateColor_();
}
