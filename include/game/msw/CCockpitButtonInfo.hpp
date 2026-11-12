#ifndef CCOCKPITBUTTONINFO_HPP_
#define CCOCKPITBUTTONINFO_HPP_

#include <string.h>
#include <wchar.h>

namespace msw {
    class CCockpitLayoutBase;
    class CMenuPaneAccessor;

    // Size 0xC, vptr at 0x08. The texts and sounds of a cockpit menu's
    // buttons: an array the constructor of the menu allocates (0x114 bytes
    // an entry). Lives in the DOL; a base of CCockpitMenuBase. Its
    // functions open CCockpitMenuBase's file in the DOL (see
    // game/msw/CCockpitButtonInfo.cpp).
    class CCockpitButtonInfo {
        public:
            // What SetButtonInfo takes for each button (a table of them,
            // 0x1C bytes an entry; TourF's CTournamentExit passes one).
            // Member names guessed; only the members used.
            struct ButtonInfo {
                long m_Unk00;              // 0x00
                long m_nMenuResultId;      // 0x04
                const char *m_pFileName;   // 0x08, message file of the text
                const char *m_pLabel;      // 0x0C, label of the text in it
                unsigned char m_bDefault;  // 0x10, the leading pad's cursor starts here
                unsigned char m_Unk11[0x1C - 0x11];
            };

            // (name guessed) What ButtonParameterInitialize takes for each
            // button, 0x16 bytes an entry. Only the members used.
            struct ButtonPaneInfo {
                short m_Unk00[2];               // 0x00
                short m_nTextPaneId;            // 0x04, below 0: none
                short m_Unk06[3];               // 0x06
                unsigned short m_nBtnKindId;    // 0x0C
                short m_Unk0E[4];               // 0x0E
            };

            // (name guessed) One button. Size 0x114. No constructor: the
            // menu's constructor allocates the array without an array
            // cookie and calls Init (out of line in the DOL, 0x801ECE98;
            // name guessed) on each entry.
            struct Button {
                void Init();

                long m_Unk00;                  // 0x000
                long m_nMenuResultId;          // 0x004
                wchar_t m_aText[0x80];         // 0x008
                unsigned char m_abDefault[4];  // 0x108, per pad: the cursor starts here
                long m_aSECode[2];             // 0x10C
            };

            // (names guessed for the ones without a symbol) Index nButton
            // must be below the number of buttons; the getters return -1
            // (or null) for another.
            long GetMenuResultId(long nButton);
            void SetMenuResultId(long nButton, long nId);
            const wchar_t *GetButtonText(long nButton);
            void SetButtonText(long nButton, const wchar_t *pText);
            long GetButtonSECode(long nButton, long nIdx);
            void SetButtonSECodeArray(long nButton, long nSE0, long nSE1);
            // Puts the leading pad's cursor on button nButton.
            void SetDefaultCursorFlag(long nButton);
            void SetButtonInfo(const ButtonInfo *pInfo);
            // (DOL fn_801ED44C, no symbol; return type guessed) Gives the
            // buttons' text panes their texts and the owner the default
            // button of each pad.
            void ButtonParameterInitialize(CCockpitLayoutBase *pOwner, const ButtonPaneInfo *pPaneInfo,
                                         CMenuPaneAccessor *pPaneAccessor, int nArg);

        private:
            // (added for game/msw/CCockpitMenuBase.cpp) Its step functions
            // read the result ids inline.
            friend class CCockpitMenuBase;

            bool IsValidButton(long nButton) const
            {
                bool bValid = false;
                if (nButton < m_nNum && m_nNum > 0) {
                    bValid = true;
                }
                return bValid;
            }

            void SetButtonText(long nButton, const wchar_t *pText, unsigned long nLen)
            {
                if (IsValidButton(nButton)) {
                    wchar_t *pDst = m_pInfo[nButton].m_aText;
                    unsigned long nCopy = nLen;
                    if (nLen >= 0x80) {
                        nCopy = 0x7F;
                    }
                    memcpy(pDst, pText, nCopy * sizeof(wchar_t));
                    pDst[nCopy] = 0;
                }
            }

            void SetDefaultCursor(long nButton, unsigned char bDefault, long nLeader)
            {
                if (IsValidButton(nButton)) {
                    for (long i = 0; i < 4; i++) {
                        m_pInfo[nButton].m_abDefault[i] = (i == nLeader) ? bDefault : (unsigned char)0;
                    }
                }
            }

            bool IsDefaultCursor(long nButton, long nPad) const
            {
                return IsValidButton(nButton) ? m_pInfo[nButton].m_abDefault[nPad] : false;
            }

            Button *m_pInfo; // 0x00
            long m_nNum;     // 0x04

        public:
            virtual ~CCockpitButtonInfo();
    };
}

#endif /* !CCOCKPITBUTTONINFO_HPP_ */
