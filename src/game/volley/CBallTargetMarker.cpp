// The ball markers of the volley game: msw::CBallTargetMarker (the marker on
// the court where the ball will come down), its vs-boss and rhythm-receive
// variants, and msw::CBallMarkerVolley (the marker on the ball).
#include "game/msw/CBallTargetMarker.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMarkerVolley.hpp"
#include "game/msw/CChrMarker.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CItemBall.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/math/triangular.h"

// Radians in one degree.
static const float scDegToRad = 0.017453292f;

namespace {
    // Model names, by role: [0] the ground mark and the item mark, [1] the
    // copy on the ground mark, [2] the chance-ball mark, [3] the rings.
    const char *s_MdlName[] = {
        "mv_target_space01_01", "mv_target_space01_01", "mv_chance_target01_01", "mv_target_marker01_01"
    };
    // The two materials the colours go to, the rings' material, and the
    // material of CBallTargetMarkerRhythmReceive's team mark.
    const char *s_pMatName0 = "marker_color00";
    const char *s_pMatName1 = "marker_color01";
    const char *s_pRingMatName = "marker_color00";
    const char *s_pMarkMatName = "marker_color00";
    // CBallTargetMarkerRhythmReceive: the mark of each team.
    const char *s_MarkMdlName[] = { "mc_mark_1", "mc_mark_2", "mc_mark_3", "mc_mark_4" };

    template <class T>
    inline void DeleteScnObj(T *&pObj)
    {
        if (pObj) {
            bcs::CScnObj::AppendDelayDeleteObject(pObj);
        }
        pObj = 0;
    }

    // Sets or clears the two hide bits of a model.
    inline void SetHide(bcs::CScnObj *pObj, int bHide)
    {
        if (bHide) {
            pObj->OnFlag(0xC0);
        } else {
            pObj->OffFlag(0xC0);
        }
    }

    // Moves pObj over the court, keeping its height.
    inline void SetPosXZ(bcs::CScnObj *pObj, float x, float z)
    {
        pObj->m_vTrans.x = x;
        pObj->m_vTrans.z = z;
    }

    // Stand-in for an unknown user of -1 and 0.23, which the original keeps
    // in .rodata before Main's first constant; 0.23 has no reader in the
    // unit (likely stripped code). See notes/decisions.md, "Unreferenced data
    // kept by dead references".
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

    inline msw::CItem *GetItem(unsigned long nHandle)
    {
        return static_cast<msw::CItem *>(bcs::CTaskMan::GetTask(nHandle));
    }
}

msw::CBallTargetMarker::CBallTargetMarker(bcs::CTask *pParent, unsigned long nBallHandle, int bRing)
    : bcs::CTask(pParent, 0x19)
{
    m_pGroup = 0;
    m_pMdlBase = 0;
    m_pMdlBaseSub = 0;
    m_pMdlChance = 0;
    m_pMdlItem = 0;
    m_pMdlItemSub = 0;
    m_nBallHandle = nBallHandle;
    m_fAppearRate = 0.0f;
    m_bChance = 0;
    m_nSpikeType = 0;
    for (int i = 0; i < 2; i++) {
        m_pMdlRing[i] = 0;
        m_pMdlRingSub[i] = 0;
    }

    InitFsm(this, STATE_NUM);
    SetState(STATE_SLEEP, &CBallTargetMarker::CtorStateSleep, &CBallTargetMarker::LoopStateSleep,
             &CBallTargetMarker::DtorStateSleep);
    SetState(STATE_APPEAR, &CBallTargetMarker::CtorStateAppear, &CBallTargetMarker::LoopStateAppear,
             &CBallTargetMarker::DtorStateAppear);
    SetState(STATE_MAIN, &CBallTargetMarker::CtorStateMain, &CBallTargetMarker::LoopStateMain,
             &CBallTargetMarker::DtorStateMain);
    SetState(STATE_DISAPPEAR, &CBallTargetMarker::CtorStateDisappear, &CBallTargetMarker::LoopStateDisappear,
             &CBallTargetMarker::DtorStateDisappear);
    ChangeState(STATE_SLEEP);

    Create_(bRing);
}

msw::CBallTargetMarker::~CBallTargetMarker()
{
    Finalize_();
}

void msw::CBallTargetMarker::Finalize_()
{
    for (int i = 0; i < 2; i++) {
        if (m_pMdlRingSub[i] != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdlRingSub[i]);
        }
        m_pMdlRingSub[i] = 0;
        if (m_pMdlRing[i] != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMdlRing[i]);
        }
        m_pMdlRing[i] = 0;
    }
    if (m_pMdlItemSub != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlItemSub);
    }
    m_pMdlItemSub = 0;
    if (m_pMdlItem != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlItem);
    }
    m_pMdlItem = 0;
    if (m_pMdlChance != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlChance);
    }
    m_pMdlChance = 0;
    if (m_pMdlBaseSub != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlBaseSub);
    }
    m_pMdlBaseSub = 0;
    if (m_pMdlBase != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pMdlBase);
    }
    m_pMdlBase = 0;
    if (m_pGroup != 0) {
        bcs::CScnObj::AppendDelayDeleteObject(m_pGroup);
    }
    m_pGroup = 0;
}

