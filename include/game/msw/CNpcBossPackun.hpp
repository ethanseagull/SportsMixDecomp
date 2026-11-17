#ifndef MSW_CNPCBOSSPACKUN_HPP_
#define MSW_CNPCBOSSPACKUN_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/CListNodeBase.hpp"
#include "bcs/CUnivTask.hpp"
#include "game/msw/CNpcBase.hpp"
#include "game/msw/CStgMdl.hpp"

namespace nw4r {
    namespace g3d {
        class ResMdl;
    }
}

namespace msw {

    class CAtkCollision;
    class CChrMan;
    class CFgMdl;

    // INCOMPLETE: the boss Packun (Petey Piranha) NPC. A DOL class; BasketF
    // derives CNpcBossPackunBasketBase from it. Its state queue
    // bcs::CFsmQueue<CNpcBossPackun> is at 0x188 (InitFsm_ in the DOL).
    // Nothing else is declared; it must not be instantiated.
    class CNpcBossPackun : public CNpcBase, public bcs::CFsmQueue<CNpcBossPackun> {
        public:
            // (added for BasketF game/basket/CNpcBossPackunBasketBase.cpp;
            // names from the state functions, in their vtable order) The
            // states of the queue.
            enum STATE {
                STATE_SLEEP,
                STATE_IDLE,
                STATE_RUN,
                STATE_ATTACK,
                STATE_MUD_ATTACK,
                STATE_SHOT_IN,
                STATE_EAT_WAIT,
                STATE_EAT,
                STATE_MELT,
                STATE_FEARS,
                STATE_END_IDLE
            };

            // (DOL; added for BasketF game/basket/CNpcBossPackunBasketBase.cpp)
            CNpcBossPackun(CChrMan *pMan);
            virtual ~CNpcBossPackun();

            // (DOL overrides of CNpcBase's; added for BasketF
            // game/basket/CNpcBossPackunBasketBase.cpp, which calls them
            // from its own. GetNpcType and DispFunc are inline: BasketF has
            // weak copies.)
            virtual void Create();
            virtual long GetNpcType() const { return 2; }
            virtual long SendMessage(long nMsg, long, long, long, long);
            virtual void MainFunc();
            virtual void DispFunc() {}

            // The class's own virtual functions, after the state queue's
            // part of the vtable (from 0x80), named by BasketF's
            // __vt__Q23msw24CNpcBossPackunBasketBase up to GetRingNodeIdx
            // (0x130), which BasketF's CGoalObjBasket_BossPackun calls.
            // Return types are guessed; nothing defines them yet.
            virtual long GetGoalIdx() const;                            // 0x80
            virtual int IsMouthOpen() const;                            // 0x84
            virtual float GetRemainderMouthOpenFrame();                 // 0x88
            virtual float GetRemainderMouthCloseFrame();                // 0x8C
            virtual void CreateMdlObj_();                               // 0x90
            virtual void InitFsm_();                                    // 0x94
            virtual unsigned long GetQueueWorkSize() const;             // 0x98
            // (inline: BasketF has a weak copy)
            virtual unsigned long GetQueueStackSize() const { return 4; } // 0x9C
            virtual bcs::CMdlObj *NewMdlObj_(nw4r::g3d::ResMdl mdl, unsigned long heap); // 0xA0
            virtual void CtorStateSleep();                              // 0xA4
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateIdle();                               // 0xB0
            virtual void LoopStateIdle();
            virtual void DtorStateIdle();
            virtual void CtorStateRun();                                // 0xBC
            virtual void LoopStateRun();
            virtual void DtorStateRun();
            virtual void CtorStateAttack();                             // 0xC8
            virtual void LoopStateAttack();
            virtual void DtorStateAttack();
            virtual void CtorStateMudAttack();                          // 0xD4
            virtual void LoopStateMudAttack();
            virtual void DtorStateMudAttack();
            // (inline and empty: BasketF has weak copies)
            virtual void CtorStateShotIn() {}                           // 0xE0
            virtual void LoopStateShotIn() {}
            virtual void DtorStateShotIn() {}
            virtual void CtorStateEatWait();                            // 0xEC
            virtual void LoopStateEatWait();
            virtual void DtorStateEatWait();
            virtual void CtorStateEat();                                // 0xF8
            virtual void LoopStateEat();
            virtual void DtorStateEat();
            virtual void CtorStateMelt();                               // 0x104
            virtual void LoopStateMelt();
            virtual void DtorStateMelt();
            virtual void CtorStateFears();                              // 0x110
            virtual void LoopStateFears();
            virtual void DtorStateFears();
            virtual void CtorStateEndIdle();                            // 0x11C
            virtual void LoopStateEndIdle();
            virtual void DtorStateEndIdle();
            // (inline: BasketF has a weak copy)
            virtual long GetRequestFruit() const { return 0; }          // 0x128
            // The handle of the goal model the boss carries, and the node of
            // its ring (BasketF's CGoalObjBasket_BossPackun passes them on).
            virtual unsigned long GetMdlHandle();                       // 0x12C
            virtual long GetRingNodeIdx();                              // 0x130
            // (added for BasketF game/basket/CNpcBossPackunBasketBase.cpp;
            // the last two slots of the DOL vtable) Place the models at the
            // boss, and decide which are drawn.
            virtual void CalcRoot_();                                   // 0x134
            virtual void CheckDisp_();                                  // 0x138

