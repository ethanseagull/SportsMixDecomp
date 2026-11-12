// The model's header comes first: the vtables come out in the reverse order
// of the class declarations, and the original has the model's last.
#include "game/msw/CStgGmk_Pinball_Mdl.hpp"
#include "game/msw/CStgGmk_Pinball.hpp"
#include "game/msw/CPinballMan.hpp"
#include "game/msw/CStgGmk_Pinball_Act.hpp"
#include "bcs/BCS_Color.hpp"
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
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/CStgMdlObjBase.hpp"
#include "game/msw/CStgSrt.hpp"
#include "nw4r/g3d/Camera.hpp"
#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/types.h"
#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXGeometry.h"
#include "revolution/gx/GXPixel.h"
#include "revolution/gx/GXTransform.h"
#include "revolution/mtx.h"
#include <math.h>

// The balls of a sport's stage 12 ("pinball"): a launcher on the stage
// shoots iron balls into the field; they roll, bounce off the walls round
// the field and are kicked away by the characters' attacks, and a ball over
// a goal line scores for the team behind it.
//
// In the original one file holds a helper that builds the walls, the
// ball's model (CStgGmk_Pinball_Mdl), the actor (CStgGmk_Pinball_Act), the
// driver (CStgGmk_Pinball_Drv), the gimmick (CStgGmk_Pinball), the manager
// (CPinballMan) and its launcher (CPinballMecha), with their weak functions
// at the end of .text: 0x801E0130 to 0x801E782C, the vtables in one .data
// block (0x80484E50 to 0x804857B8), the walls in .bss. Evidence: the
// gimmick's NewMdl inlines the model's constructor, the manager's vtable
// lies between the actor's state table and the gimmick's in .data, one
// float pool (.sdata2 0x805C4D80 to 0x805C4E10) serves them all, and the
// weak copies of all of them are mixed in one tail. The class names of the
// model and the launcher and the names of the unnamed functions are
// guesses.

namespace {

    // A wall round the field, in the ground plane. Types 0 and 1 are the
    // goal lines (left and right), 5 the slanted guides at the corners,
    // 6 the straight walls of the side lines; -1 ends the table. (names
    // guessed)
    struct WALL {
        WALL() : nType(-1) {}
        WALL(long type, const bcs::FVEC3 &rP0, const bcs::FVEC3 &rP1, float decay)
            : nType(type), p0(rP0), p1(rP1), fDecay(decay)
        {
        }

        long nType;       // 0x00
        bcs::FVEC3 p0;    // 0x04
        bcs::FVEC3 p1;    // 0x10, the ball bounces off the side to the left of p0 -> p1
        float fDecay;     // 0x1C, share of the speed a bounce takes
    };

    enum {
        WALL_GOAL_LEFT = 0,
        WALL_GOAL_RIGHT = 1,
        WALL_GUIDE = 5,
        WALL_SIDE = 6,
        WALL_MAX = 16
    };

    WALL s_Wall[WALL_MAX];

    // The angle brought into [0, 2 pi).
    inline float NormalizeAngle(float angle)
    {
        while (angle > 6.2831855f) {
            angle -= 6.2831855f;
        }
        while (angle < 0.0f) {
            angle += 6.2831855f;
        }
        return angle;
    }

    // Builds the walls from the outer field (the ground rectangle) and
    // the guides at its corners, GetParam_GuideWidth by GetParam_GuideDepth.
    void BuildWall(const msw::CStgGmk_Pinball_Act *pAct)
    {
        // The goal lines and the side walls reach this far along z.
        float end = 100.0f;
        float right;
        float left;
        float front;
        float back;
        pAct->GetParam_OuterRect(&left, &right, &front, &back);
        float width = pAct->GetParam_GuideWidth();
        float depth = pAct->GetParam_GuideDepth();
        float guideLeft = left + width;
        float guideRight = right - width;
        float guideFront = front - depth;
        float guideBack = back + depth;
        // The goal lines and the side walls keep the whole speed.
        float noDecay = 0.0f;
        float decay = pAct->GetParam_GuideDecay();

        WALL *pWall = s_Wall;
        *pWall++ = WALL(WALL_GOAL_LEFT, bcs::FVEC3(left, 0.0f, end), bcs::FVEC3(left, 0.0f, -end), noDecay);
        *pWall++ = WALL(WALL_GOAL_RIGHT, bcs::FVEC3(right, 0.0f, -end), bcs::FVEC3(right, 0.0f, end), noDecay);
        *pWall++ = WALL(WALL_GUIDE, bcs::FVEC3(left, 0.0f, front), bcs::FVEC3(guideLeft, 0.0f, guideFront), decay);
        *pWall++ = WALL(WALL_GUIDE, bcs::FVEC3(guideRight, 0.0f, guideFront), bcs::FVEC3(right, 0.0f, front), decay);
        *pWall++ = WALL(WALL_GUIDE, bcs::FVEC3(guideLeft, 0.0f, guideBack), bcs::FVEC3(left, 0.0f, back), decay);
        *pWall++ = WALL(WALL_GUIDE, bcs::FVEC3(right, 0.0f, back), bcs::FVEC3(guideRight, 0.0f, guideBack), decay);
        *pWall++ = WALL(WALL_SIDE, bcs::FVEC3(guideLeft, 0.0f, guideFront), bcs::FVEC3(guideLeft, 0.0f, -end), noDecay);
        *pWall++ = WALL(WALL_SIDE, bcs::FVEC3(guideRight, 0.0f, -end), bcs::FVEC3(guideRight, 0.0f, guideFront), noDecay);
        *pWall++ = WALL(WALL_SIDE, bcs::FVEC3(guideLeft, 0.0f, end), bcs::FVEC3(guideLeft, 0.0f, guideBack), noDecay);
        *pWall++ = WALL(WALL_SIDE, bcs::FVEC3(guideRight, 0.0f, guideBack), bcs::FVEC3(guideRight, 0.0f, end), noDecay);
        *pWall = WALL();
    }

    // The rows of the rotation part of rMtx made unit length.
    inline void NormalizeRows(bcs::FMTX34 &rMtx)
    {
        bcs::FVEC3 &rRow0 = *reinterpret_cast<bcs::FVEC3 *>(rMtx.m[0]);
        bcs::BCS_Vec_Normalize(rRow0, rRow0);
        bcs::FVEC3 &rRow1 = *reinterpret_cast<bcs::FVEC3 *>(rMtx.m[1]);
        bcs::BCS_Vec_Normalize(rRow1, rRow1);
        bcs::FVEC3 &rRow2 = *reinterpret_cast<bcs::FVEC3 *>(rMtx.m[2]);
        bcs::BCS_Vec_Normalize(rRow2, rRow2);
    }
}

namespace {

    const char s_MdlName[] = "s12_tekyu";

    const msw::CStgMdl::MDL_PARAM s_MdlParam = { s_MdlName, s_MdlName, s_MdlName, 0, 0, 0, 0, 0 };
}

namespace msw {

