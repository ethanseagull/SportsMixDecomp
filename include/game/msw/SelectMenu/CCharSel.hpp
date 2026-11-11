#ifndef SELECTMENU_CCHARSEL_HPP_
#define SELECTMENU_CCHARSEL_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x7BC. The character select screen (menu object 0x45), with
        // the characters standing on the court. A second base CButtonEntry
        // follows CSelectMenuLayoutBase (its destructor has a `@28@`
        // thunk). Only the members used so far are written; do not create
        // or destroy one through this header. Return types and argument
        // names are guesses.
        class CCharSel : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                // Added for MenuF game/menu/CCharaStatus.cpp. Shows or hides
                // the pad sign over character slot nIndex.
                void SetCharaObjPadSign(long nUnk, long nIndex);
                void ClearPadSign(long nIndex, long nUnk);
                // The court characters of slot nIndex (CCharaStatus passes
                // a motion and its variant).
                void SetCourtCharaObjImplEx(long nUnk, long nMotion, long nMotionSub, long nIndex);
                void SetCourtCharaObjMotionEx(long nIndex, long nMotion, long nMotionSub);
                void SetCourtCharaObj(long nUnk0, long nMotion, long nMotionSub, long nUnk1);

                // (added for MenuF game/menu/unit_6EE84.cpp; return types
                // and argument names guessed) The character (and its colour)
                // of button nBtnKindId, negative for none; the button of
                // character nChara.
                long FindChrTypeByBtnKind(unsigned short nBtnKindId, long &rChara, long &rColor);
                long FindBtnKindByChrType(long nChara);
                int FindChrTypeChangeColor(unsigned short nBtnKindId);
                long GetChrColor(unsigned short nBtnKindId, unsigned char nColor);
                unsigned char GetNextChrColor(unsigned short nBtnKindId, unsigned char nColor);
                // The button next to nBtnKindId in direction nInput, 0 for
                // none.
                unsigned short GetNextButtonKindId(long nBtnKindId, unsigned long nInput);
                // A button the random draw moves the cursor to.
                long RandomMoveCheck();
                void PushButtonIn(long nPad, unsigned char nChara, unsigned char nCharaSub);
                void PushCancelButton(long nPad, long nBtnKindId);
                void SetAllCourtCharaObj();
                void SetMixChara(int bMix);
                void SetPadCharaObjMotion(long nPad, long nMotion, long nMotionSub);
                void CallCharVoice(long nVoice);

            private:
                unsigned char m_Unk5A0[0x64C - 0x5A0];

            public:
                // 0x64C. (added for MenuF game/menu/CWiFiGame.cpp; meaning
                // not known) Cleared by CSelectMenu::WifiGame_In.
                long m_Unk64C[4];

            private:
                unsigned char m_Unk65C[0x7A4 - 0x65C];

            public:
                // 0x7A4. (added for MenuF game/menu/unit_6EE84.cpp; meaning
                // not known) Sent with event 0x49 to menu object 2 when an
                // "OK?" window is closed.
                long m_Unk7A4;
        };
    }
}

#endif /* !SELECTMENU_CCHARSEL_HPP_ */
