#ifndef CMENUPAD_HPP_
#define CMENUPAD_HPP_

#include "bcs/FVEC2.hpp"

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

            // (name guessed; inline, no symbol; added for
            // game/msw/CMenuPad.cpp) 1 when any of the input bits nMask of
            // pad nPad is set.
            int IsInput(long nPad, unsigned long nMask) const
            {
                return (GetInput(nPad) & nMask) != 0;
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

            // (DOL fn_801413B8 and fn_801416EC; names guessed; called once
            // a frame by fn_80140484) Records the pointer positions and
            // switches each pad between pointer and stick mode; counts down
            // the frames without input.
            void UpdateInputMode();
            void UpdateNoInputCount();

            // (added for MenuF game/menu/CSelectMenu.cpp; argument meaning
            // guessed) Restarts the count of frames without input; the
            // first one also sets its limit (fTime frames).
            void ResetNoInputCountTime(float fTime);
            void ResetNoInputCountTime();
            // (DOL fn_80141CD4; name guessed; added for
            // game/msw/CCockpitMenuBase.cpp) s_Instance, or while bit 0 of
            // its flags at 0x144 is set another CMenuPad (lbl_804D4788).
            static CMenuPad *GetInputInstance();

            // (name guessed; inline, no symbol; added for
            // game/menu/CBackIcon.cpp) The instance; CBackIcon::Main tests
            // the address it returns for null.
            static CMenuPad *GetInstance() { return &s_Instance; }

        private:
            struct Pad {
                unsigned char m_Unk00[0x48];
                int m_nStyle; // 0x48
            };

            unsigned char m_Unk00[0x14]; // 0x00
            Pad m_Pad[4];                // 0x14
            unsigned short m_nFlags;      // 0x144
            unsigned char m_Unk146[0xF2];
            // (added for game/msw/CMenuPad.cpp; names guessed) Per pad,
            // 0x88 bytes: the last 16 pointer positions (newest first), a
            // countdown in frames that holds the input mode, and the mode
            // (0: pointer, 1: stick).
            struct PointerState {
                bcs::FVEC2 m_History[16]; // 0x00
                float m_fModeTime;        // 0x80
                unsigned char m_nMode;    // 0x84

                // (name guessed; inline, no symbol) Moves the history one
                // place back and puts pos in front.
                void PushHistory(const bcs::FVEC2 &pos)
                {
                    for (int i = 15; i > 0; i--) {
                        m_History[i] = m_History[i - 1];
                    }
                    // Dead store kept for the match: the original writes a zeroed
                    // temporary first. A port can drop it.
                    m_History[0] = bcs::FVEC2(0.0f, 0.0f);
                    m_History[0] = pos;
                }
            };
            PointerState m_Pointer[4];    // 0x238
            unsigned long m_nInput[4];    // 0x458, the entry count is a guess
            // (added for game/msw/CMenuPad.cpp; names guessed) Per pad: the
            // remote channel at +4 (what bcs::CPadWI is given).
            struct PadChan {
                long m_Unk00;
                long m_nChan;             // 0x04
            };
            PadChan m_Chan[4];            // 0x468
            // (added for game/msw/CMenuPad.cpp; meaning unknown) Per pad;
            // above 0.1 it counts as input for the no-input count.
            float m_fUnk488[4];           // 0x488
            // Per pad: frames left before the pad counts as idle, and the
            // value they restart from.
            float m_fNoInputTime[4];      // 0x498
            float m_fNoInputLimit;        // 0x4A8
        public:
            // 0x4AC. (name guessed; added for game/menu/CSelectMenu.cpp)
            // Per pad: set to 1 while the pointer is on a button of the
            // menu object (CSelectMenu::InputDataToMenuObj).
            unsigned long m_nPointerOnObj[4];
            // (name guessed; added for game/menu/CBackIcon.cpp) Per pad:
            // 1 while the pointer of a pad in pointer style is off the
            // screen, written by CBackIcon::Main.
            unsigned long m_nPointerOut[4]; // 0x4BC
        private:
            unsigned char m_Unk4CC[0x14];
    };
}

#endif /* !CMENUPAD_HPP_ */
