#ifndef SELECTMENU_CBACKICON_HPP_
#define SELECTMENU_CBACKICON_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x7C. The back button in the corner of the select menu
        // screens ("me_back_ico"). It follows the owner's current screen:
        // shows or hides itself, and sets the background's title tag and
        // colour bar for the screen (TagTitle). While the camera of the
        // menu background moves between screens, the button (or event
        // 0x56) speeds it up. Member names are guesses.
        class CBackIcon : public CSelectMenuLayoutBase {
            public:
                CBackIcon(long nId);
                virtual ~CBackIcon();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // Shows or hides the button for screen nMode.
                void IndividualSelect(long nMode);
                // Sets the background's title tag, colour bar and lines
                // for screen nMode.
                void TagTitle(long nMode);
                // The common case of TagTitle: the title from the table
                // of screens, and the colour bar it names.
                void SetTagTitleIn(long nMode);

            private:
                // Inline in the game (no symbols): the background events
                // that TagTitle and SendEvent repeat.
                void TagIn(CSelectMenu *pSelectMenu);
                void TagOut(CSelectMenu *pSelectMenu);
                void SetTagColorByGameKind();
                void LineIn(long nEvent);

                // 0x5C. Where the button is: 0 shown, 1 hidden, 3 while it
                // swaps between two screens.
                long m_nIconPos;
                long m_nMode;          // 0x60, the owner's screen last seen
                long m_bPushed;        // 0x64, set when the button is pushed (event 0x2C reads it)
                long m_bTagIn;         // 0x68, the background's title tag is shown
                long m_bLineIn;        // 0x6C, the background's lines are shown
                float m_fSpeedRate;    // 0x70, how much the camera has been sped up
                long m_bSpeedUp;       // 0x74, the camera has been sped up on this screen
                long m_bSpeedRequest;  // 0x78, set by event 0x56: speed the camera up
        };
    }
}

#endif /* !SELECTMENU_CBACKICON_HPP_ */
