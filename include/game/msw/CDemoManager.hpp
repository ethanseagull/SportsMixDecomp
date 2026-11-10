#ifndef CDEMOMANAGER_HPP_
#define CDEMOMANAGER_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // The task that plays one event demo, named after its file (DOL, size
    // 0x40: BossF's CEventDemoBoss::CreateDemo allocates it). The
    // constructor calls bcs::CTask's with priority -1 and keeps the name at
    // 0x3C. Only the members used so far; the data members are not mapped.
    class CDemoManager : public bcs::CTask {
        public:
            CDemoManager(const char *pName, bcs::CTask *pParent);
            virtual ~CDemoManager();
            virtual void Main();

        private:
            unsigned char m_Unk34[0x08];
            const char *m_pName; // 0x3C (name guessed)
    };
}

#endif /* !CDEMOMANAGER_HPP_ */
