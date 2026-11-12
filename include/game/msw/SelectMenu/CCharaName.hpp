#ifndef SELECTMENU_CCHARANAME_HPP_
#define SELECTMENU_CCHARANAME_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "nw4r/g3d/ResMat.hpp"
#include "revolution/gx/GXEnum.h"

namespace bcs {
    namespace G2D {
        class CLayoutDL;
    }
}

namespace msw {
    namespace SelectMenu {

        // The character name plates of the character select screen (menu
        // object 0x11). Only SetRewriteTextureData is left in the module
        // (game/menu/CCharaName.cpp); the rest was stripped by the link.
        // The base is a guess (its own members start at 0x5C, the size of
        // CSelectMenuLayoutBase); the size is not known, so do not create
        // one through this header. Member names are guesses.
        class CCharaName : public CSelectMenuLayoutBase {
            public:
                // Draws its texts with texture texObj in slot mapId.
                void SetRewriteTextureData(nw4r::g3d::ResTexObj texObj, _GXTexMapID mapId);

            private:
                bcs::G2D::CLayoutDL *m_pLayout;   // 0x5C
                unsigned char m_Unk60[0x8];       // 0x60
                // The texture rewritten from texObj: an RGB565 copy of it.
                void *m_pTexImage;                // 0x68, allocated with new[]
                unsigned short m_nTexWidth;       // 0x6C
                unsigned short m_nTexHeight;      // 0x6E
                long m_nTexFormat;                // 0x70, GX_TF_RGB565
                unsigned char m_bTexMipmap;       // 0x74
                nw4r::g3d::ResTexObj m_ResTexObj; // 0x78
                _GXTexMapID m_TexMapId;           // 0x7C
        };
    }
}

#endif /* !SELECTMENU_CCHARANAME_HPP_ */
