#ifndef SELECTMENU_CINITGUIDE_HPP_
#define SELECTMENU_CINITGUIDE_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. The attention window shown at start-up
        // ("me_attention"), with a text and the A button or two-button
        // prompt that matches the pad style.
        class CInitGuide : public CSelectMenuLayoutBase {
            public:
                CInitGuide(long nId);
                virtual ~CInitGuide();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

            private:
                // Inline in the game (Main and Set have the same code).
                void SetButtonDispByPadStyle();
        };
    }
}

#endif /* !SELECTMENU_CINITGUIDE_HPP_ */
