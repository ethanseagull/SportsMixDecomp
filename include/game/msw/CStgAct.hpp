#ifndef MSW_CSTGACT_HPP_
#define MSW_CSTGACT_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgEffectSlot.hpp"
#include "game/msw/CStgSeSlot.hpp"

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
            virtual CStgSrtMove *GetSrt() { return reinterpret_cast<CStgSrtMove *>(m_Srt); }
            virtual const CStgSrtMove *GetSrt() const { return reinterpret_cast<const CStgSrtMove *>(m_Srt); }
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
            virtual CSESlot::CPosFunc *SE_GetDefaultFunc()
            {
                return reinterpret_cast<CSESlot::CPosFunc *>(m_PosFunc);
            }

            // (name guessed; DOL fn_8016F6C0) The per-frame update:
            // SetFrameRateF(rate), the effect and sound slots, OnUpdate(rate),
            // UpdateMdlMtx(). msw::CStgObj::Main calls it.
            void Update(float rate);
            // (name guessed; DOL fn_8016F764) OnCalcSum();
            // msw::CStgObj::CalcSum adds it in.
            unsigned long CalcSum() const;

        private:
            // 0x18: the sound slots' default position callback, a
            // CSESlot::CPosFunc of a DOL class without an exported name
            // (its vptr, then this actor; CStgAct's constructor sets both).
            unsigned long m_PosFunc[2];
            CStgObj *m_pParent;           // 0x20
            unsigned char m_Srt[0x144];   // 0x24, the CStgSrtMove GetSrt returns
            CStgMdl *m_pMdl;              // 0x168, set by SetMdl, read by GetMdl(long) const
            float m_fFrameRate;           // 0x16C
    };
}

#endif /* !MSW_CSTGACT_HPP_ */
