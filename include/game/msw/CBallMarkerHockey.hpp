#ifndef MSW_CBALLMARKERHOCKEY_HPP_
#define MSW_CBALLMARKERHOCKEY_HPP_

#include "game/msw/CBallMarker.hpp"

namespace msw {

    // The marker of the hockey game's ball (HockeyF), size 0x94 (allocation size).
    //
    // INCOMPLETE: only what game/hockey/CBallManHockey.cpp needs to create
    // one. Its overrides (MainFunc, AppearCheck, GetAppearFrameNum,
    // GetDisappearFrameNum, GetDispScale, GetColor_) are not declared yet.
    class CBallMarkerHockey : public CBallMarker {
        public:
            CBallMarkerHockey(bcs::CTask *pParent, unsigned long nBallHandle);
            virtual ~CBallMarkerHockey();

        private:
            // 0x90: CBallMarker is 0x90 bytes (CBallMarkerDodge adds
            // nothing), so the hockey marker has one word of its own.
            unsigned char m_Unk90[4];
    };
}

#endif /* !MSW_CBALLMARKERHOCKEY_HPP_ */
