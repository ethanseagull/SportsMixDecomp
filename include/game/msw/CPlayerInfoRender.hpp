#ifndef MSW_CPLAYERINFORENDER_HPP_
#define MSW_CPLAYERINFORENDER_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CMiiRenderBase.hpp"

namespace msw {

    // Size 0x90 (allocated so by SelectMenu::CWiFiFrindVS::Set). Renders the
    // Mii of the player info screens, in a pose that depends on the state:
    // bcs::CFsm<CPlayerInfoRender> at 0x48 (vptr at 0x8C). Defined in
    // game/menu/CTopMenuChrObj.cpp; argument and state names are guesses.
    class CPlayerInfoRender : public CMiiRenderBase, public bcs::CFsm<CPlayerInfoRender> {
        public:
            enum {
                STATE_OFFLINE,
                STATE_ONLINE,
                STATE_BATTLE,
                STATE_NUM
            };

            CPlayerInfoRender(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight);
            virtual ~CPlayerInfoRender();
            virtual void MainFunc();

            // Return type guessed (the result is not used).
            void SetDispState(long nState);

            void CtorStateOffline();
            void LoopStateOffline();
            void DtorStateOffline();
            void CtorStateOnline();
            void LoopStateOnline();
            void DtorStateOnline();
            void CtorStateBattle();
            void LoopStateBattle();
            void DtorStateBattle();

        private:
            // Sets up the three states.
            void InitState();
    };
}

#endif /* !MSW_CPLAYERINFORENDER_HPP_ */
