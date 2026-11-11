#ifndef BCS_G2D_CPICTUREDL_HPP_
#define BCS_G2D_CPICTUREDL_HPP_

#include "nw4r/lyt/Material.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/ut/RuntimeTypeInfo.h"

namespace bcs {
    namespace G2D {

        // (name guessed) The material of the engine's picture panes. Every
        // writer of a picture's texture sets the word at 0x3A4 to 1 after
        // Material::SetTexture (VolleyF CCockpitChrIcon2DVolley::SetIconFace
        // and the DOL's fn_802597A0), so the material is a derived class with
        // a flag there, most likely "display list out of date". Opaque
        // otherwise; never instantiated.
        class CMaterialDL : public nw4r::lyt::Material {
            public:
                void SetModified() { m_bModified = 1; }

            private:
                unsigned char m_Unk44[0x3A4 - 0x44];
                int m_bModified; // 0x3A4
        };

        // The engine's picture pane. Lives in the DOL. Only what the game's
        // code needs so far: its type, for nw4r::ut::DynamicCast. Its true
        // base is nw4r::lyt::Picture, which has no header yet.
        class CPictureDL : public nw4r::lyt::Pane {
            public:
                static const nw4r::ut::detail::RuntimeTypeInfo typeInfo;
        };
    }
}

#endif /* !BCS_G2D_CPICTUREDL_HPP_ */
