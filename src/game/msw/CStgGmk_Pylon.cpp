#include "game/msw/CStgGmk_PylonCluster.hpp"
#include "game/msw/CStgGmk_Pylon.hpp"
#include "game/msw/CStgGmk_Pylon_Act.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgMdlObjBase.hpp"
#include "game/msw/CStgSrt.hpp"
#include "nw4r/math/types.h"
#include "revolution/mtx.h"

// The pylons of stage 04 (red cones). In the original one file holds the
// model (CStgGmk_Pylon_Mdl, from 0x80217F10), the actor
// (CStgGmk_Pylon_Act), the driver (CStgGmk_Pylon_Drv), the gimmick
// (CStgGmk_Pylon) and the cluster that places them
// (CStgGmk_PylonCluster), with their weak functions at the end of .text
// (to 0x8021C96C) and the vtables in one .data block (0x80488838 to
// 0x804890C8). Evidence: the gimmick's NewMdl inlines the model's
// constructor, the template cluster's NewGmk the gimmick's, the inline
// getters the actor calls are emitted between the actor's functions, and
// the .sdata2 constants (0x805C5138 to 0x805C51A0) are shared by all five.
//
// A pylon stands at its cluster's location; a push slides it (slowed by
// GetParam_PushResist), a ball hit costs a life and a pylon out of lives
// breaks. A pylon pushed off the field falls over and is gone until
// GetParam_Interval frames later.

namespace {

    const msw::CStgMdl::MDL_PARAM s_MdlParam[] = {
        { "s04_redcorn_00", 0, 0, 0, 0, 0, 0, 0 },
    };

    // (layout guessed) The location table (CLoadData 0x6C): the number of
    // entries at 0x08 and the entries, 20.12 fixed point, from 0x10. The
    // same table as stage 17's trolley reads.
    struct LOC_RAW_TBL {
        unsigned char m_Unk00[8];
        long m_nNum;              // 0x08
        unsigned char m_Unk0C[4];
        long m_aData[1];          // 0x10
    };

    inline long LocRawTblGetNum(const LOC_RAW_TBL *pTbl)
    {
        return pTbl == 0 ? 0 : pTbl->m_nNum;
    }

    // Entry idx of the table, 0 when there is none.
    inline long LocRawTblGet(const LOC_RAW_TBL *pTbl, long idx)
    {
        if (pTbl == 0) {
            return 0;
        }
        const long *pData = pTbl->m_aData;
        if (idx < 0 || idx >= LocRawTblGetNum(pTbl)) {
            return 0;
        }
        return pData[idx];
    }

    // (names guessed) The work memory of the actor's states: the frames to
    // wait in STANDBY (negative: for ever), the direction to fall in FALL.
    struct STATE_WORK {
        long m_Unk00;
        float m_fArg; // 0x04
    };

    // (guess) A handle on the actor's gimmick; CtorState_MOVE makes one on
    // the stack and shows the model through it. Its vtable (0x80488934)
    // and its base's (after the model's) are in the original's .data.
    class CGmkRefBase {
        public:
            CGmkRefBase(msw::CStgObj *pObj) : m_pObj(pObj) {}
            virtual ~CGmkRefBase() {}

            msw::CStgObj *Get() const { return m_pObj; }

        protected:
            msw::CStgObj *m_pObj;
    };

    class CGmkRef : public CGmkRefBase {
        public:
            CGmkRef(msw::CStgObj *pObj) : CGmkRefBase(pObj) {}
            virtual ~CGmkRef() {}
    };
}

namespace msw {

    CStgGmk_Pylon_Mdl::CStgGmk_Pylon_Mdl(CStgGmk *pParent, long idx) : CStgMdl(pParent)
    {
        m_nMdlIdx = idx;
    }

    CStgGmk_Pylon_Mdl::~CStgGmk_Pylon_Mdl()
    {
    }

    void CStgGmk_Pylon_Mdl::Create()
    {
        long idx = m_nMdlIdx;
        idx = idx > 0 ? 0 : (idx < 0 ? 0 : idx);
        CGenericGimmickCreator creator(CLoadData::GetRes(0), &s_MdlParam[idx]);
        CStgMdl::Create(creator);
    }

    bcs::CMdlObj *CStgGmk_Pylon_Mdl::NewMdl(long heap) const
    {
        return new (heap) CStgMdlObjBase<bcs::CMdlObj>;
    }

    void CStgGmk_Pylon_Mdl::OnPostCreate()
    {
    }

    // Scales the model to the actor's radius and height.
    void CStgGmk_Pylon_Mdl::Main(float rate)
    {
        float height;
        CStgGmk_Pylon_Act *pAct = static_cast<CStgGmk_Pylon_Act *>(GetParent()->GetAct());
        bcs::CMdlObj *pMdl = GetMdlObj();
        if (pAct != 0 && pMdl != 0) {
            float depth = pAct->GetParam_MdlRadius() / (0.5f * GetParam_OrgDepth());
            height = pAct->GetParam_MdlHeight() / GetParam_OrgHeight();
            pMdl->m_vScale.x = pAct->GetParam_MdlRadius() / (0.5f * GetParam_OrgWidth());
            pMdl->m_vScale.y = height;
            pMdl->m_vScale.z = depth;
            pMdl->CalcRootMtx();
            pMdl->CalcWorldMtx(0);
        }
        CStgMdl::Main(rate);
    }

