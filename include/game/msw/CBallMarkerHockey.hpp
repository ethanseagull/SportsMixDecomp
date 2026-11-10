#ifndef MSW_CBALLMARKERHOCKEY_HPP_
#define MSW_CBALLMARKERHOCKEY_HPP_

#include "game/msw/CBallMarker.hpp"

namespace msw {

    // The marker of the hockey game's ball (HockeyF), size 0x94.
    //
    // INCOMPLETE: only what game/hockey/CBallManHockey.cpp needs to create
    // one. Its overrides (MainFunc, AppearCheck, GetAppearFrameNum,
    // GetDisappearFrameNum, GetDispScale, GetColor_) are not declared yet.
    class CBallMarkerHockey : public CBallMarker {
        public:
            CBallMarkerHockey(bcs::CTask *pParent, unsigned long nBallHandle);
            virtual ~CBallMarkerHockey();
    };
}

#endif /* !MSW_CBALLMARKERHOCKEY_HPP_ */
