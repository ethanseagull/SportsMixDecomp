#ifndef SELECTMENU_CMIISEL_HPP_
#define SELECTMENU_CMIISEL_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    class CMiiSelectRender;

    namespace SelectMenu {

        // Size 0xA40. The Mii select screen ("me_Mii"): the player's Miis,
        // ten to a page, or the six guest Miis, each drawn by a
        // CMiiSelectRender into a picture pane, with two page arrows and
        // two tabs (own Miis, guests). Its buttons are read from
        // "me_Mii.btn" by the CButtonEntry base at 0x5C, as in CTeam. Menu
        // object 0x17 of CSelectMenu; CSelectMenu::MiiSel_* drive it.
        // Member names are guesses.
        class CMiiSel : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    // The state while a page is being changed (see
                    // CheckPageChange).
                    STATE_PAGE = 6
                };

                enum {
                    SLOT_MAX = 20,    // the arrays have room for twenty
                    PAGE_MII_NUM = 10, // Miis shown on one page
                    GUEST_MII_NUM = 6,
                    NAME_LEN = 16
                };

                CMiiSel(long nId);
                virtual ~CMiiSel();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long nPad, long nBtnKindId);
                virtual long ClearCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // The opening, page change and closing animations.
                void StateControl();
                // Goes to the next page (bNext) or the previous one;
                // returns -1 when there is none.
                long ChangeMiiPage(int bNext);
                // Waits for the page change animations, then sets the
                // page arrows and the cursor (STATE_PAGE).
                void CheckPageChange();
                // Shows page m_nPage of the player's Miis, or the guest
                // Miis.
                void SetMiiPage();
                void SetGuestMiiPage();
                // The button the stick moves to from nBtnKindId with input
                // nInput when the plain move finds none, or 0.
                unsigned short GetNextButtonKindId(long nBtnKindId, unsigned long nInput);
                // The next button that can be chosen right of nBtnKindId,
                // or 0.
                unsigned short GetRightButtonKindId(long nBtnKindId);

                // Inline in the game (names guessed; no symbols).
                // Whether button nBtnKindId can take the cursor.
                int IsSelectButton(unsigned short nBtnKindId);
                // The data of the Mii in slot nSlot of the page shown.
                SAVE_DATA_PLAYER::MiiData *GetSlotMii(long nSlot);
                // Its name id, source and id.
                long GetSlotMiiNameId(long nSlot);
                RFLDataSource GetSlotMiiSource(long nSlot);
                RFLCreateID GetSlotMiiCreateId(long nSlot);

            private:
                // Inlined in the game, with no out-of-line copy left in the
                // module; only their local statics have symbols.
                void CheckButtonAnim();
                // Writes pText into text pane nPane when the pane exists.
                void SetTextPaneString(long nPane, const wchar_t *pText);
                // Inline in the game (names guessed; no symbols). Empties
                // the ten slots; starts the animations of the slot buttons
                // again.
                inline void ClearPage();
                // The first Mii button of the page that can be chosen, else
                // a tab (for SetDefaultMoveCursor).
                inline long GetFirstButton();
                inline void InitSlotButtons();

                friend class msw::CSelectMenu;

                CMiiSelectRender *m_pMiiRender[SLOT_MAX];   // 0x5A0, the face of each slot
                nw4r::lyt::Pane *m_pMiiPane[SLOT_MAX];      // 0x5F0, the picture pane made for it
                wchar_t m_MiiName[SLOT_MAX][NAME_LEN];      // 0x640, the name of each slot
                unsigned char m_Unk8C0[0x9D8 - 0x8C0];      // 0x8C0
                long m_nMiiIndex[SLOT_MAX];                 // 0x9D8, the Mii (or guest Mii) in each slot
                long m_nMiiCount;                           // 0xA28, Miis in the player's list (6 for guests)
                long m_nPage;                               // 0xA2C, the page shown
                long m_nWinPane;                            // 0xA30, the pane of the window animations
                long m_nPagePane;                           // 0xA34, the pane of the page number animation
                long m_bPagePrev;                           // 0xA38, the last page change went back
                unsigned char m_bGuest;                     // 0xA3C, the guest Miis are shown
                unsigned char m_UnkA3D[3];                  // 0xA3D
        };

        // Size 0x64. The window that shows the two Miis of a Wi-Fi entry
        // for two ("me_2p_entry"), the player's and the second player's.
        // Made by CSelectMenu::MiiSel_Loop as menu object 0xFF.
        class CWiFi_2PEntry : public CSelectMenuLayoutBase {
            public:
                // Inline: MiiSel_Loop is the only place that makes one.
                CWiFi_2PEntry(long nId)
                    : CSelectMenuLayoutBase(nId)
                {
                    m_pMiiRender[0] = 0;
                    m_pMiiRender[1] = 0;
                }
                virtual ~CWiFi_2PEntry();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

            private:
                CMiiSelectRender *m_pMiiRender[2]; // 0x5C, the player's and the second player's Mii
        };
    }
}

#endif /* !SELECTMENU_CMIISEL_HPP_ */
