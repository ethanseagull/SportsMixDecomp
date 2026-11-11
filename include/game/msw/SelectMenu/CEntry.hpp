#ifndef SELECTMENU_CENTRY_HPP_
#define SELECTMENU_CENTRY_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C4. The entry screen ("me_entry"): each of the four pads
        // shows whether a remote (and an extension) is connected. Its
        // buttons are read from "me_entry.btn" by the CButtonEntry base at
        // 0x5C (a second base, as in CScoreTop and CTeam). CSelectMenu's
        // Entry_* and WifiEntry_* steps drive it.
        class CEntry : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    PAD_NUM = 4
                };

                // Values of m_nCtrlState: what is connected to a pad. The
                // names are guesses.
                enum {
                    CTRL_NONE = 0,
                    CTRL_REMOTE = 1,
                    CTRL_EXTENSION = 3,   // remote plus an extension
                    CTRL_UNSET = 0xF      // set by Set(), before the first check
                };

                CEntry(long nId);
                virtual ~CEntry();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // Steps m_Unk30 (STATE_*); called by Main().
                void StateControl();
                // Plays the animations of each pad whose controllers have
                // changed, then those of the pads that take part.
                void CheckControllerState();

            private:
                // Inline in the game; only its local static has a symbol.
                void CheckButtonAnim();

                // 0x5A0. (names guessed) CTRL_* per pad, as last shown.
                unsigned char m_nCtrlState[PAD_NUM];
                // 0x5A4. Whether the pad takes part: pads 0 to n-1 with a
                // remote each.
                long m_bPadOn[PAD_NUM];
                // 0x5B4. Frames left before a change of the pad's
                // controllers is shown.
                float m_fCtrlWait[PAD_NUM];
        };
    }
}

#endif /* !SELECTMENU_CENTRY_HPP_ */
