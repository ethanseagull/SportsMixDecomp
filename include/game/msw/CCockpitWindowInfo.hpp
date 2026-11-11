#ifndef CCOCKPITWINDOWINFO_HPP_
#define CCOCKPITWINDOWINFO_HPP_

#include "nw4r/lyt/Pane.hpp"

namespace msw {
    class CCockpitLayoutBase;
    class CMenuPaneAccessor;

    // Size 0xC, vptr at 0x08. The text windows of a cockpit menu: an array
    // the constructor of the menu allocates (0x80C bytes an entry). Lives in
    // the DOL; a base of CCockpitMenuBase. Only the members used so far.
    // Its functions sit in the DOL between CCockpitButtonInfo's and
    // CCockpitPartsInfo's, in the file of CCockpitMenuBase (see
    // game/msw/CCockpitWindowInfo.cpp).
    class CCockpitWindowInfo {
        public:
            // One text window. Size 0x80C. The menu's constructor sets the
            // pane to -1, the text to "?" and the font size to entry 0 of
            // s_aFontSize.
            struct WindowInfo {
                // Not inline: the DOL has it out of line (0x801ED5D0), passed
                // to __construct_new_array by the menu's constructor.
                WindowInfo();

                short m_nPaneId;            // 0x000, index in the menu's pane name table
                unsigned short m_nTextLen;  // 0x002, length of m_aText with the terminator
                wchar_t m_aText[0x400];     // 0x004
                nw4r::lyt::Size m_FontSize; // 0x804
            };

            // Copies pText (at most 0x3FF characters) as the text of window
            // nWindow.
            void SetWindowText(long nWindow, const wchar_t *pText);
            // (DOL fn_801ED6FC, no symbol) The same with the length given.
            void SetWindowText(long nWindow, const wchar_t *pText, unsigned long nLen);
            // Sets window nWindow's font size to entry nSize of s_aFontSize.
            void SetWindowFontSize(long nWindow, long nSize);

            // pPaneId has one entry for each window: the index of its text
            // pane in the menu's pane name table.
            void SetWindowTextPaneId(const short *pPaneId);
            // Return type guessed. Gives each window's text pane its text.
            void InfoParameterInitialize(CCockpitLayoutBase *pOwner, const short *pPaneId,
                                         CMenuPaneAccessor *pPaneAccessor, int nArg);

            // (name guessed; DOL lbl_804D7738, set by the static initialiser
            // at the end of CCockpitMenuBase's file) The font sizes
            // SetWindowFontSize picks from.
            static nw4r::lyt::Size s_aFontSize[3];

        private:
            WindowInfo *m_pInfo; // 0x00
            long m_nNum;         // 0x04

        public:
            virtual ~CCockpitWindowInfo();
    };
}

#endif /* !CCOCKPITWINDOWINFO_HPP_ */
