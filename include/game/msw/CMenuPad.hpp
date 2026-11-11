#ifndef CMENUPAD_HPP_
#define CMENUPAD_HPP_

namespace msw {

    // Size 0x4E0 (the size of s_Instance). Lives in the DOL. Only the member
    // used so far.
    class CMenuPad {
        public:
            // (name guessed; no symbol) What menu::SetDispPaneByPadStyle is
            // given for pad nPad. Only the address is known: 0x5C + nPad *
            // 0x4C. Where the array starts inside those first 0x5C bytes, and
            // how many entries it has, are guesses.
            int GetPadStyle(long nPad) const { return m_Pad[nPad].m_nStyle; }

            // (name guessed; no symbol) The input bits of pad nPad, or 0
            // while bit 0 of the flags at 0x144 is set. The same sequence
            // is inline in the DOL's own CMenuPad code and in the modules.
            unsigned long GetInput(long nPad) const
            {
                if (m_nFlags & 1) {
                    return 0;
                }
                return m_nInput[nPad];
            }

            // Not zero when pad nPad has a nunchaku. In the DOL.
            int IsNunchakuStyle(long nPad);
            // (added for MenuF game/menu/CEntry.cpp; return type guessed)
            // Not zero while pad nPad is connected.
            int IsConnect(long nPad);
            // Added for MenuF game/menu/CWiFiFrindVS.cpp (return types and
            // argument names guessed).
            int IsInputPointerMode(long nPad);
            void ChangeInputStickModeForce(long nPad);
            // (added for MenuF game/menu/CFriendCode.cpp; return type
            // guessed)
            void ChangeInputPointerModeForce(long nPad);

            static CMenuPad s_Instance;

        private:
            struct Pad {
                unsigned char m_Unk00[0x48];
                int m_nStyle; // 0x48
            };

            unsigned char m_Unk00[0x14]; // 0x00
            Pad m_Pad[4];                // 0x14
            unsigned short m_nFlags;      // 0x144
            unsigned char m_Unk146[0x312];
            unsigned long m_nInput[4];    // 0x458, the entry count is a guess
            unsigned char m_Unk468[0x78];
    };
}

#endif /* !CMENUPAD_HPP_ */
