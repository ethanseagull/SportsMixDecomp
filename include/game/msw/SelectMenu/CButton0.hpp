#ifndef SELECTMENU_CBUTTON0_HPP_
#define SELECTMENU_CBUTTON0_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. A text window ("main_fmake_04") with no buttons of its
        // own.
        class CButton0 : public CSelectMenuLayoutBase {
            public:
                CButton0(long nId);
                virtual ~CButton0();
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

#endif /* !SELECTMENU_CBUTTON0_HPP_ */
