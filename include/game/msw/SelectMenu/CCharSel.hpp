#ifndef SELECTMENU_CCHARSEL_HPP_
#define SELECTMENU_CCHARSEL_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "game/msw/SAVE_DATA_PLAYER.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x7BC. The character select screen (menu object 0x45), with
        // the characters standing on the court. A second base CButtonEntry
        // follows CSelectMenuLayoutBase (its destructor has a `@28@`
        // thunk). The screen comes in four layouts ("me_cha_A" to
        // "me_cha_D") for 8, 12, 15 and 18 open characters. Member names,
        // return types and argument names are guesses.
        class CCharSel : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    CHARA_BUTTON_MAX = 18, // character buttons 0x8010 to 0x8021
                    LAYOUT_NUM = 4
                };

                // Size 6. A character button of the layout shown.
                struct Chara {
                    unsigned short m_nBtnKindId; // 0x0, 0 ends the list
                    unsigned char m_nChara;      // 0x2, 0x80 and up: a "?" button
                    unsigned char m_nColor;      // 0x3, not written here
                    unsigned char m_nColorMask;  // 0x4, the colours that are open
                    unsigned char m_Unk5;        // 0x5
                };

                CCharSel(long nId);
                virtual ~CCharSel();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual void CallbackButtonAnimIn(long);
                virtual void CallbackButtonAnimOut(long);
                virtual void CallbackButtonAnimPush(long);
                virtual void CallbackButtonAnimCancel(long);
                virtual void CallbackButtonAnimMultiIn(long, long);
                virtual void CallbackButtonAnimMultiOut(long, long);
                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long nPad, long nBtnKindId);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsMarkerOnPointer(long, long);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);
                // New in this class (as in CButton1); both return 0 here.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

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
                long PushButtonIn(long nPad, unsigned char nChara, unsigned char nCharaSub);
                long PushCancelButton(long nPad, long nBtnKindId);
                void SetAllCourtCharaObj();
                void SetMixChara(int bMix);
                void SetPadCharaObjMotion(long nPad, long nMotion, long nMotionSub);
                void CallCharVoice(long nVoice);

                // (added for MenuF game/menu/CCharSel.cpp)
                void SetMiiButtonColor();
                void SetUpChar();
                // The button the stick moves to from nBtnKindId when the
                // plain move finds none: on along the row, or to one of the
                // two buttons beside the next one.
                unsigned short GetNextButtonKindId3Point(long nBtnKindId, unsigned long nInput);
                // Shows face nColor of the character on button nBtnKindId.
                void SetButtonFace(unsigned short nBtnKindId, unsigned char nColor);
                // Picks the layout (m_nLayout) from the characters open.
                void OpenCharCount();
                void SetCourtCharaObjImpl(long nPad, long nMotion, long nMotionSub, long nIndex);
                void SetPadSign(long nIndex, long nPad, long nPos);
                void SetColorMarkerObj();

            private:
                // Inline in the game (names guessed; no symbols).
                int IsSelectButton(unsigned short nBtnKindId);
                long GetCharaNum();
                void ResetCursorFace(long nPad);
                void MovePane(long nPane, long nParent);
                void AddPaneCopy(long nParent, const char *pSrcName, const char *pName);
                // Inlined into Main(); only its local static has a symbol.
                void CheckButtonAnim();

            public:
                Chara m_aChara[CHARA_BUTTON_MAX];    // 0x5A0
                // 0x60C. Set when the screen also passes its events to menu
                // object 1 (the Mii button panes are hung under the
                // window); cleared in the star Mii mode.
                long m_bMiiObj;
                long m_nLayout;                      // 0x610, 0 to 3: "me_cha_A" to "me_cha_D"
                long m_nMiiColorNum;                 // 0x614, the pads on the Mii button last coloured
                long m_nColumn[4];                   // 0x618, per pad: the column the cursor left the rows in
                char m_LayoutName[0x20];             // 0x628
                unsigned char m_Unk648[4];           // 0x648

                // 0x64C. (added for MenuF game/menu/CWiFiGame.cpp; meaning
                // not known) Cleared by CSelectMenu::WifiGame_In.
                long m_Unk64C[4];

                // 0x65C. The characters open in the mix sport: what the
                // four sports' records have in common.
                SAVE_DATA_PLAYER::CharaScore m_MixScore[SAVE_DATA_PLAYER::CHARA_SCORE_NUM];
                long m_bStarChara;                   // 0x79C, a star colour is open
                long m_bNewChara;                    // 0x7A0, a colour of mask 0x54 is open

                // 0x7A4. (added for MenuF game/menu/unit_6EE84.cpp; meaning
                // not known) Sent with event 0x49 to menu object 2 when an
                // "OK?" window is closed.
                long m_Unk7A4;
                long m_nCursorBtnKindId[4];          // 0x7A8, per pad: the button with its face shown
                float m_fWait;                       // 0x7B8, frames before the buttons come in
        };
    }
}

#endif /* !SELECTMENU_CCHARSEL_HPP_ */
