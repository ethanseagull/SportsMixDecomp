#ifndef BCS_G2D_CTEXMAP_HPP_
#define BCS_G2D_CTEXMAP_HPP_

#include "nw4r/lyt/TexMap.hpp"

namespace bcs {
    namespace G2D {

        // A layout texture slot made from a GX texture object. Lives in the
        // DOL. Size 0x20: an nw4r::lyt::TexMap (0x1C bytes, filled through
        // TexMap::Set; copied into a material's TexMap field by field in
        // msw::CRadarIcon2D::CreateCharaIconPicture), then the vptr at 0x1C.
        class CTexMap : public nw4r::lyt::TexMap {
            public:
                CTexMap(const _GXTexObj &rTexObj);

                // vptr at 0x1C
                virtual ~CTexMap();
        };
    }
}

#endif /* !BCS_G2D_CTEXMAP_HPP_ */
