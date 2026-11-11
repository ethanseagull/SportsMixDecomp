#ifndef MSW_CMIISELECTRENDER_HPP_
#define MSW_CMIISELECTRENDER_HPP_

#include "game/msw/CMiiRenderBase.hpp"

namespace msw {

    // Size 0x48 (allocated so by SelectMenu::CMiiSel::SetMiiPage). Renders
    // the face of one Mii of the Mii select screen into a texture. Defined
    // in game/menu/CTopMenuChrObj.cpp.
    class CMiiSelectRender : public CMiiRenderBase {
        public:
            CMiiSelectRender(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight);
            virtual ~CMiiSelectRender();
            virtual void MainFunc();
    };
}

#endif /* !MSW_CMIISELECTRENDER_HPP_ */
