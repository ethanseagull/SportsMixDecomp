#ifndef SELECTMENU_CCUPSELECTTAB_HPP_
#define SELECTMENU_CCUPSELECTTAB_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C8 (layout.py). The difficulty tabs over the cup select
        // screen; its buttons come from the CButtonEntry base at 0x5C.
        // Menu object 0x34 of CSelectMenu, shown with CCupSelect (object
        // 0x33). Defined in game/menu/CCupSelectTab.cpp. Only what
        // game/menu/unit_61C00.cpp uses; member names are guesses.
        class CCupSelectTab : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CCupSelectTab(long nId);
                virtual ~CCupSelectTab();
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

                // New in this class (as in CButton1). The return type is a
                // guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // Makes difficulty nLevel the selected tab; not zero when
                // the selection changed.
                int SetDifficultySelect(long nLevel);
                // The button kind id of the tab of difficulty nLevel.
                long GetCupBtnKindId(long nLevel);
                void AllStopTabAnim();

            private:
                // (fields from 0x5A0 to 0x5AC and from 0x5B0 named for
                // game/menu/CCupSelectTab.cpp; names guessed)
                // 0x5A0. The animation of pane 0x14 requested by events
                // 0x3D (1, then 2 while it plays) and 0x3E (3, then 4).
                long m_nTabAnimState;
                // 0x5A4. The selected difficulty, -1 before the first.
                long m_nLevel;
                unsigned char m_Unk5A8[0x5];        // 0x5A8

            public:
                // 0x5AD. The number of difficulties open, less one (set by
                // CSelectMenu::Cup_In from _DifficultyBitCount).
                unsigned char m_nLevelNum;

            private:
                unsigned char m_Unk5AE[0x2];        // 0x5AE
                // 0x5B0, 0x5B4. Not zero while the left / right arrow is
                // hidden (at the first / the last difficulty).
                long m_bLeftArrow;
                long m_bRightArrow;
                // 0x5B8. Not zero while the pointer is on tab i.
                long m_bTabPointed[4];
        };
    }
}

#endif /* !SELECTMENU_CCUPSELECTTAB_HPP_ */
