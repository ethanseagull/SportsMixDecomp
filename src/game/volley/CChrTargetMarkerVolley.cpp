// msw::CChrTargetMarkerVolley: the marker a volleyball character puts on the
// court where it aims (its spike or serve target), with four rings that
// close in on it while it comes up.
#include "game/msw/CChrTargetMarkerVolley.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/New.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrMarker.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/math/triangular.h"

namespace {
    // Model names: the ground mark, the rings, the star mark.
    const char *const s_pMdlBaseName = "mv_target_space01_01";
    const char *const s_pMdlRingName = "mv_target_marker01_01";
    const char *const s_pMdlStarName = "mv_star_space01_01";
    // The two materials the colours go to, and the rings' material.
    const char *const s_pMatName0 = "marker_color00";
    const char *const s_pMatName1 = "marker_color01";
    const char *const s_pRingMatName = "marker_color00";

    // Set by the file's static initialiser (see CBallManBasket.cpp for the
    // same kind of angle): how fast m_fPulse turns, per frame.
    float s_fPulseSpeed = bcs::DEG_TO_RAD(10.0f);

    // Turns pObj about the Y axis by fAdd.
    inline void AddRotY(bcs::CScnObj *pObj, float fAdd)
    {
        pObj->SetRotY(pObj->m_fRotY + fAdd);
    }

    inline float Clamp01(float x)
    {
        return x > 1.0f ? 1.0f : (x < 0.0f ? 0.0f : x);
    }
}

msw::CChrTargetMarkerVolley::CChrTargetMarkerVolley(bcs::CTask *pParent, long nChr, int bPlayer)
    : bcs::CTask(pParent, -1)
{
    m_nChr = nChr;
    m_bPadCtrl = 0;
    m_bStealth = 0;
    m_nFixedType = 0;
    m_pGroup = 0;
    m_pMdlBase = 0;
    m_pMdlStar = 0;
    m_pMdlBaseSub = 0;
    m_pMdlStarSub = 0;
    m_fAppearRate = 0.0f;
    m_fRingRate = 0.0f;
    m_fPulse = 1.5707964f;
    m_fSubRate = 0.0f;
    m_fWait = 0.0f;
    m_fWaitInit = 0.0f;
    for (int i = 0; i < 4; i++) {
        m_pMdlRing[i] = 0;
        m_pMdlRingSub[i] = 0;
    }

    InitState_();

    Create_();
}

msw::CChrTargetMarkerVolley::~CChrTargetMarkerVolley()
{
    Finalize_();
}

void msw::CChrTargetMarkerVolley::Finalize_()
{
    for (int i = 0; i < 4; i++) {
        if (m_pMdlRingSub[i] != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdlRingSub[i]);
        }
        m_pMdlRingSub[i] = 0;
        if (m_pMdlRing[i] != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdlRing[i]);
        }
        m_pMdlRing[i] = 0;
    }
    if (m_pMdlStarSub != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlStarSub);
    }
    m_pMdlStarSub = 0;
    if (m_pMdlBaseSub != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlBaseSub);
    }
    m_pMdlBaseSub = 0;
    if (m_pMdlStar != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlStar);
    }
    m_pMdlStar = 0;
    if (m_pMdlBase != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlBase);
    }
    m_pMdlBase = 0;
    if (m_pGroup != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pGroup);
    }
    m_pGroup = 0;
}

// Defined after the destructor and inlined into the constructor: the
// original emits CStateTemplate's constructor after Finalize_ and keeps a
// second, unread copy of the state table's member pointers (the out-of-line
// copy, stripped by the link), as CChr1on1MarkerBase (main CChrMarker.cpp).
void msw::CChrTargetMarkerVolley::InitState_()
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_SLEEP, &CChrTargetMarkerVolley::CtorStateSleep, &CChrTargetMarkerVolley::LoopStateSleep,
             &CChrTargetMarkerVolley::DtorStateSleep);
    SetState(STATE_APPEAR, &CChrTargetMarkerVolley::CtorStateAppear, &CChrTargetMarkerVolley::LoopStateAppear,
             &CChrTargetMarkerVolley::DtorStateAppear);
    SetState(STATE_MAIN, &CChrTargetMarkerVolley::CtorStateMain, &CChrTargetMarkerVolley::LoopStateMain,
             &CChrTargetMarkerVolley::DtorStateMain);
    SetState(STATE_DISAPPEAR, &CChrTargetMarkerVolley::CtorStateDisappear,
             &CChrTargetMarkerVolley::LoopStateDisappear, &CChrTargetMarkerVolley::DtorStateDisappear);
    ChangeState(STATE_SLEEP);
}

