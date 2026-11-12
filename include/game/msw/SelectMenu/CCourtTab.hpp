#ifndef SELECTMENU_CCOURTTAB_HPP_
#define SELECTMENU_CCOURTTAB_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    class CSelectMenu;

    namespace SelectMenu {

        // Size 0x90. The cup tabs over the court select screen
        // ("me_bg_select_w"): up to four tabs (kind ids 0x56 to 0x59).
        // Menu object 0x32 of CSelectMenu, shown with CCourt (object 0x31).
        // Member names are guesses.
        class CCourtTab : public CSelectMenuLayoutBase {
            public:
                CCourtTab(long nId);
                virtual ~CCourtTab();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual int Set();
                virtual void Remove();

                virtual void CallbackButtonAnimIn(long nBtnKindId);
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

                void StateControl();
                // Makes cup nCup the selected tab; 1 when the tabs were
                // redrawn, 0 when nothing changed.
                int SetCupSelect(long nCup);
                long GetCupBtnKindId(long nCup);
                void AllStopTabAnim();

            private:
                // CSelectMenu's court select steps fill and read the
                // fields below (game/menu/CCourtSelect.cpp).
                friend class msw::CSelectMenu;

                // (no symbol) The number of enabled cups before cup nCup.
                long GetEnabledCupIndex(long nCup) const;

                unsigned char m_Unk5C[4];           // 0x5C
                long m_nCup;                        // 0x60, the selected cup, -1 for none
                long m_Unk64;                       // 0x64, set to 4 by the constructor
                unsigned char m_Unk68[3];           // 0x68
                unsigned char m_Unk6B;              // 0x6B

            public:
                // 0x6C. 0 for no tabs, else one less than the number of
                // tabs (set by the owner before Set()).
                unsigned char m_nTabType;
                // 0x6D. Bit n is set when cup n is enabled.
                unsigned char m_nCupMask;

            private:
                unsigned char m_Unk6E[2];           // 0x6E
                int m_bLeftArrow;                   // 0x70
                int m_bRightArrow;                  // 0x74
                // 0x78. (added for game/menu/CCourtSelect.cpp) Set by
                // CSelectMenu::_BuildCourtSelect: the tabs of a party game.
                int m_bParty;
                // 0x7C. Whether the pointer is over tab n.
                int m_bTabPointed[4];
                // 0x8C. The cup each shown tab stands for.
                unsigned char m_nTabCup[4];
        };
    }
}

#endif /* !SELECTMENU_CCOURTTAB_HPP_ */