    float CStgGmk_Pylon_Mdl::GetParam_OrgWidth() const
    {
        return 1.0f;
    }

    float CStgGmk_Pylon_Mdl::GetParam_OrgDepth() const
    {
        return 1.0f;
    }

    float CStgGmk_Pylon_Mdl::GetParam_OrgHeight() const
    {
        return 1.0f;
    }

    CStgGmk_Pylon_Act::CStgGmk_Pylon_Act(CStgGmk *pParent)
        : CStgGmkAct(pParent), m_bAlive(0), m_fInvincibleTime(0.0f), m_fPushSeWait(0.0f)
    {
        FsmqInit(this, 7, 4, sizeof(STATE_WORK));
        FsmqSetState(STATE_DEFAULT, &CStgGmk_Pylon_Act::CtorState_DEFAULT, &CStgGmk_Pylon_Act::LoopState_DEFAULT,
                     &CStgGmk_Pylon_Act::DtorState_DEFAULT);
        FsmqSetState(STATE_STANDBY, &CStgGmk_Pylon_Act::CtorState_STANDBY, &CStgGmk_Pylon_Act::LoopState_STANDBY,
                     &CStgGmk_Pylon_Act::DtorState_STANDBY);
        FsmqSetState(STATE_MOVE, &CStgGmk_Pylon_Act::CtorState_MOVE, &CStgGmk_Pylon_Act::LoopState_MOVE,
                     &CStgGmk_Pylon_Act::DtorState_MOVE);
        FsmqSetState(STATE_BREAK, &CStgGmk_Pylon_Act::CtorState_BREAK, &CStgGmk_Pylon_Act::LoopState_BREAK,
                     &CStgGmk_Pylon_Act::DtorState_BREAK);
        FsmqSetState(STATE_RESPAWN, &CStgGmk_Pylon_Act::CtorState_RESPAWN, &CStgGmk_Pylon_Act::LoopState_RESPAWN,
                     &CStgGmk_Pylon_Act::DtorState_RESPAWN);
        FsmqSetState(STATE_INACTIVE, &CStgGmk_Pylon_Act::CtorState_INACTIVE, &CStgGmk_Pylon_Act::LoopState_INACTIVE,
                     &CStgGmk_Pylon_Act::DtorState_INACTIVE);
        FsmqSetState(STATE_FALL, &CStgGmk_Pylon_Act::CtorState_FALL, &CStgGmk_Pylon_Act::LoopState_FALL,
                     &CStgGmk_Pylon_Act::DtorState_FALL);
        CStgSrt *pSrt = GetSrt();
        pSrt->ResetVelocity(GetParam_Gravity());

        long idx;
        if (pParent != 0) {
            idx = pParent->GetIdx();
        } else {
            idx = -1;
        }
        float radius = GetParam_Radius();
        float height = GetParam_Height();
        GetParam_Height2();

        // 0 pushes the characters away, 1 and 2 are pushed by them, 2 is
        // also hit and caught.
        m_BodyColl.Create(3, 5, 3, 0);
        m_BodyColl.SetCollType(0, 1);
        m_BodyColl.SetRadius(0, radius);
        m_BodyColl.SetPushRank(0, 5);
        m_BodyColl.SetPushPower(0, 6);
        m_BodyColl.SetTargetFlag(0, 4);
        m_BodyColl.SetIndex(0, idx);
        for (long i = 0; i < 2; i++) {
            long n = i + 1;
            m_BodyColl.SetCollType(n, 4);
            m_BodyColl.SetRadius(n, radius);
            m_BodyColl.SetPushRank(n, 1);
            m_BodyColl.SetPushPower(n, 3);
            m_BodyColl.SetTargetFlag(n, 0x22);
            m_BodyColl.SetIndex(n, idx);
        }
        m_BodyColl.SetCollType(2, 1);
        m_BodyColl.SetRadius(2, radius);
        m_BodyColl.SetTargetFlag(2, 0x16);
        m_BodyColl.SetIndex(2, idx);
        RegistBodyPushCallback(m_BodyColl);
        RegistBodyDamageCallback(m_BodyColl);
        RegistBallCatchCallback(m_BodyColl);

        m_DefColl.Create(1, 5, 4, 0);
        m_DefColl.GetColl(0)->SetCollType(4);
        m_DefColl.GetColl(0)->SetCollType(1);
        m_DefColl.GetColl(0)->SetShape(4);
        bcs::FVEC3 size(radius, height, radius);
        m_DefColl.GetColl(0)->SetSize(size);
        m_DefColl.GetColl(0)->OnFlagA0(1);
        RegistReflectHitCallback(m_DefColl);
    }

    CStgGmk_Pylon_Act::~CStgGmk_Pylon_Act()
    {
    }