// The models of the marker, all in m_pGroup or attached to a model in it,
// drawn without depth test and hidden until the marker comes up. The main
// models are seen in the views of the other side of the net, the sub models
// in those of the character's own side (view masks 5 and 10).
void msw::CChrTargetMarkerVolley::Create_()
{
    nw4r::g3d::ResFile file = CChrMarker::GetRsrcPack();
    nw4r::g3d::ResMdl mdlBase = file.GetResMdl(s_pMdlBaseName);
    nw4r::g3d::ResMdl mdlStar = file.GetResMdl(s_pMdlStarName);

    unsigned short nView = 5;
    if (m_nChr & 1) {
        nView = 10;
    }
    if (CGame::g_GameWork.m_nUnk00 != 0) {
        nView = 10;
        if (m_nChr & 1) {
            nView = 5;
        }
    }

    m_pGroup = new (0) bcs::CScnGroup(0x10, 0);
    m_pGroup->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pGroup->OnFlag(0x4000);

    m_pMdlBase = new (0) bcs::CMdlObj;
    m_pMdlBase->Create(mdlBase, 0, 0);
    m_pMdlBase->OnFlag(0x4000);
    m_pMdlBase->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlBase->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlBase->SetDrawPriority(4, 4);
    m_pMdlBase->SetViewFlag(~nView);
    m_pMdlBase->SetAlphaRate(0);
    m_pGroup->AppendChild(m_pMdlBase);

    m_pMdlStar = new (0) bcs::CMdlObj;
    m_pMdlStar->Create(mdlStar, 0, 0);
    m_pMdlStar->OnFlag(0x4000);
    m_pMdlStar->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlStar->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlStar->SetDrawPriority(4, 4);
    m_pMdlStar->SetViewFlag(~nView);
    m_pMdlStar->SetAlphaRate(0);
    m_pGroup->AppendChild(m_pMdlStar);

    m_pMdlBaseSub = new (0) bcs::CMdlObj;
    m_pMdlBaseSub->Create(mdlBase, 0, 0);
    m_pMdlBaseSub->OnFlag(0x4000);
    m_pMdlBaseSub->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlBaseSub->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlBaseSub->SetDrawPriority(1, 1);
    m_pMdlBaseSub->AttachToObject(m_pMdlBase, 0, bcs::CScnObj::ATTACHMODE_0);
    m_pMdlBaseSub->SetViewFlag(nView);
    m_pMdlBaseSub->SetAlphaRate(0);

    m_pMdlStarSub = new (0) bcs::CMdlObj;
    m_pMdlStarSub->Create(mdlStar, 0, 0);
    m_pMdlStarSub->OnFlag(0x4000);
    m_pMdlStarSub->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlStarSub->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlStarSub->SetDrawPriority(1, 1);
    m_pMdlStarSub->AttachToObject(m_pMdlStar, 0, bcs::CScnObj::ATTACHMODE_0);
    m_pMdlStarSub->SetViewFlag(nView);
    m_pMdlStarSub->SetAlphaRate(0);

    // Four rings, a quarter turn apart.
    nw4r::g3d::ResMdl mdl = file.GetResMdl(s_pMdlRingName);
    for (int i = 0; i < 4; i++) {
        m_pMdlRing[i] = new (0) bcs::CMdlObj;
        m_pMdlRing[i]->Create(mdl, 0, 0);
        m_pMdlRing[i]->OnFlag(0x4000);
        m_pMdlRing[i]->m_vTrans.Set(0.0f, 0.0f, 0.0f);
        m_pMdlRing[i]->MatSetZModeAll(1, GX_ALWAYS, 0);
        m_pMdlRing[i]->SetDrawPriority(5, 5);
        m_pMdlRing[i]->SetViewFlag(~nView);
        m_pMdlRing[i]->SetAlphaRate(0);
        m_pGroup->AppendChild(m_pMdlRing[i]);

        float fRot = bcs::ROUND_PI(bcs::DEG_TO_RAD((float)(i * 90)));
        m_pMdlRing[i]->SetRotY(fRot);
    }

    mdl = file.GetResMdl(s_pMdlRingName);
    for (int i = 0; i < 4; i++) {
        m_pMdlRingSub[i] = new (0) bcs::CMdlObj;
        m_pMdlRingSub[i]->Create(mdl, 0, 0);
        m_pMdlRingSub[i]->OnFlag(0x4000);
        m_pMdlRingSub[i]->m_vTrans.Set(0.0f, 0.0f, 0.0f);
        m_pMdlRingSub[i]->MatSetZModeAll(1, GX_ALWAYS, 0);
        m_pMdlRingSub[i]->SetDrawPriority(5, 5);
        m_pMdlRingSub[i]->AttachToObject(m_pMdlRing[i], 0, bcs::CScnObj::ATTACHMODE_0);
        m_pMdlRingSub[i]->SetViewFlag(nView);
        m_pMdlRingSub[i]->SetAlphaRate(0);
    }

    m_fSubRate = 0.0f;
    m_fWait = 0.0f;
    m_fWaitInit = 0.0f;
    PushBack_(1);
}

