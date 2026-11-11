#ifndef SELECTMENU_CTITLE_HPP_
#define SELECTMENU_CTITLE_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace bcs {
    namespace G2D {
        class CPaneObj;
    }
}

namespace msw {

    // Defined in game/menu/CTitle.cpp. Puts back the GX state the movie
    // player changed (one texture, no lighting, plain swap tables).
    void THPGXRestore();

    namespace SelectMenu {

        // Size 0x6C. The title screen ("me_title"): the logo, "press A",
        // and the opening movie played behind them.
        class CTitle : public CSelectMenuLayoutBase {
            public:
                // Values of m_nMovieState. The names are guesses.
                enum {
                    MOVIE_NONE = 0,
                    MOVIE_STOP = 1,  // stop what is playing
                    MOVIE_OPEN = 2,  // set by CSelectMenu::Title_Loop: open the movie
                    MOVIE_START = 3, // waiting to start it
                    MOVIE_PLAY = 4
                };

                CTitle(long nId);
                virtual ~CTitle();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                // The title animation and "press A" off (state 3) and on
                // (state 1) again; both act only from the other state.
                void ButtonOff();
                void ButtonOn();
                // Shows the background pane.
                void BGOn();
                void MovieControl();
                // -1 hides both movie panes; otherwise shows the pane for the
                // current location adjustment when nVisible is 1. Returns the
                // last pane object it changed.
                bcs::G2D::CPaneObj *SetMovieVisible(long nVisible);

            private:
                // Title_In and Title_Loop drive the screen through these.
                friend class msw::CSelectMenu;

                unsigned char m_nMovie;        // 0x5C, which movie MovieControl opens
                long m_nMovieState;            // 0x60, MOVIE_*
                unsigned long m_Unk64;         // 0x64, cleared by Set()
                unsigned long m_nMovieScnObj;  // 0x68, handle of the msw::menu::CMovieScnObj
        };

        // The copyright screen ("me_co_logo"): a layout and nothing else.
        // The game never creates one (no constructor exists); only its
        // destructor, vtable and these overrides are in MenuF.
        class CTitleCo : public CSelectMenuLayoutBase {
            public:
                CTitleCo(long nId);
                virtual ~CTitleCo();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();
        };
    }
}

#endif /* !SELECTMENU_CTITLE_HPP_ */
