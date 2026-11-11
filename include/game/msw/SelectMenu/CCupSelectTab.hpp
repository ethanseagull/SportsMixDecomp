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
                unsigned char m_Unk5A0[0xD];        // 0x5A0

            public:
                // 0x5AD. The number of difficulties open, less one (set by
                // CSelectMenu::Cup_In from _DifficultyBitCount).
                unsigned char m_nLevelNum;

            private:
                unsigned char m_Unk5AE[0x1A];       // 0x5AE
        };
    }
}

#endif /* !SELECTMENU_CCUPSELECTTAB_HPP_ */