void msw::CChrTargetMarkerVolley::PushBack_(int bOn)
{
    if (bOn) {
        if (m_pGroup) {
            m_pGroup->DetachFromParent();
            bcs::CRenderMan::PushBack(m_pGroup, 5);
        }
        if (m_pMdlBaseSub) {
            m_pMdlBaseSub->DetachFromParent();
            bcs::CRenderMan::PushBack(m_pMdlBaseSub, 5);
        }
        if (m_pMdlStarSub) {
            m_pMdlStarSub->DetachFromParent();
            bcs::CRenderMan::PushBack(m_pMdlStarSub, 5);
        }
        for (int i = 0; i < 4; i++) {
            if (m_pMdlRingSub[i]) {
                m_pMdlRingSub[i]->DetachFromParent();
                bcs::CRenderMan::PushBack(m_pMdlRingSub[i], 5);
            }
        }
        UpdateColor_();
    } else {
        if (m_pGroup) {
            m_pGroup->DetachFromParent();
        }
        if (m_pMdlBaseSub) {
            m_pMdlBaseSub->DetachFromParent();
        }
        if (m_pMdlStarSub) {
            m_pMdlStarSub->DetachFromParent();
        }
        for (int i = 0; i < 4; i++) {
            if (m_pMdlRingSub[i]) {
                m_pMdlRingSub[i]->DetachFromParent();
            }
        }
    }
}

int msw::CChrTargetMarkerVolley::CheckAppear_()
{
    m_bPadCtrl = 0;
    CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0 && pChr->IsEnable() && !(pChr->GetFlagEC() & 0x0A000000)) {
        if (pChr->IsPadCtrl()) {
            m_bPadCtrl = 1;
        }
        if (pChr->IsVolleyTargetMarkerActive()) {
            m_nFixedType = pChr->GetVolleyTargetMarkerFixedType();
            m_bStealth = pChr->IsVolleyTargetMarkerStealthMode();
            switch (pChr->GetVolleyTargetMarkerType()) {
                case 0:
                case 1:
                case 2:
                    m_fWaitInit = 0.0f;
                    break;
                default:
                    m_fWaitInit = 0.0f;
                    break;
            }
            return 2;
        }
    }
    return 3;
}

void msw::CChrTargetMarkerVolley::GetTargetPos_(bcs::FVEC3 &rPos)
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    if (m_pGroup != 0) {
        rPos = m_pGroup->m_vTrans;
    }
    CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
    if (pChr != 0 && pChr->IsEnable()) {
        pChr->GetVolleyTargetMarkerPos(rPos, 0);
    }
}

void msw::CChrTargetMarkerVolley::MoveToTargetPos_(const bcs::FVEC3 &rPos, float fFrameRate)
{
    if (m_pGroup != 0) {
        m_pGroup->m_vTrans = rPos;
    }
}

