#ifndef CMENUPADINFO_HPP_
#define CMENUPADINFO_HPP_

namespace msw {

    // Size 0x50 (the size of s_Instance). Lives in the DOL. Only the member
    // used so far.
    class CMenuPadInfo {
        public:
            // (name guessed; no symbol) The pad that leads the menus. The
            // DOL's own SelectPadLeader(int) returns the same word.
            long GetPadLeader() const { return m_nPadLeader; }
            // The DOL's body returns the same word and ignores its argument.
            long SelectPadLeader(int);

            // (name guessed) The halfword at 0x0A of each of the four 0x10-byte
            // pad entries, gathered in a static array that is returned (the
            // DOL's fn_80142138; CCockpitLayoutBase::CheckButtonAnim passes
            // it to CMenuPartList::CheckButtonAnim).
            const unsigned short *GetPadStates();

            // (added for game/menu/CTitle.cpp; return type guessed) The
            // player number pad nPad has by default.
            long GetDefaultPlayerNo(long nPad);

            // (name guessed; inline, no symbol; added for
            // game/menu/CPlusMinus.cpp) Either of the two low flags of pad
            // nPad's entry.
            int IsUsedCursor(long nPad) const { return m_aPad[nPad].IsUsedCursor(); }

            // (name guessed; inline, no symbol; added for
            // game/menu/CEntry.cpp) The flag byte at 0x04 of pad nPad's
            // entry: 0x01 while a remote is connected, 0x02 while it has
            // an extension.
            unsigned char GetPadFlags(long nPad) const { return m_aPad[nPad].m_nFlags04; }

            // (name guessed; inline, no symbol; added for
            // game/menu/unit_6EE84.cpp) Pad nPad takes part in the character
            // select: bit 0x04 of the flags at 0x04 is set and the one at
            // 0x06 is not.
            int IsCharSelPad(long nPad) const { return m_aPad[nPad].IsCharSelPad(); }

            // (names guessed; inline, no symbol; added for
            // game/msw/CCokcpitMenuDataList.cpp) The halfword at 0x0A of
            // pad nPad's entry, which GetPadStates gathers.
            // (added for DOL game/msw/CMenuPartBase.cpp; name guessed)
            unsigned char GetPlayerNo(long nPad) const { return m_aPad[nPad].m_nPlayerNo; }

            unsigned short GetPadState(long nPad) const { return m_aPad[nPad].m_nState; }
            void SetPadState(long nPad, unsigned short nState) { m_aPad[nPad].m_nState = nState; }
            // (name guessed; inline, no symbol; added for
            // game/msw/CCockpitLayoutBase.cpp) The byte at 0x49.
            unsigned char GetCursorMode() const { return m_nCursorMode; }

            static CMenuPadInfo s_Instance;

        private:
            // (added for game/menu/CTitle.cpp) CSelectMenu's title screen
            // steps write the flags at 0x04 and 0x06 and the byte at 0x07.
            friend class CSelectMenu;
            // (added for game/msw/CCockpitMenuBase.cpp) Its pad parameter
            // functions write the same bytes.
            friend class CCockpitMenuBase;

            // 0x10 bytes per pad from 0x04. (2026-11-11, game/menu/unit_6EE84.cpp)
            // The pads start at 0x04, not 0x00: the original's loops over
            // them keep a pointer to 0x04 + 0x10 * nPad and reach the flags
            // at 0 and 2 from it. The absolute offsets are as before.
            struct PAD {
                // 0x04. (name guessed; added for game/menu/CTitle.cpp)
                // Flag bits: CSelectMenu::Title_Loop sets 0x04 for pad 0
                // and clears it for the others, with byte masks (andi.).
                unsigned char m_nFlags04;
                unsigned char m_Unk05;       // 0x05
                // 0x06. (names guessed; added for game/menu/CPlusMinus.cpp)
                // CSelectMenu::_GetUsedCursol tests the low two bits.
                // m_nFlags06 is the same byte whole (added for
                // game/menu/CTitle.cpp): CSelectMenu::Title_In clears 0x08
                // with a byte mask (andi.), which a bit-field does not
                // compile to.
                union {
                    struct {
                        unsigned char m_Unk06_FC : 6;
                        unsigned char m_bUnk06_02 : 1;
                        unsigned char m_bUnk06_01 : 1;
                    };
                    unsigned char m_nFlags06;
                };
                // 0x07. (name guessed; added for game/menu/CTitle.cpp) What
                // GetDefaultPlayerNo returns for the pad.
                unsigned char m_nPlayerNo;
                unsigned char m_Unk08[2];    // 0x08
                unsigned short m_nState;     // 0x0A (name guessed)
                unsigned char m_Unk0C[8];    // 0x0C, to the next pad's 0x04

                // (see CMenuPadInfo::IsUsedCursor)
                int IsUsedCursor() const { return m_bUnk06_02 | m_bUnk06_01; }

                // (see CMenuPadInfo::IsCharSelPad)
                int IsCharSelPad() const
                {
                    int bSel = 0;
                    if ((m_nFlags04 & 4) && !(m_nFlags06 & 4)) {
                        bSel = 1;
                    }
                    return bSel;
                }
            };

            unsigned char m_Unk00[4];    // 0x00
            PAD m_aPad[4];               // 0x04
            long m_nPadLeader;           // 0x44 (name guessed)
            unsigned char m_Unk48;       // 0x48
            // 0x49: 1 while the menus are driven by the pointer
            // (CCockpitLayoutBase::OnCursorCheck); meaning guessed.
            unsigned char m_nCursorMode;
            unsigned char m_Unk4A[6];    // 0x4A
    };
}

#endif /* !CMENUPADINFO_HPP_ */
