#ifndef CEVENTCAMERA_HPP_
#define CEVENTCAMERA_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // The camera of an event demo (DOL, 0x570 bytes; class name guessed,
    // nothing in the DOL or the modules names it). Its constructor
    // (fn_802450AC) makes a bcs::CCameraAnm at 0x4B4 and loads CLoadData
    // entry 0x60 into it. It reports camera events through
    // CEventDemoBase::SendCameraEvent.
    class CEventCamera {
        public:
            // fn_80246DC0 (name guessed). CEventDemoBase::_Init passes itself.
            static CEventCamera *CreateInstance(bcs::CTask *pParent);
    };
}

#endif /* !CEVENTCAMERA_HPP_ */
