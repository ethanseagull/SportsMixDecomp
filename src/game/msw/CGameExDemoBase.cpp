// The game's short demos and screen effects (DOL): the goal demo, the
// blur, the speed lines, the explanation demo's base and mask, and the
// scored view. One file in the original: they share one float pool and
// their vtables sit together in .data. The headers are included in the
// order the classes are defined here: the compiler emits vtables in
// reverse order of declaration, which is the original's .data order.
#include "game/msw/CGameGoalDemo.hpp"
#include "game/msw/CGameBlurEffect.hpp"
#include "game/msw/CGameSaturatedLinework.hpp"
#include "game/msw/CGameExDemoMaskObj.hpp"
#include "game/msw/CGameExDemoBase.hpp"
#include "game/msw/CGameScoredView.hpp"

#include "bcs/BCS_Draw2D.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CCamera.hpp"
#include "bcs/CCapture.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnObjGather.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CStageMan.hpp"
#include "math.h"
#include "nw4r/g3d/G3DState.hpp"
#include "revolution/gx/GXGeometry.h"
#include "revolution/gx/GXPixel.h"
#include "revolution/gx/GXTev.h"
#include "revolution/gx/GXTexture.h"
#include "revolution/gx/GXTransform.h"
#include "revolution/gx/GXVert.h"
#include "revolution/mtx.h"

namespace msw {

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

        // rOut += rAdd.
        inline void AddVec(bcs::FVEC3 &rOut, const bcs::FVEC3 &rAdd)
        {
            bcs::FVEC3Add(&rOut, &rOut, &rAdd);
        }

        // rVec scaled to length 1 (zero stays zero).
        inline void NormalizeVec(bcs::FVEC3 &rVec)
        {
            bcs::FVEC3 vec(rVec);
            bcs::BCS_Vec_Normalize(rVec, vec);
        }

