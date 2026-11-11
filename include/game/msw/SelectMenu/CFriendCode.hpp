#ifndef SELECTMENU_CFRIENDCODE_HPP_
#define SELECTMENU_CFRIENDCODE_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C0. The friend code entry screen ("me_f_cord"): the
        // player types the twelve digits of a friend's code on a number
        // pad, and CSelectMenu::FriendCode_* register the friend. Its
        // buttons are read from "me_f_cord.btn" by the CButtonEntry base at
        // 0x5C (a second base, as in CTeam). Menu object 0x1E of
        // CSelectMenu. Member names are guesses.
        class CFriendCode : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    INPUT_DIGIT_NUM = 12, // the digits of a friend code
                    DIGIT_NONE = 0xFF     // a digit not typed yet
                };

                CFriendCode(long nId);
                virtual ~CFriendCode();
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

                // Shows each typed digit in its text box and hides the
                // others. Returns 0 (return type guessed).
                long SetTextPaneFromInputDigit();

                // (no symbols; inline, names guessed) Forgets every typed
                // digit.
                void ClearInputDigit()
                {
                    for (long i = 0; i < INPUT_DIGIT_NUM; i++) {
                        m_nInputDigit[i] = DIGIT_NONE;
                    }
                    m_nInputNum = 0;
                }

                // The code typed so far as a number (the first digit is the
                // most significant).
                unsigned long long GetInputCode() const;

                // Shows the cursor under digit nIndex while the code is not
                // complete, and enables the OK button once it is.
                void SetInputCursor(long nIndex);

            private:
                // (no symbol; inline, name guessed) Puts pText into text
                // box pane nPane.
                void SetTextPaneString(long nPane, const wchar_t *pText);
                // Inlined in the game, with no out-of-line copy left in the
                // module; only its local static has a symbol.
                void CheckButtonAnim();

            public:
                // 0x5A0. The number of digits typed (0 to INPUT_DIGIT_NUM).
                long m_nInputNum;
                unsigned char m_Unk5A4[0xC];                    // 0x5A4
                // 0x5B0. The typed digits, DIGIT_NONE when not typed yet.
                unsigned char m_nInputDigit[INPUT_DIGIT_NUM];
                unsigned char m_Unk5BC[0x4];                    // 0x5BC
        };
    }
}

#endif /* !SELECTMENU_CFRIENDCODE_HPP_ */
