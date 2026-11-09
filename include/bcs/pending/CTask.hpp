#ifndef PENDING_BCS_CTASK_HPP_
#define PENDING_BCS_CTASK_HPP_

// Both headers define bcs::CTask; taking the real header's guard makes a
// later include of it a no-op and an earlier one an error.
#ifdef BCS_CTASK_HPP_
#error "include bcs/pending/CTask.hpp before anything that includes bcs/CTask.hpp"
#endif
#define BCS_CTASK_HPP_

// PENDING MERGE into bcs/CTask.hpp, which this session may not edit. This is
// bcs::CTask as game/boss/CBallManBoss.cpp needs it: that header's class,
// unchanged, plus IsDelete (m_nFlag is private there, so the test of its
// bit 0 cannot be written from outside the class). Once IsDelete is in
// CTask.hpp, delete this file and include that one.

#include "bcs/CMainApp.hpp"

namespace bcs {

    // As in CTask.hpp: size 0x34, vptr at 0x00.
    class CTask {
        public:
            // A negative priority means "take the parent's".
            CTask(CTask *pParent, short priority);
            virtual ~CTask();
            virtual void Main() = 0; // no bcs::CTask::Main exists in the DOL
            virtual void Disp() {}
            virtual float GetFrameRateF() const { return CMainApp::GetAppFrameRateF(); }

            void DeleteAllChild();

            // (name guessed) The value CTaskMan::GetTask looks a task up by.
            unsigned long GetId() const { return m_nId; }

            // (name guessed) Asks the task manager to delete this task.
            void Delete() { m_nFlag |= 1; }

            // THE ADDITION. (name guessed) Not zero once Delete was called.
            // msw::CBallMan's functions in the DOL and
            // msw::CBallManBoss::MainFunc_ in BossF test the bit on every
            // task they get from CTaskMan::GetTask.
            int IsDelete() const { return m_nFlag & 1; }

            // (name guessed) Replaces the word at 0x08. The CTask constructor
            // leaves 0xFCFF there, bcs::CCamera's 0xFC00, and
            // msw::CTournamentCamera's stores 0xFC01.
            void SetUnk08(unsigned long value) { m_Unk08 = value; }

        private:
            unsigned long m_nId;         // 0x04 (name guessed)
            unsigned long m_Unk08;       // 0x08
            unsigned long m_nFlag;       // 0x0C, bit 0 asks for deletion (name guessed)
            CTask *m_pParent;            // 0x10 (name guessed)
            unsigned char m_Unk14[0x1C]; // 0x14
            short m_nPriority;           // 0x30 (name guessed)
    };
}

#endif /* !PENDING_BCS_CTASK_HPP_ */
