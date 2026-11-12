// The containers of hockey stage 09 (the conveyor belts): their model
// (msw::CStgGmk_Container_Mdl_Hockey) and actor
// (msw::CStgGmk_Container_Act_Hockey). The gimmick's sub type picks the
// kind: 0 a plain container, 1 a bomb, 2 a treasure box. A container that
// falls off the belt ends at the court's sides; a hit bomb explodes (a
// short-lived attack sphere, CExplosion), a hit treasure box opens and
// drops coins.
#include "game/msw/CStgGmk_Container_Hockey.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FMTX34.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgSrt.hpp"
#include "nw4r/math/types.h"

namespace {

    // (stand-ins for operators) Reached through this one more inline level,
    // the float registers come out numbered as in the original (see
    // game/hockey/CStgGmk_Case_Act_Hockey.cpp).
    inline void VecSub(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Sub(&rOut, &r1, &r2);
    }
    inline void VecAdd(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        bcs::FVEC3Add(&rOut, &r1, &r2);
    }
    inline void VecNormalize(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, rIn);
    }
    inline float VecDot(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
    {
        return bcs::FVEC3Dot(&r1, &r2);
    }

    // (name guessed) The effect cluster of handle hEffect; null without one.
    inline msw::CEffectCluster *FindEffect(unsigned long hEffect)
    {
        msw::CEffectCluster *pEff;
        if (hEffect != 0) {
            pEff = msw::CEffectCtrl::GetInstance()->FindList(hEffect);
        } else {
            pEff = 0;
        }
        return pEff;
    }

    // (name guessed) The node of the treasure box the coins drop from.
    const char *s_CoinNodeName[] = { "obj_point", 0 };

    // (name guessed) The work memory of the OPEN and INACTIVE states: OPEN
    // keeps a flag (negative until the coins have dropped), INACTIVE the
    // frames to wait before the respawn.
    struct STATE_WORK {
        float m_fValue;
    };

    // (name guessed) The explosion of a bomb container: an attack sphere at
    // the container's position for HOCKEY_PRM_S09_BOMB_EXPLODE_TIME frames,
    // with the explosion effect. 0x44 bytes.
    class CExplosion : public bcs::CUnivTaskEx {
        public:
            CExplosion(const bcs::FVEC3 &rPos) : CUnivTaskEx(msw::CGame::GetTempParent(), -1)
            {
                m_fTime = msw::HOCKEY_PRM_S09_BOMB_EXPLODE_TIME;
                m_AtkColl.Create(1, 4, 0, 0);
                m_AtkColl.SetPos(0, rPos);
                m_AtkColl.SetCollKind(0, 4);
                m_AtkColl.SetTargetFlag(0, 2);
                m_AtkColl.SetAtkType(0, 0x11F01);
                m_AtkColl.SetRadius(0, msw::HOCKEY_PRM_S09_BOMB_EXPLODE_SIZE);
                m_AtkColl.SetEnable(0);
                unsigned long hEffect = msw::CEffectCtrl::CreateEffect(6, 3, 0, 0, 0);
                msw::CEffectCluster *pEff = FindEffect(hEffect);
                if (pEff != 0) {
                    float scale = msw::HOCKEY_PRM_S09_BOMB_EXPLODE_EFF_SCALE;
                    bcs::FVEC3 vScale(scale, scale, scale);
                    pEff->SetTranslate(&rPos);
                    pEff->SetScale(&vScale);
                }
            }
            virtual ~CExplosion();
            virtual void Main();

        private:
            msw::CAtkCollision m_AtkColl; // 0x34
            float m_fTime;                // 0x40, frames left
    };
}

namespace msw {

    // The three models: plain, bomb and treasure box; the treasure box has
    // two animations (opening, then open).
    void CStgGmk_Container_Mdl_Hockey::Create()
    {
        const char *anm_suffix[] = { "_m01", "_m01", "_m02", 0 };
        static const MDL_PARAM mdl_param[] = {
            { "s09_container", "s09_container", "s09_container", 0, 0, 0, 0, 0 },
            { "s09_bombhei", "s09_bombhei", "s09_bombhei", 0, 0, 0, 0, 0 },
            { "s09_tbox_01", "s09_tbox_01", "s09_tbox_01", 0, anm_suffix, "s09_tbox", 0, 0 },
        };

        long idx = GetType();
        idx = idx > 2 ? 2 : (idx < 0 ? 0 : idx);
        CStgMdl::Create(CGenericGimmickCreator(CLoadData::GetRes(0), &mdl_param[idx]));
    }