    // Runs the states. A pylon that lands hard plays the landing effect and
    // sound; the map keeps it inside.
    void CStgGmk_Pylon_Act::OnUpdate(float rate)
    {
        CStgSrt *pSrt = GetSrt();
        CStageMan::GetInstance();
        bcs::FVEC3 prev = pSrt->GetPos();
        if (m_fInvincibleTime > 0.0f) {
            m_fInvincibleTime -= rate;
        }
        FsmqExec();
        pSrt->Update(rate);

        if (IsAlive()) {
            bcs::FVEC3 pos = pSrt->GetPos();
            if (pos.y < 0.05f) {
                pos.y = 0.05f;
                pSrt->SetPos(pos);
                if (__fabsf(pSrt->GetUnkE4().y) > 3.0f) {
                    long eff = GetParam_Effect(3);
                    if (eff >= 0) {
                        CEffectCtrl::SimpleEffect(eff, 0, &pos, 0, 3, 0, 0, 0);
                    }
                    long se = GetParam_SE(4);
                    if (se >= 0) {
                        CGameSnd::Play(se, pSrt->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
                    }
                    OnLanding();
                }
                pSrt->ClearUnkE4();
            }
        }
        UpdateColl();
        UpdatePushSe(rate);

        bcs::FVEC3 hitPos;
        bcs::FVEC3 hitNormal;
        if (CheckCollBg(hitPos, hitNormal, prev, pSrt->GetPos())) {
            pSrt->SetPos(hitPos);
        }
    }

    // Whether the way from rFrom to rTo hits the map; rPos takes the point
    // it stops at.
    int CStgGmk_Pylon_Act::CheckCollBg(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, const bcs::FVEC3 &rFrom,
                                        const bcs::FVEC3 &rTo)
    {
        float radius = GetParam_Radius();
        float time;
        return CCollMan::DetectMapHit(rPos, rNormal, time, rFrom, rTo, radius + GetParam_FieldMargin(), 5, 0);
    }

    void CStgGmk_Pylon_Act::UpdateMdlMtx()
    {
        CStgAct::UpdateMdlMtx();
    }

    // The collision is on only while the pylon is alive: unit 0 at its
    // foot pushes, 1 around its middle is pushed, 2 over its height is hit.
    void CStgGmk_Pylon_Act::UpdateColl()
    {
        CStgSrt *pSrt = GetSrt();
        if (m_bAlive) {
            bcs::FVEC3 pos = pSrt->GetPos();
            float radius = GetParam_Radius();
            float height = GetParam_Height();
            float height2 = GetParam_Height2();
            CStgGmk *pGmk = static_cast<CStgGmk *>(GetParent());
            long idx;
            if (pGmk != 0) {
                idx = pGmk->GetClusterIndex();
            } else {
                idx = -1;
            }
            m_BodyColl.SetEnableAll();
            m_BodyColl.SetIndexAll(idx);
            m_BodyColl.SetPos(0, bcs::FVEC3(pos));
            m_BodyColl.SetSize(0, radius, height, 0.0f);
            m_BodyColl.SetPushEnable(0);
            m_BodyColl.SetPos(1, bcs::FVEC3(pos.x, 0.5f * height2 + pos.y, pos.z));
            m_BodyColl.SetSize(1, radius, 0.5f * height2, radius);
            m_BodyColl.SetPushRank(1, GetParam_PushRank());
            m_BodyColl.SetPushPower(1, GetParam_PushPower());
            m_BodyColl.SetPushEnable(1);
            m_BodyColl.SetPos(2, bcs::FVEC3(pos));
            m_BodyColl.SetSize(2, radius, height > 0.0f ? height : 0.0f, 0.0f);
            m_BodyColl.SetBodyEnable(2);
            m_BodyColl.SetCatchEnable(2);
            if (IsEnableDefColl()) {
                m_DefColl.GetColl(0)->SetPos(bcs::FVEC3(pos));
                m_DefColl.GetColl(0)->SetSize(radius, height, 0.0f);
                m_DefColl.GetColl(0)->OnFlag10(1);
                m_DefColl.GetColl(0)->OnFlagA0(1);
                return;
            }
            m_DefColl.GetColl(0)->OffFlag10(1);
            m_DefColl.GetColl(0)->OffFlagA0(1);
            return;
        }
        m_BodyColl.SetDisableAll();
        m_DefColl.SetDisableAll();
    }

    // A push moves the pylon by the push slowed by GetParam_PushResist, as
    // far as the map lets it, and pushes the other back by what it could
    // not move.
    void CStgGmk_Pylon_Act::OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        int bOther5 = pOther != 0 ? pOther->GetOwnerKind() == 5 : 0;
        if (pPush != 0) {
            bcs::FVEC3 from = GetSrt()->GetPos();
            GetParam_Radius();
            float resist = GetParam_PushResist();
            bcs::FVEC3 to;
            to.x = from.x + pPush->x;
            to.y = from.y;
            to.z = from.z + pPush->z;
            if (pBody->GetUnkA0() != 5) {
                float t = resist > 1.0f ? 1.0f : (resist < 0.0f ? 0.0f : resist);
                bcs::FVEC3Blend(&to, &from, &to, t);
            }
            bcs::FVEC3 hitPos;
            bcs::FVEC3 push;
            if (CheckCollBg(hitPos, push, from, to) || pOther->GetOwnerKind() == 5) {
                push.x = hitPos.x - to.x;
                push.z = hitPos.z - to.z;
                pOther->DoBodyPushCallback(pBody, &push);
            } else {
                push.x = to.x - from.x;
                push.z = to.z - from.z;
                bcs::FVEC3Scale(&push, &push, -1.0f);
                pOther->DoBodyPushCallback(pBody, &push);
            }
            if (pBody->GetUnkA0() != 5) {
                GetSrt()->SetPos(hitPos);
                bcs::FVEC3 move;
                bcs::FVEC3Sub(&move, &hitPos, &from);
                AutoPlayPushSe(move);
            }
        } else if (!bOther5) {
            pOther->DoBodyPushCallback(pBody, 0);
        }
    }

