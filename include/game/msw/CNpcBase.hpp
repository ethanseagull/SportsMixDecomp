#ifndef MSW_CNPCBASE_HPP_
#define MSW_CNPCBASE_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    // INCOMPLETE: a non-player character on the court (the boss Packun,
    // ...). A DOL class and a task (its constructor
    // __ct__Q23msw8CNpcBaseFPQ23msw7CChrManl calls bcs::CTask's). Size
    // 0x188: the constructor's last store is to 0x184, and
    // msw::CNpcBossPackun's state queue starts at 0x188. Only part of its
    // virtual functions (below) and of its members are declared, so it must
    // not be instantiated or derived from with a vtable emitted yet.
    class CBall;
    class CBodyCollision;
    class CCollAtkUnit;
    class CCollBodyUnit;

    // The base is bcs::CUnivTask (same layout as bcs::CTask): the derived
    // vtables hold bcs::CUnivTask::GetFrameRateF at 0x14
    // (__vt__Q23msw19CNpcGhostBasketBase, BasketF).
    class CNpcBase : public bcs::CUnivTask {
        public:
            // Virtual functions 0x18 to 0x50, in the order and with the names
            // of the slots of __vt__Q23msw16CNpcStringBasket (BasketF), for
            // BasketF's CBallBasketFruit, which calls CanbeCatch. Return
            // types guessed. (the class's destructor and later slots are not
            // declared yet)
            // DOL; overrides of bcs::CTask's (slots 0x0C and 0x10 of the
            // BossF vtables of the classes derived from it).
            virtual void Main();
            virtual void Disp();

            virtual void Create();                                     // 0x18
            virtual long GetNpcType() const;                           // 0x1C
            virtual void Reset();                                      // 0x20
            virtual long SendMessage(long, long, long, long, long);    // 0x24
            virtual long GetThinkLevel() const;                        // 0x28
            virtual void AnimPlay(long, int, float, float);            // 0x2C
            virtual void AnimStop();                                   // 0x30
            virtual void AnimSetSpeed(float);                          // 0x34
            virtual void AnimSetFrame(float);                          // 0x38
            virtual void AttachBall(CBall *);                          // 0x3C
            virtual void AttachBall(CBall *, long, float, unsigned long); // 0x40
            virtual void DetachBall();                                 // 0x44
            virtual int IsHaveBall() const;                            // 0x48
            virtual int IsHoldBall() const;                            // 0x4C
            virtual int CanbeCatch() const { return 0; }               // 0x50
            // 0x54 to 0x70 (added for BasketF game/basket/CNpcGhostBasketBase.cpp;
            // names from __vt__Q23msw19CNpcGhostBasketBase, return types
            // guessed except where noted).
            virtual void GetBodyCollInfo(bcs::FVEC3 &, bcs::FVEC3 &);     // 0x54
            virtual void MainFunc();                                    // 0x58
            virtual void DispFunc();                                    // 0x5C
            virtual void CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *); // 0x60
            // (the ghost's override returns 1 after taking the hit, 0 when it
            // ignores it)
            virtual unsigned long CallbackBodyDamage_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                      const bcs::FVEC3 &rPos); // 0x64
            virtual void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush); // 0x68
            virtual void CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall); // 0x6C
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &,
                                            unsigned long); // 0x70

            // Added for BasketF game/basket/CNpcGhostBasketBase.cpp (all in
            // the DOL; return types and parameter names guessed).
            long AnimGetMotID();
            float AnimGetFrame();
            float AnimGetFrameNum();
            int IsAnimEnd();
            int IsAnimLoop();
            // Plays sound nId at pPos (null: at the NPC).
            void PlaySePos(long nId, const bcs::FVEC3 *pPos, unsigned long, long, float, float, float);
            // The ball the NPC carries; null when none.
            CBall *GetAttachBall() const;
            // Turns the yaw towards fRotY by fRate of the difference.
            void RotateYRate(float fRotY, float fRate);
            // The yaw (radians) from the NPC towards rPos, and from rPos
            // towards the NPC, in the XZ plane.
            float GetRotY_ToPosXZ(const bcs::FVEC3 &rPos) const;
            float GetRotY_FromPosXZ(const bcs::FVEC3 &rPos) const;
            void GetCompPos(bcs::FVEC3 &rPos) const;
            void SetScale(const bcs::FVEC3 &rScale);

            // The functions registered with the body collision; pArg is the
            // NPC, and each calls the virtual function of the same name with
            // a trailing underscore.
            static void CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg, bcs::FVEC3 *pPush);
            static unsigned long CallbackBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                    const bcs::FVEC3 &rPos, void *pArg);
            static void CallbackBallCatch(CCollBodyUnit *pBody, CBall *pBall, void *pArg);

            // (names guessed) Read by CStageBasketHungryBossPackun::Main_ in
            // BasketF to make the goal follow the boss.
            const bcs::FVEC3 &GetPos() const { return m_vPos; }
            const bcs::FVEC3 &GetRot() const { return m_vRot; }
            // (names guessed) Inline in msw::CStgGmk_Trolley_Mdl (DOL),
            // which places its heyho; the yaw is brought into [0, 2 pi).
            void SetPos(const bcs::FVEC3 &rPos) { m_vPos = rPos; }
            void SetRotY(float rot)
            {
                while (rot > 6.2831855f) {
                    rot -= 6.2831855f;
                }
                while (rot < 0.0f) {
                    rot += 6.2831855f;
                }
                m_vRot.y = rot;
            }

        protected:
            unsigned char m_Unk34[0x48 - 0x34];
            // 0x48 (name guessed): the model (what NewMdlObj_ made).
            bcs::CMdlObj *m_pMdl;
            unsigned char m_Unk4C[0x5C - 0x4C];
            // 0x5C (name guessed): the body collision units.
            CBodyCollision *m_pBodyColl;
            bcs::FVEC3 m_vPos;   // 0x60
            bcs::FVEC3 m_vRot;   // 0x6C, written by RotateYRate
            bcs::FVEC3 m_vScale; // 0x78, written by SetScale
            unsigned char m_Unk84[0x188 - 0x84];
    };
}

#endif /* !MSW_CNPCBASE_HPP_ */