    // Turned half round when the camera looks at goal 0.
    void CStgGmk_Container_Mdl_Hockey::Main(float rate)
    {
        bcs::FMTX34 mtx;
        bcs::FMTX34Copy(&mtx, &GetMtx());
        float rot = 0.0f;
        CGameCamera *pCamera = CGameCamera::GetInstance(0);
        if (pCamera != 0 && pCamera->GetTargetGoalIdx() == 0) {
            rot += 3.1415927f;
        }
        bcs::FMTX34 rotMtx;
        nw4r::math::MTX34RotXYZRad(reinterpret_cast<nw4r::math::MTX34 *>(&rotMtx), 0.0f, rot, 0.0f);
        PSMTXConcat(mtx.m, rotMtx.m, mtx.m);
        SetMtx(mtx);
        CStgGmk_Case_Mdl::Main(rate);
    }

    float CStgGmk_Container_Act_Hockey::GetParam_Width() const
    {
        switch (GetSubType_()) {
            case 0:
                return HOCKEY_PRM_S09_CONTAINER_WIDTH;
            case 1:
                return HOCKEY_PRM_S09_BOMB_SIZE;
            default:
                return HOCKEY_PRM_S09_TREASURE_WIDTH;
        }
    }

    float CStgGmk_Container_Act_Hockey::GetParam_Depth() const
    {
        switch (GetSubType_()) {
            case 0:
                return HOCKEY_PRM_S09_CONTAINER_WIDTH;
            case 1:
                return HOCKEY_PRM_S09_BOMB_SIZE;
            default:
                return HOCKEY_PRM_S09_TREASURE_WIDTH;
        }
    }

    float CStgGmk_Container_Act_Hockey::GetParam_Height() const
    {
        switch (GetSubType_()) {
            case 0:
                return HOCKEY_PRM_S09_CONTAINER_HEIGHT;
            case 1:
                return HOCKEY_PRM_S09_BOMB_SIZE;
            default:
                return HOCKEY_PRM_S09_TREASURE_HEIGHT;
        }
    }

    float CStgGmk_Container_Act_Hockey::GetParam_MdlWidth() const
    {
        switch (GetSubType_()) {
            case 0:
                return 2.0f;
            case 1:
                return 1.0f;
            case 2:
                return 2.0f;
        }
        return 1.0f;
    }

    float CStgGmk_Container_Act_Hockey::GetParam_MdlDepth() const
    {
        switch (GetSubType_()) {
            case 0:
                return 2.0f;
            case 1:
                return 1.0f;
            case 2:
                return 2.0f;
        }
        return 1.0f;
    }

    float CStgGmk_Container_Act_Hockey::GetParam_MdlHeight() const
    {
        switch (GetSubType_()) {
            case 0:
                return 1.0f;
            case 1:
                return 1.0f;
            case 2:
                return 1.5f;
        }
        return 1.0f;
    }

    // Only the break effect.
    long CStgGmk_Container_Act_Hockey::GetParam_Effect(long type) const
    {
        switch (type) {
            case 2:
                return 0xC;
        }
        return -1;
    }

    // Only the break sound, one per kind.
    long CStgGmk_Container_Act_Hockey::GetParam_SE(long type) const
    {
        if (type == 0) {
            switch (static_cast<const CStgGmk_Case *>(GetParent())->GetSubType()) {
                case 0:
                    return 0xD4F;
                case 1:
                    return 0xD4E;
                case 2:
                    return 0xD50;
            }
        }
        return -1;
    }

    // A treasure box stays as long as its opening animation.
    float CStgGmk_Container_Act_Hockey::GetParam_BreakTime() const
    {
        if (GetSubType_() == 2) {
            const CStgMdl *pMdl = GetMdl(0);
            if (pMdl != 0) {
                return pMdl->GetAnmTotalFrame(static_cast<CStageAnm::ANM_IDX>(0));
            }
        }
        return CStgGmk_Case_Act::GetParam_BreakTime();
    }