    CStgGmk_Pinball_Mdl::~CStgGmk_Pinball_Mdl()
    {
        if (m_pDebugObj != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pDebugObj);
        }
        m_pDebugObj = 0;
    }

    void CStgGmk_Pinball_Mdl::Create()
    {
        CStgMdl::Create(CGenericGimmickCreator(CLoadData::GetRes(0), &s_MdlParam));
    }

    bcs::CMdlObj *CStgGmk_Pinball_Mdl::NewMdl(long heap) const
    {
        return new (heap) CStgMdlObjBase<bcs::CMdlObj>;
    }

    bcs::CScnObj *CStgGmk_Pinball_Mdl::GetScnObj()
    {
        return CStgMdl::GetMdlObj();
    }

    const bcs::CScnObj *CStgGmk_Pinball_Mdl::GetScnObj() const
    {
        return CStgMdl::GetMdlObj();
    }

    void CStgGmk_Pinball_Mdl::OnPostCreate()
    {
    }

    void CStgGmk_Pinball_Mdl::SetAttach(bcs::CScnObj *pObj, long node)
    {
        if (GetMdlObj() != 0) {
            m_pAttachObj = pObj;
            m_nAttachNode = node;
        }
    }

    // The model is scaled to the actor's radius (the model's own is 0.5).
    void CStgGmk_Pinball_Mdl::Main(float rate)
    {
        CStgGmk_Pinball_Act *pAct = static_cast<CStgGmk_Pinball_Act *>(GetParent()->GetAct());
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pAct != 0 && pMdl != 0) {
            float scale = pAct->GetCurRadius() / 0.5f;
            pMdl->m_vScale.x = scale;
            pMdl->m_vScale.y = scale;
            pMdl->m_vScale.z = scale;
            pMdl->CalcRootMtx();
            pMdl->CalcWorldMtx(0);
        }
        CStgMdl::Main(rate);
    }

    void CStgGmk_Pinball_Mdl::DrawDebug(long nView)
    {
        // (the original reads the actor and clears a vector it never uses)
        GetParent()->GetAct();
        bcs::FVEC3 unused(0.0f, 0.0f, 0.0f);
        float x = GetRadius();
        float z = GetRadius();
        float y = GetRadius();
        bcs::BCS_Color face(0xFFC00080);
        bcs::BCS_Color edge(0x80600040);
        // The corners: 0 to 3 on the +z side, 4 to 7 on the -z side.
        bcs::FVEC3 aPos[8] = {
            bcs::FVEC3(-x, y, z), bcs::FVEC3(x, y, z),   bcs::FVEC3(x, -y, z),   bcs::FVEC3(-x, -y, z),
            bcs::FVEC3(x, y, -z), bcs::FVEC3(-x, y, -z), bcs::FVEC3(-x, -y, -z), bcs::FVEC3(x, -y, -z),
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
        bcs::BCS_GX_DrawQuad(aPos[0], aPos[1], aPos[2], aPos[3], face, face, face,
                             face);
        bcs::BCS_GX_DrawQuad(aPos[1], aPos[4], aPos[7], aPos[2], face, face, face,
                             face);
        bcs::BCS_GX_DrawQuad(aPos[4], aPos[5], aPos[6], aPos[7], face, face, face,
                             face);
        bcs::BCS_GX_DrawQuad(aPos[5], aPos[0], aPos[3], aPos[6], face, face, face,
                             face);
        bcs::BCS_GX_DrawQuad(aPos[5], aPos[4], aPos[1], aPos[0], face, face, face,
                             face);
        bcs::BCS_GX_DrawQuad(aPos[3], aPos[2], aPos[7], aPos[6], face, face, face,
                             face);

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

    // Turns the ball by dist along rDir (a negative distance rolls it
    // backwards) for a ball of the given radius.
    void CStgGmk_Pinball_Mdl::Roll(const bcs::FVEC3 &rDir, float radius, float dist)
    {
        bcs::FVEC3 dir;
        bcs::BCS_Vec_Normalize(dir, rDir);
        m_bMtxDirty = 1;
        if (!(fabsf(radius) < 0.001f) && !(fabsf(dist) < 0.0001f)) {
            if (dist < 0.0f) {
                dist *= -1.0f;
                bcs::FVEC3Scale(&dir, &dir, -1.0f);
            }
            float angle = dist / radius;
            bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
            bcs::FVEC3 axis;
            PSVECCrossProduct(reinterpret_cast<const Vec *>(&up), reinterpret_cast<const Vec *>(&dir),
                              reinterpret_cast<Vec *>(&axis));
            bcs::FMTX34 rot;
            nw4r::math::MTX34RotAxisRad(reinterpret_cast<nw4r::math::MTX34 *>(&rot),
                                        reinterpret_cast<const nw4r::math::VEC3 *>(&axis), angle);
            PSMTXConcat(rot.m, m_RollMtx.m, m_RollMtx.m);
            NormalizeRows(m_RollMtx);
        }
    }

    // Following the carrying model's node, or rolling by the distance moved
    // in the ground plane.
    void CStgGmk_Pinball_Mdl::SetMtx(const bcs::FMTX34 &rMtx)
    {
        // Declared in this order for the original's stack layout.
        bcs::FMTX34 prev;
        bcs::FMTX34 mtx;
        bcs::FMTX34Copy(&mtx, &rMtx);
        prev = CStgMdl::GetMtx();
        m_bMtxDirty = 1;
        if (m_pAttachObj != 0) {
            CStgGmk_Pinball_Act *pAct = static_cast<CStgGmk_Pinball_Act *>(GetParent()->GetAct());
            if (pAct != 0) {
                float radius = pAct->GetCurRadius();
                bcs::FMTX34 nodeMtx;
                m_pAttachObj->GetNodeWorldMtx(m_nAttachNode, nodeMtx);
                NormalizeRows(nodeMtx);
                bcs::FVEC3 pos;
                bcs::FVEC3 zero(0.0f, 0.0f, 0.0f);
                PSMTXMultVec(nodeMtx.m, reinterpret_cast<const Vec *>(&zero), reinterpret_cast<Vec *>(&pos));
                mtx.m[0][3] = pos.x;
                mtx.m[1][3] = pos.y - radius;
                mtx.m[2][3] = pos.z;
            }
        } else {
            bcs::FVEC3 prevPos(prev.m[0][3], prev.m[1][3], prev.m[2][3]);
            bcs::FVEC3 pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
            bcs::FVEC3 move;
            bcs::FVEC3Sub(&move, &pos, &prevPos);
            move.y = 0.0f;
            bcs::FVEC3 dir;
            bcs::BCS_Vec_Normalize(dir, move);
            float radius = GetRadius();
            Roll(dir, radius, bcs::FVEC3Len(&move));
        }
        CStgMdl::SetMtx(mtx);
    }

    const bcs::FMTX34 &CStgGmk_Pinball_Mdl::GetMtx() const
    {
        if (m_bMtxDirty) {
            bcs::FMTX34 mtx;
            bcs::FMTX34Copy(&mtx, &CStgMdl::GetMtx());
            PSMTXConcat(mtx.m, m_RollMtx.m, m_DispMtx.m);
            float radius = GetRadius();
            m_bMtxDirty = 0;
            m_DispMtx.m[1][3] += radius;
        }
        return m_DispMtx;
    }

    // Half the height of the scaled model; 1 without one.
    float CStgGmk_Pinball_Mdl::GetRadius() const
    {
        const bcs::CScnObj *pObj = GetScnObj();
        if (pObj != 0) {
            return 0.5f * pObj->m_vScale.y;
        }
        return 1.0f;
    }

    CStgGmk_Pinball_Act::CStgGmk_Pinball_Act(CStgGmk *pParent)
        : CStgGmkAct(pParent), m_bAlive(0), m_fInvincibleTime(0.0f), m_fGrowTime(0.0f), m_fGrowTimeMax(0.0f),
          m_fShotAngle(0.0f)
    {
        FsmqInit(this, STATE_NUM, 4, 4);
        FsmqSetState(STATE_DEFAULT, &CStgGmk_Pinball_Act::CtorState_DEFAULT, &CStgGmk_Pinball_Act::LoopState_DEFAULT,
                     &CStgGmk_Pinball_Act::DtorState_DEFAULT);
        FsmqSetState(STATE_STANDBY, &CStgGmk_Pinball_Act::CtorState_STANDBY, &CStgGmk_Pinball_Act::LoopState_STANDBY,
                     &CStgGmk_Pinball_Act::DtorState_STANDBY);
        FsmqSetState(STATE_MOVE, &CStgGmk_Pinball_Act::CtorState_MOVE, &CStgGmk_Pinball_Act::LoopState_MOVE,
                     &CStgGmk_Pinball_Act::DtorState_MOVE);
        FsmqSetState(STATE_BREAK, &CStgGmk_Pinball_Act::CtorState_BREAK, &CStgGmk_Pinball_Act::LoopState_BREAK,
                     &CStgGmk_Pinball_Act::DtorState_BREAK);
        FsmqSetState(STATE_SLEEP, &CStgGmk_Pinball_Act::CtorState_SLEEP, &CStgGmk_Pinball_Act::LoopState_SLEEP,
                     &CStgGmk_Pinball_Act::DtorState_SLEEP);
        FsmqSetState(STATE_RESPAWN, &CStgGmk_Pinball_Act::CtorState_RESPAWN, &CStgGmk_Pinball_Act::LoopState_RESPAWN,
                     &CStgGmk_Pinball_Act::DtorState_RESPAWN);
        FsmqSetState(STATE_INACTIVE, &CStgGmk_Pinball_Act::CtorState_INACTIVE,
                     &CStgGmk_Pinball_Act::LoopState_INACTIVE, &CStgGmk_Pinball_Act::DtorState_INACTIVE);
        CStgSrtMove *pSrt = GetSrt();
        pSrt->ResetVelocity(GetParam_Gravity());

        float width = GetParam_Radius();
        float depth = GetParam_Radius();
        float height = GetParam_Height();
        float radius = GetParam_Radius();
        CStgObj *pObj = GetParent();
        long idx;
        if (pObj != 0) {
            idx = pObj->GetIdx();
        } else {
            idx = -1;
        }

        // 1 pushes the characters away, 0 is hit and caught.
        m_BodyColl.Create(2, 5, 3, 0);
        if (idx >= 0) {
            m_BodyColl.SetIndexAll(idx);
        }
        m_BodyColl.SetCollType(1, 1);
        m_BodyColl.SetRadius(1, radius);
        m_BodyColl.SetPushRank(1, 5);
        m_BodyColl.SetPushPower(1, 6);
        m_BodyColl.SetTargetFlag(1, 6);
        m_BodyColl.SetCollType(0, 1);
        m_BodyColl.SetRadius(0, radius);
        m_BodyColl.SetPushRank(0, 5);
        m_BodyColl.SetPushPower(0, 6);
        m_BodyColl.SetTargetFlag(0, 0x34);
        RegistBodyPushCallback(m_BodyColl);
        RegistBodyDamageCallback(m_BodyColl);
        RegistBallCatchCallback(m_BodyColl);

        m_DefColl.Create(1, 5, 4, 0);
        if (idx >= 0) {
            m_DefColl.SetIndexAll(idx);
        }
        m_DefColl.GetColl(0)->SetCollType(4);
        m_DefColl.GetColl(0)->SetCollType(1);
        m_DefColl.GetColl(0)->SetShape(4);
        bcs::FVEC3 size(width, height, depth);
        m_DefColl.GetColl(0)->SetSize(size);
        m_DefColl.GetColl(0)->OnFlagA0(1);
        RegistReflectHitCallback(m_DefColl);

        m_AtkColl.Create(1, 5, 0, 0);
        if (idx >= 0) {
            m_AtkColl.SetIndexAll(idx);
        }
        if (depth < width) {
            width = depth;
        }
        float atkRadius = 0.9f * width;
        m_AtkColl.SetCollType(0, 3);
        m_AtkColl.SetTargetFlag(0, 0x22);
        m_AtkColl.SetAtkType(0, 0xC01);
        m_AtkColl.SetRadius(0, atkRadius);
        RegistAttackHitCallback(m_AtkColl);
        m_AtkColl.SetEnable(0);
        m_hEffect = 0;
    }

    CStgGmk_Pinball_Act::~CStgGmk_Pinball_Act()
    {
    }

    void CStgGmk_Pinball_Act::OnInit()
    {
        BuildWall(this);
    }

    // Runs the states. The floor's flow carries the ball while it is on
    // the ground; a ball that falls hard on it shakes the camera, and
    // bounces with half its upward speed.
    void CStgGmk_Pinball_Act::OnUpdate(float rate)
    {
        CStgSrtMove *pSrt = GetSrt();
        CStageMan *pStage = CStageMan::GetInstance();
        if (m_fInvincibleTime > 0.0f) {
            m_fInvincibleTime -= rate;
        }
        FsmqExec();

        bcs::FVEC3 pos = pSrt->GetPos();
        if (pos.y <= 0.05f) {
            bcs::FVEC3 flow;
            pStage->GetFloorFlow(flow, pos, CStageMan::FLOW_TYPE_0);
            bcs::FVEC3Add(&pos, &pos, &flow);
            pSrt->SetPos(pos);
        }

        bcs::FVEC3 ground = pSrt->GetPos();
        if (ground.y < 0.05f) {
            bcs::FVEC3 fall = pSrt->GetUnkE4();
            if (fall.y < -6.0f) {
                CGame::StartCameraQuake(0x31, -1, -1);
            }
            ground.y = 0.05f;
            pSrt->SetPos(ground);
            pSrt->ClearUnkE4();
            bcs::FVEC3 vel = pSrt->GetVel();
            vel.y = vel.y * 0.5f;
            pSrt->SetVel(vel);
        }
        UpdateColl();

        if (IsAlive() && !m_bInField && CStageMan::GetInstance()->GetAreaFlagByPos(pSrt->GetPos(), 0xFF0F)) {
            m_bInField = 1;
        }
    }

    void CStgGmk_Pinball_Act::UpdateMdlMtx()
    {
        for (long i = 0; i < 3; i++) {
            CStgMdl *pMdl = GetParent()->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetMtx(GetSrt()->GetMtx());
            }
        }
    }

    // The collision is on only while the ball is alive and in the field:
    // the body at the ball, the attack a capsule from its centre upwards.
    void CStgGmk_Pinball_Act::UpdateColl()
    {
        CStgSrt *pSrt = GetSrt();
        if (m_bAlive && m_bInField) {
            float height = GetParam_Height();
            float radius = GetParam_CollRadius();
            float atkRadius = GetParam_AtkCollRadius();
            bcs::FVEC3 pos = pSrt->GetPos();
            m_BodyColl.SetEnableAll();
            m_BodyColl.SetPos(1, bcs::FVEC3(pos));
            m_BodyColl.SetSize(1, radius, height > 0.0f ? height : 0.0f, 0.0f);
            m_BodyColl.SetPushEnable(1);
            m_BodyColl.SetPos(0, bcs::FVEC3(pos));
            m_BodyColl.SetSize(0, radius, height > 0.0f ? height : 0.0f, 0.0f);
            m_BodyColl.SetBodyEnable(0);
            m_BodyColl.SetCatchEnable(0);
            if (IsEnableDefColl()) {
                m_DefColl.GetColl(0)->SetPos(bcs::FVEC3(pos));
                m_DefColl.GetColl(0)->SetSize(radius, height, 0.0f);
                m_DefColl.GetColl(0)->OnFlag10(1);
                m_DefColl.GetColl(0)->OnFlagA0(1);
            } else {
                m_DefColl.GetColl(0)->OffFlag10(1);
                m_DefColl.GetColl(0)->OffFlagA0(1);
            }
            float len = 0.0f;
            float rest = height - 2.0f * atkRadius;
            if (rest > 0.0f) {
                len = rest;
            }
            bcs::FVEC3 atkPos(pos.x, pos.y + atkRadius, pos.z);
            m_AtkColl.SetPos(0, atkPos);
            atkPos.y += len;
            m_AtkColl.SetPos2(0, atkPos);
            m_AtkColl.SetRadius(0, atkRadius);
            m_AtkColl.SetEnable(0);
            m_AtkColl.SetHitEffectFrame(0, GetParam_AtkHitEffectFrame());
            return;
        }
        m_BodyColl.SetDisableAll();
        m_AtkColl.SetDisableAll();
        m_DefColl.SetDisableAll();
    }

    // A push against the map is cut down to what the map leaves free.
    void CStgGmk_Pinball_Act::OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (pPush != 0) {
            bcs::FVEC3 from = GetSrt()->GetPos();
            bcs::FVEC3 to(from.x + pPush->x, from.y, from.z + pPush->z);
            bcs::FVEC3 hit;
            bcs::FVEC3 push;
            float time;
            if (CCollMan::DetectMapHit(hit, push, time, from, to, 2.0f, 1, 0)) {
                push.x = hit.x - to.x;
                push.z = hit.z - to.z;
                pOther->DoBodyPushCallback(pBody, &push);
            } else {
                pOther->DoBodyPushCallback(pBody, 0);
            }
        } else {
            pOther->DoBodyPushCallback(pBody, 0);
        }
    }

    unsigned long CStgGmk_Pinball_Act::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                    const bcs::FVEC3 &rPos)
    {
        return 2;
    }

    // A catch by the attacking side plays the hit and makes the ball
    // invincible for 15 frames.
    void CStgGmk_Pinball_Act::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (FsmqGetState() != STATE_BREAK && pBall->IsOffensive() && !IsInvincible()) {
            bcs::FVEC3 pos = pBall->GetPos();
            long eff = GetParam_Effect(2);
            if (eff >= 0) {
                CEffectCtrl::SimpleEffect(eff, 0, &pos, 0, 3, 0, 0, 0);
            }
            m_fInvincibleTime = 15.0f;
            OnDamage(pBody, pBall);
        }
    }

    // A hit on a character bounces the ball off it, faster by
    // GetParam_KickPower; the kick effect plays on the ball's surface
    // towards the character, and the camera shakes.
    void CStgGmk_Pinball_Act::OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                          const bcs::FVEC3 &rPos, unsigned long flag)
    {
        if (flag == 1 && !IsInvincible()) {
            CStgSrtMove *pSrt = GetSrt();
            // Declared in this order for the original's stack layout.
            bcs::FVEC3 atkPos;
            bcs::FVEC3 bodyPos;
            bcs::FVEC3 vel;
            bodyPos = pBody->GetPos();
            vel = pSrt->GetVel();
            atkPos.Set(pAtk->m_fPosX, pAtk->m_fPosY, pAtk->m_fPosZ);
            float speed = bcs::FVEC3Len(&vel);
            speed += GetParam_KickPower();

            bcs::FVEC3 normal;
            bcs::FVEC3Sub(&normal, &atkPos, &bodyPos);
            normal.y = 0.0f;
            bcs::BCS_Vec_Normalize(normal, normal);
            float dot = 2.0f * bcs::FVEC3Dot(&vel, &normal);
            bcs::FVEC3 ofs;
            bcs::FVEC3Scale(&ofs, &normal, dot);
            bcs::FVEC3Sub(&vel, &vel, &ofs);
            bcs::BCS_Vec_Normalize(vel, vel);
            bcs::FVEC3Scale(&vel, &vel, speed);
            pSrt->SetVel(vel);
            m_fInvincibleTime = 15.0f;

            bcs::FVEC3 center = GetSrt()->GetPos();
            center.y += 0.5f * GetParam_Height();
            bcs::FVEC3 dir;
            bcs::FVEC3Sub(&dir, &pBody->GetPos(), &center);
            bcs::BCS_Vec_Normalize(dir, dir);
            bcs::FVEC3Scale(&dir, &dir, GetParam_Radius());
            bcs::FVEC3Add(&dir, &dir, &center);
            KickHitEffect(dir);

            if (pBody != 0 && pBody->GetOwnerKind() == 1) {
                long quake = GetParam_HitCameraQuake();
                long idx = pBody->m_nOwnerIdx;
                if (quake >= 0 && idx >= 0) {
                    CGame::StartCameraQuake(quake, idx, -1);
                }
            }
        }
    }

    void CStgGmk_Pinball_Act::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
    {
    }

    // The ball grows from GetParam_InitRadius to GetParam_Radius over
    // m_fGrowTimeMax frames.
    float CStgGmk_Pinball_Act::GetCurRadius() const
    {
        float radius = GetParam_Radius();
        if (m_fGrowTimeMax > 0.0f) {
            float rate = m_fGrowTime / m_fGrowTimeMax;
            rate = rate > 1.0f ? 1.0f : (rate < 0.0f ? 0.0f : rate);
            float full = GetParam_Radius();
            float init = GetParam_InitRadius();
            radius = rate * (full - init) + init;
        }
        return radius;
    }

    float CStgGmk_Pinball_Act::GetParam_InitRadius() const
    {
        return 0.6f;
    }

    float CStgGmk_Pinball_Act::GetParam_Radius() const
    {
        return 1.0f;
    }

    float CStgGmk_Pinball_Act::GetParam_Height() const
    {
        return 2.0f;
    }

    float CStgGmk_Pinball_Act::GetParam_CollRadius() const
    {
        return 1.0f;
    }

    float CStgGmk_Pinball_Act::GetParam_CollHeight() const
    {
        return 2.0f;
    }

    float CStgGmk_Pinball_Act::GetParam_Dec() const
    {
        return 0.0f;
    }

    float CStgGmk_Pinball_Act::GetParam_KickPower() const
    {
        return GetParam_Speed();
    }

    float CStgGmk_Pinball_Act::GetParam_Speed() const
    {
        return 3.0f;
    }

    float CStgGmk_Pinball_Act::GetParam_VelLimit() const
    {
        return 80.0f;
    }

    float CStgGmk_Pinball_Act::GetParam_Gravity() const
    {
        return -14.700001f;
    }

    float CStgGmk_Pinball_Act::GetParam_Interval() const
    {
        return 300.0f;
    }

    void CStgGmk_Pinball_Act::GetParam_CourtRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
    {
        CStageMan::GetInstance()->GetCourtRect(pLeft, pRight, pFront, pBack);
    }

    void CStgGmk_Pinball_Act::GetParam_OuterRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
    {
        CStageMan::GetInstance()->GetGroundRect(pLeft, pRight, pFront, pBack);
    }

    long CStgGmk_Pinball_Act::GetParam_MdlIdx() const
    {
        return bcs::BCS_RandMax(2L);
    }

    long CStgGmk_Pinball_Act::GetParam_Effect(long type) const
    {
        return -1;
    }

    long CStgGmk_Pinball_Act::GetParam_SE(long type) const
    {
        switch (type) {
            case 0:
                return 0xD76;
            case 1:
                return 0xD73;
            case 2:
                return 0xD78;
            case 3:
                return 0xD79;
        }
        return -1;
    }

    float CStgGmk_Pinball_Act::GetParam_MaxLife() const
    {
        return 3.0f;
    }

    float CStgGmk_Pinball_Act::GetParam_ForceAcc() const
    {
        return 0.5f;
    }

    long CStgGmk_Pinball_Act::GetParam_EndLineEnabled() const
    {
        return 0;
    }

    float CStgGmk_Pinball_Act::GetParam_GuideWidth() const
    {
        return 3.94f;
    }

    float CStgGmk_Pinball_Act::GetParam_GuideDepth() const
    {
        return 2.3f;
    }

    long CStgGmk_Pinball_Act::GetParam_GuideBoundEnabled() const
    {
        return 1;
    }

    float CStgGmk_Pinball_Act::GetParam_GuideDecay() const
    {
        return 0.2f;
    }

    long CStgGmk_Pinball_Act::GetParam_HitCameraQuake() const
    {
        return 0x16;
    }

    void CStgGmk_Pinball_Act::Inactivate()
    {
        m_bAlive = 0;
    }

    // Sleeps for frame frames, then respawns.
    void CStgGmk_Pinball_Act::Sleep(float frame)
    {
        float *pFrame = static_cast<float *>(FsmqSetState(STATE_SLEEP));
        if (pFrame != 0) {
            *pFrame = frame;
        }
    }

    // Respawns, to be shot at angle.
    void CStgGmk_Pinball_Act::Respawn(float angle)
    {
        float *pAngle = static_cast<float *>(FsmqSetState(STATE_RESPAWN));
        if (pAngle != 0) {
            *pAngle = angle;
        }
    }

    void CStgGmk_Pinball_Act::Standby()
    {
        FsmqChangeState(STATE_STANDBY);
    }

    // The ball out of the field scores: over the left goal line (type 0)
    // for side 0 or 2, over the right one for side 1 or 3, by the half of
    // the field it crossed in. Returns true when it is out.
    int CStgGmk_Pinball_Act::WallHitCheck(bcs::FVEC3 *pPos, bcs::FVEC3 *pVel, const bcs::FVEC3 &rPrev,
                                          const bcs::FVEC3 &rPos, const bcs::FVEC3 &rVel)
    {
        bcs::FVEC3 prev = rPrev;
        bcs::FVEC3 pos = rPos;
        bcs::FVEC3 vel = rVel;
        float radius = GetParam_CollRadius();
        float right;
        float left;
        float front;
        float back;
        float margin = 6.0f;
        GetParam_OuterRect(&left, &right, &front, &back);
        float depth = GetParam_GuideDepth();
        float minZ = front - depth - margin;
        float maxZ = back + depth + margin;
        BuildWall(this);
        if (pos.z < minZ || pos.z > maxZ || pos.x < 2.0f * left || pos.x > 2.0f * right) {
            return 1;
        }

        if (m_bInField) {
            for (WALL *pWall = s_Wall; pWall->nType >= 0; pWall++) {
                if (WallHitCheckSub(&pos, &vel, prev, pos, radius, vel, pWall->p0, pWall->p1, pWall->fDecay)) {
                    long side;
                    switch (pWall->nType) {
                        case WALL_GOAL_LEFT:
                            side = pos.z < 0.0f ? 0 : 2;
                            break;
                        case WALL_GOAL_RIGHT:
                            side = pos.z < 0.0f ? 1 : 3;
                            break;
                        default:
                            side = -1; // an ordinary wall
                            break;
                    }
                    if (side >= 0) {
                        // A goal line: the ball has scored.
                        SE_PlayByParam(1);
                        CGame::SendMessage(0x25, 3, side, 0);
                    } else {
                        // An ordinary wall: just the bounce sound.
                        SE_PlayByParam(0);
                    }
                    break;
                }
            }
        }

        if (pPos != 0) {
            *pPos = pos;
        }
        if (pVel != 0) {
            *pVel = vel;
        }
        return 0;
    }

    // The ball, moving from rPrev to rPos, against the wall from rWall0 to
    // rWall1, whose outside is to the right of that direction. Three cases
    // are tried in turn: the ball's surface crossing the wall, the ball's
    // centre ending up within its radius of the wall, and the ball touching
    // one of the wall's ends. The hit position is put just off the wall;
    // the velocity is mirrored on the wall and keeps the share (1 - decay
    // times the steepness of the hit) of its speed.
    int CStgGmk_Pinball_Act::WallHitCheckSub(bcs::FVEC3 *pPos, bcs::FVEC3 *pVel, const bcs::FVEC3 &rPrev,
                                             const bcs::FVEC3 &rPos, float radius, const bcs::FVEC3 &rVel,
                                             const bcs::FVEC3 &rWall0, const bcs::FVEC3 &rWall1,
                                             float decay) const
    {
        // Declared in this order for the original's stack layout.
        bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
        bcs::FVEC3 normal;   // the wall's outward normal; for an end, the bounce direction
        bcs::FVEC3 wallDir;
        bcs::FVEC3 moveDir;
        bcs::FVEC3 hit;
        bcs::FVEC3 touch;
        bcs::FVEC3 ofs;
        bcs::FVEC3 wall0;
        bcs::FVEC3 wall1;
        bcs::FVEC3 from;
        bcs::FVEC3 step;
        bcs::FVEC3 rel;

        bcs::FVEC3Sub(&wallDir, &rWall1, &rWall0);
        wallDir.y = 0.0f;
        bcs::BCS_Vec_Normalize(wallDir, wallDir);
        PSVECCrossProduct(reinterpret_cast<const Vec *>(&wallDir), reinterpret_cast<const Vec *>(&up),
                          reinterpret_cast<Vec *>(&normal));
        bcs::FVEC3Sub(&moveDir, &rPos, &rPrev);
        moveDir.y = 0.0f;
        bcs::BCS_Vec_Normalize(moveDir, moveDir);

        float cos = bcs::FVEC3Dot(&normal, &moveDir);
        cos = cos > 1.0f ? 1.0f : (cos < -1.0f ? -1.0f : cos);
        if (cos > 0.0f) {
            // moving away from the wall
            return 0;
        }

        // The wall moved out by the radius, and the start of the move
        // moved back by the radius along the wall's direction.
        float back = radius * fabsf(bcs::FVEC3Dot(&moveDir, &wallDir));
        bcs::FVEC3Scale(&ofs, &normal, radius);
        wall0 = rWall0;
        wall1 = rWall1;
        bcs::FVEC3Add(&wall0, &wall0, &ofs);
        bcs::FVEC3Add(&wall1, &wall1, &ofs);
        bcs::FVEC3Scale(&from, &moveDir, -back);
        bcs::FVEC3Add(&from, &from, &rPrev);

        int bHit = bcs::BCS_GetLineToLineCross(&hit.x, &hit.z, wall0.x, wall0.z, wall1.x, wall1.z, from.x, from.z,
                                               rPos.x, rPos.z);
        if (bHit) {
            float dot = bcs::FVEC3Dot(&normal, &moveDir);
            float dist = dot > 0.0f ? radius / dot : radius;
            hit.y = rPrev.y;
            bcs::FVEC3Scale(&step, &moveDir, -dist);
            bcs::FVEC3Add(&hit, &hit, &step);

            // where the ball touches the wall, within the wall?
            bcs::FVEC3Scale(&touch, &normal, -radius);
            bcs::FVEC3Add(&touch, &touch, &hit);
            bcs::FVEC3Sub(&rel, &touch, &rWall0);
            float along = bcs::FVEC3Dot(&rel, &wallDir);
            float wallLen = nw4r::math::FSqrt(
                PSVECSquareDistance(reinterpret_cast<const Vec *>(&rWall1), reinterpret_cast<const Vec *>(&rWall0)));
            if (along < 0.0f || along > wallLen) {
                bHit = 0;
            }

            // within the move?
            bcs::FVEC3Sub(&rel, &hit, &rPrev);
            along = bcs::FVEC3Dot(&rel, &moveDir);
            float moveLen = nw4r::math::FSqrt(
                PSVECSquareDistance(reinterpret_cast<const Vec *>(&rPos), reinterpret_cast<const Vec *>(&rPrev)));
            if (along < 0.0f || along > moveLen) {
                bHit = 0;
            }

            bcs::FVEC3Scale(&rel, &normal, 0.01f);
            bcs::FVEC3Add(&hit, &hit, &rel);
        }

        if (!bHit) {
            // The end of the move within the radius of the wall.
            const bcs::FVEC3 *apPos[2] = { &rPos, &rPrev };
            bcs::FVEC3 inward;
            bcs::FVEC3Scale(&inward, &normal, radius);
            for (long i = 0; !bHit && i < 1; i++) {
                bcs::FVEC3 inner;
                bcs::FVEC3Add(&inner, &inward, apPos[i]);
                bcs::FVEC3 pos = *apPos[i];
                bHit = bcs::BCS_GetSegLineToSegLineCross(&hit.x, &hit.z, wall0.x, wall0.z, wall1.x, wall1.z, inner.x,
                                                         inner.z, pos.x, pos.z);
            }
            if (bHit) {
                hit.y = rPos.y;
                bcs::FVEC3Scale(&touch, &normal, -(0.01f + radius));
                bcs::FVEC3Add(&touch, &touch, &hit);
            }
        }

        if (!bHit) {
            // The end of the move within the radius of an end of the wall,
            // moving towards it.
            const bcs::FVEC3 *apPos[2] = { &rPos, &rPrev };
            const bcs::FVEC3 *apEnd[2] = { &rWall0, &rWall1 };
            float aSign[2] = { -1.0f, 1.0f };
            for (long i = 0; !bHit && i < 1; i++) {
                const bcs::FVEC3 *pPt = apPos[i];
                for (long j = 0; !bHit && j < 2; j++) {
                    bcs::FVEC3 endRel;
                    bcs::FVEC3Sub(&endRel, pPt, apEnd[j]);
                    endRel.y = 0.0f;
                    float dist = bcs::FVEC3Len(&endRel);
                    bcs::FVEC3 out;
                    bcs::FVEC3Scale(&out, &wallDir, aSign[j]);
                    if (dist < radius && bcs::FVEC3Dot(&moveDir, &out) < 0.0f) {
                        // the ball bounces off the end as off a wall
                        // square to the line to it
                        bcs::BCS_Vec_Normalize(normal, endRel);
                        float dot = 2.0f * bcs::FVEC3Dot(&moveDir, &normal);
                        if (dot > 0.0f) {
                            normal = moveDir;
                        } else {
                            bcs::FVEC3 sub;
                            bcs::FVEC3Scale(&sub, &normal, dot);
                            bcs::FVEC3Sub(&normal, &moveDir, &sub);
                        }
                        bcs::FVEC3Scale(&normal, &normal, -1.0f);
                        float proj = bcs::FVEC3Dot(&moveDir, &endRel);
                        bcs::FVEC3 edge;
                        bcs::BCS_Vec_Normalize(edge, endRel);
                        bcs::FVEC3Scale(&edge, &edge, radius);
                        bHit = 1;
                        float push = bcs::FVEC3Dot(&moveDir, &edge) - proj + 0.01f;
                        bcs::FVEC3 endStep;
                        bcs::FVEC3Scale(&endStep, &moveDir, push);
                        bcs::FVEC3Add(&hit, pPt, &endStep);
                    }
                }
            }
        }

        if (!bHit) {
            return 0;
        }

        if (pVel != 0) {
            float speed = bcs::FVEC3Len(&rVel);
            float dot = 2.0f * bcs::FVEC3Dot(&rVel, &normal);
            decay *= fabsf(bcs::FVEC3Dot(&moveDir, &normal));
            bcs::FVEC3 sub;
            bcs::FVEC3Scale(&sub, &normal, dot);
            bcs::FVEC3 vel;
            bcs::FVEC3Sub(&vel, &rVel, &sub);
            bcs::BCS_Vec_Normalize(vel, vel);
            bcs::FVEC3Scale(&vel, &vel, speed * (1.0f - decay));
            *pVel = vel;
        }
        if (pPos != 0) {
            *pPos = bcs::FVEC3(hit.x, hit.y, hit.z);
        }
        return bHit;
    }

    void CStgGmk_Pinball_Act::KickHitEffect(const bcs::FVEC3 &rPos)
    {
        long eff = GetParam_Effect(2);
        if (eff >= 0) {
            CEffectCtrl::SimpleEffect(eff, 0, &rPos, 0, 3, 0, 0, 0);
        }
        SE_PlayByParam(0, rPos);
    }

    void CStgGmk_Pinball_Act::SE_PlayByParam(long type)
    {
        SE_PlayByParam(type, GetSrt()->GetPos());
    }

    void CStgGmk_Pinball_Act::SE_PlayByParam(long type, const bcs::FVEC3 &rPos)
    {
        long se = GetParam_SE(type);
        if (se >= 0) {
            CGameSnd::Play(se, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    // Spawns the small ball at the gimmick's default position, moved out by
    // GetParam_LocOffsetZ, and starts it moving.
    void CStgGmk_Pinball_Act::CtorState_DEFAULT()
    {
        CStgSrtMove *pSrt = GetSrt();
        CStgGmk_Pinball *pGmk = static_cast<CStgGmk_Pinball *>(GetParent());
        pSrt->SetGravity(GetParam_Gravity());
        pSrt->ClearUnkE4();
        m_bAlive = 1;
        pGmk->GetMdl(0)->SetVisible(1);
        static_cast<CStgGmk_Pinball_Mdl *>(pGmk->GetMdl(0))->SetAttach(0, 0);
        m_bInField = 0;
        bcs::FVEC3 pos = pGmk->GetDefaultPos();
        float ofs = GetParam_LocOffsetZ();
        pos.z += pos.z > 0.0f ? ofs : -ofs;
        pos.y -= GetParam_InitRadius();
        pSrt->SetPos(pos);
        m_fLife = m_fMaxLife = GetParam_MaxLife();
        m_fInvincibleTime = 0.0f;
        m_fGrowTimeMax = 30.0f;
        m_fGrowTime = 0.0f;
        FsmqChangeState(STATE_MOVE);
        FsmqRetry();
    }

    void CStgGmk_Pinball_Act::LoopState_DEFAULT()
    {
    }

    void CStgGmk_Pinball_Act::DtorState_DEFAULT()
    {
    }

    // Waiting in the launcher.
    void CStgGmk_Pinball_Act::CtorState_STANDBY()
    {
        GetParent()->GetMdl(0)->SetVisible(1);
        m_fGrowTimeMax = 30.0f;
        m_fGrowTime = 0.0f;
        m_bInField = 0;
    }

    void CStgGmk_Pinball_Act::LoopState_STANDBY()
    {
        GetFrameRateF();
    }

    void CStgGmk_Pinball_Act::DtorState_STANDBY()
    {
    }

    // Shot at GetShotAngle at GetParam_Speed.
    void CStgGmk_Pinball_Act::CtorState_MOVE()
    {
        CStgSrtMove *pSrt = GetSrt();
        pSrt->ClearFlagC8(0x10);
        bcs::FVEC3 vel;
        bcs::BCS_VecOneX_RotY(vel, GetShotAngle());
        bcs::BCS_Vec_Normalize(vel, vel);
        bcs::FVEC3Scale(&vel, &vel, GetParam_Speed());
        pSrt->SetVel(vel);
        float limit = GetParam_VelLimit();
        bcs::FVEC3 velMax(limit, limit, limit);
        pSrt->SetVelLimit(velMax);
    }

    // Rolls: grows, slows down above GetParam_Speed, is pulled towards the
    // goal line of its half, and bounces off the walls; out of the field
    // it scores and sleeps for GetParam_Interval. The rolling sound plays
    // while it is on the ground.
    void CStgGmk_Pinball_Act::LoopState_MOVE()
    {
        float rate = GetFrameRateF();
        CStgSrtMove *pSrt = GetSrt();
        bcs::FVEC3 prev = pSrt->GetPos();
        m_fGrowTime += rate;
        float grow = m_fGrowTime;
        if (m_fGrowTimeMax < grow) {
            grow = m_fGrowTimeMax;
        }
        m_fGrowTime = grow;
        float limit = GetParam_VelLimit();
        pSrt->SetVelLimit(bcs::FVEC3(limit, limit, limit));

        float speed = GetParam_Speed();
        GetParam_Dec();
        bcs::FVEC3 curVel = pSrt->GetVel();
        if (bcs::FVEC3Len(&curVel) > speed) {
            pSrt->SetDec(GetParam_Dec());
        } else {
            pSrt->SetDec(0.0f);
        }
        float acc = GetParam_ForceAcc();
        if (prev.z < 0.0f) {
            pSrt->SetAccel(bcs::FVEC3(0.0f, 0.0f, -acc));
        } else {
            pSrt->SetAccel(bcs::FVEC3(0.0f, 0.0f, acc));
        }
        pSrt->Update(rate);

        bcs::FVEC3 pos = pSrt->GetPos();
        // The distance moved this frame. Unused; the original computes it
        // too (perhaps for stripped debug code).
        bcs::FVEC3 move;
        bcs::FVEC3 vel = pSrt->GetVel();
        bcs::FVEC3Sub(&move, &pos, &prev);
        if (WallHitCheck(&pos, &vel, prev, pos, vel)) {
            m_bAlive = 0;
            SE_PlayByParam(3);
            CGame::SendMessage(0x25, 4, !(pos.z < 0.0f), 0);
            Sleep(GetParam_Interval());
            FsmqRetry();
            return;
        }

        pSrt->SetPos(pos);
        float len = bcs::FVEC3Len(&vel);
        float max = GetParam_VelLimit();
        if (len > max) {
            bcs::BCS_Vec_Normalize(vel, vel);
            bcs::FVEC3Scale(&vel, &vel, max);
        }
        pSrt->SetVel(vel);

        if (pSrt->GetPos().y <= 0.05f) {
            if (!SE_IsAlive(0)) {
                SE_PlayPos(0, GetParam_SE(2), 3.0f, 0, 0);
            }
        } else {
            SE_Stop(0, 0.0f);
        }
    }

    void CStgGmk_Pinball_Act::DtorState_MOVE()
    {
        GetSrt()->StopRun();
        if (m_hEffect != 0) {
            CEffectCtrl::StopAttachEffect(m_hEffect);
            m_hEffect = 0;
        }
        SE_Stop(0, 0.0f);
    }

    // Breaks: hidden, the break effect at the ball's size; three frames
    // later it sleeps.
    void CStgGmk_Pinball_Act::CtorState_BREAK()
    {
        CStgSrt *pSrt = GetSrt();
        GetParent()->GetMdl(0)->SetVisible(0);
        m_fStateTime = 3.0f;
        bcs::FVEC3 pos = pSrt->GetPos();
        long eff = GetParam_Effect(0);
        if (eff >= 0) {
            unsigned long hEffect = CEffectCtrl::CreateAttachEffect(eff, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0);
            CEffectCluster *pCluster;
            if (hEffect != 0) {
                pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
            } else {
                pCluster = 0;
            }
            if (pCluster != 0) {
                float size = GetCurRadius() / 0.5f;
                bcs::FVEC3 scale(size, size, size);
                pCluster->SetTranslate(&pos);
                pCluster->SetScale(&scale);
            }
        }
    }

    void CStgGmk_Pinball_Act::LoopState_BREAK()
    {
        m_fStateTime -= GetFrameRateF();
        if (m_fStateTime <= 0.0f) {
            m_bAlive = 0;
            Sleep(GetParam_Interval());
        }
    }

    void CStgGmk_Pinball_Act::DtorState_BREAK()
    {
    }

    void CStgGmk_Pinball_Act::CtorState_SLEEP()
    {
        m_fStateTime = 0.0f;
        m_bAlive = 0;
        GetParent()->GetMdl(0)->SetVisible(0);
    }

    // Respawns after the frames given to Sleep (never for a negative
    // count).
    void CStgGmk_Pinball_Act::LoopState_SLEEP()
    {
        const float *pFrame = FsmqGetWork<float>();
        float rate = GetFrameRateF();
        if (*pFrame >= 0.0f) {
            m_fStateTime += rate;
            if (m_fStateTime >= *pFrame) {
                FsmqChangeState(STATE_RESPAWN);
                FsmqRetry();
            }
        }
    }

    void CStgGmk_Pinball_Act::DtorState_SLEEP()
    {
    }

    void CStgGmk_Pinball_Act::CtorState_RESPAWN()
    {
        const float *pAngle = FsmqGetWork<float>();
        m_bInField = 0;
        m_fLife = m_fMaxLife = GetParam_MaxLife();
        m_fInvincibleTime = 0.0f;
        m_fShotAngle = *pAngle;
        FsmqChangeState(STATE_DEFAULT);
        FsmqRetry();
    }

    void CStgGmk_Pinball_Act::LoopState_RESPAWN()
    {
    }

    void CStgGmk_Pinball_Act::DtorState_RESPAWN()
    {
    }

    void CStgGmk_Pinball_Act::CtorState_INACTIVE()
    {
        m_bAlive = 0;
        GetParent()->GetMdl(0)->SetVisible(0);
    }

    void CStgGmk_Pinball_Act::LoopState_INACTIVE()
    {
    }

    void CStgGmk_Pinball_Act::DtorState_INACTIVE()
    {
    }

    CStgGmk_Pinball_Drv::CStgGmk_Pinball_Drv(CStgGmk *pParent) : CStgDrv(pParent) {}

    CStgGmk_Pinball_Drv::~CStgGmk_Pinball_Drv() {}

    void CStgGmk_Pinball_Drv::Main(float rate)
    {
        CStgDrv::Main(rate);
    }

    CStgGmk_Pinball::CStgGmk_Pinball(bcs::CTask *pParent, int arg) : CStgGmk(pParent, 1), m_Unk68(arg) {}

    CStgGmk_Pinball::~CStgGmk_Pinball() {}

    CStgMdl *CStgGmk_Pinball::NewMdl(long heap, unsigned long idx)
    {
        return new (heap) CStgGmk_Pinball_Mdl(this);
    }

    void CStgGmk_Pinball::OnPostCreate() {}

    void CStgGmk_Pinball::OnSceneEntry() {}

    void CStgGmk_Pinball::OnSceneLeave() {}

    void CStgGmk_Pinball::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    void CStgGmk_Pinball::Respawn(float angle)
    {
        CStgGmk_Pinball_Act *pAct = GetPinballAct();
        if (pAct != 0 && !IsActive()) {
            pAct->Respawn(angle);
        }
    }

    int CStgGmk_Pinball::IsActive() const
    {
        const CStgGmk_Pinball_Act *pAct = GetPinballAct();
        if (pAct != 0) {
            return pAct->IsAlive();
        }
        return 0;
    }

    void CStgGmk_Pinball::Inactivate()
    {
        CStgGmk_Pinball_Act *pAct = GetPinballAct();
        if (pAct != 0) {
            pAct->FsmqChangeState(CStgGmk_Pinball_Act::STATE_INACTIVE);
        }
    }

    void CStgGmk_Pinball::Sleep(float frame)
    {
        CStgGmk_Pinball_Act *pAct = GetPinballAct();
        if (pAct != 0) {
            pAct->Sleep(frame);
        }
    }

    void CStgGmk_Pinball::Standby()
    {
        CStgGmk_Pinball_Act *pAct = GetPinballAct();
        if (pAct != 0) {
            pAct->Standby();
        }
    }

    void CStgGmk_Pinball::SetAccel(const bcs::FVEC3 &rAccel)
    {
        CStgGmk_Pinball_Act *pAct = GetPinballAct();
        if (pAct != 0) {
            pAct->GetSrt()->SetAccel(rAccel);
        }
    }

    void CStgGmk_Pinball::Disp() {}

    CPinballMan::CPinballMan() : m_ppGmk(0), m_nGmkNum(0), m_fShotTime(-1.0f), m_fUnk0C(-1.0f), m_vDefaultPos(0.0f, 0.0f, 0.0f)
    {
    }

    CPinballMan::~CPinballMan()
    {
        if (m_ppGmk != 0) {
            delete[] m_ppGmk;
        }
        m_ppGmk = 0;
    }

    void CPinballMan::Init()
    {
        long num = GetMaxNum();
        if (m_ppGmk != 0) {
            delete[] m_ppGmk;
        }
        m_ppGmk = 0;
        m_nGmkNum = num;
        m_ppGmk = new CStgGmk_Pinball *[num];
        for (long i = 0; i < num; i++) {
            m_ppGmk[i] = 0;
        }
        GetMecha()->Init();
    }

    long CPinballMan::GetMaxNum() const
    {
        return 1;
    }

    void CPinballMan::CreateGmk(bcs::CTask *pParent, int arg)
    {
        DeleteGmk();
        const bcs::FVEC3 &rPos = GetDefaultPos();
        for (long i = 0; i < m_nGmkNum; i++) {
            CStgGmk_Pinball *pGmk = NewGmk(pParent, arg);
            m_ppGmk[i] = pGmk;
            if (pGmk != 0) {
                pGmk->SetIdx(i);
                pGmk->Create();
                pGmk->SetDefaultPos(rPos);
                pGmk->SceneEntry();
                pGmk->Inactivate();
            }
        }
    }

    const bcs::FVEC3 &CPinballMan::GetDefaultPos() const
    {
        return m_vDefaultPos;
    }

    void CPinballMan::DeleteGmk()
    {
        if (m_ppGmk != 0) {
            for (long i = 0; i < m_nGmkNum; i++) {
                CStgGmk_Pinball *pGmk = m_ppGmk[i];
                if (pGmk != 0) {
                    pGmk->Delete();
                    m_ppGmk[i] = 0;
                }
            }
        }
    }

    void CPinballMan::Reset()
    {
        m_fShotTime = GetParam_InitialInterval();
        InactivateAll();
        MechaReset();
    }

    float CPinballMan::GetParam_InitialInterval() const
    {
        return 60.0f;
    }

    void CPinballMan::MechaReset()
    {
        GetMecha()->Reset();
    }

    // While fewer balls than GetParam_BallNum are in play, the launcher
    // shoots an idle one every GetParam_Interval frames; balls over the
    // number are taken out.
    void CPinballMan::Main(float frame)
    {
        if (m_ppGmk == 0) {
            return;
        }
        long active = GetActiveNum();
        long ballNum = GetParam_BallNum();
        long num = ballNum < m_nGmkNum ? ballNum : m_nGmkNum;
        if (active < num && GetMecha()->IsReady()) {
            CStgGmk_Pinball *pIdle = 0;
            for (long i = 0; i < m_nGmkNum; i++) {
                CStgGmk_Pinball *pGmk = m_ppGmk[i];
                if (pGmk != 0 && !pGmk->IsActive()) {
                    pIdle = m_ppGmk[i];
                    break;
                }
            }
            if (pIdle != 0 && m_fShotTime >= 0.0f) {
                m_fShotTime -= frame;
                if (m_fShotTime <= 0.0f) {
                    m_fShotTime = GetParam_Interval();
                    GetMecha()->Shoot(pIdle, GetShotAngle());
                }
            }
        } else if (active > num) {
            long count = 0;
            for (long i = 0; i < m_nGmkNum; i++) {
                CStgGmk_Pinball *pGmk = m_ppGmk[i];
                if (pGmk != 0 && pGmk->IsActive()) {
                    count++;
                    if (count > num) {
                        pGmk->Inactivate();
                    }
                }
            }
        }
    }

    long CPinballMan::GetParam_BallNum() const
    {
        return 1;
    }

    float CPinballMan::GetParam_Interval() const
    {
        return 180.0f;
    }

    void CPinballMan::MechaMain(float frame)
    {
        GetMecha()->Main(frame);
    }

    long CPinballMan::GetActiveNum() const
    {
        if (m_ppGmk == 0) {
            return 0;
        }
        long num = 0;
        for (long i = 0; i < m_nGmkNum; i++) {
            CStgGmk_Pinball *pGmk = m_ppGmk[i];
            if (pGmk != 0 && pGmk->IsActive()) {
                num++;
            }
        }
        return num;
    }

    void CPinballMan::SetDefaultPos(const bcs::FVEC3 &rPos)
    {
        m_vDefaultPos = rPos;
        for (long i = 0; i < m_nGmkNum; i++) {
            CStgGmk_Pinball *pGmk = m_ppGmk[i];
            if (pGmk != 0 && !pGmk->IsDelete()) {
                pGmk->SetDefaultPos(rPos);
            }
        }
    }

    void CPinballMan::InactivateAll()
    {
        for (long i = 0; i < m_nGmkNum; i++) {
            CStgGmk_Pinball *pGmk = m_ppGmk[i];
            if (pGmk != 0) {
                pGmk->Inactivate();
            }
        }
    }

    // The angle a ball is shot at, towards the goal of the team chosen:
    // the team behind (the leader's opponent) except at the side rate's
    // odds, at random on a draw.
    float CPinballMan::GetShotAngle() const
    {
        float angle = GetParam_ShotAngle();
        long team = GetWinTeam();
        long rate = GetParam_ShotSideRate();
        if (team >= 0) {
            if (rate == 0 || bcs::BCS_RandMax(100L) > rate) {
                team ^= 1;
            }
        } else {
            team = bcs::BCS_RandMax(100L) >= 50;
        }
        if (CGame::GetTargetGoalIdx(team) == 0) {
            return 3.1415927f + angle;
        }
        return 3.1415927f - angle;
    }

    float CPinballMan::GetParam_ShotAngle() const
    {
        return 0.0f;
    }

    long CPinballMan::GetWinTeam() const
    {
        return -1;
    }

    long CPinballMan::GetParam_ShotSideRate() const
    {
        return 50;
    }

    // The launcher's part of the entrance event: it takes up an idle ball,
    // or just idles.
    void CPinballMan::MechaEvent00()
    {
        long active = GetActiveNum();
        long ballNum = GetParam_BallNum();
        long num = ballNum < m_nGmkNum ? ballNum : m_nGmkNum;
        if (active < num && GetMecha()->IsReady()) {
            CStgGmk_Pinball *pIdle = 0;
            for (long i = 0; i < m_nGmkNum; i++) {
                CStgGmk_Pinball *pGmk = m_ppGmk[i];
                if (pGmk != 0 && !pGmk->IsActive()) {
                    pIdle = m_ppGmk[i];
                    break;
                }
            }
            if (pIdle != 0) {
                GetMecha()->StartEvent(pIdle);
                return;
            }
            GetMecha()->SetIdle();
            return;
        }
        GetMecha()->SetIdle();
    }

    // The launcher needs a model with the node the balls leave from and
    // an animation.
    void CPinballMecha::SetMdl(CStageMdlListNode *pNode)
    {
        m_pNode = pNode;
        if (pNode != 0) {
            bool bValid = false;
            bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(pNode->GetObj());
            if (pMdl != 0) {
                nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
                if (mdl.IsValid()) {
                    nw4r::g3d::ResNode node = mdl.GetResNode("tekkyu_exit_pos_null");
                    if (node.IsValid()) {
                        m_nExitNode = node.GetID();
                        if (pNode->GetAnm() != 0) {
                            bValid = true;
                        }
                    }
                }
            }
            if (!bValid) {
                m_pNode = 0;
            }
        }
    }

    void CPinballMecha::Main(float frame)
    {
        SE_Update(frame);
        if (IsValid()) {
            UpdateQuake();
            switch (m_nState) {
                case STATE_IDLE:
                    UpdateIdle(frame);
                    break;
                case STATE_EVENT:
                    UpdateEvent(frame);
                    break;
                case STATE_SHOOT_LEFT:
                    UpdateShootLeft(frame);
                    break;
                case STATE_SHOOT_RIGHT:
                    UpdateShootRight(frame);
                    break;
            }
        }
    }

    // Drops the ball being shot and idles.
    void CPinballMecha::Reset()
    {
        if (m_pGmk != 0) {
            m_pGmk->Inactivate();
            static_cast<CStgGmk_Pinball_Mdl *>(m_pGmk->GetMdl(0))->ClearAttach();
        }
        m_pGmk = 0;
        ClearQuake();
        SetIdle();
        SE_StopAll();
    }

    void CPinballMecha::SetIdle()
    {
        m_nState = STATE_IDLE;
        m_nStep = 0;
        ClearQuake();
        if (IsValid()) {
            CStageNodeAnm *pAnm = m_pNode->GetAnm();
            pAnm->Play(1, 0, 1.0f, 0.0f, 0.0f, 1);
            pAnm->Play(2, 0, 1.0f, 0.0f, 0.0f, 2);
            ResetIdleCount();
            bcs::FVEC3 pos = m_pNode->GetObj()->m_vTrans;
            SE_PlayPos(0, 0xD71, pos, 0.0f, 0);
        }
    }

    // The entrance event: the launcher takes the ball up, at 210 degrees.
    void CPinballMecha::StartEvent(CStgGmk_Pinball *pGmk)
    {
        m_nState = STATE_EVENT;
        m_nStep = 0;
        m_pGmk = pGmk;
        m_fAngle = NormalizeAngle(3.6651917f);
        if (IsValid()) {
            CStageNodeAnm *pAnm = m_pNode->GetAnm();
            pAnm->Play(7, 0, 1.0f, 0.0f, 0.0f, 1);
            pAnm->Play(1, 0, 1.0f, 0.0f, 0.0f, 2);
            pGmk->Standby();
            bcs::CScnObj *pObj = m_pNode->GetObj();
            CStgGmk_Pinball_Mdl *pMdl = static_cast<CStgGmk_Pinball_Mdl *>(pGmk->GetMdl(0));
            pMdl->SetAttach(pObj, m_nExitNode);
            bcs::FVEC3 pos = m_pNode->GetObj()->m_vTrans;
            SE_PlayPos(0, 0xD6F, pos, 0.0f, 0);
            AddQuake(250.0f);
            AddQuake(265.0f);
        }
    }

    // Shoots pGmk at angle, swinging to the left or to the right.
    void CPinballMecha::Shoot(CStgGmk_Pinball *pGmk, float angle)
    {
        angle = NormalizeAngle(angle);
        m_nState = angle > 3.1415927f ? STATE_SHOOT_LEFT : STATE_SHOOT_RIGHT;
        m_nStep = 0;
        m_pGmk = pGmk;
        m_fAngle = angle;
        if (!IsValid() || pGmk == 0) {
            return;
        }
        CStageNodeAnm *pAnm = m_pNode->GetAnm();
        if (m_nState == STATE_SHOOT_LEFT) {
            pAnm->Play(3, 0, 1.0f, 0.0f, 0.0f, 1);
            AddQuake(150.0f);
            AddQuake(165.0f);
        } else {
            pAnm->Play(5, 0, 1.0f, 0.0f, 0.0f, 1);
            AddQuake(150.0f);
            AddQuake(165.0f);
        }
        pAnm->Play(1, 0, 1.0f, 0.0f, 0.0f, 2);
        pGmk->Standby();
        bcs::CScnObj *pObj = m_pNode->GetObj();
        CStgGmk_Pinball_Mdl *pMdl = static_cast<CStgGmk_Pinball_Mdl *>(pGmk->GetMdl(0));
        pMdl->SetAttach(pObj, m_nExitNode);
        bcs::FVEC3 pos = m_pNode->GetObj()->m_vTrans;
        SE_PlayPos(0, 0xD70, pos, 0.0f, 0);
    }

    // Idling: after a few loops of animation 1 it fidgets once (2).
    void CPinballMecha::UpdateIdle(float frame)
    {
        if (!IsValid()) {
            return;
        }
        CStageNodeAnm *pAnm = m_pNode->GetAnm();
        if (!pAnm->IsEnd(1)) {
            return;
        }
        if (--m_nIdleCount < 0) {
            pAnm->Play(2, 0, 1.0f, 0.0f, 0.0f, 1);
            ResetIdleCount();
            bcs::FVEC3 pos = m_pNode->GetObj()->m_vTrans;
            SE_PlayPos(0, 0xD72, pos, 0.0f, 0);
        } else {
            pAnm->Play(1, 0, 1.0f, 0.0f, 0.0f, 1);
            bcs::FVEC3 pos = m_pNode->GetObj()->m_vTrans;
            SE_PlayPos(0, 0xD71, pos, 0.0f, 0);
        }
    }

    // The three shots: the swing (anim), then the ball is let go and the
    // launcher idles when the next animation ends.
    void CPinballMecha::UpdateEvent(float frame)
    {
        if (!IsValid()) {
            return;
        }
        CStageNodeAnm *pAnm = m_pNode->GetAnm();
        if (pAnm->IsEnd(2)) {
            pAnm->Play(2, 0, 1.0f, 0.0f, 0.0f, 2);
        }
        switch (m_nStep) {
            case 0:
                if (pAnm->IsEnd(1)) {
                    pAnm->Play(8, 0, 1.0f, 0.0f, 0.0f, 1);
                    UpdateGmkPos();
                    m_nStep++;
                }
                break;
            case 1:
                if (pAnm->IsEnd(1)) {
                    SetIdle();
                }
                break;
        }
    }

    void CPinballMecha::UpdateShootLeft(float frame)
    {
        if (!IsValid()) {
            return;
        }
        CStageNodeAnm *pAnm = m_pNode->GetAnm();
        if (pAnm->IsEnd(2)) {
            pAnm->Play(2, 0, 1.0f, 0.0f, 0.0f, 2);
        }
        switch (m_nStep) {
            case 0:
                if (pAnm->IsEnd(1)) {
                    pAnm->Play(4, 0, 1.0f, 0.0f, 0.0f, 1);
                    UpdateGmkPos();
                    m_nStep++;
                }
                break;
            case 1:
                if (pAnm->IsEnd(1)) {
                    SetIdle();
                }
                break;
        }
    }

    void CPinballMecha::UpdateShootRight(float frame)
    {
        if (!IsValid()) {
            return;
        }
        CStageNodeAnm *pAnm = m_pNode->GetAnm();
        if (pAnm->IsEnd(2)) {
            pAnm->Play(2, 0, 1.0f, 0.0f, 0.0f, 2);
        }
        switch (m_nStep) {
            case 0:
                if (pAnm->IsEnd(1)) {
                    pAnm->Play(6, 0, 1.0f, 0.0f, 0.0f, 1);
                    UpdateGmkPos();
                    m_nStep++;
                }
                break;
            case 1:
                if (pAnm->IsEnd(1)) {
                    SetIdle();
                }
                break;
        }
    }

    void CPinballMecha::ResetIdleCount()
    {
        m_nIdleCount = 3;
        m_nIdleCount += bcs::BCS_RandMax(2L);
    }

    // Lets the ball go at the exit node, to be shot at m_fAngle. (The
    // original tests with "or": a launcher without a model and with a ball
    // would read the null model.)
    void CPinballMecha::UpdateGmkPos()
    {
        if (IsValid() || m_pGmk != 0) {
            bcs::FMTX34 mtx;
            m_pNode->GetObj()->GetNodeWorldMtx(m_nExitNode, mtx);
            bcs::FVEC3 pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
            m_pGmk->SetDefaultPos(pos);
            m_pGmk->Respawn(m_fAngle);
        }
    }

    void CPinballMecha::ClearQuake()
    {
        m_afQuake[0] = -1.0f;
        m_afQuake[1] = -1.0f;
        m_nQuakeNum = 0;
    }

    void CPinballMecha::AddQuake(float frame)
    {
        if (m_nQuakeNum < QUAKE_NUM) {
            m_afQuake[m_nQuakeNum++] = frame;
        }
    }

    // The camera shakes once at each frame added by AddQuake.
    void CPinballMecha::UpdateQuake()
    {
        if (!IsValid()) {
            return;
        }
        CStageNodeAnm *pAnm = m_pNode->GetAnm();
        if (pAnm == 0) {
            return;
        }
        for (long i = 0; i < m_nQuakeNum; i++) {
            if (!(m_afQuake[i] < 0.0f) && pAnm->GetFrame(0) >= m_afQuake[i]) {
                m_afQuake[i] = -1.0f;
                CGame::StartCameraQuake(0x30, -1, -1);
            }
        }
    }

    void CPinballMan::SetMechaMdl(CStageMdlListNode *pNode)
    {
        GetMecha()->SetMdl(pNode);
    }

    void CPinballMan::MechaCancel()
    {
        GetMecha()->Cancel();
    }

    CStgGmk_Pinball *CPinballMan::GetGmk(long idx)
    {
        if (m_ppGmk != 0) {
            return m_ppGmk[idx];
        }
        return 0;
    }

    const CStgGmk_Pinball *CPinballMan::GetGmk(long idx) const
    {
        if (m_ppGmk != 0) {
            return m_ppGmk[idx];
        }
        return 0;
    }
}
