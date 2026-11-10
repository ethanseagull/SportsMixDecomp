#ifndef SELECTMENU_CENTRYEXPLAIN_HPP_
#define SELECTMENU_CENTRYEXPLAIN_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. The explanation window of the entry screen
        // ("me_ent_a"): a message and one A button whose guide follows the
        // pad style.
        class CEntryExplain : public CSelectMenuLayoutBase {
            public:
                CEntryExplain(long nId);
                virtual ~CEntryExplain();
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
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // Steps m_Unk30 (STATE_*); called by Main().
                void StateControl();

            private:
                // Inline in the game (no symbol): shows the button guide
                // that fits the pad style of pad 0.
                void SetDispButtonGuide();
        };
    }
}

#endif /* !SELECTMENU_CENTRYEXPLAIN_HPP_ */