// The models of the marker, all in m_pGroup or attached to a model in it,
// drawn without depth test and hidden until the marker comes up.
void msw::CBallTargetMarker::Create_(int bRing)
{
    nw4r::g3d::ResFile file = CChrMarker::GetRsrcPack();
    GetBall(m_nBallHandle); // (the result is not used)

    m_pGroup = new (0) bcs::CScnGroup(0x10, 0);
    m_pGroup->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pGroup->OnFlag(0x4000);

    nw4r::g3d::ResMdl mdl = file.GetResMdl(s_MdlName[0]);
    m_pMdlBase = new (0) bcs::CMdlObj;
    m_pMdlBase->Create(mdl, 0, 0);
    m_pMdlBase->OnFlag(0x4000);
    m_pMdlBase->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlBase->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlBase->SetDrawPriority(0, 0);
    m_pMdlBase->SetAlphaRate(0);
    m_pGroup->AppendChild(m_pMdlBase);

    mdl = file.GetResMdl(s_MdlName[1]);
    m_pMdlBaseSub = new (0) bcs::CMdlObj;
    m_pMdlBaseSub->Create(mdl, 0, 0);
    m_pMdlBaseSub->OnFlag(0x4000);
    m_pMdlBaseSub->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlBaseSub->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlBaseSub->SetDrawPriority(0, 0);
    m_pMdlBaseSub->AttachToObject(m_pMdlBase, 0, bcs::CScnObj::ATTACHMODE_0);
    m_pMdlBaseSub->SetAlphaRate(0);

    mdl = file.GetResMdl(s_MdlName[2]);
    m_pMdlChance = new (0) bcs::CMdlObj;
    nw4r::g3d::ResAnmClr resClr;
    nw4r::g3d::ResAnmTexSrt resTexSrt;
    nw4r::g3d::ResAnmTexPat resTexPat;
    unsigned long anmFlag = 0;
    if (file.HasResAnmClr()) {
        resClr = file.GetResAnmClr(s_MdlName[2]);
        if (resClr.IsValid()) {
            anmFlag |= 0x2;
        }
    }
    if (file.HasResAnmTexSrt()) {
        resTexSrt = file.GetResAnmTexSrt(s_MdlName[2]);
        if (resTexSrt.IsValid()) {
            anmFlag |= 0x204;
        }
    }
    if (file.HasResAnmTexPat()) {
        resTexPat = file.GetResAnmTexPat(s_MdlName[2]);
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

    mdl = file.GetResMdl(s_MdlName[0]);
    m_pMdlItem = new (0) bcs::CMdlObj;
    m_pMdlItem->Create(mdl, 0, 0);
    m_pMdlItem->OnFlag(0x4000);
    m_pMdlItem->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlItem->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlItem->SetDrawPriority(-1, -1);
    m_pMdlItem->OnFlag(0xC0);
    m_pMdlItem->SetAlphaRate(0);
    m_pGroup->AppendChild(m_pMdlItem);

    mdl = file.GetResMdl(s_MdlName[0]);
    m_pMdlItemSub = new (0) bcs::CMdlObj;
    m_pMdlItemSub->Create(mdl, 0, 0);
    m_pMdlItemSub->OnFlag(0x4000);
    m_pMdlItemSub->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlItemSub->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlItemSub->SetDrawPriority(-1, -1);
    m_pMdlItemSub->AttachToObject(m_pMdlItem, 0, bcs::CScnObj::ATTACHMODE_0);
    m_pMdlItemSub->SetAlphaRate(0);

    if (bRing) {
        // Two rings on opposite sides, 1.8 from the centre.
        mdl = file.GetResMdl(s_MdlName[3]);
        for (int i = 0; i < 2; i++) {
            m_pMdlRing[i] = new (0) bcs::CMdlObj;
            m_pMdlRing[i]->Create(mdl, 0, 0);
            m_pMdlRing[i]->OnFlag(0x4000);
            m_pMdlRing[i]->m_vTrans.Set(0.0f, 0.0f, 0.0f);
            m_pMdlRing[i]->MatSetZModeAll(1, GX_ALWAYS, 0);
            m_pMdlRing[i]->SetDrawPriority(1, 1);
            m_pMdlRing[i]->SetAlphaRate(0);
            m_pGroup->AppendChild(m_pMdlRing[i]);

            float fOfs = scDegToRad * (float)(i * 180);
            float fRot = 1.5707964f + fOfs;
            while (fRot > 6.2831855f) {
                fRot -= 6.2831855f;
            }
            while (fRot < 0.0f) {
                fRot += 6.2831855f;
            }
            m_pMdlRing[i]->SetRotY(fRot);
            bcs::FVEC3 pos;
            bcs::BCS_VecZ_RotY(pos, 1.8f, fRot);
            m_pMdlRing[i]->m_vTrans = pos;
        }

        mdl = file.GetResMdl(s_MdlName[3]);
        for (int i = 0; i < 2; i++) {
            m_pMdlRingSub[i] = new (0) bcs::CMdlObj;
            m_pMdlRingSub[i]->Create(mdl, 0, 0);
            m_pMdlRingSub[i]->OnFlag(0x4000);
            m_pMdlRingSub[i]->m_vTrans.Set(0.0f, 0.0f, 0.0f);
            m_pMdlRingSub[i]->MatSetZModeAll(1, GX_ALWAYS, 0);
            m_pMdlRingSub[i]->SetDrawPriority(1, 1);
            m_pMdlRingSub[i]->AttachToObject(m_pMdlRing[i], 0, bcs::CScnObj::ATTACHMODE_0);
            m_pMdlRingSub[i]->SetAlphaRate(0);
        }
    }

    UpdateColor_();
    PushBack_(1);
}

void msw::CBallTargetMarker::PushBack_(int bOn)
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
        if (m_pMdlChance) {
            m_pMdlChance->DetachFromParent();
            bcs::CRenderMan::PushBack(m_pMdlChance, 5);
        }
        if (m_pMdlItemSub) {
            m_pMdlItemSub->DetachFromParent();
            bcs::CRenderMan::PushBack(m_pMdlItemSub, 5);
        }
        for (int i = 0; i < 2; i++) {
            if (m_pMdlRingSub[i]) {
                m_pMdlRingSub[i]->DetachFromParent();
                bcs::CRenderMan::PushBack(m_pMdlRingSub[i], 5);
            }
        }
    } else {
        if (m_pGroup) {
            m_pGroup->DetachFromParent();
        }
        if (m_pMdlBaseSub) {
            m_pMdlBaseSub->DetachFromParent();
        }
        if (m_pMdlChance) {
            m_pMdlChance->DetachFromParent();
        }
        if (m_pMdlItemSub) {
            m_pMdlItemSub->DetachFromParent();
        }
        for (int i = 0; i < 2; i++) {
            if (m_pMdlRingSub[i]) {
                m_pMdlRingSub[i]->DetachFromParent();
            }
        }
    }
}

