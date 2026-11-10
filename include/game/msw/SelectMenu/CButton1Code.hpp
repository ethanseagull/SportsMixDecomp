#ifndef SELECTMENU_CBUTTON1CODE_HPP_
#define SELECTMENU_CBUTTON1CODE_HPP_

#include "game/msw/SelectMenu/CButton1.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. CButton1's window with the "main_fmake_01" layout: the
        // same button, plus a code text box ("t_fcode") and a cursor pane
        // ("tex_csr") that plays its own animation. Every other virtual is
        // CButton1's.
        class CButton1Code : public CButton1 {
            public:
                CButton1Code(long nId);
                virtual ~CButton1Code();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

            private:
                // Inline in the game; only its local statics have symbols.
                void InitPaneAnim();
        };
    }
}

#endif /* !SELECTMENU_CBUTTON1CODE_HPP_ */
