#include "game/msw/CStgGmkAct.hpp"

#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CDefCollision.hpp"

namespace msw {

    void CStgGmkAct::OnBallAtkHit(CBall *pBall)
    {
    }

    unsigned long CStgGmkAct::OnBallDamage(CBall *pBall)
    {
        return 1;
    }

    void CStgGmkAct::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
    {
    }

    unsigned long CStgGmkAct::OnBallReflect(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                            bcs::FVEC3 &rVel)
    {
        return 1;
    }

    void CStgGmkAct::OnBallPush(CBall *pBall)
    {
    }

    void CStgGmkAct::RegistAttackHitCallback(CAtkCollision *pColl)
    {
        pColl->SetAttackHitCallbackAll(AttackHitCallback_, this);
    }

    void CStgGmkAct::RegistBodyPushCallback(CBodyCollision *pColl)
    {
        pColl->SetBodyPushCallbackAll(BodyPushCallback_, this);
    }

    void CStgGmkAct::RegistBodyDamageCallback(CBodyCollision *pColl)
    {
        pColl->SetBodyDamageCallbackAll(BodyDamageCallback_, this);
    }

    void CStgGmkAct::RegistBallCatchCallback(CBodyCollision *pColl)
    {
        pColl->SetBallCatchCallbackAll(BallCatchCallback_, this);
    }

    void CStgGmkAct::RegistReflectHitCallback(CDefCollision *pColl)
    {
        pColl->SetReflectHitCallbackAll(ReflectHitCallback_, this);
    }

    // The collision callbacks: pArg is the actor. An event that involves a
    // unit whose owner is of kind 2 goes to the ball proxy when the actor has one.

    void CStgGmkAct::AttackHitCallback_(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                        const bcs::FVEC3 &rPos, unsigned long flag, void *pArg)
    {
        CStgGmkAct *pAct = static_cast<CStgGmkAct *>(pArg);
        if (pAct == 0) {
            return;
        }
        if (pAct->m_pBallCollProxy != 0 && pBody->GetOwnerKind() == 2) {
            pAct->m_pBallCollProxy->OnAttackHit(pAtk, pBody, rPos, flag);
            return;
        }
        pAct->OnAttackHit(pAtk, pBody, rPos, flag);
    }

    void CStgGmkAct::BodyPushCallback_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                       bcs::FVEC3 *pPush)
    {
        CStgGmkAct *pAct = static_cast<CStgGmkAct *>(pArg);
        if (pAct == 0) {
            return;
        }
        if (pAct->m_pBallCollProxy != 0 && pOther->GetOwnerKind() == 2) {
            pAct->m_pBallCollProxy->OnBodyPush(pBody, pOther, pPush);
            return;
        }
        pAct->OnBodyPush(pBody, pOther, pPush);
    }

    unsigned long CStgGmkAct::BodyDamageCallback_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                  const bcs::FVEC3 &rPos, void *pArg)
    {
        CStgGmkAct *pAct = static_cast<CStgGmkAct *>(pArg);
        if (pAct == 0) {
            return 0;
        }
        if (pAct->m_pBallCollProxy != 0 && pAtk->GetOwnerKind() == 2) {
            return pAct->m_pBallCollProxy->OnBodyDamage(pBody, pAtk, rPos);
        }
        return pAct->OnBodyDamage(pBody, pAtk, rPos);
    }

    void CStgGmkAct::BallCatchCallback_(CCollBodyUnit *pBody, CBall *pBall, void *pArg)
    {
        CStgGmkAct *pAct = static_cast<CStgGmkAct *>(pArg);
        if (pAct == 0) {
            return;
        }
        if (pAct->m_pBallCollProxy != 0) {
            pAct->m_pBallCollProxy->OnBallCatch(pBody, pBall);
            return;
        }
        pAct->OnBallCatch(pBody, pBall);
    }

    unsigned long CStgGmkAct::ReflectHitCallback_(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                                  bcs::FVEC3 &rVel, void *pArg)
    {
        CStgGmkAct *pAct = static_cast<CStgGmkAct *>(pArg);
        if (pAct == 0) {
            return 1;
        }
        if (pAct->m_pBallCollProxy != 0) {
            return pAct->m_pBallCollProxy->OnReflectHit(pDef, pBall, rPos, rVel);
        }
        return pAct->OnReflectHit(pDef, pBall, rPos, rVel);
    }
}