void msw::CChrTargetMarkerVolley::AllDispOff_()
{
    if (m_pMdlBase != 0) {
        m_pMdlBase->SetAlphaRate(0);
    }
    if (m_pMdlStar != 0) {
        m_pMdlStar->SetAlphaRate(0);
    }
    if (m_pMdlBaseSub != 0) {
        m_pMdlBaseSub->SetAlphaRate(0);
    }
    if (m_pMdlStarSub != 0) {
        m_pMdlStarSub->SetAlphaRate(0);
    }
    for (int i = 0; i < 4; i++) {
        if (m_pMdlRing[i] != 0) {
            m_pMdlRing[i]->SetAlphaRate(0);
        }
        if (m_pMdlRingSub[i] != 0) {
            m_pMdlRingSub[i]->SetAlphaRate(0);
        }
    }
}

// Hides the models the character's marker type does not use; type 3 turns
// the star.
void msw::CChrTargetMarkerVolley::Main()
{
    if (m_pMdlBase != 0) {
        ExecState();

        switch (m_nFixedType) {
            case 4:
                if (m_pMdlBase != 0) {
                    m_pMdlBase->SetAlphaRate(0);
                }
                if (m_pMdlBaseSub != 0) {
                    m_pMdlBaseSub->SetAlphaRate(0);
                }
                if (m_pMdlStar != 0) {
                    m_pMdlStar->SetAlphaRate(0);
                }
                if (m_pMdlStarSub != 0) {
                    m_pMdlStarSub->SetAlphaRate(0);
                }
                for (int i = 0; i < 4; i++) {
                    if (m_pMdlRing[i] != 0) {
                        m_pMdlRing[i]->SetAlphaRate(0);
                    }
                    if (m_pMdlRingSub[i] != 0) {
                        m_pMdlRingSub[i]->SetAlphaRate(0);
                    }
                }
                break;
            case 2:
                if (m_pMdlStar != 0) {
                    m_pMdlStar->SetAlphaRate(0);
                }
                if (m_pMdlStarSub != 0) {
                    m_pMdlStarSub->SetAlphaRate(0);
                }
                for (int i = 0; i < 4; i++) {
                    if (m_pMdlRing[i] != 0 && !(i & 1)) {
                        m_pMdlRing[i]->SetAlphaRate(0);
                    }
                    if (m_pMdlRingSub[i] != 0 && !(i & 1)) {
                        m_pMdlRingSub[i]->SetAlphaRate(0);
                    }
                }
                break;
            case 3:
                if (m_pMdlStar != 0) {
                    AddRotY(m_pMdlStar, 0.06981317f * GetFrameRateF());
                }
                if (m_pMdlStarSub != 0) {
                    AddRotY(m_pMdlStarSub, 0.06981317f * GetFrameRateF());
                }
                if (m_pMdlBase != 0) {
                    m_pMdlBase->SetAlphaRate(0);
                }
                if (m_pMdlBaseSub != 0) {
                    m_pMdlBaseSub->SetAlphaRate(0);
                }
                for (int i = 0; i < 4; i++) {
                    if (m_pMdlRing[i] != 0) {
                        m_pMdlRing[i]->SetAlphaRate(0);
                    }
                    if (m_pMdlRingSub[i] != 0) {
                        m_pMdlRingSub[i]->SetAlphaRate(0);
                    }
                }
                break;
            default:
                if (m_pMdlStar != 0) {
                    m_pMdlStar->SetAlphaRate(0);
                }
                if (m_pMdlStarSub != 0) {
                    m_pMdlStarSub->SetAlphaRate(0);
                }
                for (int i = 0; i < 4; i++) {
                    if (m_pMdlRingSub[i] != 0) {
                        m_pMdlRingSub[i]->SetAlphaRate(0);
                    }
                }
                break;
        }

        // The sub models are not shown in stealth mode, nor when the
        // screen is not split by sides.
        int bHideSub = m_bStealth;
        if (CGame::g_GameWork.m_nUnk1C8 & 2) {
            bHideSub = 1;
        }
        if (CGame::g_GameWork.m_nUnk40 & 0x40) {
            bHideSub = 1;
        }
        if (bHideSub) {
            if (m_pMdlBaseSub != 0) {
                m_pMdlBaseSub->SetAlphaRate(0);
            }
            if (m_pMdlStarSub != 0) {
                m_pMdlStarSub->SetAlphaRate(0);
            }
            for (int i = 0; i < 4; i++) {
                if (m_pMdlRingSub[i] != 0) {
                    m_pMdlRingSub[i]->SetAlphaRate(0);
                }
            }
        }
    }
}

