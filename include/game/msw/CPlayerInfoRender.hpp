#ifndef MSW_CPLAYERINFORENDER_HPP_
#define MSW_CPLAYERINFORENDER_HPP_

#include "game/msw/CMiiRenderBase.hpp"

namespace msw {

    // Size 0x90 (allocated so by SelectMenu::CWiFiFrindVS::Set). Renders the
    // Mii of the player info screens. It has a second base,
    // bcs::CFsm<CPlayerInfoRender>, at 0x48 (the `@72@` thunk), which is not
    // modelled here: do not destroy one through this header. Only the
    // members used so far; argument names are guesses.
    class CPlayerInfoRender : public CMiiRenderBase {
        public:
            CPlayerInfoRender(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight);
            virtual ~CPlayerInfoRender();
            virtual void MainFunc();

            // Return type guessed (the result is not used).
            void SetDispState(long nState);

        private:
            unsigned char m_Unk34[0x90 - 0x34];
    };
}

#endif /* !MSW_CPLAYERINFORENDER_HPP_ */
