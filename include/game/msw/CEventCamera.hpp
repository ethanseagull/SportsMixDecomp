#ifndef CEVENTCAMERA_HPP_
#define CEVENTCAMERA_HPP_

#include "bcs/CScnObj.hpp"
#include "bcs/CTask.hpp"

namespace bcs {
    class CCamera;
}

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

    // A scene object that only passes a camera on (DOL class, 0x11C bytes;
    // msw::CTournamentMan in TourF keeps one in the render list and points
    // it at the camera in use). Only the members used so far.
    class CEventCameraNull : public bcs::CScnObj {
        public:
            CEventCameraNull();
            virtual void UpdateFrame(float);

            // (name guessed) The camera passed on.
            void SetCamera(bcs::CCamera *pCamera) { m_pCamera = pCamera; }

        private:
            bcs::CCamera *m_pCamera; // 0x118
    };
}

#endif /* !CEVENTCAMERA_HPP_ */
