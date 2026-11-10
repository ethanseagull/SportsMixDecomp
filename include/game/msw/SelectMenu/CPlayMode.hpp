#ifndef SELECTMENU_CPLAYMODE_HPP_
#define SELECTMENU_CPLAYMODE_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x60. The play mode screen of a multiplayer game
        // ("me_multiplay_00"): one button per play mode, as many as the
        // number of players allows, and the court characters of the mode
        // under the cursor. Menu object 0xD of CSelectMenu;
        // CSelectMenu::PlayMode_* drive it.
        class CPlayMode : public CSelectMenuLayoutBase {
            public:
                CPlayMode(long nId);
                virtual ~CPlayMode();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual void CallbackButtonAnimIn(long nBtnKindId);
                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long);
                virtual long ClearCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                void StateControl();
                // Shows the court characters of the play mode button
                // nBtnKindId and their captions.
                void SetCourtCharaObj(long nBtnKindId);

            private:
                // Inlined in the game, with no out-of-line copy left in the
                // module; only their local statics have symbols.
                void CheckButtonAnim();
                void PaneColorInit();
                void ButtonDispInit(long nButtonNum);

                // 0x5C. (name guessed) The number of players less one
                // (0 to 3), set by Set(): picks the button table and the
                // court characters.
                unsigned char m_nPlayerIdx;
        };
    }
}

#endif /* !SELECTMENU_CPLAYMODE_HPP_ */
