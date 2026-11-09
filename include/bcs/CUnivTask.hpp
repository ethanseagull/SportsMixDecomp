#ifndef BCS_CUNIVTASK_HPP_
#define BCS_CUNIVTASK_HPP_

#include "bcs/CMainApp.hpp"
#include "bcs/CTask.hpp"

namespace bcs {

    // A task whose frame rate follows the application's speed factor.
    // Nothing of this class has a name in the DOL; it is known from the
    // GetFrameRateF the modules emit for the classes derived from it.
    class CUnivTask : public CTask {
        public:
            // (guessed) msw::CEffectCtrl's constructor calls CTask's directly,
            // so this one is inline.
            CUnivTask(CTask *pParent, short priority) : CTask(pParent, priority) {}

            virtual float GetFrameRateF() const
            {
                return CTask::GetFrameRateF() * CMainApp::GetUnivSpeed();
            }
    };
}

#endif /* !BCS_CUNIVTASK_HPP_ */
