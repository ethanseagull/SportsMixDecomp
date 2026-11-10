#ifndef SELECTMENU_CWIFIFRINDVS_HPP_
#define SELECTMENU_CWIFIFRINDVS_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    class CPlayerInfoRender;

    namespace SelectMenu {

        // Size 0x5A4. The friend match screen ("me_fvs_ent"): a friend has
        // asked for a match over Wi-Fi, and the player accepts or refuses.
        // Shows the friend's name, Mii and trophies. Its buttons are read
        // from "me_fvs_ent.btn" by the CButtonEntry base at 0x5C (a second
        // base, as in CTeam). Menu object 0x2B of CSelectMenu;
        // CSelectMenu::FriendVS_* drive it. ("Frind" is the game's spelling.)
        class CWiFiFrindVS : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CWiFiFrindVS(long nId);
                virtual ~CWiFiFrindVS();
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

            private:
                // Inline in the game; only its local static has a symbol.
                // Puts pText into text box nPane when that pane exists, or
                // nValue as a number when pText is null.
                void SetTextPaneString(long nPane, const wchar_t *pText, long nValue);

                // 0x5A0. (name guessed) Renders the friend's Mii; made by
                // Set(), deleted by Remove().
                CPlayerInfoRender *m_pMiiRender;
        };
    }
}

#endif /* !SELECTMENU_CWIFIFRINDVS_HPP_ */
