#ifndef SELECTMENU_CTOPMENU_HPP_
#define SELECTMENU_CTOPMENU_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5D0. The top menu ("me_top_new"): the sport is chosen on
        // the turning character table behind it (SelectMenu::CBgChr), and
        // three buttons lead elsewhere. Its buttons are read from
        // "me_top_new.btn" by the CButtonEntry base at 0x5C (a second base,
        // as in CTeam). Menu object 0x46 of CSelectMenu; CSelectMenu's
        // TopMenu_* steps drive it. Member names are guesses.
        class CTopMenu : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    BTN_ANIM_NUM = 3
                };

                CTopMenu(long nId);
                virtual ~CTopMenu();
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
                // Inline in the game; only their local statics have
                // symbols.
                void CheckButtonAnim();
                // Records the sport under the pointer (nChara, -1 for none)
                // in m_nHitChara, as a character table place.
                void SetCursolHit(long nChara);

                // 0x5A0. The last button the stick moved to, where the
                // stick comes back to from the character table.
                long m_nStickBtnKindId;
                long m_Unk5A4;                   // 0x5A4, set to -1 with m_nStickBtnKindId
                // 0x5A8. The button the cursor is on (0x8020: the
                // character table).
            public:
                long m_nCursorBtnKindId;
                // 0x5AC. The sport under the pointer, -1 for none. Read by
                // CSelectMenu::TopMenu_LoopStep0 and
                // TopMenu_ChangeStateInit.
                long m_nHitChara;
            private:
                unsigned char m_Unk5B0[0x14];    // 0x5B0
                // 0x5C4. Whether the highlight animation of each of the
                // three buttons (0x8010 to 0x8012) is playing.
                long m_bBtnAnim[BTN_ANIM_NUM];
        };
    }
}

#endif /* !SELECTMENU_CTOPMENU_HPP_ */
