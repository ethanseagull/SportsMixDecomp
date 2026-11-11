#ifndef MSW_CBALLMARKERBASKET_HPP_
#define MSW_CBALLMARKERBASKET_HPP_

#include "game/msw/CBallMarker.hpp"

namespace msw {

    // The ball marker of the basket game (BasketF), size 0x94 (the size
    // CBallManBasket allocates). Its functions are in
    // game/basket/CBallMarkerBasket.cpp; the virtual functions are declared
    // in the order of __vt__Q23msw17CBallMarkerBasket.
    class CBallMarkerBasket : public CBallMarker {
        public:
            CBallMarkerBasket(bcs::CTask *pParent, unsigned long nBallHandle);
            virtual ~CBallMarkerBasket();

            virtual void MainFunc();
            virtual int AppearCheck();
            virtual float GetAppearFrameNum() const;
            virtual float GetDisappearFrameNum() const;
            virtual float GetDispScale() const;
            virtual void GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long);

        private:
            // 0x90 (name guessed): the handle (bcs::CScnObj::GetHandle) of
            // the model the marker's model is attached to while the ball is
            // held; 0 while it follows the ball itself.
            unsigned long m_nAttachHandle;
    };
}

#endif /* !MSW_CBALLMARKERBASKET_HPP_ */
