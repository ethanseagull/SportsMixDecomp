#include "game/msw/CStgGmkAct.hpp"

#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"

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
}
