#ifndef MSW_CBALLMARKER_HPP_
#define MSW_CBALLMARKER_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace msw {

    // The marker that shows a ball (the constructor is a DOL function;
    // each game module derives its own). bcs::CTask at 0x00,
    // bcs::CFsm<CBallMarker> at 0x34 (vptr at 0x78), own members from 0x7C.
    // Size 0x90: msw::CBallMarkerVolley (0x94) adds a float at 0x90.
    //
    // INCOMPLETE: only the constructor and the CTask overrides are
    // declared; the other virtual functions
    // (MainFunc, AppearCheck, GetAppearFrameNum, ... and the state
    // functions) and the members are not mapped.
    class CBallMarker : public bcs::CTask, public bcs::CFsm<CBallMarker> {
        public:
            // `nBallHandle` is the task id of the ball. (what the long
            // selects is not known; CBallMarkerVolley passes 1)
            CBallMarker(bcs::CTask *pParent, unsigned long nBallHandle, long);
            virtual ~CBallMarker();
            virtual void Main();
            virtual void Disp();

        protected:
            unsigned char m_Unk7C[0x14]; // 0x7C
    };
}

#endif /* !MSW_CBALLMARKER_HPP_ */
