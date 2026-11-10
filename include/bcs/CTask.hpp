#ifndef BCS_CTASK_HPP_
#define BCS_CTASK_HPP_

#include "bcs/CMainApp.hpp"

namespace bcs {

    // Size 0x34, vptr at 0x00. The offsets of the mapped members come from
    // the constructor in the DOL (__ct__Q23bcs5CTaskFPQ23bcs5CTasks).
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

            // (name guessed) Not zero once Delete was called. msw::CBallMan's
            // functions in the DOL and msw::CBallManBoss::MainFunc_ in BossF
            // test the bit on every task they get from CTaskMan::GetTask.
            int IsDelete() const { return m_nFlag & 1; }

            // (name guessed) Replaces the word at 0x08. The CTask constructor
            // leaves 0xFCFF there, bcs::CCamera's 0xFC00, and
            // msw::CTournamentCamera's stores 0xFC01.
            void SetUnk08(unsigned long value) { m_Unk08 = value; }
            // (name guessed) Clears bits of the word at 0x08 (DodgeF's
            // CTelopVoice, CTelopOut and CLifeGaugeTask clear 0x4).
            void ClearUnk08(unsigned long bits) { m_Unk08 &= ~bits; }

        private:
            unsigned long m_nId;         // 0x04 (name guessed)
            unsigned long m_Unk08;       // 0x08
            unsigned long m_nFlag;       // 0x0C, bit 0 asks for deletion (name guessed)
            CTask *m_pParent;            // 0x10 (name guessed)
            unsigned char m_Unk14[0x1C]; // 0x14
            short m_nPriority;           // 0x30 (name guessed)
    };
}

#endif /* !BCS_CTASK_HPP_ */
