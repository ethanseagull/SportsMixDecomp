#ifndef MSW_CSYSTEMTEST_HPP_
#define MSW_CSYSTEMTEST_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // A test task defined in SystemF and created by DebugF's module entry
    // point. Size 0x38 (DebugModuleMain allocates 0x38 bytes).
    class CSystemTest : public bcs::CTask {
        public:
            CSystemTest();
            virtual ~CSystemTest();
            virtual void Main();

        private:
            unsigned long m_Unk34; // 0x34
    };
}

#endif /* !MSW_CSYSTEMTEST_HPP_ */
