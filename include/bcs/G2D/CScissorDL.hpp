#ifndef BCS_G2D_CSCISSORDL_HPP_
#define BCS_G2D_CSCISSORDL_HPP_

#include "bcs/G2D/CPaneDL.hpp"

namespace bcs {
    namespace G2D {

        // A pane that clips what is drawn under it (name from the symbol).
        // Lives in the DOL. Size 0x170 (what MenuF game/menu/CBackground.cpp
        // allocates for one through nw4r::lyt::Layout::NewObj). Only the
        // constructor is used so far. Built on bcs::G2D::CPaneDL and adds
        // nothing (DOL game/bcs/G2D/CLineStripDL.cpp holds its functions).
        class CScissorDL : public CPaneDL {
            public:
                CScissorDL();
                virtual ~CScissorDL();

                // Sets the scissor box to the pane's size.
                virtual void MakeDL();
        };
    }
}

#endif /* !BCS_G2D_CSCISSORDL_HPP_ */
