#ifndef BCS_G2D_CSCISSORDL_HPP_
#define BCS_G2D_CSCISSORDL_HPP_

#include "nw4r/lyt/Pane.hpp"

namespace bcs {
    namespace G2D {

        // A pane that clips what is drawn under it (name from the symbol).
        // Lives in the DOL. Size 0x170 (what MenuF game/menu/CBackground.cpp
        // allocates for one through nw4r::lyt::Layout::NewObj). Only the
        // constructor is used so far.
        class CScissorDL : public nw4r::lyt::Pane {
            public:
                CScissorDL();

            private:
                unsigned char m_UnkD8[0x170 - 0xD8];
        };
    }
}

#endif /* !BCS_G2D_CSCISSORDL_HPP_ */
