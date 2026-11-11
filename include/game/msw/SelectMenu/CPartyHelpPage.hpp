#ifndef SELECTMENU_CPARTYHELPPAGE_HPP_
#define SELECTMENU_CPARTYHELPPAGE_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5BC. The help page of a party game, shown beside the tabs
        // of CPartyInfo ("me_ptg_operation_1P"): two pages (picked by the
        // tab, event 0x31), the controls text for the current pad style and
        // a movie of the game. Its buttons are read from
        // "me_ptg_operation_1P.btn" by the CButtonEntry base at 0x5C. Menu
        // objects 0x47 to 0x4A of CSelectMenu, one per party game; defined
        // in game/menu/CPartyInfo.cpp. Member names are guesses.
        class CPartyHelpPage : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                // Values of m_nMovieState, as in CPartySelect.
                enum {
                    MOVIE_NONE = 0,
                    MOVIE_STOP = 1,  // set by Set(): stop what is playing
                    MOVIE_OPEN = 2,  // waiting to open the movie
                    MOVIE_START = 3, // waiting to start it
                    MOVIE_PLAY = 4   // started and hung in the picture pane
                };

                enum {
                    PAGE_NUM = 2
                };

                CPartyHelpPage(long nId);
                virtual ~CPartyHelpPage();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual int Set();
                virtual void Remove();

                void MovieControl();
                // Writes the controls text of the current pad style, when it
                // has changed.
                void DispControlType();

            private:
                long m_nMovieState;              // 0x5A0, MOVIE_*
                long m_bPageOn[PAGE_NUM];        // 0x5A4, page i is shown
                long m_nOutPane;                 // 0x5AC, the page pane that plays the closing animation
                long m_bMovieWait;               // 0x5B0, the movie is still to be shown
                float m_fMovieWait;              // 0x5B4, frames left until then
                long m_nPadStyle;                // 0x5B8, the pad style shown, -1 before the first
        };
    }
}

#endif /* !SELECTMENU_CPARTYHELPPAGE_HPP_ */
