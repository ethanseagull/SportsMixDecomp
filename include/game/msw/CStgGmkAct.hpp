#ifndef CSTGGMKACT_HPP_
#define CSTGGMKACT_HPP_

#include "game/msw/CStgAct.hpp"

namespace msw {

    class CStgGmkAct;

    // Lets the ball collide with a gimmick actor on the actor's behalf (made
    // by CStgGmkAct::NewBallCollProxy, which returns none by default). No
    // DOL class of this name has functions; the layout is what
    // CStgGmkAct::Init and DeleteBallCollProxy show: a vptr at 0x04 with the
    // destructor in the first slot and a function taking the actor in the
    // second (name guessed).
    class CStgGmkActBallCollProxy {
        private:
            unsigned long m_Unk00; // 0x00

        public:
            virtual ~CStgGmkActBallCollProxy();
            virtual void SetOwner(CStgGmkAct *pAct);
    };

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
    // original; the bodies are those copies (HockeyF has them in
    // game/hockey/CStgGmk_WaterWorks_Hockey.cpp).
    //
    // This header replaces a stub inherited from the EU project (no base,
    // no virtual functions).
    class CStgGmkAct : public CStgAct {
        public:
            // Inline (msw::CStgGmk_WaterPillar_Act's constructor and
            // destructor in the DOL build this part inline).
            CStgGmkAct(CStgObj *pParent) : CStgAct(pParent), m_pBallCollProxy(0) {}
            virtual ~CStgGmkAct();

            // Drops the old ball proxy and makes a new one.
            virtual void Init()
            {
                CStgAct::Init();
                DeleteBallCollProxy(m_pBallCollProxy);
                CStgGmkActBallCollProxy *pProxy = NewBallCollProxy();
                if (pProxy != 0) {
                    m_pBallCollProxy = pProxy;
                    pProxy->SetOwner(this);
                }
            }
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
            virtual CStgGmkActBallCollProxy *NewBallCollProxy() { return 0; }
            virtual void DeleteBallCollProxy(CStgGmkActBallCollProxy *pProxy)
            {
                if (pProxy != 0) {
                    delete pProxy;
                }
            }
            virtual void OnBallAtkHit(CBall *pBall);
            // Both return 1 in the DOL (return type guessed from the
            // collision callbacks, which return an unsigned long).
            virtual unsigned long OnBallDamage(CBall *pBall);
            virtual unsigned long OnBallReflect(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                                bcs::FVEC3 &rVel);
            virtual void OnBallPush(CBall *pBall);

            // Overrides declared after the new functions: the modules emit
            // them before NewBallCollProxy and DeleteBallCollProxy (inline
            // functions come out in reverse order of declaration).
            // The reference forms pass on to the pointer forms.
            virtual void RegistAttackHitCallback(CAtkCollision &rColl) { RegistAttackHitCallback(&rColl); }
            virtual void RegistAttackHitCallback(CAtkCollision *pColl);
            virtual void RegistBodyPushCallback(CBodyCollision &rColl) { RegistBodyPushCallback(&rColl); }
            virtual void RegistBodyPushCallback(CBodyCollision *pColl);
            virtual void RegistBodyDamageCallback(CBodyCollision &rColl) { RegistBodyDamageCallback(&rColl); }
            virtual void RegistBodyDamageCallback(CBodyCollision *pColl);
            virtual void RegistBallCatchCallback(CBodyCollision &rColl) { RegistBallCatchCallback(&rColl); }
            virtual void RegistBallCatchCallback(CBodyCollision *pColl);
            virtual void RegistReflectHitCallback(CDefCollision &rColl) { RegistReflectHitCallback(&rColl); }
            virtual void RegistReflectHitCallback(CDefCollision *pColl);

        private:
            // The collision callbacks the pointer forms of the Regist*
            // functions install, with the actor as pArg. Each passes the
            // event to the ball proxy when there is one (for a body of kind 2
            // where a body is involved), otherwise to the actor's own
            // OnAttackHit, OnBodyPush, OnBodyDamage or OnBallCatch. In the
            // DOL they follow RegistReflectHitCallback without exported
            // names (fn_80170188, fn_80170200, fn_80170260, fn_801702D4);
            // whether they are static members or file-local is not known,
            // and their names are guesses.
            static void AttackHitCallback_(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                           const bcs::FVEC3 &rPos, unsigned long flag, void *pArg);
            static void BodyPushCallback_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                          bcs::FVEC3 *pPush);
            static unsigned long BodyDamageCallback_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                     const bcs::FVEC3 &rPos, void *pArg);
            static void BallCatchCallback_(CCollBodyUnit *pBody, CBall *pBall, void *pArg);

            CStgGmkActBallCollProxy *m_pBallCollProxy; // 0x170
    };
}

#endif /* !CSTGGMKACT_HPP_ */
