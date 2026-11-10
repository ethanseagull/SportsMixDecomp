#ifndef MSW_CBALLTARGETMARKER_HPP_
#define MSW_CBALLTARGETMARKER_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace msw {

    // The marker on the court where a ball will come down (VolleyF), size
    // 0xB4 (the size CBallManVolley allocates): bcs::CTask at 0x00,
    // bcs::CFsm<CBallTargetMarker> at 0x34 (vptr at 0x78), own members from
    // 0x7C.
    //
    // INCOMPLETE: only what game/volley/CBallManVolley.cpp needs to create
    // one. The class's own virtual functions (Main, Disp, UpdateViewFlag_,
    // UpdateColor_) and its state functions are not declared yet, and the
    // members are not mapped.
    class CBallTargetMarker : public bcs::CTask, public bcs::CFsm<CBallTargetMarker> {
        public:
            // `nBallHandle` is the task id of the ball the marker follows.
            // (what the int selects is not known; CBallManVolley passes 1)
            CBallTargetMarker(bcs::CTask *pParent, unsigned long nBallHandle, int);
            virtual ~CBallTargetMarker();
            virtual void Main();

        private:
            unsigned char m_Unk7C[0x38]; // 0x7C
    };

    // The target marker of the volley game against a boss (VolleyF), size
    // 0xB4 like its base (the size BALL_MAN_VOLLEY::CBallManVolleyVsBoss
    // allocates). Its constructor passes 1 as the base's int.
    //
    // INCOMPLETE: only the constructor and destructor are declared.
    class CBallTargetMarkerVsBoss : public CBallTargetMarker {
        public:
            CBallTargetMarkerVsBoss(bcs::CTask *pParent, unsigned long nBallHandle);
            virtual ~CBallTargetMarkerVsBoss();
    };
}

#endif /* !MSW_CBALLTARGETMARKER_HPP_ */