void msw::CChrTargetMarkerVolley::CtorStateSleep()
{
    AllDispOff_();
    PushBack_(0);
    m_fAppearRate = 0.0f;
    m_fRingRate = 0.0f;
    m_fSubRate = 0.0f;
    m_fWait = 0.0f;
    m_fWaitInit = 0.0f;
    m_fPulse = 1.5707964f;
}

void msw::CChrTargetMarkerVolley::LoopStateSleep()
{
    if (CheckAppear_() == 2) {
        ChangeState(STATE_APPEAR);
    }
}

void msw::CChrTargetMarkerVolley::DtorStateSleep()
{
    PushBack_(1);
}

void msw::CChrTargetMarkerVolley::CtorStateAppear()
{
    m_fWait = m_fWaitInit;
}

// Fades the marker in and closes the rings in on it.
void msw::CChrTargetMarkerVolley::LoopStateAppear()
{
    float fRingAlpha;
    switch (CheckAppear_()) {
        case 1:
            ChangeState(STATE_SLEEP);
            break;
        case 3:
            ChangeState(STATE_DISAPPEAR);
            break;
        default: {
            float fFrameRate = GetFrameRateF();
            bcs::FVEC3 pos;
            GetTargetPos_(pos);
            MoveToTargetPos_(pos, fFrameRate);

            if (m_fWait > 0.0f) {
                m_fWait -= fFrameRate;
            } else {
                m_fSubRate += 0.0625f * fFrameRate;
                m_fSubRate = Clamp01(m_fSubRate);
            }

            m_fAppearRate += 0.0625f * fFrameRate;
            m_fAppearRate = Clamp01(m_fAppearRate);
            float fRate = nw4r::math::SinRad(1.5707964f * m_fAppearRate);
            m_pMdlBase->SetAlphaRateF(fRate);
            m_pMdlBase->m_vScale.Set(5.0f * fRate, 1.0f, 5.0f * fRate);
            m_pMdlStar->SetAlphaRateF(0.9f * fRate);
            m_pMdlStar->m_vScale.Set(4.5f * fRate, 1.0f, 4.5f * fRate);

            m_fRingRate += 0.2f * GetFrameRateF();
            m_fRingRate = Clamp01(m_fRingRate);
            fRingAlpha = nw4r::math::SinRad(1.5707964f * m_fAppearRate);
            for (int i = 0; i < 4; i++) {
                if (m_pMdlRing[i] != 0) {
                    m_pMdlRing[i]->SetAlphaRateF(fRingAlpha);
                    bcs::FVEC3 ringPos;
                    bcs::BCS_VecZ_RotY(ringPos, 1.8f + (1.0f - m_fRingRate), m_pMdlRing[i]->m_fRotY);
                    m_pMdlRing[i]->m_vTrans = ringPos;
                }
            }

            float fSubAlpha = nw4r::math::SinRad(1.5707964f * m_fSubRate);
            m_pMdlBaseSub->SetAlphaRateF(fSubAlpha);
            m_pMdlStarSub->SetAlphaRateF(0.9f * fSubAlpha);
            float fRingSubAlpha = fSubAlpha * nw4r::math::SinRad(1.5707964f * m_fAppearRate);
            for (int i = 0; i < 4; i++) {
                if (m_pMdlRingSub[i] != 0) {
                    m_pMdlRingSub[i]->SetAlphaRateF(fRingSubAlpha);
                }
            }

            if (m_fAppearRate >= 1.0f) {
                m_fRingRate = 1.0f;
                ChangeState(STATE_MAIN);
            }
            break;
        }
    }
}

void msw::CChrTargetMarkerVolley::DtorStateAppear()
{
}

void msw::CChrTargetMarkerVolley::CtorStateMain()
{
}

