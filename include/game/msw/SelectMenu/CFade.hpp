#ifndef SELECTMENU_CFADE_HPP_
#define SELECTMENU_CFADE_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5C. Opens and closes the cockpit's fade object on menu
        // events 0x31 and 0x32.
        class CFade : public CSelectMenuLayoutBase {
            public:
                CFade(long nId);
                virtual ~CFade();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();
        };
    }
}

#endif /* !SELECTMENU_CFADE_HPP_ */
