#ifndef SELECTMENU_CCOURT_HPP_
#define SELECTMENU_CCOURT_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    class CSelectMenu;

    namespace SelectMenu {

        class CCourtTab;

        // Size 0x84. The court (stage) select screen ("me_bg_select"):
        // three or four stage buttons (kind ids 0x50 to 0x53) with their
        // names. Menu object 0x31 of CSelectMenu; CCourtTab (object 0x32)
        // is the cup tab row shown with it. Defined in
        // game/menu/CCourtTab.cpp. Member names are guesses.
        class CCourt : public CSelectMenuLayoutBase {
            public:
                CCourt(long nId);
                virtual ~CCourt();
                virtual void Main();
                virtual int Set();
                virtual void Remove();

                virtual void CallbackButtonAnimIn(long nBtnKindId);
                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long nPad, long nBtnKindId);
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
                // Puts pText into the name box of stage button nStage.
                void SetTextStageName(long nStage, const wchar_t *pText);
                void SetStageId(long nStage, long nIdx, long nStageId);
                // Enables or disables stage button nStage; bDisp shows the
                // button's "new" mark when it is enabled.
                void SetStageEnable(long nStage, int bEnable, unsigned char bDisp);
                // The stage index (0 to 3) of a stage button kind id, or -1.
                long GetStageButtonKindId(long nBtnKindId);
                // Plays the opening animation of the stage buttons.
                void SetAppearanceAnim();
                void SetOneTabAnim(long nTab);
                // nBtnKindId, or the first enabled button reached from it
                // through the parts' neighbour 2; 0 when there is none.
                unsigned short GetNextButtonKindId(long nBtnKindId);

            private:
                // CCourtTab waits for this screen's closing animation.
                friend class CCourtTab;
                // CSelectMenu's court select steps read the two flags below
                // (game/menu/CCourtSelect.cpp).
                friend class msw::CSelectMenu;

                // Inlined in the game, with no out-of-line copy left in the
                // module; only their local statics have symbols.
                void CheckButtonAnim();
                void SetTextPaneString(long nPane, const wchar_t *pText);

                long m_Unk5C;                       // 0x5C
                long m_nCursorStage;                // 0x60, set by SetCursorMarkerPosition
                long m_Unk64;                       // 0x64, set to 4 by the constructor
                unsigned char m_Unk68[2];           // 0x68
                unsigned char m_nStageId[4][4];     // 0x6A, written by SetStageId
                unsigned char m_Unk7A[2];           // 0x7A
                // 0x7C. Zero for the three-stage layout, else four stages.
                int m_bFourStages;
                // 0x80. Whether events go to menu object 1 as well as 2.
                int m_bSendObj1;
        };
    }
}

#endif /* !SELECTMENU_CCOURT_HPP_ */
