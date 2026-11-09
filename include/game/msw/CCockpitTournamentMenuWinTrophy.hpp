#ifndef CCOCKPITTOURNAMENTMENUWINTROPHY_HPP_
#define CCOCKPITTOURNAMENTMENUWINTROPHY_HPP_

#include "game/msw/CCockpitMenuBaseButtonA.hpp"

namespace msw {

    // Size 0x9C. The tournament's window with one text and an A button
    // (layout TO_WIN_I). Adds no data.
    class CCockpitTournamentMenuWinTrophy : public CCockpitMenuBaseButtonA {
        public:
            // The last argument is not used.
            CCockpitTournamentMenuWinTrophy(long nId, float fSortZ, unsigned long);
            virtual ~CCockpitTournamentMenuWinTrophy();
            // 0, or a negative number that tells which step failed.
            virtual int Set();
            virtual void WriteText();
            virtual void StateControl();
    };
}

#endif /* !CCOCKPITTOURNAMENTMENUWINTROPHY_HPP_ */
