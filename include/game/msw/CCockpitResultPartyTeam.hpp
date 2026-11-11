#ifndef CCOCKPITRESULTPARTYTEAM_HPP_
#define CCOCKPITRESULTPARTYTEAM_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // The party game's team result screen. Lives in the DOL. Only what
    // VolleyF's CCockpitVolley uses; the base is a guess (as for
    // CCockpitResultPartyVS), the size is not known.
    class CCockpitResultPartyTeam : public CCockpitLayoutBase {
        public:
            void SetSubTitleText(const wchar_t *pText);
            void SetRankText(const wchar_t *pText);
            void SetDelayResultButton();

        private:
            unsigned char m_Unk60[0x70 - 0x60];

        public:
            // 0x70 (name guessed): 1 to 4 picks a "PlayStyle" text for the
            // rank line; anything else shows none.
            long m_nPlayStyle;
    };
}

#endif /* !CCOCKPITRESULTPARTYTEAM_HPP_ */
