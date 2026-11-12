#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"
#include "bcs/BCS_Draw2D.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CFgMdl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgGmk_Karon.hpp"
#include "game/msw/CStgSrt.hpp"
#include "nw4r/g3d/Camera.hpp"
#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXGeometry.h"
#include "revolution/gx/GXPixel.h"
#include "revolution/gx/GXTransform.h"
#include "revolution/mtx.h"

// The cases (crates) of the ship stage (stage 15). In the original one
// file holds the model (CStgGmk_Case_Mdl, from 0x8014BE64), the actor
// (CStgGmk_Case_Act), the driver (CStgGmk_Case_Drv) and the gimmick
// (CStgGmk_Case), with their weak functions and the thunks at the end of
// .text (to 0x80150AB4). Evidence: the gimmick's NewMdl inlines the model's
// constructor, the weak NewAct and NewDrv the actor's and the driver's,
// the vtables are in one .data block and the .sdata2 constants are shared
// by all four.
//
// A case slides across the court with the ship's roll (the stage sets its
// acceleration through SetAccel). It is held within the court's sides
// once it is in; a ball or an item of the right kind hitting it takes a
// life, and out of lives it breaks (the modes make a karon come out),
// then waits GetParam_Interval frames hidden and comes back with a random
// model.

namespace {

    // The three case models; the model's Create uses the last one.
    const msw::CStgMdl::MDL_PARAM s_MdlParam[] = {
        { "s15_box00", "s15_box00", "s15_box00", 0x32C, 0, 0, 0, 0 },
        { "s15_box01", "s15_box01", "s15_box01", 0x32C, 0, 0, 0, 0 },
        { "s15_box02", "s15_box02", "s15_box02", 0x32C, 0, 0, 0, 0 },
    };

    // (name guessed) The work memory of the INACTIVE state: the frames to
    // wait before the respawn (not positive: for ever).
    struct INACTIVE_WORK {
        float m_fInterval;
    };

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;
}

namespace msw {

    CStgGmk_Case_Mdl::CStgGmk_Case_Mdl(CStgGmk *pParent, long type) : CStgMdl(pParent), m_pDebugObj(0), m_nType(type)
    {
    }

