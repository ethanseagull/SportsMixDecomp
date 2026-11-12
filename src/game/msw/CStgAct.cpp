#include "game/msw/CStgAct.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CStgObj.hpp"
#include "game/msw/CStgSrt.hpp"

// msw::CStgAct, the logic of a stage object: its constructor, destructor
// and vtable, the per-frame update and the collision callbacks.

namespace msw {

    CStgAct::CStgAct(CStgObj *pParent) : m_PosFunc(this), m_pParent(pParent)
    {
    }

    CStgAct::~CStgAct()
    {
    }

    void CStgAct::Init()
    {
        Eff_Init();
        SE_Init();
        OnInit();
    }

    void CStgAct::Update(float rate)
    {
        SetFrameRateF(rate);
        Eff_Update(rate);
        SE_Update(rate);
        OnUpdate(rate);
        UpdateMdlMtx();
    }

    void CStgAct::OnUpdate(float rate)
    {
    }

    unsigned long CStgAct::CalcSum() const
    {
        return OnCalcSum();
    }

    // Gives the models the matrix of the CStgSrt: every model of the
    // parent, or without a parent the actor's own.
    void CStgAct::UpdateMdlMtx()
    {
        CStgObj *pParent = GetParent();
        if (pParent) {
            const bcs::FMTX34 &rMtx = GetSrt()->GetMtx();
            long num = pParent->GetMdlNum();
            for (long i = 0; i < num; i++) {
                CStgMdl *pMdl = pParent->GetMdl(i);
                if (pMdl) {
                    pMdl->SetMtx(rMtx);
                }
            }
        } else {
            CStgMdl *pMdl = GetMdl(0);
            if (pMdl) {
                pMdl->SetMtx(GetSrt()->GetMtx());
            }
        }
    }

    // The parent's model idx, or without a parent the one set by SetMdl.
    CStgMdl *CStgAct::GetMdl(long idx)
    {
        CStgObj *pParent = GetParent();
        return pParent ? pParent->GetMdl(idx) : m_pMdl;
    }

    const CStgMdl *CStgAct::GetMdl(long idx) const
    {
        const CStgObj *pParent = GetParent();
        return pParent ? pParent->GetMdl(idx) : m_pMdl;
    }

    void CStgAct::EventNotify(long event, void *pArg)
    {
        CStgObj *pParent = GetParent();
        if (pParent) {
            pParent->EventNotify(this, event, pArg);
        }
    }

    void CStgAct::RegistAttackHitCallback(CAtkCollision *pColl)
    {
        pColl->SetAttackHitCallbackAll(AttackHitCallback_, this);
    }

    void CStgAct::RegistBodyPushCallback(CBodyCollision *pColl)
    {
        pColl->SetBodyPushCallbackAll(BodyPushCallback_, this);
    }

    void CStgAct::RegistBodyDamageCallback(CBodyCollision *pColl)
    {
        pColl->SetBodyDamageCallbackAll(BodyDamageCallback_, this);
    }

    void CStgAct::RegistBallCatchCallback(CBodyCollision *pColl)
    {
        pColl->SetBallCatchCallbackAll(BallCatchCallback_, this);
    }

    void CStgAct::RegistReflectHitCallback(CDefCollision *pColl)
    {
        pColl->SetReflectHitCallbackAll(ReflectHitCallback_, this);
    }

    void CStgAct::AttackHitCallback_(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                     unsigned long flag, void *pArg)
    {
        if (pArg) {
            static_cast<CStgAct *>(pArg)->OnAttackHit(pAtk, pBody, rPos, flag);
        }
    }

    void CStgAct::BodyPushCallback_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg, bcs::FVEC3 *pPush)
    {
        if (pArg) {
            static_cast<CStgAct *>(pArg)->OnBodyPush(pBody, pOther, pPush);
        }
    }

    unsigned long CStgAct::BodyDamageCallback_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                               const bcs::FVEC3 &rPos, void *pArg)
    {
        if (pArg == 0) {
            return 0;
        } else {
            return static_cast<CStgAct *>(pArg)->OnBodyDamage(pBody, pAtk, rPos);
        }
    }

    void CStgAct::BallCatchCallback_(CCollBodyUnit *pBody, CBall *pBall, void *pArg)
    {
        if (pArg) {
            static_cast<CStgAct *>(pArg)->OnBallCatch(pBody, pBall);
        }
    }

    void CStgAct::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
    }

    unsigned long CStgAct::ReflectHitCallback_(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel, void *pArg)
    {
        if (pArg == 0) {
            return 1;
        } else {
            return static_cast<CStgAct *>(pArg)->OnReflectHit(pDef, pBall, rPos, rVel);
        }
    }

    // Moves the actor by the push (in the XZ plane), stopping at the map,
    // and passes what is left of the push to the other body.
    void CStgAct::OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        if (pPush) {
            bcs::FVEC3 from(GetSrt()->GetPos());
            bcs::FVEC3 to;
            to.x = from.x + pPush->x;
            to.y = from.y;
            to.z = from.z + pPush->z;
            bcs::FVEC3 hit;
            bcs::FVEC3 rest;
            float time;
            if (CCollMan::DetectMapHit(hit, rest, time, from, to, 1.0f, 1, 0)) {
                rest.x = hit.x - to.x;
                rest.z = hit.z - to.z;
                pOther->DoBodyPushCallback(pBody, &rest);
            } else {
                pOther->DoBodyPushCallback(pBody, 0);
            }
            GetSrt()->SetPos(hit);
        } else {
            pOther->DoBodyPushCallback(pBody, 0);
        }
    }

    unsigned long CStgAct::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos)
    {
        return 1;
    }

    unsigned long CStgAct::OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                        bcs::FVEC3 &rVel)
    {
        return 1;
    }
}
