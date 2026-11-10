#ifndef SELECTMENU_CPLAYERINFO_HPP_
#define SELECTMENU_CPLAYERINFO_HPP_

#include "game/msw/CInfoTabChange.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x68. The player information window of the Wi-Fi menu
        // ("main_p_inf"): five tabs with two arrows to step through them,
        // a rules button and a button that leaves for mode 0x1B. Menu
        // object 0x20 of CSelectMenu; CSelectMenu::WifiPlayerInfo_* drive
        // it. CInfoTabChange is at 0x5C (five tabs).
        class CPlayerInfo : public CSelectMenuLayoutBase, public CInfoTabChange {
            public:
                CPlayerInfo(long nId);
                virtual ~CPlayerInfo();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long nPad, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // CInfoTabChange's; the class gives them slots of its own
                // as well (vtable words 36 and 37).
                virtual long ChangeTab(long nTabNo);
                virtual long SetVisibleArrow(unsigned short nBtnKindId, int bVisible);

            private:
                // Inlined in the game; no symbols.
                void CheckButtonAnim()
                {
                    static const int backEvent = 0x20;

                    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
                    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
                }
                void InitPaneAnim();
        };
    }
}

#endif /* !SELECTMENU_CPLAYERINFO_HPP_ */