// Follows the target, the marker pulsing in size.
void msw::CChrTargetMarkerVolley::LoopStateMain()
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
            if (m_fWait > 0.0f) {
                m_fWait -= fFrameRate;
            } else {
                m_fSubRate += 0.0625f * fFrameRate;
                m_fSubRate = Clamp01(m_fSubRate);
            }

            bcs::FVEC3 pos;
            GetTargetPos_(pos);
            MoveToTargetPos_(pos, fFrameRate);

            m_fPulse += fFrameRate * s_fPulseSpeed;
            m_fPulse = bcs::ROUND_PI(m_fPulse);
            float fScale = 0.1f * nw4r::math::SinRad(m_fPulse) + 0.9f;
            m_pMdlBase->SetAlphaRateF(1.0f);
            m_pMdlBase->m_vScale.Set(5.0f * fScale, 1.0f, 5.0f * fScale);
            m_pMdlStar->SetAlphaRateF(0.9f);
            m_pMdlStar->m_vScale.Set(4.5f * fScale, 1.0f, 4.5f * fScale);
            for (int i = 0; i < 4; i++) {
                if (m_pMdlRing[i] != 0) {
                    float fRot = m_pMdlRing[i]->m_fRotY;
                    bcs::FVEC3 ringPos;
                    bcs::BCS_VecZ_RotY(ringPos, 1.8f, fRot);
                    m_pMdlRing[i]->m_vTrans = ringPos;
                    m_pMdlRing[i]->SetAlphaRateF(1.0f);
                }
            }

            float fSubAlpha = nw4r::math::SinRad(1.5707964f * m_fSubRate);
            m_pMdlBaseSub->SetAlphaRateF(fSubAlpha);
            m_pMdlStarSub->SetAlphaRateF(0.9f * fSubAlpha);
            for (int i = 0; i < 4; i++) {
                if (m_pMdlRingSub[i] != 0) {
                    m_pMdlRingSub[i]->SetAlphaRateF(fSubAlpha);
                }
            }
            break;
        }
    }
}

void msw::CChrTargetMarkerVolley::DtorStateMain()
{
}

void msw::CChrTargetMarkerVolley::CtorStateDisappear()
{
}

// Fades the marker out and opens the rings again.
void msw::CChrTargetMarkerVolley::LoopStateDisappear()
{
    if (CheckAppear_() == 1) {
        ChangeState(STATE_SLEEP);
        return;
    }

    float fFrameRate = GetFrameRateF();
    bcs::FVEC3 pos;
    GetTargetPos_(pos);
    m_pGroup->m_vTrans = pos;

    m_fAppearRate -= 0.1f * fFrameRate;
    m_fAppearRate = Clamp01(m_fAppearRate);
    float fRate = nw4r::math::SinRad(1.5707964f * m_fAppearRate);
    m_pMdlBase->SetAlphaRateF(fRate);
    m_pMdlBase->m_vScale.Set(5.0f * fRate, 1.0f, 5.0f * fRate);
    m_pMdlStar->SetAlphaRateF(0.9f * fRate);
    m_pMdlStar->m_vScale.Set(4.5f * fRate, 1.0f, 4.5f * fRate);

    m_fRingRate -= 0.2f * fFrameRate;
    m_fRingRate = Clamp01(m_fRingRate);
    float fRingAlpha = nw4r::math::SinRad(1.5707964f * m_fRingRate);
    for (int i = 0; i < 4; i++) {
        if (m_pMdlRing[i] != 0) {
            m_pMdlRing[i]->SetAlphaRateF(fRingAlpha);
            bcs::FVEC3 ringPos;
            bcs::BCS_VecZ_RotY(ringPos, 1.8f, m_pMdlRing[i]->m_fRotY);
            m_pMdlRing[i]->m_vTrans = ringPos;
        }
    }

    m_fSubRate -= 0.1f * fFrameRate;
    m_fSubRate = Clamp01(m_fSubRate);
    float fSubAlpha = nw4r::math::SinRad(1.5707964f * m_fSubRate);
    m_pMdlBaseSub->SetAlphaRateF(fSubAlpha);
    m_pMdlStarSub->SetAlphaRateF(0.9f * fSubAlpha);
    float fRingSubAlpha = fSubAlpha * nw4r::math::SinRad(1.5707964f * m_fRingRate);
    for (int i = 0; i < 4; i++) {
        if (m_pMdlRingSub[i] != 0) {
            m_pMdlRingSub[i]->SetAlphaRateF(fRingSubAlpha);
        }
    }

    if (m_fAppearRate <= 0.0f) {
        m_fRingRate = 0.0f;
        ChangeState(STATE_SLEEP);
    }
}

