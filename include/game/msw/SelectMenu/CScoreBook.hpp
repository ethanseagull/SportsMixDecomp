#ifndef SELECTMENU_CSCOREBOOK_HPP_
#define SELECTMENU_CSCOREBOOK_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "revolution/rfl.h"

namespace msw {
    namespace SelectMenu {

        // Size 0x700. The record pages of the score book ("me_scorebook_B")
        // for the game kind chosen on its first page (CScoreTop): four pages
        // turned with two buttons, the sport's totals, the five best
        // characters or Miis and the five best stages. Its buttons are read
        // from "me_scorebook_B.btn" by the CButtonEntry base at 0x5C, as in
        // CScoreTop. Menu object 0x44 of CSelectMenu; CSelectMenu::
        // ScoreBook_* drive it. (member names guessed)
        class CScoreBook : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    PAGE_NUM = 4,
                    RANK_NUM = 5
                };

                // The state after the base's: a page is turning.
                enum {
                    STATE_PAGE = 6
                };

                CScoreBook(long nId);
                virtual ~CScoreBook();
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

                // New in this class (as in CScoreTop); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // Turns to the previous page (bBack not zero) or the next
                // one; 0 when there is none.
                int PageChange(int bBack);
                // Waits for the page turn to end.
                void CheckPageChange();
                // Writes nValue into text pane nPane in format nType (0 to 2:
                // the units of "ScoreBookList", -1: a plain number, -2: an
                // empty text). -2 when the pane does not exist.
                long SetTextPaneString(long nPane, long nValue, long nType);
                void SetUpBasketScore();
                void SetUpVolleyScore();
                void SetUpDodgeScore();
                void SetUpHockeyScore();
                // The rankings every sport has: characters and Miis, then
                // stages.
                void SetUpCommonScore();
                // Draws the Mii face of ranking row nRow into its texture
                // and puts it in the row's picture pane.
                void CreateMiiFacePane(long nRow, RFLStoreData data);

                // Size 0x34. One row of the character ranking.
                struct CharaRank {
                    long m_nChr;              // 0x00, -1 for a Mii
                    RFLDataSource m_Source;   // 0x04, the Mii's
                    RFLCreateID m_CreateId;   // 0x08, the Mii's
                    long m_nScore;            // 0x10, -1 for an empty row
                    wchar_t m_Name[0x10];     // 0x14, the Mii's name
                };

                // Size 8. One row of the stage ranking.
                struct StageRank {
                    const wchar_t *m_pName;   // 0x0
                    long m_nScore;            // 0x4, -1 for an empty row
                };

            private:
                // CSelectMenu::ScoreBook_Loop reads the page.
                friend class msw::CSelectMenu;

                // Inlined in the game, with no out-of-line copy left in the
                // module; only their local statics have symbols.
                void CheckButtonAnim();
                // Writes pText into text pane nPane when the pane exists.
                void SetTextPaneString(long nPane, const wchar_t *pText);

                long m_nPage;                       // 0x5A0
                // 0x5A4. Not zero while the page turns back.
                int m_bPageBack;
                // 0x5A8. The face textures of the ranking rows. The game
                // clears and frees only the first four, although row 4 has
                // one too.
                unsigned char *m_pFaceTex[RANK_NUM];
                wchar_t m_Name[0xC];                // 0x5BC, a work buffer
                CharaRank m_CharaRank[RANK_NUM];    // 0x5D4
                StageRank m_StageRank[RANK_NUM];    // 0x6D8
        };
    }
}

#endif /* !SELECTMENU_CSCOREBOOK_HPP_ */
