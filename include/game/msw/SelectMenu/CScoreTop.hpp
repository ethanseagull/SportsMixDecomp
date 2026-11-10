#ifndef SELECTMENU_CSCORETOP_HPP_
#define SELECTMENU_CSCORETOP_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5FC. The first page of the score book ("me_scorebook_A"):
        // one button per game kind, its buttons and animation names read
        // from "me_scorebook_A.btn" by the CButtonEntry part at 0x5C. Menu
        // object 0x43 of CSelectMenu; CSelectMenu::ScoreTop_* drive it.
        // CButtonEntry is a second base, not a member: the constructor
        // builds it before the vtable stores, and the destructor passes it
        // 0, not -1.
        class CScoreTop : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CScoreTop(long nId);
                virtual ~CScoreTop();
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
                // Inlined into Main() in the game, with no out-of-line copy
                // left in the module; only its local static has a symbol.
                void CheckButtonAnim();
        };
    }
}

#endif /* !SELECTMENU_CSCORETOP_HPP_ */
