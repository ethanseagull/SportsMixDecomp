#ifndef MSW_DWC_CGAMEDWC_HPP_
#define MSW_DWC_CGAMEDWC_HPP_

#include "bcs/dwc/CDwc.hpp"

namespace msw {
    namespace dwc {

        // The game's Wi-Fi connection object; CSelectMenu keeps a pointer
        // to it at 0x6FC. Only the members used so far; the size is not
        // known. Argument names are guesses.
        class CGameDwc {
            public:
                // Opens the message window with message pLabel of group
                // pGroup and an A button. The two texts fill parts of the
                // message (null: none); what nUnk1 and nUnk2 select is not
                // known.
                void WindowA_Button(char *pGroup, char *pLabel, wchar_t *pText0, wchar_t *pText1, int nUnk1,
                                    unsigned long nUnk2);
                // The same without a button.
                void Window_Message(char *pGroup, char *pLabel, int nUnk1, unsigned long nUnk2);
                void Window_Close();

                // (no symbol; name guessed) Forgets the answer of the last
                // message window. Inline in MenuF.
                void ClearWindowResult()
                {
                    m_UnkDBC = 0;
                    m_UnkDC0 = 0;
                }

            private:
                unsigned char m_Unk00[0x38]; // 0x00

            public:
                // 0x38. The connection itself. bcs::dwc::CDwc has no data
                // members in our header, so its size is not modelled: the
                // padding below keeps the offsets after it.
                bcs::dwc::CDwc m_Dwc;

            private:
                unsigned char m_Unk39[0xDBC - 0x39];

            public:
                // 0xDBC and 0xDC0. Cleared once the window's answer has been
                // read (names unknown).
                long m_UnkDBC;
                long m_UnkDC0;
        };
    }
}

#endif /* !MSW_DWC_CGAMEDWC_HPP_ */
