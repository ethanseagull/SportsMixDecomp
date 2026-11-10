#ifndef MSW_CBALLMARKER_HPP_
#define MSW_CBALLMARKER_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace msw {

    // A marker that follows a ball (a DOL class; the game modules derive
    // their own). bcs::CTask at 0x00, bcs::CFsm<CBallMarker> at 0x34 (vptr
    // at 0x78), own members from 0x7C.
    //
    // INCOMPLETE: only the constructor, destructor, Main and Disp are
    // declared. The other virtual functions (slots 0x2C-0x74 of a derived vtable: MainFunc,
    // AppearCheck, Get*FrameNum, GetDispScale, GetColor_, UpdateColor_ and
    // the four states) and the members are not mapped. The size 0x94 is the
    // allocation size of CBallMarkerHockey, which may add members of its own.
    class CBallMarker : public bcs::CTask, public bcs::CFsm<CBallMarker> {
        public:
            // `nBallHandle` is the task id of the ball the marker follows.
            // (what the long selects is not known)
            CBallMarker(bcs::CTask *pParent, unsigned long nBallHandle, long);
            virtual ~CBallMarker();
            virtual void Main();
            virtual void Disp();

        protected:
            unsigned char m_Unk7C[0x18]; // 0x7C
    };
}

#endif /* !MSW_CBALLMARKER_HPP_ */
