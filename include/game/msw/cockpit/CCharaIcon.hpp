#ifndef MSW_COCKPIT_CCHARAICON_HPP_
#define MSW_COCKPIT_CCHARAICON_HPP_

#include "game/msw/menu/CDispTaskList.hpp"

namespace msw {
    namespace cockpit {

        // Size 0x34. Lives in the DOL: a display task that keeps a pane over
        // a character (BasketF CCockpitBasketHarapeko::SetCrownIcon makes
        // one per crown). The constructor sets m_bEnd and the word at 0x30
        // to 0. Names are guesses.
        class CCharaIcon : public menu::CDispTaskLink {
            public:
                CCharaIcon();

                void SetUnk30(long nValue) { m_Unk30 = nValue; }

            private:
                long m_Unk30; // 0x30
        };
    }
}

#endif /* !MSW_COCKPIT_CCHARAICON_HPP_ */