void msw::CChrTargetMarkerVolley::DtorStateDisappear()
{
}

// The marker's colours, by the character's marker type; the rings show
// only for a pad-driven character.
void msw::CChrTargetMarkerVolley::UpdateColor_()
{
    if (m_pMdlBase != 0) {
        bcs::BCS_Color color0;
        bcs::BCS_Color color1;
        switch (m_nFixedType) {
            case 2:
                color0 = bcs::BCS_Color(0x00, 0x00, 0xFF, 0xFF);
                color1 = bcs::BCS_Color(0x00, 0x0A, 0x00, 0xB4);
                break;
            case 1:
                color0 = bcs::BCS_Color(0x00, 0x00, 0xFF, 0xFF);
                color1 = bcs::BCS_Color(0x00, 0x0A, 0x00, 0xB4);
                break;
            case 3:
                color0 = bcs::BCS_Color(0x46, 0xFF, 0x00, 0x78);
                color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
                break;
            default:
                color0 = bcs::BCS_Color(0x00, 0x00, 0xFF, 0xFF);
                color1 = bcs::BCS_Color(0x00, 0x0A, 0x00, 0xB4);
                break;
        }
        m_pMdlBase->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlBase->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
        m_pMdlStar->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlStar->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
    }

    if (m_pMdlBaseSub != 0) {
        bcs::BCS_Color color0;
        bcs::BCS_Color color1;
        switch (m_nFixedType) {
            case 2:
                color0 = bcs::BCS_Color(0xA0, 0x00, 0x64, 0xFF);
                color1 = bcs::BCS_Color(0x46, 0x46, 0x46, 0x64);
                break;
            case 1:
                color0 = bcs::BCS_Color(0xA0, 0x00, 0x64, 0xFF);
                color1 = bcs::BCS_Color(0x46, 0x46, 0x46, 0x64);
                break;
            case 3:
                color0 = bcs::BCS_Color(0x46, 0xFF, 0x00, 0x78);
                color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
                break;
            default:
                color0 = bcs::BCS_Color(0xA0, 0x00, 0x64, 0xFF);
                color1 = bcs::BCS_Color(0x46, 0x46, 0x46, 0x64);
                break;
        }
        m_pMdlBaseSub->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlBaseSub->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
        m_pMdlStarSub->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlStarSub->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
    }

    bcs::BCS_Color ringColor(0x3D, 0x00, 0xA7, m_bPadCtrl ? 0xFF : 0x00);
    switch (m_nFixedType) {
        case 2:
            ringColor = bcs::BCS_Color(0x3D, 0x00, 0xA7, m_bPadCtrl ? 0xFF : 0x00);
            break;
        case 1:
            ringColor = bcs::BCS_Color(0x3D, 0x00, 0xA7, m_bPadCtrl ? 0xFF : 0x00);
            break;
    }
    for (int i = 0; i < 4; i++) {
        if (m_pMdlRing[i] != 0) {
            if (ringColor.a != 0) {
                m_pMdlRing[i]->SetMatTevColor(s_pRingMatName, GX_TEVREG0, ringColor);
            } else {
                m_pMdlRing[i]->SetAlphaRate(0);
            }
        }
    }

    bcs::BCS_Color ringSubColor(0x00, 0x00, 0x00, 0x00);
    switch (m_nFixedType) {
        case 2:
            ringSubColor = bcs::BCS_Color(0xFF, 0x00, 0x00, 0xFF);
            break;
        case 1:
            ringSubColor = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
            break;
    }
    for (int i = 0; i < 4; i++) {
        if (m_pMdlRingSub[i] != 0) {
            if (ringSubColor.a != 0) {
                m_pMdlRingSub[i]->SetMatTevColor(s_pRingMatName, GX_TEVREG0, ringSubColor);
            } else {
                m_pMdlRingSub[i]->SetAlphaRate(0);
            }
        }
    }
}

void msw::CChrTargetMarkerVolley::Disp()
{
    UpdateColor_();
}
