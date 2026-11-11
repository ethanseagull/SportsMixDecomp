#ifndef BCS_G2D_CTEXTBOXDL_HPP_
#define BCS_G2D_CTEXTBOXDL_HPP_

#include "nw4r/lyt/TextBox.hpp"
#include "nw4r/ut/Rect.h"

namespace bcs {
    namespace G2D {

        // The engine's text box pane. Lives in the DOL. Only what the game's
        // code needs so far: its type, for nw4r::ut::DynamicCast.
        class CTextBoxDL : public nw4r::lyt::TextBox {
            public:
                static const nw4r::ut::detail::RuntimeTypeInfo typeInfo;

                // Added for MenuF game/menu/CBackground.cpp: the rectangle
                // the text takes, ignoring line feeds (name from the
                // symbol; returned by value).
                nw4r::ut::Rect GetTextDrawRectNoLineFeed();
                // Added for BossF game/boss/CCockpitBossCoder.cpp: draws with
                // the font in resource pFontRes (a .brfnt image; return type
                // guessed). Resets the font size, which callers restore.
                void SetFontByResource(void *pFontRes);
        };
    }
}

#endif /* !BCS_G2D_CTEXTBOXDL_HPP_ */
