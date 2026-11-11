#ifndef SELECTMENU_CCHARANAME_HPP_
#define SELECTMENU_CCHARANAME_HPP_

#include "nw4r/g3d/ResMat.hpp"
#include "revolution/gx/GXEnum.h"

namespace msw {
    namespace SelectMenu {

        // The character name plates of the character select screen (menu
        // object 0x11). Added for MenuF game/menu/unit_6EE84.cpp with only
        // the member called there; its base and size are not known, so do
        // not create one through this header.
        class CCharaName {
            public:
                // Draws its texts with texture texObj in slot mapId.
                void SetRewriteTextureData(nw4r::g3d::ResTexObj texObj, _GXTexMapID mapId);
        };
    }
}

#endif /* !SELECTMENU_CCHARANAME_HPP_ */
