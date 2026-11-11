#ifndef BCS_CCAMERAANMCALC_HPP_
#define BCS_CCAMERAANMCALC_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {

    class CCameraAnm;

    // (name of the class from its two DOL functions) The calculation that
    // turns a frame of a camera animation into the camera's position and
    // target. bcs::CCameraAnm makes one of these itself (its constructor
    // keeps it at 0x118 and 0x11C) and calls the one at 0x11C every frame;
    // derived classes such as msw::CSceneCalc replace it. Size 0x34: the
    // task and nothing more. Vtable 0x80464680.
    class CCameraAnmCalc : public CTask {
        public:
            // Inline in bcs::CCameraAnm's constructor and in
            // msw::CSceneCalc's: the task with the parent's priority.
            CCameraAnmCalc(CTask *pParent) : CTask(pParent, -1) {}
            // The DOL's copy (0x800145C0) is the weak one; msw::CSceneCalc's
            // destructor calls bcs::CTask's directly.
            virtual ~CCameraAnmCalc() {}
            // (DOL body 0x800145BC, empty)
            virtual void Main() {}
            // Slot 0x18 (name guessed): called by bcs::CCameraAnm with the
            // animation, the position, the target and the roll; the DOL's
            // own body is 0x80013918.
            virtual void Calc(CCameraAnm &rAnm, FVEC3 &rPos, FVEC3 &rAt, float &rRoll);

        protected:
            void _CalcRotOffset(CCameraAnm &rAnm, FVEC3 &rPos, FVEC3 &rAt, float &rRoll);
            void _CalcRev(CCameraAnm &rAnm, FVEC3 &rPos, FVEC3 &rAt, float &rRoll);
    };
}

#endif /* !BCS_CCAMERAANMCALC_HPP_ */