    CStgGmk_Case_Mdl::~CStgGmk_Case_Mdl()
    {
        if (m_pDebugObj != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pDebugObj);
        }
        m_pDebugObj = 0;
    }

    void CStgGmk_Case_Mdl::Create()
    {
        CStgMdl::Create(CGenericMVAGimmickCreator(CLoadData::GetRes(0), &s_MdlParam[2]));
    }

    // A model that fades out when the camera looks at it from above
    // (msw::CFgMdl).
    bcs::CMdlObj *CStgGmk_Case_Mdl::NewMdl(long heap) const
    {
        CFgMdl *pMdl = new (heap) CFgMdl;
        if (pMdl != 0) {
            pMdl->SetAlphaTestNearRange(10000.0f);
            pMdl->SetAlphaTestFarRange(10000.1f);
            pMdl->SetUnk214And218(0.8f);
            pMdl->SetExBlendMode(1);
            pMdl->CalcBoundingVolume();
            pMdl->OnFlag(0x10000);
        }
        return pMdl;
    }

    bcs::CScnObj *CStgGmk_Case_Mdl::GetScnObj()
    {
        return CStgMdl::GetMdlObj();
    }

    const bcs::CScnObj *CStgGmk_Case_Mdl::GetScnObj() const
    {
        return CStgMdl::GetMdlObj();
    }

    void CStgGmk_Case_Mdl::OnPostCreate()
    {
    }

    // The model is scaled to the actor's size. Its fade faces the camera
    // side of the court: towards +z for a case on the far half, -z on the
    // near half.
    void CStgGmk_Case_Mdl::Main(float rate)
    {
        CStgGmk_Case_Act *pAct = static_cast<CStgGmk_Case_Act *>(GetParent()->GetAct());
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pAct != 0 && pMdl != 0) {
            float depth = pAct->GetParam_Depth() / pAct->GetParam_MdlDepth();
            float height = pAct->GetParam_Height() / pAct->GetParam_MdlHeight();
            pMdl->m_vScale.x = pAct->GetParam_Width() / pAct->GetParam_MdlWidth();
            pMdl->m_vScale.y = height;
            pMdl->m_vScale.z = depth;
            pMdl->CalcRootMtx();
            pMdl->CalcWorldMtx(0);
            if (pMdl != 0) {
                CFgMdl *pFg = static_cast<CFgMdl *>(pMdl);
                bcs::FVEC3 pos = pAct->GetSrt()->GetPos();
                const bcs::FMTX34 &rMtx = GetMtx();
                pos.Set(rMtx.m[0][3], rMtx.m[1][3], rMtx.m[2][3]);
                if (pos.z > 0.0f) {
                    pFg->SetFadeDir(0.0f);
                } else {
                    pFg->SetFadeDir(PI);
                }
                pFg->SetFadeAngle1(1.3962635f);
                pFg->SetFadeAngle0(1.7453293f);
                pFg->SetUnk214And218(0.8f);
                pFg->SetFadeEnable(1);
            }
        }
        CStgMdl::Main(rate);
    }

    void CStgGmk_Case_Mdl::Disp(long nView)
    {
        CStgGmk_Case_Act *pAct = static_cast<CStgGmk_Case_Act *>(GetParent()->GetAct());
        // (the original clears a vector it never uses)
        bcs::FVEC3 unused(0.0f, 0.0f, 0.0f);
        float x = 0.5f * pAct->GetParam_Width();
        float z = 0.5f * pAct->GetParam_Depth();
        float y = pAct->GetParam_Height();
        bcs::BCS_Color face(0xFFC00080);
        bcs::BCS_Color edge(0x80600040);
        // The corners: 0 to 3 on the +z side, 4 to 7 on the -z side, the
        // bottom at the case's position.
        bcs::FVEC3 aPos[8] = {
            bcs::FVEC3(-x, y, z), bcs::FVEC3(x, y, z),   bcs::FVEC3(x, 0.0f, z),   bcs::FVEC3(-x, 0.0f, z),
            bcs::FVEC3(x, y, -z), bcs::FVEC3(-x, y, -z), bcs::FVEC3(-x, 0.0f, -z), bcs::FVEC3(x, 0.0f, -z),
        };
        if (m_pDebugObj == 0) {
            return;
        }

        nw4r::g3d::Camera camera(reinterpret_cast<nw4r::g3d::CameraData *>(bcs::CWorldEnv::GetViewCameraData(nView)));
        camera.GXSetViewport();
        camera.GXSetProjection();
        camera.GXSetScissor();
        camera.GXSetScissorBoxOffset();
        bcs::FMTX34 mtx;
        nw4r::math::MTX34 viewMtx;
        camera.GetCameraMtx(&viewMtx);
        m_pDebugObj->GetMtx(bcs::CScnObj::SCNOBJMTXTYPE_1, mtx);
        PSMTXConcat(reinterpret_cast<const float(*)[4]>(&viewMtx), mtx.m, mtx.m);
        GXLoadPosMtxImm(mtx.m, 0);
        GXSetCurrentMtx(0);
        GXSetCullMode(GX_CULL_BACK);
        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_TRUE);
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        bcs::Draw2D_SetBlendAlpha();

        // The faces, translucent orange.
        bcs::BCS_GX_DrawQuad(aPos[0], aPos[1], aPos[2], aPos[3], face, face, face, face);
        bcs::BCS_GX_DrawQuad(aPos[1], aPos[4], aPos[7], aPos[2], face, face, face, face);
        bcs::BCS_GX_DrawQuad(aPos[4], aPos[5], aPos[6], aPos[7], face, face, face, face);
        bcs::BCS_GX_DrawQuad(aPos[5], aPos[0], aPos[3], aPos[6], face, face, face, face);
        bcs::BCS_GX_DrawQuad(aPos[5], aPos[4], aPos[1], aPos[0], face, face, face, face);
        bcs::BCS_GX_DrawQuad(aPos[3], aPos[2], aPos[7], aPos[6], face, face, face, face);

        // The edges.
        GXSetLineWidth(0x12, GX_TO_ZERO);
        bcs::BCS_GX_DrawLine(aPos[0].x, aPos[0].y, aPos[0].z, aPos[1].x, aPos[1].y, aPos[1].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[1].x, aPos[1].y, aPos[1].z, aPos[2].x, aPos[2].y, aPos[2].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[2].x, aPos[2].y, aPos[2].z, aPos[3].x, aPos[3].y, aPos[3].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[3].x, aPos[3].y, aPos[3].z, aPos[0].x, aPos[0].y, aPos[0].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[4].x, aPos[4].y, aPos[4].z, aPos[5].x, aPos[5].y, aPos[5].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[5].x, aPos[5].y, aPos[5].z, aPos[6].x, aPos[6].y, aPos[6].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[6].x, aPos[6].y, aPos[6].z, aPos[7].x, aPos[7].y, aPos[7].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[7].x, aPos[7].y, aPos[7].z, aPos[4].x, aPos[4].y, aPos[4].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[0].x, aPos[0].y, aPos[0].z, aPos[5].x, aPos[5].y, aPos[5].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[1].x, aPos[1].y, aPos[1].z, aPos[4].x, aPos[4].y, aPos[4].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[2].x, aPos[2].y, aPos[2].z, aPos[7].x, aPos[7].y, aPos[7].z, edge, edge);
        bcs::BCS_GX_DrawLine(aPos[3].x, aPos[3].y, aPos[3].z, aPos[6].x, aPos[6].y, aPos[6].z, edge, edge);
    }

    CStgGmk_Case_Act::CStgGmk_Case_Act(CStgGmk *pParent)
        : CStgGmkAct(pParent), m_bAlive(0), m_fInvincibleTime(0.0f), m_fBreakTimer(-1.0f)
    {
        FsmqInit(this, 7, 4, sizeof(INACTIVE_WORK));
        FsmqSetState(STATE_DEFAULT, &CStgGmk_Case_Act::CtorState_DEFAULT, &CStgGmk_Case_Act::LoopState_DEFAULT,
                     &CStgGmk_Case_Act::DtorState_DEFAULT);
        FsmqSetState(STATE_STANDBY, &CStgGmk_Case_Act::CtorState_STANDBY, &CStgGmk_Case_Act::LoopState_STANDBY,
                     &CStgGmk_Case_Act::DtorState_STANDBY);
        FsmqSetState(STATE_MOVE, &CStgGmk_Case_Act::CtorState_MOVE, &CStgGmk_Case_Act::LoopState_MOVE,
                     &CStgGmk_Case_Act::DtorState_MOVE);
        FsmqSetState(STATE_BREAK, &CStgGmk_Case_Act::CtorState_BREAK, &CStgGmk_Case_Act::LoopState_BREAK,
                     &CStgGmk_Case_Act::DtorState_BREAK);
        FsmqSetState(STATE_RESPAWN, &CStgGmk_Case_Act::CtorState_RESPAWN, &CStgGmk_Case_Act::LoopState_RESPAWN,
                     &CStgGmk_Case_Act::DtorState_RESPAWN);
        FsmqSetState(STATE_INACTIVE, &CStgGmk_Case_Act::CtorState_INACTIVE, &CStgGmk_Case_Act::LoopState_INACTIVE,
                     &CStgGmk_Case_Act::DtorState_INACTIVE);
        FsmqSetState(STATE_OPEN, &CStgGmk_Case_Act::CtorState_OPEN, &CStgGmk_Case_Act::LoopState_OPEN,
                     &CStgGmk_Case_Act::DtorState_OPEN);
        CStgSrtMove *pSrt = GetSrt();
        pSrt->ResetVelocity(GetParam_Gravity());

        float fullWidth = GetParam_Width();
        float fullDepth = GetParam_Depth();
        float width = 0.5f * fullWidth;
        float depth = 0.5f * fullDepth;
        float height = GetParam_Height();
        float radius = width;
        if (depth < width) {
            radius = depth;
        }
        CStgMdl *pMdl = GetMdl(0);
        bcs::CMdlObj *pMdlObj = pMdl != 0 ? pMdl->GetMdlObj() : 0;

        // Body units 0 to 3: the four walls, (i & 2) the ones across x.
        m_BodyColl.Create(8, 5, 3, 0);
        if (pMdlObj != 0) {
            m_BodyColl.SetOwnerHandleAll(pMdlObj->GetHandle());
        }
        float zero = 0.0f;
        for (long i = 0; i <= 3; i++) {
            m_BodyColl.SetCollType(i, 2);
            if (i & 2) {
                m_BodyColl.SetSize(i, bcs::FVEC3(width, height, zero));
            } else {
                m_BodyColl.SetSize(i, bcs::FVEC3(zero, height, depth));
            }
            m_BodyColl.SetPushRank(i, 5);
            m_BodyColl.SetPushPower(i, 6);
            m_BodyColl.SetTargetFlag(i, 0x22);
        }
        // Units 5 to 7: three spheres along the case.
        for (long i = 0; i < 3; i++) {
            long idx = i + 5;
            m_BodyColl.SetCollType(idx, 1);
            m_BodyColl.SetRadius(idx, radius);
            m_BodyColl.SetPushRank(idx, 5);
            m_BodyColl.SetPushPower(idx, 6);
            m_BodyColl.SetTargetFlag(idx, 0x26);
        }
        // Unit 4: the box on top that catches the ball.
        m_BodyColl.SetCollType(4, 4);
        m_BodyColl.SetSize(4, bcs::FVEC3(width, 0.5f * height, depth));
        m_BodyColl.SetPushRank(4, 5);
        m_BodyColl.SetPushPower(4, 6);
        m_BodyColl.SetTargetFlag(4, 0x14);
        RegistBodyPushCallback(m_BodyColl);
        RegistBodyDamageCallback(m_BodyColl);
        RegistBallCatchCallback(m_BodyColl);

        m_DefColl.Create(1, 5, 4, 0);
        m_DefColl.GetColl(0)->SetCollType(4);
        m_DefColl.GetColl(0)->SetShape(4);
        m_DefColl.GetColl(0)->SetSize(bcs::FVEC3(width, height, depth));
        m_DefColl.GetColl(0)->OnFlagA0(1);
        RegistReflectHitCallback(m_DefColl);

        m_AtkColl.Create(3, 5, 0, 0);
        if (depth < width) {
            width = depth;
        }
        float atkRadius = 0.9f * width;
        for (long i = 0; i < 3; i++) {
            m_AtkColl.SetCollType(i, 3);
            m_AtkColl.SetTargetFlag(i, 2);
            m_AtkColl.SetAtkType(i, 0);
            m_AtkColl.SetRadius(i, atkRadius);
            RegistAttackHitCallback(m_AtkColl);
            m_AtkColl.SetEnable(i);
        }
    }

    CStgGmk_Case_Act::~CStgGmk_Case_Act()
    {
    }

    // Runs the states and moves the case by its velocity; it never sinks
    // below 0.05. ForceBreak's delay counts down while the case is alive.
    void CStgGmk_Case_Act::OnUpdate(float rate)
    {
        CStgSrtMove *pSrt = GetSrt();
        CStageMan::GetInstance();
        if (m_fInvincibleTime > 0.0f) {
            m_fInvincibleTime -= rate;
        }
        if (m_fBreakTimer >= 0.0f && IsAlive()) {
            m_fBreakTimer -= rate;
            if (m_fBreakTimer <= 0.0f) {
                m_fBreakTimer = -1.0f;
                FsmqChangeState(STATE_BREAK);
                FsmqRetry();
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
        UpdateColl();
    }

    // Every model follows the actor; the body units belong to the model in
    // use (the gimmick's sub-type).
    void CStgGmk_Case_Act::UpdateMdlMtx()
    {
        CStgGmk_Case *pParent = static_cast<CStgGmk_Case *>(GetParent());
        for (long i = 0; i < 3; i++) {
            CStgMdl *pMdl = GetParent()->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetMtx(GetSrt()->GetMtx());
                if (i == pParent->GetSubType()) {
                    bcs::CMdlObj *pMdlObj = pMdl->GetMdlObj();
                    if (pMdlObj != 0) {
                        m_BodyColl.SetOwnerHandleAll(pMdlObj->GetHandle());
                    }
                }
            }
        }
    }

    // While the case is alive its collisions follow it: four walls (8 high)
    // round it, three spheres and three attack capsules along its longer
    // side, the catch box on top and, when IsEnableDefColl, the reflecting
    // box. Otherwise they are all off.
    void CStgGmk_Case_Act::UpdateColl()
    {
        const CStgSrt *pSrt = GetSrt();
        if (m_bAlive) {
            float width = GetParam_Width();
            float depth = GetParam_Depth();
            float halfWidth = 0.5f * width;
            float halfDepth = 0.5f * depth;
            float height = GetParam_Height();
            float wallHeight = 8.0f;

            m_BodyColl.SetEnableAll();
            bcs::FVEC3 pos = pSrt->GetPos();
            for (long i = 0; i <= 3; i++) {
                bcs::FVEC3 wallPos = pos;
                if (i & 2) {
                    wallPos.z += (i & 1) ? halfDepth : -halfDepth;
                    m_BodyColl.SetSize(i, bcs::FVEC3(halfWidth, wallHeight, 0.0f));
                } else {
                    wallPos.x += (i & 1) ? halfWidth : -halfWidth;
                    m_BodyColl.SetSize(i, bcs::FVEC3(0.0f, wallHeight, halfDepth));
                }
                m_BodyColl.SetPos(i, wallPos);
                m_BodyColl.SetPushEnable(i);
            }

            float radius = halfWidth;
            if (halfDepth < halfWidth) {
                radius = halfDepth;
            }
            float span = width < depth ? depth - width : width - depth;
            float halfSpan = 0.5f * span;
            float scale = 1.0f;
            // (a variable: the original converts sphereNum - 1 to float at
            // run time and divides)
            long sphereNum = 3;
            for (long i = 0; i < sphereNum; i++) {
                long idx = i + 5;
                bcs::FVEC3 spherePos = pos;
                float ofs = span * ((float)i / (float)(sphereNum - 1)) - halfSpan;
                if (width < depth) {
                    spherePos.z += ofs;
                } else {
                    spherePos.x += ofs;
                }
                m_BodyColl.SetPos(idx, spherePos);
                m_BodyColl.SetSize(idx, radius * scale, height > 0.0f ? height : 0.0f, 0.0f);
                m_BodyColl.SetPushEnable(idx);
            }

            float halfHeight = 0.5f * height;
            bcs::FVEC3 catchPos = pos;
            bcs::FVEC3 catchSize = m_BodyColl.GetColl(4)->GetSize();
            catchPos.y += catchSize.y;
            m_BodyColl.SetPos(4, catchPos);
            m_BodyColl.SetSize(4, bcs::FVEC3(halfWidth, halfHeight, halfDepth));
            m_BodyColl.SetBodyEnable(4);
            m_BodyColl.SetCatchEnable(4);

            if (IsEnableDefColl()) {
                float size = 0.5f * (halfDepth - halfWidth) + halfWidth;
                bcs::FVEC3 defPos(pos.x, 0.5f * height + pos.y, pos.z);
                m_DefColl.GetColl(0)->SetPos(defPos);
                float defHeight = 0.0f;
                if (0.5f * height > defHeight) {
                    defHeight = 0.5f * height;
                }
                m_DefColl.GetColl(0)->SetSize(size, defHeight, size);
                m_DefColl.GetColl(0)->OnFlagA0(1);
                m_DefColl.GetColl(0)->OnFlag10(1);
            } else {
                m_DefColl.GetColl(0)->OffFlag10(1);
                m_DefColl.GetColl(0)->OffFlagA0(1);
            }

            if (halfDepth < halfWidth) {
                halfWidth = halfDepth;
            }
            float atkRadius = 0.9f * halfWidth;
            float atkSpan = width < depth ? depth - width : width - depth;
            float atkLen = 0.0f;
            if (height - 2.0f * atkRadius > atkLen) {
                atkLen = height - 2.0f * atkRadius;
            }
            float atkY = pos.y + atkRadius;
            float atkHalfSpan = 0.5f * atkSpan;
            for (long i = 0; i < 3; i++) {
                bcs::FVEC3 atkPos(pos.x, atkY, pos.z);
                float ofs = (float)i * 0.5f * atkSpan - atkHalfSpan;
                if (width < depth) {
                    atkPos.z += ofs;
                } else {
                    atkPos.x += ofs;
                }
                m_AtkColl.SetPos(i, atkPos);
                atkPos.y += atkLen;
                m_AtkColl.SetPos2(i, atkPos);
                m_AtkColl.SetRadius(i, atkRadius);
                m_AtkColl.SetEnable(i);
            }
        } else {
            m_BodyColl.SetDisableAll();
            m_AtkColl.SetDisableAll();
            m_DefColl.SetDisableAll();
        }
    }

    // A push is passed on with the part of it the map lets through.
    void CStgGmk_Case_Act::OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (pPush != 0) {
            const bcs::FVEC3 &rPos = GetSrt()->GetPos();
            bcs::FVEC3 from = rPos;
            bcs::FVEC3 to(from.x + pPush->x, from.y, from.z + pPush->z);
            bcs::FVEC3 hitPos;
            bcs::FVEC3 push;
            float time;
            if (CCollMan::DetectMapHit(hitPos, push, time, from, to, 2.0f, 1, 0)) {
                push.x = hitPos.x - to.x;
                push.z = hitPos.z - to.z;
                pOther->DoBodyPushCallback(pBody, &push);
            } else {
                pOther->DoBodyPushCallback(pBody, 0);
            }
        } else {
            pOther->DoBodyPushCallback(pBody, 0);
        }
    }

    // Items of attack kinds 0x1C, 0x1D and 0x1F hurt the case (unless their
    // sub-kind is 4 or 0xD); with GetParam_ItemHitMethod 1 such a hit breaks
    // it at once. Kinds 0x1E and 0x21 pass.
    unsigned long CStgGmk_Case_Act::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                 const bcs::FVEC3 &rPos)
    {
        int bDamage = 0;
        if (!IsInvincible()) {
            if (pAtk != 0 && pAtk->GetOwnerKind() == 4 && GetParam_ItemHitMethod() != 0) {
                unsigned long atkType = pAtk->m_nAtkType;
                switch ((atkType >> 8) & 0xFF) {
                    case 0x1C:
                    case 0x1D:
                    case 0x1F:
                        bDamage = 1;
                        break;
                    case 0x1E:
                    case 0x21:
                        return 0;
                }
                unsigned long sub = atkType & 0xFF;
                if (sub == 0xD || sub == 4) {
                    return 0;
                }
            }
            if (bDamage) {
                bcs::FVEC3 pos = rPos;
                long eff = GetParam_Effect(2);
                if (eff >= 0) {
                    CEffectCtrl::SimpleEffect(eff, 0, &pos, 0, 3, 0, 0, 0);
                }
                long se = GetParam_SE(2);
                if (se >= 0) {
                    CGameSnd::Play(se, pos, 1, -1, 0.0f, 1.0f, 0.0f);
                }
                m_fInvincibleTime = 30.0f;
                if (GetParam_ItemHitMethod() == 1) {
                    FsmqChangeState(STATE_BREAK);
                }
                return 1;
            }
        }
        return 2;
    }

    // A ball on the attack hurts the case.
    void CStgGmk_Case_Act::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (GetState() != STATE_BREAK && pBall->IsOffensive() && DoDamage(pBall->GetPos(), 1, 1)) {
            OnDamage(pBody, pBall);
        }
    }

    // A hit on a character shakes the camera.
    void CStgGmk_Case_Act::OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                       unsigned long flag)
    {
        if (pBody != 0 && pBody->GetOwnerKind() == 1) {
            long quake = GetParam_HitCameraQuake();
            long idx = pBody->m_nOwnerIdx;
            if (quake >= 0 && idx >= 0) {
                CGame::StartCameraQuake(quake, idx, -1);
            }
        }
    }

    int CStgGmk_Case_Act::DoDamage(const bcs::FVEC3 &rPos, int bEffect, int bSE)
    {
        int bDamage = 0;
        if (!IsInvincible()) {
            if (m_fMaxLife > 0.0f) {
                m_fLife -= 1.0f;
            }
            bcs::FVEC3 pos = rPos;
            if (bEffect) {
                long eff = GetParam_Effect(2);
                if (eff >= 0) {
                    CEffectCtrl::SimpleEffect(eff, 0, &pos, 0, 3, 0, 0, 0);
                }
            }
            if (bSE) {
                long se = GetParam_SE(2);
                if (se >= 0) {
                    CGameSnd::Play(se, pos, 1, -1, 0.0f, 1.0f, 0.0f);
                }
            }
            m_fInvincibleTime = 30.0f;
            if (m_fLife <= 0.0f && m_fMaxLife > 0.0f) {
                FsmqChangeState(STATE_BREAK);
            }
            bDamage = 1;
        }
        return bDamage;
    }

    float CStgGmk_Case_Act::GetParam_Width() const
    {
        return 2.0f;
    }

    float CStgGmk_Case_Act::GetParam_Depth() const
    {
        return 2.0f;
    }

    float CStgGmk_Case_Act::GetParam_Height() const
    {
        return 2.0f;
    }

    float CStgGmk_Case_Act::GetParam_Acc() const
    {
        return 0.0f;
    }

    float CStgGmk_Case_Act::GetParam_Dec() const
    {
        return 0.0f;
    }

    float CStgGmk_Case_Act::GetParam_VelLimit() const
    {
        return 80.0f;
    }

    float CStgGmk_Case_Act::GetParam_Gravity() const
    {
        return -49.0f;
    }

    float CStgGmk_Case_Act::GetParam_Interval() const
    {
        return 300.0f;
    }

    void CStgGmk_Case_Act::GetParam_CourtRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
    {
        CStageMan::GetInstance()->GetCourtRect(pLeft, pRight, pFront, pBack);
    }

    // The court's depth and the ground's width.
    void CStgGmk_Case_Act::GetParam_OuterRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
    {
        CStageMan::GetInstance()->GetCourtRect(0, 0, pFront, pBack);
        CStageMan::GetInstance()->GetGroundRect(pLeft, pRight, 0, 0);
    }

    // One of the three models at random.
    long CStgGmk_Case_Act::GetParam_MdlIdx() const
    {
        return bcs::BCS_RandMax(2L);
    }

    long CStgGmk_Case_Act::GetParam_Effect(long type) const
    {
        return -1;
    }

    long CStgGmk_Case_Act::GetParam_SE(long type) const
    {
        return -1;
    }

    float CStgGmk_Case_Act::GetParam_MaxLife() const
    {
        return 3.0f;
    }

    long CStgGmk_Case_Act::GetParam_HitCameraQuake() const
    {
        return 0x2D;
    }

    float CStgGmk_Case_Act::GetParam_BreakTime() const
    {
        return 3.0f;
    }

    // Hidden until the interval has run (INACTIVE).
    void CStgGmk_Case_Act::Inactivate()
    {
        static_cast<INACTIVE_WORK *>(FsmqSetState(STATE_INACTIVE))->m_fInterval = GetParam_Interval();
    }

    // Breaks the case after delay frames.
    void CStgGmk_Case_Act::ForceBreak(float delay)
    {
        if (IsAlive()) {
            m_fBreakTimer = delay;
        }
    }

    long CStgGmk_Case_Act::GetSubType_() const
    {
        const CStgGmk_Case *pParent = static_cast<const CStgGmk_Case *>(GetParent());
        if (pParent != 0) {
            return pParent->GetSubType();
        }
        return 0;
    }

    // A karon comes out where the case was.
    void CStgGmk_Case_Act::CreateKaron()
    {
        bcs::FVEC3 pos = GetSrt()->GetPos();
        float scale = GetParam_KaronScale();
        float collScale = GetParam_KaronCollScale();
        float speed = GetParam_KaronSpeed();
        CStgGmk_Karon *pKaron = CGimmickMan::StgGmkCreate_Karon(pos);
        if (pKaron != 0) {
            pKaron->SetParam(scale, speed, collScale, -1.0f);
            pKaron->Start();
        }
    }

    // Shown at the gimmick's position, moved GetParam_LocOffsetZ away from
    // the court's middle line, with full lives; then it moves.
    void CStgGmk_Case_Act::CtorState_DEFAULT()
    {
        CStgSrtMove *pSrt = GetSrt();
        CStgGmk_Case *pParent = static_cast<CStgGmk_Case *>(GetParent());
        pSrt->SetGravity(GetParam_Gravity());
        pSrt->ClearUnkE4();
        m_bAlive = 1;
        pParent->GetMdl(0)->SetVisible(1);
        m_bInCourt = 0;
        bcs::FVEC3 pos = pParent->GetDefaultPos();
        float ofs = GetParam_LocOffsetZ();
        ofs = pos.z > 0.0f ? ofs : -ofs;
        pos.z += ofs;
        pSrt->SetPos(pos);
        m_fLife = m_fMaxLife = GetParam_MaxLife();
        m_fInvincibleTime = 0.0f;
        m_fBreakTimer = -1.0f;
        FsmqChangeState(STATE_MOVE);
        FsmqRetry();
    }

    void CStgGmk_Case_Act::LoopState_DEFAULT()
    {
    }

    void CStgGmk_Case_Act::DtorState_DEFAULT()
    {
    }

    void CStgGmk_Case_Act::CtorState_STANDBY()
    {
        GetSrt();
        m_fStateTimer = 180.0f;
    }

    void CStgGmk_Case_Act::LoopState_STANDBY()
    {
        m_fStateTimer -= GetFrameRateF();
        FsmqChangeState(STATE_MOVE);
        FsmqRetry();
    }

    void CStgGmk_Case_Act::DtorState_STANDBY()
    {
    }

    // The case slides with the acceleration set from outside, up to
    // GetParam_VelLimit on each axis, slowed by GetParam_Dec.
    void CStgGmk_Case_Act::CtorState_MOVE()
    {
        CStgSrtMove *pSrt = GetSrt();
        pSrt->StopRun();
        pSrt->SetVelLimit(GetParam_VelLimit());
        pSrt->SetDec(GetParam_Dec());
    }

    // While the case slides on the floor the dust effect runs, turned to
    // the direction of the move and scaled to the case, with the sliding
    // sound. Until it is in the court the case is held within the outer
    // rectangle; then within the court's sides, where it stops with a bump.
    void CStgGmk_Case_Act::LoopState_MOVE()
    {
        GetFrameRateF();
        CStgSrtMove *pSrt = GetSrt();
        bcs::FVEC3 vel = pSrt->GetVel();
        float speed = PSVECMag(reinterpret_cast<const Vec *>(&vel));
        if (pSrt->GetPos().y <= 0.05f && speed > 0.7f) {
            if (!Eff_IsAlive(0)) {
                Eff_CreateAttach(0, GetParam_Effect(1), 0, 0UL, 0, 0, 0, 0, 3);
                CEffectCluster *pEff = Eff_GetObject(0);
                if (pEff != 0) {
                    bcs::FVEC3 dir(vel.x, 0.0f, vel.z);
                    bcs::FVEC3 rot(0.0f, 0.0f, 0.0f);
                    bcs::FVEC3 front(0.0f, 0.0f, 1.0f);
                    bcs::FVEC3 unitDir;
                    bcs::BCS_Vec_Normalize(unitDir, dir);
                    rot.y = bcs::BCS_Vec_GetRot(front, unitDir);
                    bcs::FVEC3 cross;
                    PSVECCrossProduct(reinterpret_cast<const Vec *>(&front), reinterpret_cast<const Vec *>(&unitDir),
                                      reinterpret_cast<Vec *>(&cross));
                    if (cross.y < 0.0f) {
                        rot.y *= -1.0f;
                    }
                    float rotY = rot.y;
                    while (rotY > PI2) {
                        rotY -= PI2;
                    }
                    while (rotY < 0.0f) {
                        rotY += PI2;
                    }
                    rot.y = rotY;
                    pEff->SetRotateRad(&rot);
                    float width = GetParam_Width();
                    float depth = GetParam_Depth();
                    bcs::FVEC3 scale(depth, depth, width);
                    pEff->SetEmitterScale(&scale);
                }
            }
            if (!SE_IsAlive(0)) {
                SE_PlayPos(0, GetParam_SE(1), 0.0f, 1, 0);
            }
        } else {
            Eff_Stop(0);
            SE_Stop(0, 0.0f);
        }

        float halfWidth = 0.5f * GetParam_Width();
        GetParam_Depth();
        bcs::FVEC3 pos = pSrt->GetPos();
        // (the original copies the acceleration and never uses it)
        bcs::FVEC3 accel = pSrt->GetAccel();
        float courtLeft;
        float courtRight;
        GetParam_CourtRect(&courtLeft, &courtRight, 0, 0);
        courtLeft += halfWidth;
        courtRight -= halfWidth;
        float outerLeft;
        float outerRight;
        GetParam_OuterRect(&outerLeft, &outerRight, 0, 0);
        outerLeft += halfWidth;
        outerRight -= halfWidth;
        int bStop = 0;
        if (m_bInCourt) {
            if (vel.x < 0.0f && pos.x < courtLeft) {
                pos.x = courtLeft;
                bStop = 1;
            } else if (vel.x > 0.0f && pos.x > courtRight) {
                pos.x = courtRight;
                bStop = 1;
            }
        } else {
            if (pos.x < outerLeft || pos.x > outerRight) {
                pos.x = pos.x > outerRight ? outerRight : (pos.x < outerLeft ? outerLeft : pos.x);
                bStop = 1;
            }
            if (pos.x > courtLeft && pos.x < courtRight) {
                m_bInCourt = 1;
            }
        }
        if (bStop) {
            pSrt->SetPos(pos);
            vel.x = 0.0f;
            vel.z = 0.0f;
            pSrt->SetVel(vel);
            if (speed > 0.7f) {
                long se = GetParam_SE(3);
                if (se >= 0) {
                    CGameSnd::Play(se, pSrt->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
                }
            }
        }
    }

    void CStgGmk_Case_Act::DtorState_MOVE()
    {
        GetSrt()->StopRun();
        Eff_Stop(0);
        SE_Stop(0, 0.0f);
    }

    // Hidden at once, with the break effect (scaled to the case's larger
    // side) and sound; OnBreak lets the mode add to it.
    void CStgGmk_Case_Act::CtorState_BREAK()
    {
        CStgSrt *pSrt = GetSrt();
        m_bAlive = 0;
        GetParent()->GetMdl(0)->SetVisible(0);
        m_fStateTimer = GetParam_BreakTime();
        m_fBreakTimer = -1.0f;
        bcs::FVEC3 pos = pSrt->GetPos();
        long eff = GetParam_Effect(0);
        if (eff >= 0) {
            unsigned long hEffect = CEffectCtrl::CreateAttachEffect(eff, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0);
            CEffectCluster *pEff = hEffect != 0 ? CEffectCtrl::GetInstance()->FindList(hEffect) : 0;
            if (pEff != 0) {
                float size = GetParam_Width();
                float depth = GetParam_Depth();
                size = size > depth ? size : depth;
                bcs::FVEC3 scale(size, size, size);
                pEff->SetTranslate(&pos);
                pEff->SetScale(&scale);
            }
        }
        long se = GetParam_SE(0);
        if (se >= 0) {
            CGameSnd::Play(se, pos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        OnBreak();
    }

    void CStgGmk_Case_Act::LoopState_BREAK()
    {
        m_fStateTimer -= GetFrameRateF();
        if (m_fStateTimer <= 0.0f) {
            m_bAlive = 0;
            static_cast<INACTIVE_WORK *>(FsmqSetState(STATE_INACTIVE))->m_fInterval = GetParam_Interval();
            FsmqRetry();
        }
    }

    void CStgGmk_Case_Act::DtorState_BREAK()
    {
    }

    // Shown again at the gimmick's position with a new model.
    void CStgGmk_Case_Act::CtorState_RESPAWN()
    {
        CStgSrt *pSrt = GetSrt();
        CStgGmk_Case *pParent = static_cast<CStgGmk_Case *>(GetParent());
        CStgMdl *pMdl = GetMdl(0);
        m_bAlive = 1;
        if (pMdl != 0) {
            pMdl->SetVisible(1);
        }
        bcs::FVEC3 pos = pParent->GetDefaultPos();
        float ofs = GetParam_LocOffsetZ();
        pos.y = GetParam_LocPosY();
        ofs = pos.z > 0.0f ? ofs : -ofs;
        pos.z += ofs;
        pSrt->SetPos(pos);
        if (pMdl != 0 && pParent != 0) {
            long type = GetParam_MdlIdx();
            pParent->SetSubType(type);
            static_cast<CStgGmk_Case_Mdl *>(pMdl)->SetType(type);
            pMdl->Create();
            pMdl->SceneEntry();
        }
        m_bInCourt = 0;
        m_fLife = m_fMaxLife = GetParam_MaxLife();
        m_fInvincibleTime = 0.0f;
        FsmqChangeState(STATE_MOVE);
        FsmqRetry();
    }

    void CStgGmk_Case_Act::LoopState_RESPAWN()
    {
    }

    void CStgGmk_Case_Act::DtorState_RESPAWN()
    {
    }

    void CStgGmk_Case_Act::CtorState_INACTIVE()
    {
        CStgMdl *pMdl = GetMdl(0);
        m_bAlive = 0;
        if (pMdl != 0) {
            pMdl->SetVisible(0);
        }
        m_fStateTimer = 0.0f;
    }

    // While the gimmicks run, counts up to the interval in the work memory,
    // then respawns. A case without an interval stays hidden.
    void CStgGmk_Case_Act::LoopState_INACTIVE()
    {
        INACTIVE_WORK *pWork = FsmqGetWork<INACTIVE_WORK>();
        float rate = GetFrameRateF();
        if (pWork->m_fInterval > 0.0f && CStageMan::GetInstance()->IsGimmickRunning()) {
            m_fStateTimer += rate;
            if (m_fStateTimer >= pWork->m_fInterval) {
                FsmqChangeState(STATE_RESPAWN);
                FsmqRetry();
            }
        }
    }

    void CStgGmk_Case_Act::DtorState_INACTIVE()
    {
    }

    void CStgGmk_Case_Act::CtorState_OPEN()
    {
        FsmqRetry();
    }

    void CStgGmk_Case_Act::LoopState_OPEN()
    {
    }

    void CStgGmk_Case_Act::DtorState_OPEN()
    {
    }

    CStgGmk_Case_Drv::CStgGmk_Case_Drv(CStgGmk *pParent) : CStgDrv(pParent) {}

    CStgGmk_Case_Drv::~CStgGmk_Case_Drv() {}

    void CStgGmk_Case_Drv::Main(float rate)
    {
        CStgDrv::Main(rate);
    }

    CStgGmk_Case::CStgGmk_Case(bcs::CTask *pParent, int type) : CStgGmk(pParent, 1), m_nSubType(type)
    {
        SetType(5);
    }

    CStgGmk_Case::~CStgGmk_Case() {}

    CStgMdl *CStgGmk_Case::NewMdl(long heap, unsigned long idx)
    {
        return new (heap) CStgGmk_Case_Mdl(this, 0);
    }

    void CStgGmk_Case::OnPostCreate() {}

    void CStgGmk_Case::OnSceneEntry() {}

    void CStgGmk_Case::OnSceneLeave() {}

    void CStgGmk_Case::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    void CStgGmk_Case::Respawn()
    {
        CStgGmk_Case_Act *pAct = GetCaseAct();
        if (pAct) {
            pAct->FsmqChangeState(CStgGmk_Case_Act::STATE_RESPAWN);
        }
    }

    int CStgGmk_Case::IsActive() const
    {
        const CStgGmk_Case_Act *pAct = GetCaseAct();
        if (pAct) {
            return pAct->IsAlive();
        }
        return 0;
    }

    void CStgGmk_Case::Inactivate()
    {
        CStgGmk_Case_Act *pAct = GetCaseAct();
        if (pAct) {
            pAct->Inactivate();
        }
    }

    // Hidden for ever (INACTIVE without an interval).
    void CStgGmk_Case::Sleep()
    {
        CStgGmk_Case_Act *pAct = GetCaseAct();
        if (pAct) {
            pAct->FsmqChangeState(CStgGmk_Case_Act::STATE_INACTIVE);
        }
    }

    // On the floor while active.
    bool CStgGmk_Case::IsLanding() const
    {
        if (IsActive() && GetPos().y <= 0.05f) {
            return true;
        }
        return false;
    }

    // The ship's roll: rAccel scaled by the actor's GetParam_Acc.
    void CStgGmk_Case::SetAccel(const bcs::FVEC3 &rAccel)
    {
        CStgGmk_Case_Act *pAct = GetCaseAct();
        if (pAct != 0) {
            float acc = pAct->GetParam_Acc();
            // (the product goes through a temporary, as an operator*
            // returning a vector by value would give)
            bcs::FVEC3 accel;
            bcs::FVEC3 scaled;
            bcs::FVEC3Scale(&scaled, &rAccel, acc);
            accel = scaled;
            pAct->GetSrt()->SetAccel(accel);
        }
    }

    void CStgGmk_Case::ForceBreak(float delay)
    {
        CStgGmk_Case_Act *pAct = GetCaseAct();
        if (pAct != 0) {
            pAct->ForceBreak(delay);
        }
    }

    void CStgGmk_Case::Disp() {}
}
