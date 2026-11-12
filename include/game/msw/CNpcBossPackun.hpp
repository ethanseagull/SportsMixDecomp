#ifndef MSW_CNPCBOSSPACKUN_HPP_
#define MSW_CNPCBOSSPACKUN_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/CListNodeBase.hpp"
#include "bcs/CUnivTask.hpp"
#include "game/msw/CNpcBase.hpp"

namespace nw4r {
    namespace g3d {
        class ResMdl;
    }
}

namespace msw {

    // INCOMPLETE: the boss Packun (Petey Piranha) NPC. A DOL class; BasketF
    // derives CNpcBossPackunBasketBase from it. Its state queue
    // bcs::CFsmQueue<CNpcBossPackun> is at 0x188 (InitFsm_ in the DOL).
    // Nothing else is declared; it must not be instantiated.
    class CNpcBossPackun : public CNpcBase, public bcs::CFsmQueue<CNpcBossPackun> {
        public:
            virtual ~CNpcBossPackun();

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
            virtual unsigned long GetQueueStackSize() const;            // 0x9C
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
            virtual void CtorStateShotIn();                             // 0xE0
            virtual void LoopStateShotIn();
            virtual void DtorStateShotIn();
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
            virtual long GetRequestFruit() const;                       // 0x128
            // The handle of the goal model the boss carries, and the node of
            // its ring (BasketF's CGoalObjBasket_BossPackun passes them on).
            virtual unsigned long GetMdlHandle();                       // 0x12C
            virtual long GetRingNodeIdx();                              // 0x130

            class CMudBomb;
            class CMudBombMan;
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
    };

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