        inline float Clamp(float value, float min, float max)
        {
            return value > max ? max : (value < min ? min : value);
        }
    }

    // ---- CGameGoalDemo ----

    CGameGoalDemo::CGameGoalDemo(bcs::CTask *pParent, long nType, long nTeam, const bcs::FVEC3 &rPos, long nSide,
                                 long nTelop, float fSpeed)
        : bcs::CTask(pParent, -1),
          m_nType(nType),
          m_nTeam(nTeam),
          m_vPos(rPos),
          m_nSide(nSide),
          m_nStep(0),
          m_fSaveSpeed(1.0f),
          m_nTelop(nTelop),
          m_fUnk58(1.0f),
          m_fTimer(0.0f),
          m_pCamera(0)
    {
        SetUnk08(0xFC00);

        if (m_nType == 1) {
            bcs::CRenderMan::ReqPushViewSplitType(bcs::CWorldEnv::VIEW_SPLIT_1, 0);

            bcs::FVEC3 at(m_vPos);
            at.y = 1.0f;
            bcs::FVEC3 ofs(0.0f, 2.0f, 4.0f);

            float deg = bcs::BCS_RandRange(-70.0f, 70.0f);
            float rot = 0.017453292f * deg;
            if (nSide != 0) {
                rot += PI;
            }
            rot = WrapRad(rot);

            bcs::FVEC3 pos;
            bcs::BCS_Vec_RotY(pos, ofs, rot);
            AddVec(pos, at);

            if (CGame::g_GameWork.GetGameMode() == CGame::GAME_MODE_VOLLEY) {
                // Not through the net: look from the other side.
                if (IsCrossNet_(pos, at)) {
                    rot += PI;
                    rot = WrapRad(rot);
                    bcs::BCS_Vec_RotY(pos, ofs, rot);
                    AddVec(pos, at);
                }
                if (fabsf(pos.x) >= 9.0f && fabsf(at.x) < 9.0f) {
                    bcs::BCS_Vec_RotY(pos, ofs, WrapRad(-rot));
                    AddVec(pos, at);
                }
            }

            CGame::StopCameraQuake(-1);
            CGame::StopAllCameraQuake();

            m_pCamera = new bcs::CCamera(0, 0x80, 0);
            if (m_pCamera != 0) {
                m_pCamera->SetPos(pos);
                m_pCamera->SetAt(at);
                m_pCamera->SetRoll(0.0f);
                m_pCamera->SetActive(-1);
            }
            m_fTimer = 30.0f;
        } else {
            float speed = fSpeed < 0.05f ? 0.05f : fSpeed;

            // The goal telop of each sport for the three kinds of goal.
            switch (CGame::g_GameWork.m_nGameMode) {
                case CGame::GAME_MODE_DODGE:
                    m_nTelop = 0x1058;
                    break;
                case CGame::GAME_MODE_BASKET:
                    switch (m_nTelop) {
                        case 0:
                            m_nTelop = 0x1052;
                            break;
                        case 1:
                            m_nTelop = 0x1054;
                            break;
                        case 2:
                            m_nTelop = 0x1053;
                            break;
                    }
                    break;
                case CGame::GAME_MODE_VOLLEY:
                    switch (m_nTelop) {
                        case 0:
                            m_nTelop = 0x1055;
                            break;
                        case 1:
                            m_nTelop = 0x1056;
                            break;
                        case 2:
                            m_nTelop = 0x1057;
                            break;
                    }
                    break;
                case CGame::GAME_MODE_HOCKEY:
                    switch (m_nTelop) {
                        case 0:
                            m_nTelop = 0x105D;
                            break;
                        case 1:
                            m_nTelop = 0x105C;
                            break;
                        case 2:
                            m_nTelop = 0x105B;
                            break;
                    }
                    break;
            }

            bcs::CRenderMan::ReqPushViewSplitType(bcs::CWorldEnv::VIEW_SPLIT_1, 0);
            m_fSaveSpeed = CGame::g_GameWork.m_fGameSpeed;
            CGame::g_GameWork.m_fGameSpeed = speed;
            CEventDemo::SetupDemoCamera("MakeScore", m_nTeam);
            CEventDemo::SetDemoCameraParam(10.0f);
            bcs::FVEC3 target(0.0f, 0.0f, 0.0f);
            CEventDemo::SetDemoCameraInfo(m_vPos, target, m_nSide);
        }
        m_nStep = 0;
    }

    CGameGoalDemo::~CGameGoalDemo()
    {
        if (m_pCamera != 0) {
            m_pCamera->Delete();
            m_pCamera = 0;
            CGame::StopAllCameraQuake();
        }
        if (m_nTelop >= 0) {
            CGame::TelopRemove(m_nTelop);
        }
    }

    int CGameGoalDemo::IsCrossNet_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        CStageMan *pStage = CStageMan::GetInstance();
        bcs::FVEC3 netPos(pStage->GetNetPos());
        bcs::FVEC3 from(rFrom);
        bcs::FVEC3 to(rTo);
        bcs::FVEC3 hit;
        bcs::FVEC3 normal(0.0f, 0.0f, 1.0f);

        float t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(hit, normal, netPos, from, to);
        if (t >= 0.0f && t <= 1.0f) {
            return 1;
        }

        normal.Set(0.0f, 0.0f, -1.0f);
        t = CCollMan::GetIntersectionTimeOfFrontPlaneAndLine(hit, normal, netPos, from, to);
        if (t >= 0.0f && t <= 1.0f) {
            return 1;
        }
        return 0;
    }

    void CGameGoalDemo::Main()
    {
        if (m_nType == 1) {
            switch (m_nStep) {
                case 0:
                    if (m_fTimer > 0.0f) {
                        m_fTimer -= GetFrameRateF();
                    }
                    if (m_fTimer <= 0.0f) {
                        CGame::TelopCreate(m_nTeam, 1, 0);
                        m_fTimer = 60.0f;
                        m_nStep++;
                    }
                    break;
                case 1:
                    if (m_fTimer > 0.0f) {
                        m_fTimer -= GetFrameRateF();
                    }
                    if (m_fTimer <= 0.0f) {
                        if (CGame::TelopIsEnd(m_nTeam)) {
                            m_nStep = 99;
                        }
                    }
                    break;
            }
        } else {
            switch (m_nStep) {
                case 0:
                    CGame::TelopCreate(m_nTelop, 1, 0);
                    CEventDemo::PlayDemoCamera(0);
                    CGame::StartSaturatedLinework(30.0f);
                    m_nStep++;
                    break;
                case 1:
                    if (CEventDemo::IsEndDemoCamera()) {
                        m_nStep = 99;
                    }
                    break;
            }
        }
    }

    int CGameGoalDemo::IsEnd() const
    {
        return m_nStep >= 99;
    }

    void CGameGoalDemo::End()
    {
        if (IsDelete()) {
            return;
        }

        if (m_nType == 1) {
            if (m_pCamera != 0) {
                m_pCamera->Delete();
                m_pCamera = 0;
            }
            CGame::StopAllCameraQuake();
            bcs::CRenderMan::ReqPopViewSplitType(0);
        } else {
            CGame::EndSaturatedLinework();
            CEventDemo::SetEndDemoCamera();
            bcs::CRenderMan::ReqPopViewSplitType(0);
            CGame::g_GameWork.m_fGameSpeed = m_fSaveSpeed;
            CGame::StartStopEffect(5.0f, 20.0f, 0);
        }
        Delete();
    }

    // ---- CGameBlurEffect ----

    CGameBlurEffect::CGameBlurEffect(bcs::CTask *pParent, float fFadeIn, float fKeep, float fFadeOut, float fScale,
                                     bcs::BCS_Color color)
        : bcs::CTask(pParent, -1),
          m_nStep(0),
          m_fFadeIn(fFadeIn),
          m_fKeep(fKeep),
          m_fFadeOut(fFadeOut),
          m_fScale(fScale),
          m_fTime(1.0f),
          m_fCount(0.0f),
          m_Color(color)
    {
        // Phases of no length are skipped.
        m_fTime = m_fFadeIn;
        if (m_fTime <= 0.0f) {
            m_nStep = 1;
            m_fTime = m_fKeep;
            if (m_fTime <= 0.0f) {
                m_nStep = 2;
                m_fTime = m_fFadeOut;
                if (m_fTime <= 0.0f) {
                    m_nStep = 3;
                }
            }
        }
    }

    CGameBlurEffect::~CGameBlurEffect()
    {
        bcs::CRenderMan::SetBlurEffect(0, bcs::BCS_Color(0xFFFFFF80), 0, 0.0f, 0.0f);
    }

    void CGameBlurEffect::Main()
    {
        float rate;

        switch (m_nStep) {
            case 0:
                m_fCount = m_fCount + GetFrameRateF();
                rate = Clamp(m_fCount / m_fTime, 0.0f, 1.0f);
                if (m_fCount >= m_fTime) {
                    m_fCount = 0.0f;
                    m_nStep++;
                    m_fTime = m_fKeep;
                    if (m_fTime <= 0.0f) {
                        m_nStep++;
                        m_fTime = m_fFadeOut;
                        if (m_fTime <= 0.0f) {
                            m_nStep++;
                        }
                    }
                }
                {
                    float scale = m_fScale * rate;
                    bcs::BCS_Color color(m_Color);
                    color.a = m_Color.a * rate;
                    bcs::CRenderMan::SetBlurEffect(1, color, 0, scale, 0.0f);
                }
                break;
            case 1:
                m_fCount = m_fCount + GetFrameRateF();
                if (m_fCount >= m_fTime) {
                    m_fCount = 0.0f;
                    m_nStep++;
                    m_fTime = m_fFadeOut;
                    if (m_fTime <= 0.0f) {
                        m_nStep++;
                    }
                }
                bcs::CRenderMan::SetBlurEffect(1, m_Color, 0, m_fScale, 0.0f);
                break;
            case 2:
                m_fCount = m_fCount + GetFrameRateF();
                rate = Clamp(m_fCount / m_fTime, 0.0f, 1.0f);
                if (m_fCount >= m_fTime) {
                    Delete();
                    m_nStep++;
                } else {
                    bcs::BCS_Color color(m_Color);
                    color.a = m_Color.a * (1.0f - rate);
                    bcs::CRenderMan::SetBlurEffect(1, color, 0, m_fScale, 0.0f);
                }
                break;
            default:
                Delete();
                break;
        }
    }

    // ---- CGameSaturatedLinework ----

    CGameSaturatedLinework::CGameSaturatedLinework(bcs::CTask *pParent, float fFrame)
        : bcs::CTask(pParent, -1), m_nFrame(fFrame), m_hEffect(0)
    {
        m_hEffect = CEffectCtrl::CreateEffect(0x461, 4, 0, 0, 0);
    }

    CGameSaturatedLinework::~CGameSaturatedLinework()
    {
        if (m_hEffect != 0) {
            CEffectCtrl::EraseAttachEffect(m_hEffect);
        }
        m_hEffect = 0;
    }

    void CGameSaturatedLinework::Main()
    {
        if (m_nFrame > 0.0f) {
            m_nFrame = m_nFrame - GetFrameRateF();
            if (m_nFrame <= 0.0f) {
                if (m_hEffect != 0) {
                    CEffectCtrl::EraseAttachEffect(m_hEffect);
                }
                m_hEffect = 0;
            }
        }
    }

    // ---- CGameExDemoMaskObj ----

    CGameExDemoMaskObj::~CGameExDemoMaskObj()
    {
        DetachFromParent();
    }

    void CGameExDemoMaskObj::ObjProc(unsigned long nTask, unsigned long nParam, void *pInfo)
    {
        if (IsProcDisabled(nTask)) {
            return;
        }

        switch (nTask) {
            case 8:
                // As bcs::CGDLObj::ObjProc: the original tests pInfo and does
                // nothing either way (a guess at what it had).
                if (pInfo != 0) {
                    float fFrame = *static_cast<const float *>(pInfo);
                    (void)fFrame;
                }
                break;
            case 5:
                static_cast<bcs::CScnObjGather *>(pInfo)->Add(this, 0, 1,
                                                               reinterpret_cast<const bcs::CFrustum *>(nParam));
                break;
            case 7:
                Draw();
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<bcs::CG3dObj *>(pInfo));
                break;
        }
    }

    void CGameExDemoMaskObj::SetPosRate(float rate, int n)
    {
        m_fPosRate = rate;
        m_fPosRate = Clamp(m_fPosRate, 0.0f, 1.0f);
        m_nUnk11C = n;
    }

    void CGameExDemoMaskObj::Draw()
    {
        bcs::CCapture *pCapture = bcs::CCapture::GetInstance(0);
        if (pCapture == 0) {
            return;
        }

        short nWidth = bcs::CMainApp::g_Rmode.fbWidth;
        short nHeight = bcs::CMainApp::g_Rmode.efbHeight;
        float fWidth = nWidth;
        float fHeight = nHeight;
        float fHalf = fWidth * 0.5f;

        GXPixModeSync();
        GXLoadTexObj(pCapture->GetTexObj(), GX_TEXMAP0);

        // Each half of the screen is a quad cut 48 pixels past the middle
        // at the top and 48 short of it at the bottom.
        float fSlant = 48.0f;
        float fTexSlant = 0.5f * (fSlant / fHalf);
        float texMtx[3][4] = {
            {0.0f, 0.0f, 0.0f, 0.25f},
            {0.0f, 0.0f, 0.0f, 0.5f},
            {0.0f, 0.0f, 0.0f, 0.0f},
        };
        float fTexScale = 0.75f;
        texMtx[0][0] = fTexScale;
        texMtx[1][1] = fTexScale;

        bcs::Draw2D_SetProjection(-1, 0);
        bcs::Draw2D_SetZModeOff();
        bcs::Draw2D_SetChanVtxColor();

        if (m_nUnk11C != 0) {
            bcs::Draw2D_SetBlendNone();
            bcs::Draw2D_FillRect(0, 0, fWidth, fHeight, bcs::BCS_Color(0xFFFFFFFF));
            GXSetTevColor(GX_TEVREG0, bcs::BCS_Color(0xFFFFFFFF));
        } else {
            bcs::BCS_Color color(0xFFFFFFFF);
            float fRate = Clamp(pow(m_fPosRate, 3.0), 0.0f, 1.0f);
            color.a = Clamp(255.0f * (1.0f - fRate), 0.0f, 255.0f);
            GXSetTevColor(GX_TEVREG0, color);
        }

        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, 0, GX_PTIDENTITY);
        GXSetNumTevStages(1);
        GXSetNumIndStages(0);
        GXSetTevDirect(GX_TEVSTAGE0);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
        GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);

        // The two halves move apart by the rate.
        bcs::FMTX34 mtx;
        bcs::FMTX34Copy(&mtx, &bcs::CRenderMan::m_mtxUnit);
        mtx.m[0][3] = m_fPosRate * (-1.0f * (fHalf + fSlant));
        GXLoadPosMtxImm(mtx.m, GX_PNMTX0);
        mtx.m[0][3] = m_fPosRate * (fHalf + fSlant);
        GXLoadPosMtxImm(mtx.m, GX_PNMTX1);

        GXClearVtxDesc();
        GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XY, GX_F32, 0);
        GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);

        // Left half.
        GXLoadTexMtxImm(texMtx, GX_TEXMTX0, GX_MTX2x4);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(GX_QUADS, GX_VTXFMT7, 4);
        GXPosition2f32(0.0f, 0.0f);
        GXTexCoord2f32(-0.25f, -0.5f);
        GXPosition2f32(fHalf + fSlant, 0.0f);
        GXTexCoord2f32(0.25f + fTexSlant, -0.5f);
        GXPosition2f32(fHalf - fSlant, fHeight);
        GXTexCoord2f32(0.25f - fTexSlant, 0.5f);
        GXPosition2f32(0.0f, fHeight);
        GXTexCoord2f32(-0.25f, 0.5f);
        GXEnd();

        // Right half.
        texMtx[0][3] = 0.75f;
        GXLoadTexMtxImm(texMtx, GX_TEXMTX0, GX_MTX2x4);
        GXSetCurrentMtx(GX_PNMTX1);
        GXBegin(GX_QUADS, GX_VTXFMT7, 4);
        GXPosition2f32(fHalf + fSlant, 0.0f);
        GXTexCoord2f32(-0.25f + fTexSlant, -0.5f);
        GXPosition2f32(fWidth, 0.0f);
        GXTexCoord2f32(0.25f, -0.5f);
        GXPosition2f32(fWidth, fHeight);
        GXTexCoord2f32(0.25f, 0.5f);
        GXPosition2f32(fHalf - fSlant, fHeight);
        GXTexCoord2f32(-0.25f - fTexSlant, 0.5f);
        GXEnd();

        if (m_nUnk11C != 0) {
            // A white edge, fading out, along each cut.
            float fEdge = 0.7f * fHalf * m_fPosRate + 4.0f;

            GXSetNumTexGens(0);
            GXSetNumTevStages(1);
            GXSetNumIndStages(0);
            GXSetTevDirect(GX_TEVSTAGE0);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
            GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
            GXClearVtxDesc();
            GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XY, GX_F32, 0);
            GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
            GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
            GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);

            unsigned long opaque = 0xFFFFFFFF;
            unsigned long clear = 0xFFFFFF00;

            GXSetCurrentMtx(GX_PNMTX0);
            GXBegin(GX_QUADS, GX_VTXFMT7, 4);
            GXPosition2f32((fHalf + fSlant) - fEdge, 0.0f);
            GXColor1u32(clear);
            GXPosition2f32(fHalf + fSlant, 0.0f);
            GXColor1u32(opaque);
            GXPosition2f32(fHalf - fSlant, fHeight);
            GXColor1u32(opaque);
            GXPosition2f32((fHalf - fSlant) - fEdge, fHeight);
            GXColor1u32(clear);
            GXEnd();

            GXSetCurrentMtx(GX_PNMTX1);
            GXBegin(GX_QUADS, GX_VTXFMT7, 4);
            GXPosition2f32(fHalf + fSlant, 0.0f);
            GXColor1u32(opaque);
            GXPosition2f32(fEdge + (fHalf + fSlant), 0.0f);
            GXColor1u32(clear);
            GXPosition2f32(fEdge + (fHalf - fSlant), fHeight);
            GXColor1u32(clear);
            GXPosition2f32(fHalf - fSlant, fHeight);
            GXColor1u32(opaque);
            GXEnd();
        }

        nw4r::g3d::G3DState::Invalidate(0x7FF);
    }

    // ---- CGameExDemoBase ----

    CGameExDemoBase::CGameExDemoBase(bcs::CTask *pParent) : bcs::CTask(pParent, -1) {
        m_nStep = 0;
        m_Unk38 = 0;
        m_fUnk3C = 0.0f;
        m_pMask = 0;

        m_pMask = new (0x100) CGameExDemoMaskObj();
        if (m_pMask != 0) {
            bcs::CRenderMan::PushBack(m_pMask, 8);
        }

        for (int i = 0; i < CAMERA_NUM; i++) {
            m_apCamera[i] = 0;
            m_afFovy[i] = 40.0f;
        }
    }

    CGameExDemoBase::~CGameExDemoBase() {
        for (int i = 0; i < CAMERA_NUM; i++) {
            if (m_apCamera[i] != 0) {
                m_apCamera[i]->Delete();
                m_apCamera[i] = 0;
            }
        }
        if (m_pMask != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pMask);
        }
        m_pMask = 0;
    }

    void CGameExDemoBase::Main() {
        MainProc();
        for (int i = 0; i < CAMERA_NUM; i++) {
            if (m_apCamera[i] != 0) {
                bcs::CWorldEnv::SetViewFovy(i, m_afFovy[i]);
            }
        }
    }

    void CGameExDemoBase::MainProc() {}

    int CGameExDemoBase::CanbeNext() {
        return m_nStep == 1;
    }

    void CGameExDemoBase::SetNext()
    {
        if (CanbeNext()) {
            m_nStep++;
        }
    }

    // ---- CGameScoredView ----

    CGameScoredView::CGameScoredView(bcs::CTask *pParent) : bcs::CTask(pParent, -1), m_nLookChr(-1), m_pCamera(0) {}

    CGameScoredView::~CGameScoredView()
    {
        End();
    }

    void CGameScoredView::Main() {}

    void CGameScoredView::Start(long nTeam)
    {
        int bStart = 0;

        CChrMan::DecideScoredViewLookChr(nTeam);
        m_nLookChr = CChrMan::GetScoredViewLookChr(nTeam);
        CBallMan::AllReset();
        CBallMan::AllSleep();
        CGimmickMan::AllReset();

        bcs::FVEC3 at;
        bcs::FVEC3 camPos;
        if (m_nLookChr >= 0) {
            CChrCore *pChr = CChrMan::GetChrCore(m_nLookChr);
            if (pChr != 0 && pChr->IsEnable()) {
                camPos.Set(-2.0f, 1.0f, 3.0f);
                const bcs::FVEC3 &rPos = pChr->GetPos();
                at.x = rPos.x;
                at.z = rPos.z;
                at.y = 0.0f;

                // Face the goal the team attacks.
                pChr->SetRotY(WrapRad(PI * pChr->GetTargetGoalIdx()));
                switch (CGame::g_GameWork.m_nGameMode) {
                    case CGame::GAME_MODE_BASKET:
                    case CGame::GAME_MODE_DODGE:
                    case CGame::GAME_MODE_HOCKEY:
                        if (pChr->GetTargetGoalIdx() != 0) {
                            if (pChr->GetPos().z < 0.0f) {
                                pChr->SetRotY(0.0f);
                            }
                        } else {
                            if (pChr->GetPos().z > 0.0f) {
                                pChr->SetRotY(PI);
                            }
                        }
                        break;
                }

                if (bcs::BCS_RandMax(1L) != 0) {
                    camPos.x *= -1.0f;
                }

                // Keep the camera off the walls.
                bcs::FVEC3 test;
                bcs::BCS_Vec_RotY(test, camPos, pChr->GetRotY());
                AddVec(test, at);
                float fRadius = 2.0f;
                if (CGame::g_GameWork.GetGameMode() == CGame::GAME_MODE_DODGE && CGame::g_GameWork.GetStage() == 12) {
                    if (fabsf(camPos.x) > 5.8f) {
                        if (camPos.x < 0.0f) {
                            if (camPos.x < at.x) {
                                camPos.x *= -1.0f;
                            }
                        } else {
                            if (camPos.x > at.x) {
                                camPos.x *= -1.0f;
                            }
                        }
                    }
                }
                bcs::FVEC3 hitPos;
                bcs::FVEC3 hitNormal;
                float fHitTime;
                if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, test, test, fRadius, 1, 0) & 6) {
                    camPos.x *= -1.0f;
                }

                bcs::BCS_Vec_RotY(camPos, camPos, pChr->GetRotY());
                AddVec(camPos, at);
                bStart = 1;
                at.y = 1.0f;
            }
        }

        if (bStart) {
            CGame::StopAllCameraQuake();
            m_pCamera = new (0x100) bcs::CCamera(0, 0x80, 0);
            if (m_pCamera != 0) {
                SetChrHidden_(m_nLookChr, camPos);
                CGame::SendMessage(0x17, 0, 0, 0);
                CGame::SendMessage(0x2D, -1, 1, 0);
                bcs::CRenderMan::ReqPushViewSplitType(bcs::CWorldEnv::VIEW_SPLIT_1, 0);
                bcs::CRenderMan::SetDepthOfFieldParam(1, 6.0f, 7.5f, 1.5f);
                bcs::CRenderMan::EnableDepthOfFieldEffect(1);
                m_pCamera->SetPos(camPos);
                m_pCamera->SetAt(at);
                m_pCamera->SetRoll(0.0f);
                m_pCamera->SetActive(-1);
                if (CGame::g_GameWork.GetGameMode() == CGame::GAME_MODE_DODGE) {
                    bcs::CWorldEnv::SetViewFovy(0, 38.0f);
                } else {
                    bcs::CWorldEnv::SetViewFovy(0, 40.0f);
                }
            }
        }
    }

    void CGameScoredView::End()
    {
        if (m_pCamera != 0) {
            m_pCamera->Delete();
            m_pCamera = 0;

            long nChr = CChrMan::GetChrCoreNum();
            for (long i = 0; i < nChr; i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable()) {
                    pChr->SetEvtHidden(0, 0);
                }
            }
            bcs::CRenderMan::EnableDepthOfFieldEffect(0);
            bcs::CRenderMan::ResetDepthOfFieldParam();
            bcs::CRenderMan::ReqPopViewSplitType(0);
        }
    }

    void CGameScoredView::SetChrHidden_(long nLookChr, const bcs::FVEC3 &rCamPos)
    {
        if (nLookChr < 0) {
            return;
        }
        CChrCore *pLook = CChrMan::GetChrCore(nLookChr);
        if (pLook == 0) {
            return;
        }
        if (pLook->IsDisable()) {
            return;
        }

        bcs::FVEC3 lookPos(pLook->GetPos());
        bcs::FVEC3 dir;
        dir.x = rCamPos.x - lookPos.x;
        dir.z = rCamPos.z - lookPos.z;
        dir.y = 0.0f;
        if (dir.x * dir.x + dir.z * dir.z <= 0.0f) {
            return;
        }
        NormalizeVec(dir);

        // A point just behind the character: whoever is on the camera's
        // side of it is hidden.
        bcs::FVEC3 back;
        bcs::FVEC3Scale(&back, &dir, -1.0f);
        back.x += lookPos.x;
        back.z += lookPos.z;

        float zero = 0.0f;
        long nChr = CChrMan::GetChrCoreNum();
        for (long i = 0; i < nChr; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable() || nLookChr == pChr->GetPlayerNo()) {
                continue;
            }

            bcs::FVEC3 pos(pChr->GetPos());
            bcs::FVEC3 ofs;
            ofs.x = back.x - pos.x;
            ofs.z = back.z - pos.z;
            int bHide = 0;
            if (ofs.x * dir.x + ofs.z * dir.z < zero) {
                bHide = 1;
            }
            pChr->SetEvtHidden(bHide, 1);
            if (!bHide && nLookChr != pChr->GetPlayerNo()) {
                pChr->SetSeVolume(0.25f);
                pChr->SetVoiceVolume(0.25f, 0.0f);
            }
        }
    }
}
