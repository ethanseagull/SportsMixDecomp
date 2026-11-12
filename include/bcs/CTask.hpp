#ifndef BCS_CTASK_HPP_
#define BCS_CTASK_HPP_

#include "bcs/CMainApp.hpp"
#include "nw4r/ut/List.hpp"

namespace bcs {

    // Size 0x34, vptr at 0x00. The offsets of the mapped members come from
    // the constructor in the DOL (__ct__Q23bcs5CTaskFPQ23bcs5CTasks).
    class CTask {
        public:
            // A negative priority means "take the parent's".
            CTask(CTask *pParent, short priority);
            virtual ~CTask();
            // Empty and inline: the DOL has no copy, but modules that make a
            // vtable not overriding it carry a weak one (DodgeF
            // game/dodge/CcDaisyFlowerGarden.cpp, through msw::CGameTask's).
            virtual void Main() {}
            virtual void Disp() {}
            virtual float GetFrameRateF() const { return CMainApp::GetAppFrameRateF(); }

            void DeleteAllChild();

            // (name guessed) The value CTaskMan::GetTask looks a task up by.
            unsigned long GetId() const { return m_nId; }

            // (name guessed) Asks the task manager to delete this task.
            void Delete() { m_nFlag |= 1; }

            // (name guessed) Not zero once Delete was called. msw::CBallMan's
            // functions in the DOL and msw::CBallManBoss::MainFunc_ in BossF
            // test the bit on every task they get from CTaskMan::GetTask.
            int IsDelete() const { return m_nFlag & 1; }

            // (name guessed) The task given to the constructor.
            CTask *GetParent() const { return m_pParent; }

            // (name guessed) Replaces the word at 0x08. The CTask constructor
            // leaves 0xFCFF there, bcs::CCamera's 0xFC00, and
            // msw::CTournamentCamera's stores 0xFC01.
            void SetUnk08(unsigned long value) { m_Unk08 = value; }

            // (name guessed) The child after pChild, the first one when
            // pChild is null; null after the last.
            CTask *GetNextChild(const CTask *pChild) const
            {
                return static_cast<CTask *>(nw4r::ut::List_GetNext(&m_ChildList, pChild));
            }
            // (name guessed) Clears bits of the word at 0x08 (DodgeF's
            // CTelopVoice, CTelopOut and CLifeGaugeTask clear 0x4).
            void ClearUnk08(unsigned long bits) { m_Unk08 &= ~bits; }
            // (name guessed) Sets bits of the word at 0x08 (HockeyF's
            // CShootMarker sets 0x1).
            void OnUnk08(unsigned long bits) { m_Unk08 |= bits; }
            // (names guessed) The low byte of the word at 0x08 holds a value
            // of its own: msw::CGame's stop task sets it to 1, msw::CGame and
            // its pause task move the word by a constant (DOL constructors
            // in game/msw/CGame.cpp).
            void SetUnk08Low(unsigned long value) { m_Unk08 = (m_Unk08 & ~0xFFUL) | value; }
            void AddUnk08(long n) { m_Unk08 += n; }

        private:
            // Moves tasks between its priority lists and runs them.
            friend class CTaskMan;

            unsigned long m_nId;         // 0x04 (name guessed)
            unsigned long m_Unk08;       // 0x08
            unsigned long m_nFlag;       // 0x0C, bit 0 asks for deletion (name guessed)
            CTask *m_pParent;            // 0x10 (name guessed)
            unsigned char m_Unk14[0x10]; // 0x14

        public:
            // (name guessed) The constructor runs List_Init on it; bcs::CSnd's
            // functions walk the children of its root sound task through it.
            nw4r::ut::List m_ChildList;  // 0x24

        private:
            short m_nPriority;           // 0x30 (name guessed)
    };
}

#endif /* !BCS_CTASK_HPP_ */