int msw::CBallTargetMarker::CheckAppear_()
{
    CBall *pBall = GetBall(m_nBallHandle);
    UnusedReferences(m_fAppearRate);

    if (pBall == 0) {
        return 1;
    }
    if (pBall->IsSleep()) {
        return 1;
    }
    if ((CGame::g_GameWork.m_nFlag18 & 8) && !pBall->IsShootMode()) {
        if (pBall->IsScored()) {
            return 3;
        }
        return 1;
    }
    if (pBall->IsVolleyArtsAttach()) {
        return 3;
    }
    if (pBall->IsVolleyReqDelete()) {
        return 3;
    }
    if (pBall->IsScored()) {
        return 3;
    }
    return pBall->CanbeVolleyPickup(0) ? 3 : 2;
}

void msw::CBallTargetMarker::Main()
{
    if (m_pMdlBase != 0) {
        ExecState();
        SetHeightObjState_();
        UpdateViewFlag_();

        for (int i = 0; i < 2; i++) {
            if (m_pMdlRing[i] != 0) {
                m_pMdlRing[i]->SetAlphaRate(m_pMdlBase->GetAlphaRate());
            }
        }
        if (m_pMdlBaseSub != 0) {
            float fAlpha = (m_bChance ? 0.0f : 1.0f) * m_pMdlBase->GetAlphaRateF();
            m_pMdlBaseSub->SetAlphaRateF(fAlpha);
            for (int i = 0; i < 2; i++) {
                if (m_pMdlRingSub[i] != 0) {
                    m_pMdlRingSub[i]->SetAlphaRateF(fAlpha);
                }
            }
        }
        if (m_pMdlItemSub != 0 && m_pMdlItem != 0) {
            m_pMdlItemSub->SetAlphaRateF(m_pMdlItem->GetAlphaRateF());
        }
        if (m_pMdlChance != 0) {
            float fAlpha = (m_bChance ? 1.0f : 0.0f) * m_pMdlChance->GetAlphaRateF();
            if (m_pMdlBase->GetAlphaRate() == 0) {
                fAlpha = 0.0f;
            }
            m_pMdlChance->SetAlphaRateF(fAlpha);
        }
    }
}

// The original has this as a weak copy (an inline body in bcs/CScnObj.hpp,
// emitted after Main by some vtable here; the DOL never defines it). Making
// it inline in the shared header would emit weak copies in units that match
// without it, so it is defined out of line here: same code, strong binding.
unsigned char bcs::CScnObj::GetAlphaRate() const
{
    return m_nAlphaRate;
}

// Fades and scales the marker with the frames left before the ball comes
// down: hidden more than APPEAR_FRAME frames before the ball's display
// window, shrinking over the window, fading out once the ball is down.
void msw::CBallTargetMarker::SetHeightObjState_()
{
    static const float APPEAR_FRAME = 8.0f;
    CBall *pBall = GetBall(m_nBallHandle);
    float fFrame;

    if (pBall == 0) {
        fFrame = -1.0f;
    } else if (!pBall->IsActive()) {
        fFrame = -1.0f;
    } else if (pBall->IsScored()) {
        fFrame = -1.0f;
    } else if ((CGame::g_GameWork.m_nFlag18 & 8) && !pBall->IsShootMode()) {
        fFrame = -1.0f;
    } else {
        fFrame = pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
    }

    int bItem = 0;
    float fDispStart = 100.0f;
    float fDispEnd = 20.0f;
    m_bChance = 0;
    pBall = GetBall(m_nBallHandle);
    if (pBall != 0 && pBall->IsActive()) {
        pBall->GetVolleyBallMarkerDispFrame(fDispStart, fDispEnd);
        pBall->IsVolleyChanceBallMode(); // (the result is not used)
        m_bChance = 0;
        m_nSpikeType = 0;
        if (pBall->IsVolleyCurveSpike()) {
            m_nSpikeType = 1;
        } else if (pBall->IsVolleyPowerSpike()) {
            m_nSpikeType = 2;
        }
        CItemBall *pItem = pBall->GetItemEffect_();
        if (pItem != 0 && pItem->GetType() == 3) {
            bItem = 1;
        }
    }

    if (m_pMdlItem != 0) {
        int bHide = !bItem;
        SetHide(m_pMdlItem, bHide);
        if (m_pMdlItemSub != 0) {
            SetHide(m_pMdlItemSub, bHide);
        }
        if (bItem) {
            m_pMdlItem->m_vScale.Set(9.0f, 1.0f, 9.0f);
            if (fFrame <= 120.0f) {
                m_pMdlItem->SetAlphaRateF(Clamp01(m_pMdlItem->GetAlphaRateF() + 0.1f * GetFrameRateF()));
            }
        } else {
            m_pMdlItem->SetAlphaRate(0);
        }
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
            float fScale = fRate * -1.6f + 5.6f;
            m_pMdlBase->SetAlphaRateF((fRate * -0.5f + 1.0f) * m_fAppearRate);
            m_pMdlBase->m_vScale.Set(fScale, 1.0f, fScale);
            if (m_pMdlChance != 0) {
                float fChanceScale = fRate * -2.0f + 7.0f;
                m_pMdlChance->SetAlphaRateF((fRate * -0.1f + 0.6f) * m_fAppearRate);
                m_pMdlChance->m_vScale.Set(fChanceScale, 1.0f, fChanceScale);
            }
        }
    }
}

