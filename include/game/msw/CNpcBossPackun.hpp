#ifndef MSW_CNPCBOSSPACKUN_HPP_
#define MSW_CNPCBOSSPACKUN_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/CListNodeBase.hpp"
#include "bcs/CUnivTask.hpp"
#include "game/msw/CNpcBase.hpp"

namespace msw {

    // INCOMPLETE: the boss Packun (Petey Piranha) NPC. A DOL class; BasketF
    // derives CNpcBossPackunBasketBase from it. Its state queue
    // bcs::CFsmQueue<CNpcBossPackun> is at 0x188 (InitFsm_ in the DOL).
    // Nothing else is declared; it must not be instantiated.
    class CNpcBossPackun : public CNpcBase, public bcs::CFsmQueue<CNpcBossPackun> {
        public:
            virtual ~CNpcBossPackun();

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
    };

    // INCOMPLETE: the manager of the mud bombs, a DOL singleton (m_pInst in
    // .sbss). Its list of bombs is at 0x00, a word at 0x20; nothing else is mapped.
    class CNpcBossPackun::CMudBombMan {
        public:
            static CMudBombMan *GetInstance() { return m_pInst; }

            bcs::CList<CMudBomb> &GetList() { return m_List; }
            // (name guessed) The word at 0x20, added to DodgeF's
            // CStageDodge07 check sum; most likely the number of bombs.
            unsigned long GetNum() const { return m_nNum; }

        private:
            static CMudBombMan *m_pInst;

            bcs::CList<CMudBomb> m_List; // 0x00
            unsigned long m_nNum;        // 0x20 (name guessed)
    };
}

#endif /* !MSW_CNPCBOSSPACKUN_HPP_ */
