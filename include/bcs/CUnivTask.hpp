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

    // (name guessed; nothing of it is named) A class between CUnivTask and
    // the DOL's game tasks. Its destructor is inline: the destructors of
    // msw::CItemBall, msw::CBall, msw::CChrMan, msw::CEffectCtrl, msw::CGame
    // and the other CUnivTask-derived DOL classes test `this` twice before
    // calling bcs::CTask's (one test per inline level), and the DOL has two
    // weak vtables of this shape (lbl_8046B2C0: one whose destructor tests
    // `this` once, fn_800C4C3C, and CUnivTask's, fn_800C4BE4, which calls
    // CTask's directly). Both point at CUnivTask::GetFrameRateF, so it adds
    // no virtual function. Only the classes checked so far (msw::CItemBall,
    // msw::CItemBallShellWreck) derive from it here; the others listed
    // above should too.
    class CUnivTaskEx : public CUnivTask {
        public:
            CUnivTaskEx(CTask *pParent, short priority) : CUnivTask(pParent, priority) {}
    };
}

#endif /* !BCS_CUNIVTASK_HPP_ */