// Each model is drawn only in the views of its side of the net (masks on
// the models' view flags), as seen from the user team.
void msw::CBallTargetMarker::UpdateViewFlag_()
{
    long nTeam = CGame::g_GameWork.m_nUnk00;
    bcs::FVEC3 netPos(CStageMan::GetInstance()->GetNetPos());

    if (CGame::GetTargetGoalIdx(nTeam) == 0) {
        if (m_pGroup->m_vTrans.z > netPos.z) {
            m_pMdlBase->SetViewFlag(~0xA);
            m_pMdlBaseSub->SetViewFlag(0xA);
            m_pMdlChance->SetViewFlag(0xA);
            m_pMdlItem->SetViewFlag(~0xA);
            m_pMdlItemSub->SetViewFlag(0xA);
            for (int i = 0; i < 2; i++) {
                if (m_pMdlRing[i] != 0) {
                    m_pMdlRing[i]->SetViewFlag(~0xA);
                }
                if (m_pMdlRingSub[i] != 0) {
                    m_pMdlRingSub[i]->SetViewFlag(0xA);
                }
            }
        } else {
            m_pMdlBase->SetViewFlag(0xA);
            m_pMdlBaseSub->SetViewFlag(~0xA);
            m_pMdlChance->SetViewFlag(~0xA);
            m_pMdlItem->SetViewFlag(0xA);
            m_pMdlItemSub->SetViewFlag(~0xA);
            for (int i = 0; i < 2; i++) {
                if (m_pMdlRing[i] != 0) {
                    m_pMdlRing[i]->SetViewFlag(0xA);
                }
                if (m_pMdlRingSub[i] != 0) {
                    m_pMdlRingSub[i]->SetViewFlag(~0xA);
                }
            }
        }
    } else {
        if (m_pGroup->m_vTrans.z > netPos.z) {
            m_pMdlBase->SetViewFlag(~0x5);
            m_pMdlBaseSub->SetViewFlag(0x5);
            m_pMdlChance->SetViewFlag(0x5);
            m_pMdlItem->SetViewFlag(~0x5);
            m_pMdlItemSub->SetViewFlag(0x5);
            for (int i = 0; i < 2; i++) {
                if (m_pMdlRing[i] != 0) {
                    m_pMdlRing[i]->SetViewFlag(~0x5);
                }
                if (m_pMdlRingSub[i] != 0) {
                    m_pMdlRingSub[i]->SetViewFlag(0x5);
                }
            }
        } else {
            m_pMdlBase->SetViewFlag(0x5);
            m_pMdlBaseSub->SetViewFlag(~0x5);
            m_pMdlChance->SetViewFlag(~0x5);
            m_pMdlItem->SetViewFlag(0x5);
            m_pMdlItemSub->SetViewFlag(~0x5);
            for (int i = 0; i < 2; i++) {
                if (m_pMdlRing[i] != 0) {
                    m_pMdlRing[i]->SetViewFlag(0x5);
                }
                if (m_pMdlRingSub[i] != 0) {
                    m_pMdlRingSub[i]->SetViewFlag(~0x5);
                }
            }
        }
    }

    m_pMdlBase->SetViewFlag(0xF);
    for (int i = 0; i < 2; i++) {
        if (m_pMdlRing[i] != 0) {
            m_pMdlRing[i]->SetViewFlag(0xF);
        }
    }

    int bSplit = 1;
    if (CGame::g_GameWork.m_nUnk1C8 & 2) {
        bSplit = 0;
    }
    if (CGame::g_GameWork.m_nUnk40 & 0x40) {
        bSplit = 0;
    }
    if (!bSplit) {
        if (m_pMdlBaseSub != 0) {
            m_pMdlBaseSub->SetViewFlag(0xF);
        }
        if (m_pMdlChance != 0) {
            m_pMdlChance->SetViewFlag(0xF);
        }
        for (int i = 0; i < 2; i++) {
            if (m_pMdlRingSub[i] != 0) {
                m_pMdlRingSub[i]->SetViewFlag(0xF);
            }
        }
        if (m_pMdlItemSub != 0) {
            m_pMdlItemSub->SetViewFlag(0xF);
        }
    }
}

void msw::CBallTargetMarker::CtorStateSleep()
{
    PushBack_(0);
    if (m_pMdlBase != 0) {
        m_pMdlBase->SetAlphaRate(0);
    }
    m_fAppearRate = 0.0f;
}

// Follows the ball and waits for it to come into play.
void msw::CBallTargetMarker::LoopStateSleep()
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

void msw::CBallTargetMarker::DtorStateSleep()
{
    PushBack_(1);
}

void msw::CBallTargetMarker::CtorStateAppear()
{
}

inline void msw::CBallTargetMarker::UpdateTargetPos_()
{
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);

    if (m_pGroup != 0) {
        pos = m_pGroup->m_vTrans;
    }
    CBall *pBall = GetBall(m_nBallHandle);
    if (pBall != 0 && pBall->IsActive() && !pBall->IsVolleyReqDelete()) {
        pos = pBall->GetTargetPos();
    }
    if (m_pGroup != 0) {
        SetPosXZ(m_pGroup, pos.x, pos.z);
    }
}

// Grows the marker to 4 times its size over 16 frames, along a quarter
// sine.
void msw::CBallTargetMarker::LoopStateAppear()
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

void msw::CBallTargetMarker::DtorStateAppear()
{
}

void msw::CBallTargetMarker::CtorStateMain()
{
}

void msw::CBallTargetMarker::LoopStateMain()
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

void msw::CBallTargetMarker::DtorStateMain()
{
}

void msw::CBallTargetMarker::CtorStateDisappear()
{
}

// Brings m_fAppearRate down over 10 frames. (the scale is worked out as in
// LoopStateAppear but not set)
void msw::CBallTargetMarker::LoopStateDisappear()
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

void msw::CBallTargetMarker::DtorStateDisappear()
{
}