    // On the floor the container moves with the belt under it. Pushed past
    // the court's sides (by more than HOCKEY_PRM_S09_CONTAINER_OFFSET_X) it
    // is taken away. The reflect wait counts down to zero.
    void CStgGmk_Container_Act_Hockey::OnUpdate(float rate)
    {
        CStgSrt *pSrt = GetSrt();
        CStageMan *pStage = CStageMan::GetInstance();
        CStgGmk_Case_Act::OnUpdate(rate);
        bcs::FVEC3 pos = pSrt->GetPos();
        if (pos.y <= 0.05f) {
            bcs::FVEC3 flow;
            pStage->GetFloorFlow(flow, pos, CStageMan::FLOW_TYPE_0);
            VecAdd(pos, pos, flow);
            pSrt->SetPos(pos);
        }
        float ofs = HOCKEY_PRM_S09_CONTAINER_OFFSET_X;
        float left;
        float right;
        pStage->GetCourtRect(&left, &right, 0, 0);
        if (pos.x < left - ofs || pos.x > right + ofs) {
            FsmqChangeState(STATE_INACTIVE);
        }
        m_fReflectWait -= rate;
        float wait = 0.0f;
        if (m_fReflectWait > wait) {
            wait = m_fReflectWait;
        }
        m_fReflectWait = wait;
    }

    // The catch volume on top does not take the puck.
    void CStgGmk_Container_Act_Hockey::UpdateColl()
    {
        CStgGmk_Case_Act::UpdateColl();
        if (m_BodyColl.IsBodyEnable(4)) {
            unsigned long flag = m_BodyColl.GetTargetFlag(4);
            m_BodyColl.SetTargetFlag(4, flag & ~4);
        }
    }

    // A bomb explodes, a treasure box opens. The ball's direction and speed
    // are measured but not used.
    void CStgGmk_Container_Act_Hockey::OnDamage(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (pBall != 0) {
            bcs::FVEC3 dir;
            bcs::FVEC3 vel = pBall->GetVelocity();
            VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
            dir.y = 0.0f;
            VecNormalize(dir, dir);
            bcs::FVEC3Len(&vel);
        }
        switch (GetSubType_()) {
            case 1:
                new (1) CExplosion(GetSrt()->GetPos());
                CGame::StartCameraQuake(0x2B, -1, -1);
                break;
            case 2:
                FsmqChangeState(STATE_OPEN);
                break;
        }
    }

