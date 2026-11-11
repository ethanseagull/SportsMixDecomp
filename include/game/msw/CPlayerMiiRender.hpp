#ifndef MSW_CPLAYERMIIRENDER_HPP_
#define MSW_CPLAYERMIIRENDER_HPP_

#include "game/msw/CMiiRenderBase.hpp"

namespace msw {

    // Size 0x48 (no members of its own). Renders a player's Mii into a
    // texture with the same camera as CMiiRenderBase (where it is used is
    // not known yet). Defined in game/menu/CTopMenuChrObj.cpp.
    class CPlayerMiiRender : public CMiiRenderBase {
        public:
            CPlayerMiiRender(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight);
            virtual ~CPlayerMiiRender();
            virtual void MainFunc();
    };
}

#endif /* !MSW_CPLAYERMIIRENDER_HPP_ */