// The marker's colours, by spike type (only the rings' differ).
void msw::CBallTargetMarker::UpdateColor_()
{
    if (m_pMdlBase != 0) {
        bcs::BCS_Color color0;
        bcs::BCS_Color color1;
        switch (m_nSpikeType) {
            case 1:
                color0 = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
                color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
                break;
            case 2:
                color0 = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
                color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
                break;
            default:
                color0 = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
                color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
                break;
        }
        m_pMdlBase->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlBase->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
    }
    if (m_pMdlBaseSub != 0) {
        bcs::BCS_Color color0;
        bcs::BCS_Color color1;
        switch (m_nSpikeType) {
            case 1:
                color0 = bcs::BCS_Color(0xA0, 0x00, 0x64, 0xFF);
                color1 = bcs::BCS_Color(0x50, 0x50, 0x50, 0x6E);
                break;
            case 2:
                color0 = bcs::BCS_Color(0xA0, 0x00, 0x64, 0xFF);
                color1 = bcs::BCS_Color(0x50, 0x50, 0x50, 0x6E);
                break;
            default:
                color0 = bcs::BCS_Color(0xA0, 0x00, 0x64, 0xFF);
                color1 = bcs::BCS_Color(0x50, 0x50, 0x50, 0x6E);
                break;
        }
        m_pMdlBaseSub->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlBaseSub->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
    }
    if (m_pMdlChance != 0) {
        bcs::BCS_Color color0;
        bcs::BCS_Color color1;
        color0 = bcs::BCS_Color(0xFF, 0x42, 0xFF, 0xE0);
        color1 = bcs::BCS_Color(0xFB, 0xC0, 0xFF, 0xC0);
        m_pMdlChance->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlChance->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
    }
    if (m_pMdlItem != 0) {
        bcs::BCS_Color color0;
        bcs::BCS_Color color1;
        color0 = bcs::BCS_Color(0xFF, 0x00, 0x00, 0xFF);
        color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0x78);
        m_pMdlItem->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlItem->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
        if (m_pMdlItemSub != 0) {
            m_pMdlItemSub->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
            m_pMdlItemSub->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
        }
    }

    // The rings show only for a spike, in its colour.
    bcs::BCS_Color ringColor(0UL);
    switch (m_nSpikeType) {
        case 1:
            ringColor = bcs::BCS_Color(0x00, 0xA0, 0x00, 0xFF);
            break;
        case 2:
            ringColor = bcs::BCS_Color(0xFF, 0x00, 0x00, 0x00);
            break;
    }
    for (int i = 0; i < 2; i++) {
        if (m_pMdlRing[i] != 0) {
            if (ringColor.a != 0) {
                m_pMdlRing[i]->SetMatTevColor(s_pRingMatName, GX_TEVREG0, ringColor);
            } else {
                m_pMdlRing[i]->SetAlphaRate(0);
            }
        }
    }
    bcs::BCS_Color ringSubColor(0UL);
    switch (m_nSpikeType) {
        case 1:
            ringSubColor = bcs::BCS_Color(0xFF, 0x1E, 0x0A, 0xFF);
            break;
        case 2:
            ringSubColor = bcs::BCS_Color(0x00, 0x00, 0x00, 0x00);
            break;
    }
    for (int i = 0; i < 2; i++) {
        if (m_pMdlRingSub[i] != 0) {
            if (ringSubColor.a != 0) {
                m_pMdlRingSub[i]->SetMatTevColor(s_pRingMatName, GX_TEVREG0, ringSubColor);
            } else {
                m_pMdlRingSub[i]->SetAlphaRate(0);
            }
        }
    }
}

void msw::CBallTargetMarker::Disp()
{
    UpdateColor_();
}

// Built by the file's static initialiser: four colours for each of nine
// kinds, the pair shown on the user team's side and the pair shown on the
// other side (as GetColor_ blends them). All nine kinds are the same.
namespace {
    bcs::BCS_Color s_MarkerColor[9][4] = {
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
        { bcs::BCS_Color(0xFF, 0xB4, 0x50, 0x46), bcs::BCS_Color(0x00, 0x00, 0x00, 0x00),
          bcs::BCS_Color(0xE1, 0x78, 0x00, 0xFF), bcs::BCS_Color(0x64, 0x64, 0x00, 0x46) },
    };
}

msw::CBallMarkerVolley::CBallMarkerVolley(bcs::CTask *pParent, unsigned long nBallHandle)
    : CBallMarker(pParent, nBallHandle, 1)
{
    m_fRate = 0.0f;
}

msw::CBallMarkerVolley::~CBallMarkerVolley()
{
}

// 1 keeps the marker down, 2 brings it up, 3 keeps it as it is (as read
// from CBallMarker's Sleep state; the meaning of 3 is a guess).
int msw::CBallMarkerVolley::AppearCheck()
{
    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallHandle));

    if (pBall == 0) {
        return 1;
    }
    if (pBall->IsSleep()) {
        return 1;
    }
    if (pBall->IsFlag4()) {
        return 1;
    }
    if (pBall->GetTouchNum() > 0 && !pBall->IsVolleyNetHit()) {
        return 3;
    }
    if (pBall->IsAttach()) {
        return 3;
    }
    if ((CGame::g_GameWork.m_nFlag18 & 0x4306) == 0) {
        if (CGame::IsArtsEventMode()) {
            return 1;
        }
        if (pBall->IsArtsShootMode()) {
            return 1;
        }
        if (pBall->IsScored()) {
            return 3;
        }
        return pBall->IsFlag100000() ? 3 : 2;
    }
    return pBall->IsFlag100000() ? 1 : 3;
}

// Moves m_fRate towards 1 while the ball is on the far side of the net from
// the user team's goal, towards 0 otherwise.
void msw::CBallMarkerVolley::MainFunc()
{
    CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallHandle));

    if (pBall != 0) {
        // (guess) A change time of 0 frames: the rate moves by 1 a frame.
        float fStep = 1.0f;
        if (m_nState != 0) {
            float fFrames = 0.0f;
            if (fFrames > fStep) {
                fStep = fStep / fFrames;
            }
        }

        bcs::FVEC3 netPos(CStageMan::GetInstance()->GetNetPos());
        int bFar = 0;
        if (CGame::GetTargetGoalIdx(CGame::g_GameWork.GetUserTeam()) == 0) {
            if (pBall->GetPos().z > netPos.z) {
                bFar = 1;
            }
        } else {
            if (pBall->GetPos().z < netPos.z) {
                bFar = 1;
            }
        }
        if (bFar) {
            m_fRate += fStep * GetFrameRateF();
        } else {
            m_fRate -= fStep * GetFrameRateF();
        }
        m_fRate = m_fRate > 1.0f ? 1.0f : (m_fRate < 0.0f ? 0.0f : m_fRate);
    }
}

float msw::CBallMarkerVolley::GetAppearFrameNum() const
{
    return 16.0f;
}

float msw::CBallMarkerVolley::GetDisappearFrameNum() const
{
    return 12.0f;
}

// From fA on the user team's side to fB on the other.
float msw::CBallMarkerVolley::GetDispScale() const
{
    static const float fB = 1.7f;
    static const float fA = 1.4f;
    float rate = m_fRate;
    return fA + (fB - fA) * rate;
}

void msw::CBallMarkerVolley::GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long nIdx)
{
    bcs::BCS_Color near0, far0, near1, far1;
    GetColor_(near0, near1, far0, far1, nIdx);
    rColor0 = bcs::BCS_Color_Lerp(near0, far0, m_fRate);
    rColor1 = bcs::BCS_Color_Lerp(near1, far1, m_fRate);
}

