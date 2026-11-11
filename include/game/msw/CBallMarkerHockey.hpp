#ifndef MSW_CBALLMARKERHOCKEY_HPP_
#define MSW_CBALLMARKERHOCKEY_HPP_

#include "game/msw/CBallMarker.hpp"

namespace msw {

    // The marker of the hockey game's ball (HockeyF), size 0x94 (allocation
    // size). Its functions are in game/hockey/CBallMarkerHockey.cpp, the
    // same file as BasketF's CBallMarkerBasket.cpp; the virtual functions are
    // declared in the order of __vt__Q23msw17CBallMarkerHockey.
    class CBallMarkerHockey : public CBallMarker {
        public:
            CBallMarkerHockey(bcs::CTask *pParent, unsigned long nBallHandle);
            virtual ~CBallMarkerHockey();

            virtual void MainFunc();
            virtual int AppearCheck();
            virtual float GetAppearFrameNum() const;
            virtual float GetDisappearFrameNum() const;
            virtual float GetDispScale() const;
            virtual void GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long);

        private:
            // 0x90 (name guessed): the handle (bcs::CScnObj::GetHandle) of
            // the model the marker's model is attached to while the puck is
            // held; 0 while it follows the puck itself. CBallMarker is 0x90
            // bytes (CBallMarkerDodge adds nothing), so this is the hockey
            // marker's own word.
            unsigned long m_nAttachHandle;
    };
}

#endif /* !MSW_CBALLMARKERHOCKEY_HPP_ */
