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

            // (name guessed; inline, no symbol; added for
            // game/menu/CPlusMinus.cpp) Either of the two low flags of pad
            // nPad's entry.
            int IsUsedCursor(long nPad) const
            {
                return m_aPad[nPad].m_bUnk06_02 | m_aPad[nPad].m_bUnk06_01;
            }

            static CMenuPadInfo s_Instance;

        private:
            struct PAD {
                unsigned char m_Unk00[6];    // 0x00
                // 0x06. (names guessed; added for game/menu/CPlusMinus.cpp)
                // CSelectMenu::_GetUsedCursol tests the low two bits.
                unsigned char m_Unk06_FC : 6;
                unsigned char m_bUnk06_02 : 1;
                unsigned char m_bUnk06_01 : 1;
                unsigned char m_Unk07[3];    // 0x07
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
