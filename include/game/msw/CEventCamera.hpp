#ifndef CEVENTCAMERA_HPP_
#define CEVENTCAMERA_HPP_

#include "bcs/CScnObj.hpp"
#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/vm/CVMCpu.hpp"

namespace bcs {
    class CCamera;
}

namespace msw {

    // The base of the event demo camera (DOL; class name guessed, nothing
    // names it). A task that runs a camera script: bcs::CTask at 0, a
    // vm::CVMCpu at 0x34. Constructor fn_8023FABC, vtable lbl_8048C5B8, in
    // which its own virtuals follow the CVMCpu part (slots 0x30 to 0x3C from
    // the primary vptr). CEventDemoBase keeps the camera as this type: the
    // demo camera calls in game/msw/CEventDemo.cpp emit the weak copies of
    // the three inline defaults below (fn_80119DA0, fn_80119E88,
    // fn_80119F20), which the vtable points to. Only the members used so
    // far; names guessed.
    class CEventCameraBase : public bcs::CTask, public vm::CVMCpu {
        public:
            // 0x30: starts camera script pName for team nTeam.
            virtual int Setup(const char *pName, long nTeam) { return 0; }
            // 0x34: starts playing at speed fSpeed.
            virtual int Play(float fSpeed) { return 2; }
            // 0x38: stops the camera.
            virtual void Stop(int nMode) {}

            unsigned char m_Unk5C[0x4A4 - 0x5C];
            unsigned char m_nPlayMode;  // 0x4A4: PlayDemoCamera's argument
            unsigned char m_Unk4A5[2];
            signed char m_nState;       // 0x4A7: above 0 once the camera ended
            unsigned char m_Unk4A8[0x4CC - 0x4A8];
            float m_fFrame;             // 0x4CC
            int m_nFrameMode;           // 0x4D0
            float m_fParam;             // 0x4D4
            unsigned char m_Unk4D8[0x4F0 - 0x4D8];
            bcs::FVEC3 m_vTarget;       // 0x4F0
            bcs::FVEC3 m_vPos;          // 0x4FC
            float m_fUnk508;            // 0x508: SetDemoCameraInfo sets pi for side != 0
    };

    // The camera of an event demo (DOL, 0x570 bytes; class name guessed,
    // nothing in the DOL or the modules names it). Its constructor
    // (fn_802450AC) makes a bcs::CCameraAnm at 0x4B4 and loads CLoadData
    // entry 0x60 into it. It reports camera events through
    // CEventDemoBase::SendCameraEvent.
    class CEventCamera : public CEventCameraBase {
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