void msw::CBallMarkerVolley::GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, bcs::BCS_Color &rColor2,
                                       bcs::BCS_Color &rColor3, long nIdx)
{
    long kind = 0;
    switch (nIdx) {
        case 3:
            kind = 1;
            break;
        case 9:
            kind = 2;
            break;
        case 2:
            kind = 3;
            break;
        case 6:
            kind = 4;
            break;
        case 7:
            kind = 5;
            break;
        case 10:
            kind = 6;
            break;
        case 11:
            kind = 7;
            break;
        case 17:
            kind = 8;
            break;
    }
    rColor0 = s_MarkerColor[kind][0];
    rColor1 = s_MarkerColor[kind][1];
    rColor2 = s_MarkerColor[kind][2];
    rColor3 = s_MarkerColor[kind][3];
}

msw::CBallTargetMarkerVsBoss::CBallTargetMarkerVsBoss(bcs::CTask *pParent, unsigned long nBallHandle)
    : CBallTargetMarker(pParent, nBallHandle, 1)
{
}

msw::CBallTargetMarkerVsBoss::~CBallTargetMarkerVsBoss()
{
}

msw::CBallTargetMarkerRhythmReceive::CBallTargetMarkerRhythmReceive(bcs::CTask *pParent, long nTeam, int bMark)
    : bcs::CTask(pParent, 0x19)
{
    m_pGroup = 0;
    m_pMdlBase = 0;
    m_pMdlMark = 0;
    m_nHandle = 0;
    m_fAppearRate = 0.0f;
    m_bItem = 0;
    m_nTeam = nTeam;

    InitFsm(this, STATE_NUM);
    SetState(STATE_SLEEP, &CBallTargetMarkerRhythmReceive::CtorStateSleep,
             &CBallTargetMarkerRhythmReceive::LoopStateSleep, &CBallTargetMarkerRhythmReceive::DtorStateSleep);
    SetState(STATE_APPEAR, &CBallTargetMarkerRhythmReceive::CtorStateAppear,
             &CBallTargetMarkerRhythmReceive::LoopStateAppear, &CBallTargetMarkerRhythmReceive::DtorStateAppear);
    SetState(STATE_MAIN, &CBallTargetMarkerRhythmReceive::CtorStateMain,
             &CBallTargetMarkerRhythmReceive::LoopStateMain, &CBallTargetMarkerRhythmReceive::DtorStateMain);
    SetState(STATE_DISAPPEAR, &CBallTargetMarkerRhythmReceive::CtorStateDisappear,
             &CBallTargetMarkerRhythmReceive::LoopStateDisappear,
             &CBallTargetMarkerRhythmReceive::DtorStateDisappear);
    ChangeState(STATE_SLEEP);

    Create_(bMark);
    if (m_nTeam != 4) {
        SetColor_();
    } else {
        UpdateColor_();
    }
}

msw::CBallTargetMarkerRhythmReceive::~CBallTargetMarkerRhythmReceive()
{
    DeleteScnObj(m_pMdlMark);
    DeleteScnObj(m_pMdlBase);
    DeleteScnObj(m_pGroup);
}

// The ground mark and, for a team (0 to 3) when bMark is set, the team's
// mark on it, both sorted by hand (see Main).
void msw::CBallTargetMarkerRhythmReceive::Create_(int bMark)
{
    nw4r::g3d::ResFile file = CChrMarker::GetRsrcPack();

    m_pGroup = new (0x100) bcs::CScnGroup(0x10, 0);
    m_pGroup->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pGroup->OnFlag(0x4000);

    nw4r::g3d::ResMdl mdl = file.GetResMdl(s_MdlName[1]);
    m_pMdlBase = new (0x100) bcs::CMdlObj;
    m_pMdlBase->Create(mdl, 0, 0);
    m_pMdlBase->OnFlag(0x4000);
    m_pMdlBase->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    m_pMdlBase->MatSetZModeAll(1, GX_ALWAYS, 0);
    m_pMdlBase->SetDrawPriority(0, 0);
    m_pMdlBase->SetAlphaRate(0);
    m_pMdlBase->OnFlag(0x80000);
    m_pMdlBase->SetSortValue(0.0f);
    m_pGroup->AppendChild(m_pMdlBase);

    if (bMark && (unsigned long)m_nTeam <= 3) {
        mdl = file.GetResMdl(s_MarkMdlName[m_nTeam]);
        m_pMdlMark = new (0x100) bcs::CMdlObj;
        m_pMdlMark->Create(mdl, 0, 0);
        m_pMdlMark->m_vScale.Set(2.0f, 2.0f, 2.0f);
        m_pMdlMark->OnFlag(0x4000);
        m_pMdlMark->MatSetZModeAll(1, GX_ALWAYS, 0);
        m_pMdlMark->SetDrawPriority(0, 0);
        bcs::BCS_Color color = CChrMarker::GetTeamColor(m_nTeam);
        m_pMdlMark->SetMatTevColor(s_pMarkMatName, GX_TEVREG0, color);
        m_pMdlMark->OnFlag(0x80000);
        m_pMdlMark->SetSortValue(0.0f);
        m_pGroup->AppendChild(m_pMdlMark);
    }

    UpdateColor_();
    DtorStateSleep();
}

void msw::CBallTargetMarkerRhythmReceive::SetAttachToBall(unsigned long nHandle)
{
    m_nHandle = nHandle;
    m_bItem = 0;
}

void msw::CBallTargetMarkerRhythmReceive::SetAttachToItem(unsigned long nHandle)
{
    m_nHandle = nHandle;
    m_bItem = 1;
}

// As CBallTargetMarker::CheckAppear_; an item counts while it is thrown
// and has not come down.
int msw::CBallTargetMarkerRhythmReceive::CheckAppear_()
{
    if (m_bItem) {
        CItem *pItem = GetItem(m_nHandle);

        if (pItem == 0) {
            return 1;
        }
        if (!pItem->IsStateThrow()) {
            return 3;
        }
        if (pItem->IsTouchDown()) {
            return 3;
        }
    } else {
        CBall *pBall = GetBall(m_nHandle);

        if (pBall == 0) {
            return 1;
        }
        if (pBall->IsSleep()) {
            return 1;
        }
        if ((CGame::g_GameWork.m_nFlag18 & 8) && !pBall->IsShootMode()) {
            return pBall->IsScored() ? 3 : 1;
        }
        if (pBall->IsVolleyArtsAttach()) {
            return 3;
        }
        if (pBall->IsVolleyReqDelete()) {
            return 3;
        }
        if (pBall->IsScored()) {
            return 3;
        }
        if (pBall->CanbeVolleyPickup(0)) {
            return 3;
        }
    }
    return 2;
}

