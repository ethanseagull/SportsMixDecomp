#ifndef CCOCKPITCHRICON2DVOLLEY_HPP_
#define CCOCKPITCHRICON2DVOLLEY_HPP_

#include "game/msw/CCockpitChrIcon2D.hpp"

namespace msw {

    // Size 0x68: adds no data. The volley character icons (radar icons
    // with the characters' faces, and the comb); m_nType picks the layout
    // (game/volley/CCockpitChrIcon2DVolley.cpp).
    class CCockpitChrIcon2DVolley : public CCockpitChrIcon2D {
        public:
            CCockpitChrIcon2DVolley(long nId, float fSortZ, long nLayer, long nType);
            virtual ~CCockpitChrIcon2DVolley();
            virtual int Set();
            virtual void Remove();

            virtual long SetChrIcon(long nIcon, const bcs::FVEC3 &rPos, long nChr);
            virtual long SetChrIcon(long nIcon, const bcs::FVEC3 &rPos, long nChr, const void *pRot);

            // Puts character nChr's face into icon nIcon; 0, or -1 (no
            // face pane), -10 (no character), -2 (picture not made).
            long SetIconFace(long nIcon, long nChr);
    };
}

#endif /* !CCOCKPITCHRICON2DVOLLEY_HPP_ */
