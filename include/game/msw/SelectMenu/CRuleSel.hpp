#ifndef SELECTMENU_CRULESEL_HPP_
#define SELECTMENU_CRULESEL_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5F4. The rules screen ("me_opt"): up to ten options of the
        // current game on two pages of five, each with a left and a right
        // arrow that step its setting, plus OK and reset buttons per page.
        // Its buttons are read from "me_opt.btn" by the CButtonEntry base at
        // 0x5C, as in CTeam. Menu object 0x4C of CSelectMenu;
        // CSelectMenu::RuleSel_* drive it. Member names are guesses.
        class CRuleSel : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                // The state after a setting has changed: the arrows are
                // updated once m_fWait has run out (see Main).
                enum {
                    STATE_CHANGE = 6
                };

                enum {
                    OPTION_MAX = 10, // two pages of five
                    PAGE_OPTION_NUM = 5
                };

                CRuleSel(long nId);
                virtual ~CRuleSel();
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

                // Writes the names and settings of the options into their
                // text panes, and shows or hides each row and its arrows.
                // (the game's spelling)
                void SetUpOprion();
                // The option shown on row nRow (0 to 9), or on the row of
                // button nBtnKindId (a row, or one of its arrows); -1 for
                // an empty row.
                long GetRotateButtonId(long nBtnKindId);
                // Shows page nPage (0 or 1).
                void PageChange(int nPage);
                // Lights the page arrows for the current page.
                void SetArrow();
                // Turns arrow nSide (0 left, 1 right) of row nRow off
                // (bDisable) or on.
                void SetArrowDisable(long nRow, int nSide, int bDisable);

                // Inline in the game (names guessed; no symbols).
                // The row option nOption is on, -1 when none.
                long GetOptionRow(long nOption)
                {
                    for (long i = 0; i < OPTION_MAX; i++) {
                        if (nOption == m_nOptionId[i]) {
                            return i;
                        }
                    }
                    return -1;
                }
                // After a setting has changed: the arrows of row nRow are
                // set bLeftOff / bRightOff once the change has shown.
                void SetArrowWait(long nRow, int bLeftOff, int bRightOff)
                {
                    m_nArrowRow = nRow;
                    m_bArrowOff[0] = bLeftOff;
                    m_bArrowOff[1] = bRightOff;
                    m_fWait = 10.0f;
                    m_Unk30 = STATE_CHANGE;
                }
                // Sets the OK button for nothing changed yet (bUnchanged) or
                // a change.
                inline void SetRuleState(int bUnchanged);
                // Rewrites the names and settings of all rows.
                inline void SetOptionText();

            private:
                // Inlined in the game, with no out-of-line copy left in the
                // module; only its local static has a symbol.
                void CheckButtonAnim();
                // The opening animation of the window of each page by the
                // number of options on it (-1: an empty page).
                inline void GetPageAnim(long *pAnim);

                friend class msw::CSelectMenu;

                long m_nPage;           // 0x5A0, the page shown (0 or 1)
                long m_nOutPane[3];     // 0x5A4, the panes Main plays the closing animation on
                long m_Unk5B0;          // 0x5B0, not used here
                long m_bPageText;       // 0x5B4, the page text (pane 0x60) is shown
                long m_bUnchanged;      // 0x5B8, no setting changed or reset since Set
                long m_nArrowRow;       // 0x5BC, the row SetArrowWait waits for, -1 none
                long m_bArrowOff[2];    // 0x5C0, its arrows' state after the wait
                float m_fWait;          // 0x5C8, frames left in STATE_CHANGE
                long m_nOptionId[OPTION_MAX]; // 0x5CC, the option on each row, -1 none
        };
    }
}

#endif /* !SELECTMENU_CRULESEL_HPP_ */