// Where the ball or the item will come down; the marker's own place when
// that is not known.
void msw::CBallTargetMarkerRhythmReceive::GetTargetPos_(bcs::FVEC3 &rPos)
{
    rPos.Set(0.0f, 0.0f, 0.0f);
    if (m_pGroup != 0) {
        rPos = m_pGroup->m_vTrans;
    }
    if (m_bItem) {
        CItem *pItem = GetItem(m_nHandle);
        if (pItem != 0 && pItem->IsStateThrow() && !pItem->IsTouchDown()) {
            pItem->GetThrowTargetPos(rPos);
        }
    } else {
        CBall *pBall = GetBall(m_nHandle);
        if (pBall != 0 && pBall->IsActive() && !pBall->IsVolleyReqDelete()) {
            rPos = pBall->GetTargetPos();
        }
    }
}

// Frames until the ball or the item comes down, -1 when it is not on its
// way down.
float msw::CBallTargetMarkerRhythmReceive::GetTouchdownFrame_()
{
    if (m_bItem) {
        CItem *pItem = GetItem(m_nHandle);

        if (pItem == 0) {
            return -1.0f;
        }
        if (!pItem->IsStateThrow()) {
            return -1.0f;
        }
        if (pItem->IsTouchDown()) {
            return -1.0f;
        }
        return pItem->GetTouchDownFrame();
    }

    CBall *pBall = GetBall(m_nHandle);
    if (pBall == 0) {
        return -1.0f;
    }
    if (!pBall->IsActive()) {
        return -1.0f;
    }
    if (pBall->IsScored()) {
        return -1.0f;
    }
    if (pBall->IsShootMode()) {
        return pBall->GetTouchDownFrame(pBall->GetBgCollRadius());
    }
    return -1.0f;
}

// The models are sorted by their alpha, the mark in front of the ground
// mark.
void msw::CBallTargetMarkerRhythmReceive::Main()
{
    if (m_pMdlBase != 0) {
        ExecState();
        SetHeightObjState_();
        int alpha = m_pMdlBase->GetAlphaRate();
        m_pMdlBase->SetSortValue(2.0f * alpha);
        if (m_pMdlMark != 0) {
            m_pMdlMark->SetSortValue(2.0f * alpha + 1.0f);
        }
    }
}

// As CBallTargetMarker::SetHeightObjState_, without the chance-ball and
// item marks; an item's marker comes up 6 frames before it lands.
void msw::CBallTargetMarkerRhythmReceive::SetHeightObjState_()
{
    static const float APPEAR_FRAME = 8.0f;
    float fFrame = GetTouchdownFrame_();
    float fDispStart = 100.0f;
    float fDispEnd = 20.0f;

    if (m_bItem) {
        fDispStart = 100.0f;
        fDispEnd = 6.0f;
    } else {
        CBall *pBall = GetBall(m_nHandle);
        if (pBall != 0 && pBall->IsActive()) {
            pBall->GetVolleyBallMarkerDispFrame(fDispStart, fDispEnd);
        }
    }

    if (fDispEnd >= fDispStart) {
        m_pMdlBase->SetAlphaRate(0);
        if (m_pMdlMark != 0) {
            m_pMdlMark->SetAlphaRate(0);
        }
    } else if (fFrame <= fDispEnd) {
        float fAlpha = m_pMdlBase->GetAlphaRateF();
        if (fAlpha > 0.0f) {
            fAlpha -= 0.0625f * (0.5f * GetFrameRateF());
            fAlpha = 0.0f < fAlpha ? fAlpha : 0.0f;
            m_pMdlBase->SetAlphaRateF(fAlpha);
            if (m_pMdlMark != 0) {
                m_pMdlMark->SetAlphaRateF(fAlpha);
            }
        }
    } else {
        float fLen = fDispStart - fDispEnd;
        float fRest = fFrame - fDispEnd;
        if (fRest >= APPEAR_FRAME + fLen) {
            m_pMdlBase->SetAlphaRate(0);
            if (m_pMdlMark != 0) {
                m_pMdlMark->SetAlphaRate(0);
            }
        } else if (fRest >= fLen) {
            float fRate = Clamp01((fRest - fLen) / APPEAR_FRAME);
            float fAlpha = fRate * -1.0f + 1.0f;
            m_pMdlBase->SetAlphaRateF(fAlpha * m_fAppearRate);
            m_pMdlBase->m_vScale.Set(4.5f, 1.0f, 4.5f);
            if (m_pMdlMark != 0) {
                m_pMdlMark->SetAlphaRateF(fAlpha * m_fAppearRate);
            }
        } else {
            float fRate = Clamp01(1.0f - Clamp01(fRest / fLen));
            float fAlpha = fRate * -0.5f + 1.0f;
            float fScale = fRate * -2.0f + 4.5f;
            m_pMdlBase->SetAlphaRateF(fAlpha * m_fAppearRate);
            m_pMdlBase->m_vScale.Set(fScale, 1.0f, fScale);
            if (m_pMdlMark != 0) {
                m_pMdlMark->SetAlphaRateF(fAlpha * m_fAppearRate);
            }
        }
    }
}

void msw::CBallTargetMarkerRhythmReceive::CtorStateSleep()
{
    if (m_pGroup != 0) {
        m_pGroup->DetachFromParent();
    }
    if (m_pMdlBase != 0) {
        m_pMdlBase->SetAlphaRate(0);
    }
    if (m_pMdlMark != 0) {
        m_pMdlMark->SetAlphaRate(0);
    }
    m_fAppearRate = 0.0f;
}

