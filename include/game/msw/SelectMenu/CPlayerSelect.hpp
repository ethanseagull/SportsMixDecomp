#ifndef SELECTMENU_CPLAYERSELECT_HPP_
#define SELECTMENU_CPLAYERSELECT_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. The player selection screen of a Wi-Fi game
        // ("main_p_sel"): eight Mii buttons and an "add" button. Menu object
        // 0x1F of CSelectMenu; CSelectMenu::WifiPlayerSelect_* drive it.
        class CPlayerSelect : public CSelectMenuLayoutBase {
            public:
                CPlayerSelect(long nId);
                virtual ~CPlayerSelect();
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
        };
    }
}

#endif /* !SELECTMENU_CPLAYERSELECT_HPP_ */
