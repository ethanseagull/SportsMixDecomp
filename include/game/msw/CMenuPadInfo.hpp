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
            int IsUsedCursor(long nPad) const
            {
                return m_aPad[nPad].m_bUnk06_02 | m_aPad[nPad].m_bUnk06_01;
            }

            static CMenuPadInfo s_Instance;

        private:
            // (added for game/menu/CTitle.cpp) CSelectMenu's title screen
            // steps write the flags at 0x04 and 0x06 and the byte at 0x07.
            friend class CSelectMenu;

            struct PAD {
                unsigned char m_Unk00[4];    // 0x00
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
                unsigned char m_Unk0C[4];    // 0x0C
            };

            PAD m_aPad[4];               // 0x00
            unsigned char m_Unk40[4];    // 0x40
            long m_nPadLeader;           // 0x44 (name guessed)
            unsigned char m_Unk48[8];    // 0x48
    };
}

#endif /* !CMENUPADINFO_HPP_ */