            // (DOL; added for BasketF game/basket/CNpcBossPackunBasketBase.cpp)
            // The position of virtual node nNode (0: the head) of the model.
            void GetVNodePos(long nNode, bcs::FVEC3 &rPos) const;

            class CMudBomb;
            class CMudBombMan;

        protected:
            // (added for BasketF game/basket/CNpcBossPackunBasketBase.cpp;
            // names guessed) The members the DOL constructor sets.
            // 0x1E8: flag bits (the constructor sets 1). BasketF: bit 0 is
            // cleared on Reset; bit 1 is set while the boss faces the
            // player's view (the second model is drawn then).
            unsigned long m_nPackunFlag;
            float m_fAlpha;                 // 0x1EC, how visible the models are
            unsigned long m_Unk1F0;
            // 0x1F4: the second model (BasketF makes it in game mode 7 only),
            // drawn instead of the first one from the player's side.
            CFgMdl *m_pSubMdl;
            // 0x1F8: the nodes of the lower and upper jaw ("B_kuchi_jnt",
            // "U_kuchi_jnt"), 0 when the model has none.
            unsigned short m_anMouthNode[2];
    };

    // INCOMPLETE: a mud bomb the boss Packun throws. A task, and a node of
    // the bomb manager's list at 0x34 (the "@52@" destructor thunk of the
    // HockeyF vtable of CNpcBossPackunHockeyBase::CMudBombHockey). Its own
    // virtual functions follow the node's in that vtable; only those up to
    // Cancel are declared (return types and parameter names guessed). Must
    // not be instantiated.
    class CNpcBossPackun::CMudBomb : public bcs::CUnivTask, public bcs::CListNodeBase {
        public:
            virtual void Main();
            virtual void Create();
            virtual void SetGoalPos(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float frame);
            virtual void SetGoalPosAuto(const bcs::FVEC3 &rFrom, long n, float frame);
            // (meaning guessed from the name) Stops the bomb; CStageHockey07
            // calls it for every live bomb when the stage gimmick is reset,
            // stopped or cancelled.
            virtual void Cancel();
            // (return types guessed) The slots after Cancel, named by the
            // vtable of BasketF's CMudBombBasket. DodgeF's CStageDodge07
            // adds the words of GetPos() to its check sum.
            virtual const bcs::FVEC3 &GetInitPos() const;
            virtual const bcs::FVEC3 &GetPos() const;
            // (return types guessed; added for DodgeF game/dodge/
            // unit_BE1FC.cpp) Whether the bomb is blowing up, and the
            // radius of its attack.
            virtual int IsNowExplode() const;
            virtual float GetAtkCollRadius() const;

