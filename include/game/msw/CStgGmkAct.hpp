#ifndef CSTGGMKACT_HPP_
#define CSTGGMKACT_HPP_

#include "game/msw/CStgAct.hpp"

namespace msw {

    class CStgGmkActBallCollProxy;

    // The actor of a stage gimmick: a CStgAct that can also be hit by the
    // ball. A DOL class, size 0x174 (one member after CStgAct's 0x170); its
    // vtable is __vt__Q23msw10CStgGmkAct in the DOL (0x198 bytes). The
    // constructor has no symbol in the DOL: msw::CStgGmk_Wave_Act's calls
    // CStgAct's directly, so it is inline.
    //
    // The order of the new virtual functions (NewBallCollProxy to OnBallPush)
    // follows __vt__Q23msw22CStgGmk_Wave_Dodge_Act (DodgeF). The modules emit
    // weak copies of Init, NewBallCollProxy, DeleteBallCollProxy and the
    // reference forms of the Regist* functions, so those are inline in the
    // original; only NewBallCollProxy has its body here so far.
    //
    // This header replaces a stub inherited from the EU project (no base,
    // no virtual functions); src/game/msw/CStgGmkAct.cpp was written against
    // that stub and is not built for RMKE01.
    class CStgGmkAct : public CStgAct {
        public:
            CStgGmkAct(CStgObj *pParent);
            virtual ~CStgGmkAct();

            virtual void Init();
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
            virtual void RegistAttackHitCallback(CAtkCollision &rColl);
            virtual void RegistAttackHitCallback(CAtkCollision *pColl);
            virtual void RegistBodyPushCallback(CBodyCollision &rColl);
            virtual void RegistBodyPushCallback(CBodyCollision *pColl);
            virtual void RegistBodyDamageCallback(CBodyCollision &rColl);
            virtual void RegistBodyDamageCallback(CBodyCollision *pColl);
            virtual void RegistBallCatchCallback(CBodyCollision &rColl);
            virtual void RegistBallCatchCallback(CBodyCollision *pColl);
            virtual void RegistReflectHitCallback(CDefCollision &rColl);
            virtual void RegistReflectHitCallback(CDefCollision *pColl);

            virtual CStgGmkActBallCollProxy *NewBallCollProxy() { return 0; }
            virtual void DeleteBallCollProxy(CStgGmkActBallCollProxy *pProxy);
            virtual void OnBallAtkHit(CBall *pBall);
            virtual void OnBallDamage(CBall *pBall);
            virtual void OnBallReflect(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                       bcs::FVEC3 &rVel);
            virtual void OnBallPush(CBall *pBall);

        private:
            CStgGmkActBallCollProxy *m_pBallCollProxy; // 0x170
    };
}

#endif /* !CSTGGMKACT_HPP_ */
