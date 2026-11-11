#ifndef SELECTMENU_CCHARAOK_HPP_
#define SELECTMENU_CCHARAOK_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x60. The "OK?" window after the character select
        // ("me_cha_OK"): a question with a yes and a no button. Member
        // names are guesses.
        class CCharaOk : public CSelectMenuLayoutBase {
            public:
                CCharaOk(long nId);
                virtual ~CCharaOk();
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

                // Not virtual; both return 0. The return type is a guess.
                long PushNext();
                long PushCancel();

                // 0x5C. Not zero: the window asks about the characters
                // (CHA_OK_*); zero: about the stage (BG_OK_*). Not written
                // in this unit.
                long m_bCharaMode;
        };
    }
}

#endif /* !SELECTMENU_CCHARAOK_HPP_ */