            // (added for BasketF game/basket/CNpcBossPackunBasketBase.cpp;
            // the slots 0x54 to 0xB8 of the vtable of its CMudBombBasket,
            // return types and parameter names guessed. The inline ones are
            // the weak copies BasketF has.)
            // The step of the bomb (1 while it explodes).
            virtual long GetCurStep() const { return m_nStep; }                      // 0x54
            virtual void CreateAtkColl();                                            // 0x58
            virtual void UpdateAtkColl();                                            // 0x5C
            virtual void SetAttackHitCallbackAll_(CAtkCollision &rColl);             // 0x60
            virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                     unsigned long);                                 // 0x64
            virtual void Explode();                                                  // 0x68
            virtual void FadeOut();                                                  // 0x6C
            virtual void UpdatePos();                                                // 0x70
            virtual float GetFrameCnt() const { return m_fFrameCnt; }                // 0x74
            virtual void Eff_Create(long nEff);                                      // 0x78
            virtual unsigned long Eff_GetHandle(long nEff) const;                    // 0x7C
            virtual void *Eff_GetObject(long nEff) const;                            // 0x80
            virtual void Eff_Stop(long nEff);                                        // 0x84
            virtual const CStgMdl::MDL_PARAM *MdlParam_Bullet() const;               // 0x88
            virtual const CStgMdl::MDL_PARAM *MdlParam_Explode() const;              // 0x8C
            virtual long GetParam_EffectID(long nEff) const;                         // 0x90
            virtual float GetParam_FadeOutTime() const;                              // 0x94
            virtual float GetParam_LifeLimit() const;                                // 0x98
            virtual float GetParam_ExplodeTime() const;                              // 0x9C
            virtual float GetParam_StayTime() const { return 10.0f; }                // 0xA0
            virtual float GetParam_GroundHeight() const;                             // 0xA4
            virtual float GetParam_ExplodeMdlRadius() const;                         // 0xA8
            virtual float GetParam_ExplodeAtkCollRadius() const;                     // 0xAC
            virtual float GetParam_ExplodeAtkCollHeight() const;                     // 0xB0
            virtual float GetParam_BulletJumpHeight() const;                         // 0xB4
            virtual float GetParam_ExplodeScaleFactor() const { return 0.15f; }      // 0xB8

            // (DOL; added for BasketF game/basket/CNpcBossPackunBasketBase.cpp)
            CMudBomb(bcs::CTask *pParent, short nPriority);
            virtual ~CMudBomb();

        protected:
            // (added for BasketF game/basket/CNpcBossPackunBasketBase.cpp;
            // names guessed, only the members the getters above read)
            bcs::FVEC3 m_vInitPos;          // 0x44, see GetInitPos
            // (added for BasketF game/basket/unit_A3AF0.cpp, name guessed)
            bcs::FVEC3 m_vPos;              // 0x50, see GetPos
            unsigned char m_Unk5C[0x2E4 - 0x5C];
            long m_nStep;                   // 0x2E4, see GetCurStep
            unsigned char m_Unk2E8[0x2EC - 0x2E8];
            float m_fFrameCnt;              // 0x2EC, see GetFrameCnt
            unsigned char m_Unk2F0[0x2FC - 0x2F0];
    };

    inline const bcs::FVEC3 &CNpcBossPackun::CMudBomb::GetInitPos() const
    {
        return m_vInitPos;
    }

    // (added for BasketF game/basket/unit_A3AF0.cpp, which has the weak
    // copies of these three, after their first callers)
    inline const bcs::FVEC3 &CNpcBossPackun::CMudBomb::GetPos() const
    {
        return m_vPos;
    }

    inline int CNpcBossPackun::CMudBomb::IsNowExplode() const
    {
        return m_nStep == 1;
    }

    inline float CNpcBossPackun::CMudBomb::GetAtkCollRadius() const
    {
        return GetParam_ExplodeAtkCollRadius();
    }


    // INCOMPLETE: the manager of the mud bombs, a DOL singleton (m_pInst in
    // .sbss). Its list of bombs is at 0x00 (a bcs::CList, 0x28 bytes, whose
    // node count is the word at 0x20); nothing else is mapped.
    class CNpcBossPackun::CMudBombMan {
        public:
            static CMudBombMan *GetInstance() { return m_pInst; }

            bcs::CList<CMudBomb> &GetList() { return m_List; }
            // (name guessed) The word at 0x20, added to DodgeF's
            // CStageDodge07 check sum: the list's node count, the number of
            // bombs.
            unsigned long GetNum() const { return m_List.GetCount(); }

        private:
            static CMudBombMan *m_pInst;

            bcs::CList<CMudBomb> m_List; // 0x00
    };
}

#endif /* !MSW_CNPCBOSSPACKUN_HPP_ */
