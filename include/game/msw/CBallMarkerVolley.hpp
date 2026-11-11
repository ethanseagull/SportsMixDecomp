#ifndef MSW_CBALLMARKERVOLLEY_HPP_
#define MSW_CBALLMARKERVOLLEY_HPP_

#include "game/msw/CBallMarker.hpp"

namespace msw {

    // The ball marker of the volley game (VolleyF), size 0x94 (the size
    // CBallManVolley allocates). Its functions are in
    // game/volley/CBallTargetMarker.cpp; the virtual functions are declared
    // in the order of __vt__Q23msw17CBallMarkerVolley. Unlike the basket and
    // hockey markers it does not follow what holds the ball: its colour and
    // size change with the side of the net the ball is on.
    class CBallMarkerVolley : public CBallMarker {
        public:
            CBallMarkerVolley(bcs::CTask *pParent, unsigned long nBallHandle);
            virtual ~CBallMarkerVolley();

            virtual void MainFunc();
            virtual int AppearCheck();
            virtual float GetAppearFrameNum() const;
            virtual float GetDisappearFrameNum() const;
            virtual float GetDispScale() const;
            virtual void GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long nIdx);

            // The four colours of kind nIdx (the two pairs GetColor_ blends).
            // (what nIdx counts is not known; most values share one entry)
            void GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, bcs::BCS_Color &rColor2,
                           bcs::BCS_Color &rColor3, long nIdx);

        private:
            // 0x90 (name guessed): 0 while the ball is on the user team's
            // side of the net, 1 on the other side, moving between them over
            // time; blends the colours and the display scale.
            float m_fRate;
    };
}

#endif /* !MSW_CBALLMARKERVOLLEY_HPP_ */
