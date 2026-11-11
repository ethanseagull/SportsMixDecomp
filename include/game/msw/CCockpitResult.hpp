#ifndef CCOCKPITRESULT_HPP_
#define CCOCKPITRESULT_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // The result screen's team page. Lives in the DOL. Only what BasketF's
    // CCockpitBasket calls; the base is a guess (CancelMenu is called on it
    // through CCockpitLayoutBase's slot), the size is not known.
    class CCockpitResult : public CCockpitLayoutBase {
        public:
            void SetTitleText(const wchar_t *pText);
            void SetDelayResultButton();
    };
}

#endif /* !CCOCKPITRESULT_HPP_ */
