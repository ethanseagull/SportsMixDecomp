#ifndef SELECTMENU_CSAVESELECT_HPP_
#define SELECTMENU_CSAVESELECT_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    class CMenuPartBase;
    class CMiiRenderBase;

    namespace SelectMenu {

        // Size 0x5C8. The player select screen ("me_save"): the four save
        // slots, each with the player's Mii, name and friend code and the
        // cups cleared, or a "new player" button for an empty slot. Its
        // buttons (kind ids 0x8020 to 0x8023, one per slot) are read from
        // "me_save.btn" by the CButtonEntry base at 0x5C. Menu object 5 of
        // CSelectMenu; CSelectMenu::PlaterSelect_* drive it. Member names
        // are guesses.
        class CSaveSelect : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    // Values of m_Unk30 besides the STATE_* ones: the
                    // slots are being moved by MoveItem.
                    STATE_MOVE = 6
                };

                CSaveSelect(long nId);
                virtual ~CSaveSelect();
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

                // Moves the slots aside for the change or delete step
                // (m_nMoveStep 1 and 2) and back (3 and 4); not zero once
                // a move has finished.
                int MoveItem();
                // Fills the slots from the save data.
                void SetUpSaveData();
                // Steps the "new player" animation of the empty slots;
                // nPlayer is the slot under the cursor.
                void NewAnimation(long nPlayer);

                // Starts moving the slots aside, with the help text of the
                // change (bDelete 0) or delete step over them, unless they
                // have moved already. Inline in the game (name guessed).
                void OpenMove(int bDelete);
                // Starts moving the slots back when they are aside. Inline
                // in the game (name guessed).
                void CloseMove()
                {
                    if (m_nMoveStep == 2) {
                        m_nMoveStep = 3;
                        m_Unk30 = STATE_MOVE;
                    }
                }

            private:
                // CSelectMenu::PlaterSelect_* drive the screen through these.
                friend class msw::CSelectMenu;

                // Inlined in the game, with no out-of-line copy left in the
                // module; only their local statics have symbols.
                void CheckButtonAnim();
                // Writes pText into text pane nPane when the pane exists.
                void SetTextPaneString(long nPane, const wchar_t *pText);
                // The button the cursor goes to from pPart sideways (entry
                // nNext0 or else nNext1 of its m_nNextBtnKindId) when it can
                // be chosen; else 0.
                long GetSideButton(CMenuPartBase *pPart, long nNext0, long nNext1);

                CMiiRenderBase *m_pMiiRender[4];   // 0x5A0, the Mii of each slot
                nw4r::lyt::Pane *m_pMiiPane[4];    // 0x5B0, the picture pane showing it
                long m_nMoveStep;                  // 0x5C0, the step of MoveItem
                unsigned char m_nNewAnim[4];       // 0x5C4, the step of NewAnimation per slot, 0xFF idle
        };
    }
}

#endif /* !SELECTMENU_CSAVESELECT_HPP_ */
