#ifndef BCS_CTASK_HPP_
#define BCS_CTASK_HPP_

#include "bcs/CMainApp.hpp"

namespace bcs {

    // Size 0x34, vptr at 0x00. Only the virtual table order is known; the
    // data members are not mapped yet.
    class CTask {
        public:
            virtual ~CTask();
            virtual void Main() = 0; // no bcs::CTask::Main exists in the DOL
            virtual void Disp() {}
            virtual float GetFrameRateF() const { return CMainApp::GetAppFrameRateF(); }

            void DeleteAllChild();
        private:
            unsigned char m_Unk04[0x30]; // unknown
    };
}

#endif /* !BCS_CTASK_HPP_ */
