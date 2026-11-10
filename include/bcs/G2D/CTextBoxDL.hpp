#ifndef BCS_G2D_CTEXTBOXDL_HPP_
#define BCS_G2D_CTEXTBOXDL_HPP_

#include "nw4r/lyt/TextBox.hpp"

namespace bcs {
    namespace G2D {

        // The engine's text box pane. Lives in the DOL. Only what the game's
        // code needs so far: its type, for nw4r::ut::DynamicCast.
        class CTextBoxDL : public nw4r::lyt::TextBox {
            public:
                static const nw4r::ut::detail::RuntimeTypeInfo typeInfo;
        };
    }
}

#endif /* !BCS_G2D_CTEXTBOXDL_HPP_ */