void msw::CBallTargetMarkerRhythmReceive::LoopStateSleep()
{
    if (m_pGroup != 0) {
        if (m_bItem) {
            CItem *pItem = GetItem(m_nHandle);
            if (pItem != 0) {
                bcs::FVEC3 pos(pItem->GetPos());
                SetPosXZ(m_pGroup, pos.x, pos.z);
            }
        } else {
            CBall *pBall = GetBall(m_nHandle);
            if (pBall != 0) {
                SetPosXZ(m_pGroup, pBall->GetPos().x, pBall->GetPos().z);
            }
        }
    }
    if (CheckAppear_() == 2) {
        ChangeState(STATE_APPEAR);
    }
}

void msw::CBallTargetMarkerRhythmReceive::DtorStateSleep()
{
    if (m_pGroup != 0) {
        m_pGroup->DetachFromParent();
        bcs::CRenderMan::PushBack(m_pGroup, 5);
    }
}

void msw::CBallTargetMarkerRhythmReceive::CtorStateAppear()
{
}

void msw::CBallTargetMarkerRhythmReceive::LoopStateAppear()
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
            bcs::FVEC3 pos;
            GetTargetPos_(pos);
            if (m_pGroup != 0) {
                SetPosXZ(m_pGroup, pos.x, pos.z);
            }
            m_fAppearRate += 0.0625f * fFrameRate;
            m_fAppearRate = Clamp01(m_fAppearRate);
            float fScale = 2.5f * nw4r::math::SinRad(1.5707964f * m_fAppearRate);
            m_pMdlBase->m_vScale.Set(fScale, 1.0f, fScale);
            if (m_fAppearRate >= 1.0f) {
                ChangeState(STATE_MAIN);
            }
            break;
        }
    }
}

void msw::CBallTargetMarkerRhythmReceive::DtorStateAppear()
{
}

void msw::CBallTargetMarkerRhythmReceive::CtorStateMain()
{
}

void msw::CBallTargetMarkerRhythmReceive::LoopStateMain()
{
    switch (CheckAppear_()) {
        case 1:
            ChangeState(STATE_SLEEP);
            break;
        case 3:
            ChangeState(STATE_DISAPPEAR);
            break;
        default: {
            GetFrameRateF();
            bcs::FVEC3 pos;
            GetTargetPos_(pos);
            if (m_pGroup != 0) {
                SetPosXZ(m_pGroup, pos.x, pos.z);
            }
            m_pMdlBase->m_vScale.Set(2.5f, 1.0f, 2.5f);
            break;
        }
    }
}

void msw::CBallTargetMarkerRhythmReceive::DtorStateMain()
{
}

void msw::CBallTargetMarkerRhythmReceive::CtorStateDisappear()
{
}

void msw::CBallTargetMarkerRhythmReceive::LoopStateDisappear()
{
    if (CheckAppear_() == 1) {
        ChangeState(STATE_SLEEP);
        return;
    }
    float fFrameRate = GetFrameRateF();
    bcs::FVEC3 pos;
    GetTargetPos_(pos);
    if (m_pGroup != 0) {
        SetPosXZ(m_pGroup, pos.x, pos.z);
    }
    m_fAppearRate -= 0.1f * fFrameRate;
    m_fAppearRate = Clamp01(m_fAppearRate);
    float fScale = 2.5f * nw4r::math::SinRad(1.5707964f * m_fAppearRate);
    (void)fScale; // worked out but not set
    if (m_fAppearRate <= 0.0f) {
        ChangeState(STATE_SLEEP);
    }
}

void msw::CBallTargetMarkerRhythmReceive::DtorStateDisappear()
{
}

void msw::CBallTargetMarkerRhythmReceive::Disp()
{
    UpdateColor_();
}

// The ground mark in the colours of team m_nTeam.
void msw::CBallTargetMarkerRhythmReceive::SetColor_()
{
    bcs::BCS_Color color0(0xFFUL);
    bcs::BCS_Color color1(0xFFUL);

    switch (m_nTeam) {
        case 0:
            color0 = bcs::BCS_Color(0x00, 0x64, 0xFF, 0xFF);
            color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0xFF);
            break;
        case 1:
            color0 = bcs::BCS_Color(0xFF, 0x1E, 0x1E, 0xFF);
            color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0xFF);
            break;
        case 2:
            color0 = bcs::BCS_Color(0x00, 0xFF, 0x00, 0xFF);
            color1 = bcs::BCS_Color(0x00, 0x96, 0x00, 0xFF);
            break;
        case 3:
            color0 = bcs::BCS_Color(0xFF, 0xFF, 0x00, 0xFF);
            color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0xFF);
            break;
        default:
            color0 = bcs::BCS_Color(0x28, 0x19, 0x23, 0xFF);
            color1 = bcs::BCS_Color(0x00, 0x00, 0x00, 0xFF);
            break;
    }
    if (m_pMdlBase != 0) {
        m_pMdlBase->SetMatTevColor(s_pMatName0, GX_TEVREG0, color0);
        m_pMdlBase->SetMatTevColor(s_pMatName1, GX_TEVREG0, color1);
    }
}

// Without a team the ground mark takes the colour of the ball's first
// material (TEV register 0), opaque.
void msw::CBallTargetMarkerRhythmReceive::UpdateColor_()
{
    if (m_nTeam == 4) {
        bcs::BCS_Color color(0xFFUL);
        bcs::BCS_Color black(0xFFUL);

        if (!m_bItem) {
            CBall *pBall = GetBall(m_nHandle);
            if (pBall != 0) {
                bcs::CMdlObj *pMdl = pBall->GetMdlObj();
                if (pMdl != 0) {
                    nw4r::g3d::ResMat mat = pMdl->GetResMdl().GetResMat(0);
                    if (mat.IsValid()) {
                        bcs::CMdlObj::CopiedMatAccess access(pMdl, mat.GetID());
                        if (access.IsValid()) {
                            nw4r::g3d::ResMatTevColor tev = access.GetResMatTevColor(0);
                            if (tev.IsValid()) {
                                tev.GXGetTevColor(GX_TEVREG0, reinterpret_cast<GXColor *>(&color));
                                color.a = 0xFF;
                            }
                        }
                    }
                }
            }
        }
        if (m_pMdlBase != 0) {
            m_pMdlBase->SetMatTevColor(s_pMatName0, GX_TEVREG0, color);
            m_pMdlBase->SetMatTevColor(s_pMatName1, GX_TEVREG0, black);
        }
    }
}
