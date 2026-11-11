#ifndef CCOCKPITRESULTPARTYVS_HPP_
#define CCOCKPITRESULTPARTYVS_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // The party game's result screen. Lives in the DOL. Only what BasketF's
    // CCockpitBasket calls; the base is a guess, the size is not known.
    class CCockpitResultPartyVS : public CCockpitLayoutBase {
        public:
            void SetSubTitleText(const wchar_t *pText);
            void SetDelayResultButton();
    };
}

#endif /* !CCOCKPITRESULTPARTYVS_HPP_ */