    unsigned long CStgGmk_Pylon_Act::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                  const bcs::FVEC3 &rPos)
    {
        return 2;
    }

    // A catch by the attacking side hits the pylon.
    void CStgGmk_Pylon_Act::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (FsmqGetState() != STATE_BREAK && pBall->IsOffensive() && !IsInvincible()) {
            DoDamage(pBody, pBall);
        }
    }

    // One life less, the hit effect and sound, and 30 frames of
    // invincibility; a breakable pylon out of lives breaks.
    void CStgGmk_Pylon_Act::DoDamage(CCollBodyUnit *pBody, CBall *pBall)
    {
        if (m_fMaxLife > 0.0f) {
            m_fLife -= 1.0f;
        }
        bcs::FVEC3 pos = pBall->GetPos();
        long eff = GetParam_Effect(2);
        if (eff >= 0) {
            CEffectCtrl::SimpleEffect(eff, 0, &pos, 0, 3, 0, 0, 0);
        }
        long se = GetParam_SE(2);
        if (se >= 0) {
            CGameSnd::Play(se, pos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
        m_fInvincibleTime = 30.0f;
        if (m_fMaxLife > 0.0f && m_fLife <= 0.0f) {
            FsmqChangeState(STATE_BREAK);
        }
        OnDamage(pBody, pBall);
    }

    float CStgGmk_Pylon_Act::GetParam_MdlRadius() const
    {
        return 1.0f;
    }

    float CStgGmk_Pylon_Act::GetParam_MdlHeight() const
    {
        return 2.0f;
    }

    float CStgGmk_Pylon_Act::GetParam_Radius() const
    {
        return 0.7f;
    }

    float CStgGmk_Pylon_Act::GetParam_Height() const
    {
        return 2.0f;
    }

    float CStgGmk_Pylon_Act::GetParam_Height2() const
    {
        return 8.0f;
    }

    float CStgGmk_Pylon_Act::GetParam_PushResist() const
    {
        return 0.5f;
    }

    long CStgGmk_Pylon_Act::GetParam_PushRank() const
    {
        return 1;
    }

    long CStgGmk_Pylon_Act::GetParam_PushPower() const
    {
        return 3;
    }

    float CStgGmk_Pylon_Act::GetParam_Acc() const
    {
        return 10.0f;
    }

    float CStgGmk_Pylon_Act::GetParam_Dec() const
    {
        return 25.0f;
    }

    float CStgGmk_Pylon_Act::GetParam_Resist() const
    {
        return -1.0f;
    }

    float CStgGmk_Pylon_Act::GetParam_VelLimit() const
    {
        return -1.0f;
    }

    float CStgGmk_Pylon_Act::GetParam_Gravity() const
    {
        return -49.0f;
    }

    float CStgGmk_Pylon_Act::GetParam_Interval() const
    {
        return 300.0f;
    }

    void CStgGmk_Pylon_Act::GetParam_CourtRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
    {
        CStageMan::GetInstance()->GetCourtRect(pLeft, pRight, pFront, pBack);
    }

    // The ground, the sides widened by 0.8 plus the radius.
    void CStgGmk_Pylon_Act::GetParam_OuterRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
    {
        CStageMan::GetInstance()->GetCourtRect(0, 0, pFront, pBack);
        CStageMan::GetInstance()->GetGroundRect(pLeft, pRight, 0, 0);
        float radius = GetParam_Radius();
        if (pLeft != 0) {
            *pLeft -= 0.8f + radius;
        }
        if (pRight != 0) {
            *pRight += 0.8f + radius;
        }
    }

    long CStgGmk_Pylon_Act::GetParam_MdlIdx() const
    {
        return bcs::BCS_RandMax(0L);
    }

    long CStgGmk_Pylon_Act::GetParam_Effect(long type) const
    {
        return -1;
    }

    long CStgGmk_Pylon_Act::GetParam_SE(long type) const
    {
        switch (type) {
            case 0:
                return 0xD1F;
            case 1:
                return 0xD20;
            case 2:
                return 0xD1E;
            case 3:
                return -1;
            case 6:
                return 0xD20;
        }
        return -1;
    }

    // Unbreakable.
    float CStgGmk_Pylon_Act::GetParam_MaxLife() const
    {
        return -1.0f;
    }

    // Waits frame frames (for ever when negative), then moves again.
    void CStgGmk_Pylon_Act::Sleep(float frame)
    {
        m_bAlive = 0;
        static_cast<STATE_WORK *>(FsmqSetState(STATE_STANDBY))->m_fArg = frame;
    }

    bool CStgGmk_Pylon_Act::IsSleep() const
    {
        if (FsmqGetState() == STATE_STANDBY) {
            return true;
        }
        return false;
    }

    // The splash where the pylon fell in.
    void CStgGmk_Pylon_Act::KickSplashEffect()
    {
        CStgSrt *pSrt = GetSrt();
        long eff = GetParam_Effect(4);
        bcs::FVEC3 pos = pSrt->GetPos();
        if (eff >= 0) {
            pos.y = -1.8f;
            CEffectCtrl::SimpleEffect(eff, 0, &pos, 0, 3, 0, 0, 0);
        }
        long se = GetParam_SE(5);
        if (se >= 0) {
            CGameSnd::Play(se, pos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    // The push sound, at most once in 40 frames.
    void CStgGmk_Pylon_Act::PlayPushSe()
    {
        long se = GetParam_SE(6);
        if (se >= 0 && m_fPushSeWait <= 0.0f) {
            CGameSnd::Play(se, GetSrt()->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
            m_fPushSeWait = 40.0f;
        }
    }

    void CStgGmk_Pylon_Act::AutoPlayPushSe(const bcs::FVEC3 &rVel)
    {
        if (PSVECMag(reinterpret_cast<const Vec *>(&rVel)) > 0.016666668f) {
            PlayPushSe();
        }
    }

    void CStgGmk_Pylon_Act::UpdatePushSe(float rate)
    {
        if (m_fPushSeWait > 0.0f) {
            m_fPushSeWait -= rate;
            if (m_fPushSeWait <= 0.0f) {
                m_fPushSeWait = -1.0f;
            }
        }
    }

    // Moves the pylon off its place by up to GetParam_LocRndX and
    // GetParam_LocRndZ.
    void CStgGmk_Pylon_Act::ApplyLocRnd()
    {
        CStgSrt *pSrt = GetSrt();
        bcs::FVEC3 pos = static_cast<CStgGmk_Pylon *>(GetParent())->GetPos();
        float rndX = __fabsf(GetParam_LocRndX());
        if (rndX > 0.0f) {
            pos.x += bcs::BCS_RandRange(-rndX, rndX);
        }
        float rndZ = __fabsf(GetParam_LocRndZ());
        if (rndZ > 0.0f) {
            pos.z += bcs::BCS_RandRange(-rndZ, rndZ);
        }
        pSrt->SetPos(pos);
    }

    void CStgGmk_Pylon_Act::CtorState_DEFAULT()
    {
        GetSrt();
        static_cast<STATE_WORK *>(FsmqSetState(STATE_STANDBY))->m_fArg = -1.0f;
        FsmqRetry();
    }

    void CStgGmk_Pylon_Act::LoopState_DEFAULT()
    {
    }

    void CStgGmk_Pylon_Act::DtorState_DEFAULT()
    {
    }

    // Hidden, for the frames given to Sleep.
    void CStgGmk_Pylon_Act::CtorState_STANDBY()
    {
        m_fStateTime = FsmqGetWork<STATE_WORK>()->m_fArg;
        CStgObj *pObj = GetParent();
        GetSrt();
        m_bAlive = 0;
        for (unsigned long i = 0; i < pObj->GetMdlNum(); i++) {
            CStgMdl *pMdl = pObj->GetMdl(i);
            if (pMdl != 0) {
                pMdl->SetVisible(0);
            }
        }
    }

    void CStgGmk_Pylon_Act::LoopState_STANDBY()
    {
        const STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        float rate = GetFrameRateF();
        if (!(pWork->m_fArg < 0.0f)) {
            m_fStateTime -= rate;
            if (m_fStateTime <= 0.0f) {
                m_fStateTime = 0.0f;
                FsmqChangeState(STATE_MOVE);
                FsmqRetry();
            }
        }
    }

    void CStgGmk_Pylon_Act::DtorState_STANDBY()
    {
    }

    // Stands up, shown, with full lives, the motion parameters set.
    void CStgGmk_Pylon_Act::CtorState_MOVE()
    {
        CStgSrt *pSrt = GetSrt();
        CGmkRef gmk(GetParent());
        pSrt->SetFront(0.0f, 0.0f, 1.0f);
        pSrt->SetUp(0.0f, 1.0f, 0.0f);
        pSrt->SetGravity(GetParam_Gravity());
        pSrt->ClearMotion();
        float resist = GetParam_Resist();
        if (resist < 0.0f) {
            pSrt->ClearFlagC8(0x10);
        } else {
            pSrt->SetResist(bcs::FVEC3(resist, resist, resist));
        }
        float limit = GetParam_VelLimit();
        if (limit < 0.0f) {
            pSrt->ClearFlagC8(3);
        } else {
            pSrt->SetVelLimit(bcs::FVEC3(limit, limit, limit));
        }
        pSrt->SetDecel(GetParam_Dec());
        m_bAlive = 1;
        gmk.Get()->GetMdl(0)->SetVisible(1);
        m_bInCourt = 0;
        ApplyLocRnd();
        m_fLife = m_fMaxLife = GetParam_MaxLife();
        m_fInvincibleTime = 0.0f;
    }

    // Dragged along the ground: the drag effect and sound while it slides
    // faster than GetParam_DragSpeed. With the area check on, once inside
    // the court's sides it stays there, and before that it is kept within
    // the outer rectangle; a pylon stopped at a side plays the hit sound.
    void CStgGmk_Pylon_Act::LoopState_MOVE()
    {
        GetFrameRateF();
        CStgSrt *pSrt = GetSrt();
        bcs::FVEC3 vel = pSrt->GetVel();
        GetParent();
        float speed = PSVECMag(reinterpret_cast<const Vec *>(&vel));
        if (pSrt->GetPos().y <= 0.05f && speed > GetParam_DragSpeed()) {
            if (!Eff_IsAlive(0)) {
                Eff_CreateAttach(0, GetParam_Effect(1), 0, 0UL, 0, 0, 0, 0, 3);
            }
            if (!SE_IsAlive(0)) {
                SE_PlayPos(0, GetParam_SE(1), 0.0f, 1, 0);
            }
        } else {
            Eff_Stop(0);
            Eff_Stop(1);
            SE_Stop(0, 0.0f);
        }

        if (GetParam_IsEnableAreaCheck()) {
            float radius = GetParam_Radius();
            bcs::FVEC3 pos = pSrt->GetPos();
            bcs::FVEC3 accel = pSrt->GetAccel();
            float right;
            float left;
            GetParam_CourtRect(&left, &right, 0, 0);
            left += radius;
            right -= radius;
            float outerRight;
            float outerLeft;
            GetParam_OuterRect(&outerLeft, &outerRight, 0, 0);
            outerLeft += radius;
            outerRight -= radius;

            int bStop = 0;
            if (m_bInCourt) {
                if (vel.x < 0.0f && pos.x < left) {
                    pos.x = left;
                    bStop = 1;
                } else if (vel.x > 0.0f && pos.x > right) {
                    pos.x = right;
                    bStop = 1;
                }
            } else {
                if (pos.x < outerLeft || pos.x > outerRight) {
                    pos.x = pos.x > outerRight ? outerRight : (pos.x < outerLeft ? outerLeft : pos.x);
                    bStop = 1;
                }
                if (pos.x > left && pos.x < right) {
                    m_bInCourt = 1;
                }
            }
            if (bStop) {
                pSrt->SetPos(pos);
                vel.z = 0.0f;
                vel.x = 0.0f;
                pSrt->SetVel(vel);
                if (speed > 0.7f) {
                    long se = GetParam_SE(3);
                    if (se >= 0) {
                        CGameSnd::Play(se, pSrt->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
                    }
                }
            }
        }
    }

    void CStgGmk_Pylon_Act::DtorState_MOVE()
    {
        Eff_Stop(0);
        Eff_Stop(1);
        SE_Stop(0, 0.0f);
    }

    // Breaks: hidden, the break effect at the pylon's size and the break
    // sound; three frames later it sleeps for good.
    void CStgGmk_Pylon_Act::CtorState_BREAK()
    {
        CStgSrt *pSrt = GetSrt();
        GetParent()->GetMdl(0)->SetVisible(0);
        m_fStateTime = 3.0f;
        bcs::FVEC3 pos = pSrt->GetPos();
        long eff = GetParam_Effect(0);
        if (eff >= 0) {
            unsigned long hEffect = CEffectCtrl::CreateAttachEffect(eff, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0);
            CEffectCluster *pCluster = hEffect != 0 ? CEffectCtrl::GetObject(hEffect) : 0;
            if (pCluster != 0) {
                float size = GetParam_Radius() / 0.5f;
                bcs::FVEC3 scale(size, size, size);
                pCluster->SetTranslate(&pos);
                pCluster->SetScale(&scale);
            }
        }
        long se = GetParam_SE(0);
        if (se >= 0) {
            CGameSnd::Play(se, pos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CStgGmk_Pylon_Act::LoopState_BREAK()
    {
        m_fStateTime -= GetFrameRateF();
        if (m_fStateTime <= 0.0f) {
            m_bAlive = 0;
            Sleep(-1.0f);
            FsmqRetry();
        }
    }

    void CStgGmk_Pylon_Act::DtorState_BREAK()
    {
    }

    // Back at the gimmick's default position with a new model, then moves.
    void CStgGmk_Pylon_Act::CtorState_RESPAWN()
    {
        CStgSrt *pSrt = GetSrt();
        CStgGmk_Pylon *pGmk = static_cast<CStgGmk_Pylon *>(GetParent());
        CStgGmk_Pylon_Mdl *pMdl = static_cast<CStgGmk_Pylon_Mdl *>(GetMdl(0));
        m_bAlive = 1;
        if (pMdl != 0) {
            pMdl->SetVisible(1);
        }
        bcs::FVEC3 pos = pGmk->GetDefaultPos();
        pSrt->SetPos(pos);
        if (pMdl != 0) {
            pMdl->SetMdlIdx(GetParam_MdlIdx());
            pMdl->Create();
            pMdl->SceneEntry();
        }
        m_bInCourt = 0;
        m_fLife = m_fMaxLife = GetParam_MaxLife();
        m_fInvincibleTime = 0.0f;
        FsmqChangeState(STATE_MOVE);
        FsmqRetry();
    }

    void CStgGmk_Pylon_Act::LoopState_RESPAWN()
    {
    }

    void CStgGmk_Pylon_Act::DtorState_RESPAWN()
    {
    }

    // Gone for GetParam_Interval frames, then respawns.
    void CStgGmk_Pylon_Act::CtorState_INACTIVE()
    {
        CStgMdl *pMdl = GetMdl(0);
        m_bAlive = 0;
        if (pMdl != 0) {
            pMdl->SetVisible(0);
        }
        m_fStateTime = GetParam_Interval();
    }

    void CStgGmk_Pylon_Act::LoopState_INACTIVE()
    {
        m_fStateTime -= GetFrameRateF();
        if (m_fStateTime <= 0.0f) {
            FsmqChangeState(STATE_RESPAWN);
            FsmqRetry();
        }
    }

    void CStgGmk_Pylon_Act::DtorState_INACTIVE()
    {
    }

    void CStgGmk_Pylon_Act::CtorState_FALL()
    {
        CStgSrt *pSrt = GetSrt();
        pSrt->SetGravity(GetParam_Gravity());
        m_bAlive = 0;
    }

    // Falls over towards the direction in the state's work memory (none
    // when negative) by GetParam_FallRot a frame; below the field it
    // splashes and is gone.
    void CStgGmk_Pylon_Act::LoopState_FALL()
    {
        CStgSrt *pSrt = GetSrt();
        float bottom = -2.0f - GetParam_MdlHeight();
        float rate = GetFrameRateF();
        const STATE_WORK *pWork = FsmqGetWork<STATE_WORK>();
        if (pWork->m_fArg >= 0.0f) {
            bcs::FVEC3 axis;
            bcs::BCS_VecOneX_RotY(axis, pWork->m_fArg);
            bcs::FMTX34 mtx;
            bcs::FMTX34Copy(&mtx, &pSrt->GetMtx());
            bcs::FMTX34 rot;
            nw4r::math::MTX34RotAxisRad(reinterpret_cast<nw4r::math::MTX34 *>(&rot),
                                        reinterpret_cast<const nw4r::math::VEC3 *>(&axis),
                                        rate * GetParam_FallRot());
            PSMTXConcat(mtx.m, rot.m, mtx.m);
            pSrt->SetFront(mtx.m[0][2], mtx.m[1][2], mtx.m[2][2]);
            pSrt->SetUp(mtx.m[0][1], mtx.m[1][1], mtx.m[2][1]);
        }
        if (pSrt->GetPos().y < bottom) {
            KickSplashEffect();
            pSrt->ClearUnkE4();
            FsmqChangeState(STATE_INACTIVE);
            FsmqRetry();
        }
    }

    void CStgGmk_Pylon_Act::DtorState_FALL()
    {
    }

    CStgGmk_Pylon_Drv::CStgGmk_Pylon_Drv(CStgGmk *pParent) : CStgDrv(pParent) {}

    CStgGmk_Pylon_Drv::~CStgGmk_Pylon_Drv() {}

    void CStgGmk_Pylon_Drv::Main(float rate)
    {
        CStgDrv::Main(rate);
    }

    CStgGmk_Pylon::CStgGmk_Pylon(bcs::CTask *pParent, int arg) : CStgGmk(pParent, 1), m_Unk68(arg)
    {
        SetType(9);
    }

    CStgGmk_Pylon::~CStgGmk_Pylon() {}

    CStgMdl *CStgGmk_Pylon::NewMdl(long heap, unsigned long idx)
    {
        return new (heap) CStgGmk_Pylon_Mdl(this, 0);
    }

    void CStgGmk_Pylon::OnPostCreate() {}

    void CStgGmk_Pylon::OnSceneEntry() {}

    void CStgGmk_Pylon::OnSceneLeave() {}

    void CStgGmk_Pylon::Main()
    {
        GetFrameRateF();
        CStgObj::Main();
    }

    void CStgGmk_Pylon::Respawn()
    {
        CStgGmk_Pylon_Act *pAct = static_cast<CStgGmk_Pylon_Act *>(GetAct());
        if (pAct != 0) {
            pAct->FsmqChangeState(CStgGmk_Pylon_Act::STATE_MOVE);
        }
    }

    int CStgGmk_Pylon::IsActive() const
    {
        const CStgGmk_Pylon_Act *pAct = GetPylonAct();
        if (pAct != 0) {
            return pAct->IsAlive();
        }
        return 0;
    }

    void CStgGmk_Pylon::Inactivate()
    {
        CStgGmk_Pylon_Act *pAct = static_cast<CStgGmk_Pylon_Act *>(GetAct());
        if (pAct != 0) {
            pAct->Inactivate();
        }
    }

    // On the ground and active.
    bool CStgGmk_Pylon::IsLanding() const
    {
        if (IsActive()) {
            bcs::FVEC3 pos = GetPos();
            if (pos.y <= 0.05f) {
                return true;
            }
        }
        return false;
    }

    void CStgGmk_Pylon::SetAccel(const bcs::FVEC3 &rAccel)
    {
        CStgGmk_Pylon_Act *pAct = static_cast<CStgGmk_Pylon_Act *>(GetAct());
        if (pAct != 0) {
            pAct->GetSrt()->SetAccel(rAccel);
        }
    }

    // Only a pylon that is alive counts.
    unsigned long CStgGmk_Pylon::CalcSum() const
    {
        unsigned long sum = 0;
        if (GetPylonAct()->IsAlive()) {
            sum = CStgObj::CalcSum();
        }
        return sum;
    }

    void CStgGmk_Pylon::Disp() {}

    CStgGmk_PylonCluster::CStgGmk_PylonCluster(bcs::CTask *pParent)
        : CStgGmkCluster<CStgGmk_Pylon>(pParent), m_nCurPattern(0), m_nLocInfoNum(0)
    {
    }

    CStgGmk_PylonCluster::~CStgGmk_PylonCluster() {}

    // Sets the pylons out after a newly chosen pattern.
    void CStgGmk_PylonCluster::Reset()
    {
        CStgGmk_Pylon *pPylon;
        SetCurPatternAuto();
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            pPylon = static_cast<CStgGmk_Pylon *>(GetGmk_(i));
            if (pPylon != 0) {
                ApplyLocInfo(pPylon, GetLocInfo(i));
                pPylon->SetIdx(i);
            }
        }
    }

    void CStgGmk_PylonCluster::InactivateAll()
    {
        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            CStgGmk_Pylon *pPylon = static_cast<CStgGmk_Pylon *>(GetGmk_(i));
            if (pPylon != 0) {
                pPylon->Inactivate();
            }
        }
    }

    // GetParam_LocPattern, or for -1 a random pattern, -2 the next and -3
    // the previous one, of the first (at most ten) valid patterns.
    long CStgGmk_PylonCluster::SelectPattern() const
    {
        long max;
        long cur = GetCurPattern();
        long pattern = GetParam_LocPattern();
        long num = GetLocInfoNum();
        max = 10;
        if (num < 10) {
            max = num;
        }
        switch (pattern) {
            case -1:
                pattern = bcs::BCS_RandRange(0L, max - 1);
                break;
            case -2:
                pattern = cur + 1;
                break;
            case -3:
                pattern = cur - 1;
                break;
        }
        return (max + pattern % max) % max;
    }

    void CStgGmk_PylonCluster::OnPostCreateGmk()
    {
        LoadLocInfoTbl();
    }

    void CStgGmk_PylonCluster::OnUpdate(float rate)
    {
    }

    // Reads the patterns from the location table: for each position a flag
    // (whether there is a pylon) and its x and z.
    void CStgGmk_PylonCluster::LoadLocInfoTbl()
    {
        const LOC_RAW_TBL *pTbl = static_cast<const LOC_RAW_TBL *>(CLoadData::GetPtr(0x6C));
        LOC_INFO info;
        long numIdx = GetLocValidNumIdx();
        if (numIdx < 0) {
            m_nLocInfoNum = 0;
        } else {
            m_nLocInfoNum = LocRawTblGet(pTbl, numIdx) >> 12;
        }
        for (long pattern = 0; pattern < PATTERN_MAX; pattern++) {
            for (long i = 0; i < PYLON_MAX; i++) {
                long idx = GetLocRawTblIdx(pattern, i);
                info.Clear();
                if (idx >= 0) {
                    info.m_nType = (LocRawTblGet(pTbl, idx) >> 12) != 0;
                    info.m_Pos.x = LocRawTblGet(pTbl, idx + 1) / 4096.0f;
                    info.m_Pos.y = 0.0f;
                    info.m_Pos.z = LocRawTblGet(pTbl, idx + 2) / 4096.0f;
                }
                SetLocInfo(pattern, i, info);
            }
        }
    }

    const CStgGmk_PylonCluster::LOC_INFO &CStgGmk_PylonCluster::GetLocInfo(long pattern, long idx) const
    {
        return m_aLocInfo[pattern][idx];
    }

    void CStgGmk_PylonCluster::SetLocInfo(long pattern, long idx, const LOC_INFO &rInfo)
    {
        m_aLocInfo[pattern][idx] = rInfo;
    }

    // A pylon at a location of type 0 is put out of the way and inactive.
    void CStgGmk_PylonCluster::ApplyLocInfo(CStgGmk_Pylon *pPylon, const LOC_INFO &rInfo) const
    {
        bcs::FVEC3 away(100.0f, 100.0f, 0.0f);
        if (rInfo.m_nType != 0) {
            pPylon->SetDefaultPos(rInfo.m_Pos);
            pPylon->Respawn();
            pPylon->SetPos(rInfo.m_Pos);
            return;
        }
        pPylon->SetDefaultPos(away);
        pPylon->SetPos(away);
        pPylon->Inactivate();
    }
}