    void CStgGmk_Container_Act_Hockey::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
    }

    // A plain container sends the puck along X or Z, away from it (along Z
    // when it struck within 45 degrees of the front or back); a bomb or a
    // treasure box sends it straight away from its centre and breaks. The
    // container then ignores the puck for ten frames.
    unsigned long CStgGmk_Container_Act_Hockey::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                             bcs::FVEC3 &rPos, bcs::FVEC3 &rVel)
    {
        if (m_fReflectWait > 0.0f) {
            return 0;
        }

        unsigned char bHit = 1;
        bcs::FVEC3Len(&rVel);
        bcs::FVEC3 dir;
        switch (GetSubType_()) {
            case 1:
                VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
                dir.y = 0.0f;
                break;
            case 2:
                VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
                dir.y = 0.0f;
                break;
            default: {
                VecSub(dir, pBall->GetPos(), GetSrt()->GetPos());
                dir.y = 0.0f;
                VecNormalize(dir, dir);
                bcs::FVEC3 front(0.0f, 0.0f, 1.0f);
                float cos = VecDot(dir, front);
                cos = cos > 1.0f ? 1.0f : (cos < -1.0f ? -1.0f : cos);
                float angle = bcs::BCS_ACosT(cos);
                if (angle < 0.7853982f || angle > 2.3561945f) {
                    dir.x = 0.0f;
                } else {
                    dir.z = 0.0f;
                }
                break;
            }
        }
        VecNormalize(rVel, dir);

        m_fReflectWait = 10.0f;
        long type = GetSubType_();
        if ((type == 1 || type == 2) && IsAlive() && GetState() != STATE_BREAK && GetState() != STATE_OPEN) {
            FsmqChangeState(STATE_BREAK);
            OnDamage(0, 0);
        }
        unsigned char kind = 11;
        if (GetSubType_() == 2) {
            kind = 12;
        }
        return bHit | (kind << 8);
    }

    // A bomb that something walks into explodes.
    void CStgGmk_Container_Act_Hockey::OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (GetSubType_() == 1 && IsAlive() && GetState() != STATE_BREAK && GetState() != STATE_OPEN) {
            FsmqChangeState(STATE_BREAK);
            OnDamage(0, 0);
        }
    }

    // A container taken away is deleted with its gimmick.
    void CStgGmk_Container_Act_Hockey::CtorState_INACTIVE()
    {
        SetAlive(0);
        GetParent()->GetMdl(0)->SetVisible(0);
        GetParent()->Delete();
    }

    void CStgGmk_Container_Act_Hockey::LoopState_INACTIVE()
    {
    }

    void CStgGmk_Container_Act_Hockey::DtorState_INACTIVE()
    {
    }

    // A treasure box plays its glitter animation on the belt.
    void CStgGmk_Container_Act_Hockey::CtorState_MOVE()
    {
        if (GetSubType_() == 2) {
            CStgMdl *pMdl = GetMdl(0);
            if (pMdl != 0) {
                pMdl->SetAnm(1, 2, 1.0f, 0.0f, 0.0f, -1);
            }
        }
    }

    void CStgGmk_Container_Act_Hockey::LoopState_MOVE()
    {
    }

    void CStgGmk_Container_Act_Hockey::DtorState_MOVE()
    {
    }

    // Opening (a treasure box hit): the break effect, scaled to the larger
    // side, and sound; a treasure box plays its opening animation with a
    // sparkle.
    void CStgGmk_Container_Act_Hockey::CtorState_OPEN()
    {
        STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        CStgSrt *pSrt = GetSrt();
        pWork->m_fValue = -1.0f;
        bcs::FVEC3 pos = pSrt->GetPos();
        long eff = GetParam_Effect(0);
        if (eff >= 0) {
            unsigned long hEffect = CEffectCtrl::CreateAttachEffect(eff, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0);
            CEffectCluster *pEff = FindEffect(hEffect);
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
        if (GetSubType_() == 2) {
            CStgMdl *pMdl = GetMdl(0);
            if (pMdl != 0) {
                pMdl->SetAnm(2, 0, 1.0f, 0.0f, 0.0f, -1);
                bcs::CMdlObj *pObj = pMdl->GetMdlObj();
                if (pObj != 0) {
                    CEffectCtrl::CreateAttachEffectAuto(0xEC, pObj, 3);
                }
            }
        }
    }

    // From frame 15 of the opening the coins drop (once) from the box's
    // coin node; when the animation ends the container goes away.
    void CStgGmk_Container_Act_Hockey::LoopState_OPEN()
    {
        STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        CStgMdl *pMdl = GetMdl(0);
        if (pWork->m_fValue < 0.0f && (pMdl == 0 || pMdl->GetAnmFrame(static_cast<CStageAnm::ANM_IDX>(0)) >= 15.0f)) {
            pWork->m_fValue = 1.0f;
            long num = HOCKEY_PRM_S09_TREASURE_COIN_NUM;
            if (num > 0) {
                bcs::FVEC3 pos = GetSrt()->GetPos();
                bcs::CMdlObj *pObj = pMdl != 0 ? pMdl->GetMdlObj() : 0;
                if (pObj != 0) {
                    long node = pObj->GetNodeID(s_CoinNodeName[0]);
                    if (node >= 0) {
                        bcs::FMTX34 mtx;
                        pObj->GetNodeWorldMtx(node, mtx);
                        pos.x = mtx.m[0][3];
                        pos.y = mtx.m[1][3];
                        pos.z = mtx.m[2][3];
                    }
                }
                CGimmickMan::CreateDropCoin(pos, num);
            }
        }
        if (pMdl == 0 || pMdl->IsAnmEnd(-1)) {
            static_cast<STATE_WORK *>(FsmqSetState(STATE_INACTIVE))->m_fValue = GetParam_Interval();
            FsmqRetry();
        }
    }

    void CStgGmk_Container_Act_Hockey::DtorState_OPEN()
    {
    }
}

namespace {

    // Ends when its time has run out.
    void CExplosion::Main()
    {
        m_fTime -= GetFrameRateF();
        if (m_fTime <= 0.0f) {
            Delete();
        }
    }

    CExplosion::~CExplosion()
    {
    }
}

msw::CStgGmk_Container_Act_Hockey::~CStgGmk_Container_Act_Hockey()
{
}
