#ifndef MSW_CSTGACT_HPP_
#define MSW_CSTGACT_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgEffectSlot.hpp"
#include "game/msw/CStgSeSlot.hpp"
#include "game/msw/CStgSrt.hpp"

namespace msw {

    class CAtkCollision;
    class CBall;
    class CBodyCollision;
    class CCollAtkUnit;
    class CCollBodyUnit;
    class CCollDefUnit;
    class CDefCollision;
    class CStgMdl;
    class CStgObj;
    class CStgSrt;
    class CStgSrtMove;

    // The logic of a stage object, with its effect and sound slots. A DOL
    // class, size 0x170: CStgEffectSlot at 0x00 (vptr at 0x08), CStgSeSlot at
    // 0x0C (vptr at 0x14), own members from 0x18. Constructor
    // __ct__Q23msw7CStgActFPQ23msw7CStgObj.
    //
    // The order of the virtual functions follows
    // __vt__Q23msw22CStgGmk_Wave_Dodge_Act (DodgeF): CStgAct's new ones run
    // from Init to SE_GetDefaultFunc, which overrides CStgSeSlot's and so
    // gets a slot of its own at the place it is declared (after the
    // Regist* functions). The modules emit weak copies of GetParent,
    // GetSrt, SetMdl, SetFrameRateF, GetFrameRateF, OnInit, OnAttackHit, OnCalcSum
    // and SE_GetDefaultFunc, so these are inline in the original; bodies are
    // given where the copy shows them plainly. Return types not visible in a
    // body are guesses.
    class CStgAct : public CStgEffectSlot, public CStgSeSlot {
        public:
            CStgAct(CStgObj *pParent);
            virtual ~CStgAct();

            virtual CStgMdl *GetMdl(long idx);

            virtual void Init();
            virtual CStgObj *GetParent() { return m_pParent; }
            virtual const CStgObj *GetParent() const { return m_pParent; }
            // Inline in the original (the address of the embedded CStgSrtMove).
            virtual CStgSrtMove *GetSrt() { return &m_Srt; }
            virtual const CStgSrtMove *GetSrt() const { return &m_Srt; }
            virtual void SetMdl(CStgMdl *pMdl) { m_pMdl = pMdl; }
            virtual const CStgMdl *GetMdl(long idx) const;
            virtual void SetFrameRateF(float rate) { m_fFrameRate = rate; }
            virtual float GetFrameRateF() const { return m_fFrameRate; }
            virtual void UpdateMdlMtx();
            virtual void EventNotify(long event, void *pArg);
            virtual void OnInit() {}
            virtual void OnUpdate(float rate);
            virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                     unsigned long flag)
            {
            }
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
            // The two return values (2 and 0x102 in the DOL's
            // msw::CStgGmk_WaterPillar_Act) go back to the collision
            // callbacks, which return an unsigned long.
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);
            virtual unsigned long OnCalcSum() const { return 0; }
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
            virtual CSESlot::CPosFunc *SE_GetDefaultFunc() { return &m_PosFunc; }

            // (name guessed; DOL fn_8016F6C0) The per-frame update:
            // SetFrameRateF(rate), the effect and sound slots, OnUpdate(rate),
            // UpdateMdlMtx(). msw::CStgObj::Main calls it.
            void Update(float rate);
            // (name guessed; DOL fn_8016F764) OnCalcSum();
            // msw::CStgObj::CalcSum adds it in.
            unsigned long CalcSum() const;

        private:
            // (name guessed; the DOL class has no exported name, vtable
            // 0x80474BB0) The sound slots' default position: the actor's.
            class CSEPosFunc : public CSESlot::CPosFunc {
                public:
                    CSEPosFunc(CStgAct *pAct) : m_pAct(pAct) {}
                    virtual void GetPos(bcs::FVEC3 *pPos) { *pPos = m_pAct->GetSrt()->GetPos(); }

                private:
                    CStgAct *m_pAct; // 0x04
            };

            // The collision callbacks the pointer forms of the Regist*
            // functions install, with the actor as pArg: each passes the
            // event to the actor's On* function (names guessed, as
            // msw::CStgGmkAct's).
            static void AttackHitCallback_(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                           const bcs::FVEC3 &rPos, unsigned long flag, void *pArg);
            static void BodyPushCallback_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                          bcs::FVEC3 *pPush);
            static unsigned long BodyDamageCallback_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                     const bcs::FVEC3 &rPos, void *pArg);
            static void BallCatchCallback_(CCollBodyUnit *pBody, CBall *pBall, void *pArg);
            static unsigned long ReflectHitCallback_(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                                     bcs::FVEC3 &rVel, void *pArg);

            CSEPosFunc m_PosFunc;         // 0x18
            CStgObj *m_pParent;           // 0x20
            CStgSrtMove m_Srt;            // 0x24, GetSrt
            CStgMdl *m_pMdl;              // 0x168, set by SetMdl, read by GetMdl(long) const
            float m_fFrameRate;           // 0x16C
    };
}

#endif /* !MSW_CSTGACT_HPP_ */
